/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __INTEL_RAPL_H__
#define __INTEL_RAPL_H__

#include <linux/types.h>
#include <linux/powercap.h>
#include <linux/cpuhotplug.h>

enum rapl_if_type {
	RAPL_IF_MSR,	
	RAPL_IF_MMIO,	
	RAPL_IF_TPMI,	
};

enum rapl_domain_type {
	RAPL_DOMAIN_PACKAGE,	
	RAPL_DOMAIN_PP0,	
	RAPL_DOMAIN_PP1,	
	RAPL_DOMAIN_DRAM,	
	RAPL_DOMAIN_PLATFORM,	
	RAPL_DOMAIN_MAX,
};

enum rapl_domain_reg_id {
	RAPL_DOMAIN_REG_LIMIT,
	RAPL_DOMAIN_REG_STATUS,
	RAPL_DOMAIN_REG_PERF,
	RAPL_DOMAIN_REG_POLICY,
	RAPL_DOMAIN_REG_INFO,
	RAPL_DOMAIN_REG_PL4,
	RAPL_DOMAIN_REG_UNIT,
	RAPL_DOMAIN_REG_PL2,
	RAPL_DOMAIN_REG_MAX,
};

struct rapl_domain;

enum rapl_primitives {
	POWER_LIMIT1,
	POWER_LIMIT2,
	POWER_LIMIT4,
	ENERGY_COUNTER,
	FW_LOCK,
	FW_HIGH_LOCK,
	PL1_LOCK,
	PL2_LOCK,
	PL4_LOCK,

	PL1_ENABLE,		
	PL1_CLAMP,		
	PL2_ENABLE,		
	PL2_CLAMP,
	PL4_ENABLE,		

	TIME_WINDOW1,		
	TIME_WINDOW2,		
	THERMAL_SPEC_POWER,
	MAX_POWER,

	MIN_POWER,
	MAX_TIME_WINDOW,
	THROTTLED_TIME,
	PRIORITY_LEVEL,

	PSYS_POWER_LIMIT1,
	PSYS_POWER_LIMIT2,
	PSYS_PL1_ENABLE,
	PSYS_PL2_ENABLE,
	PSYS_TIME_WINDOW1,
	PSYS_TIME_WINDOW2,
	
	AVERAGE_POWER,
	NR_RAPL_PRIMITIVES,
};

struct rapl_domain_data {
	u64 primitives[NR_RAPL_PRIMITIVES];
	unsigned long timestamp;
};

#define NR_POWER_LIMITS	(POWER_LIMIT4 + 1)

struct rapl_power_limit {
	struct powercap_zone_constraint *constraint;
	struct rapl_domain *domain;
	const char *name;
	bool locked;
	u64 last_power_limit;
};

struct rapl_package;

#define RAPL_DOMAIN_NAME_LENGTH 16

union rapl_reg {
	void __iomem *mmio;
	u32 msr;
	u64 val;
};

struct rapl_domain {
	char name[RAPL_DOMAIN_NAME_LENGTH];
	enum rapl_domain_type id;
	union rapl_reg regs[RAPL_DOMAIN_REG_MAX];
	struct powercap_zone power_zone;
	struct rapl_domain_data rdd;
	struct rapl_power_limit rpl[NR_POWER_LIMITS];
	u64 attr_map;		
	unsigned int state;
	unsigned int power_unit;
	unsigned int energy_unit;
	unsigned int time_unit;
	struct rapl_package *rp;
};

struct reg_action {
	union rapl_reg reg;
	u64 mask;
	u64 value;
	int err;
};


struct rapl_if_priv {
	enum rapl_if_type type;
	struct powercap_control_type *control_type;
	enum cpuhp_state pcap_rapl_online;
	union rapl_reg reg_unit;
	union rapl_reg regs[RAPL_DOMAIN_MAX][RAPL_DOMAIN_REG_MAX];
	int limits[RAPL_DOMAIN_MAX];
	int (*read_raw)(int id, struct reg_action *ra);
	int (*write_raw)(int id, struct reg_action *ra);
	void *defaults;
	void *rpi;
};


#define PACKAGE_DOMAIN_NAME_LENGTH 30

struct rapl_package {
	unsigned int id;	
	unsigned int nr_domains;
	unsigned long domain_map;	
	struct rapl_domain *domains;	
	struct powercap_zone *power_zone;	
	unsigned long power_limit_irq;	
	struct list_head plist;
	int lead_cpu;		
	
	struct cpumask cpumask;
	char name[PACKAGE_DOMAIN_NAME_LENGTH];
	struct rapl_if_priv *priv;
};

struct rapl_package *rapl_find_package_domain_cpuslocked(int id, struct rapl_if_priv *priv,
						       bool id_is_cpu);
struct rapl_package *rapl_add_package_cpuslocked(int id, struct rapl_if_priv *priv,
						 bool id_is_cpu);
void rapl_remove_package_cpuslocked(struct rapl_package *rp);

struct rapl_package *rapl_find_package_domain(int id, struct rapl_if_priv *priv, bool id_is_cpu);
struct rapl_package *rapl_add_package(int id, struct rapl_if_priv *priv, bool id_is_cpu);
void rapl_remove_package(struct rapl_package *rp);

#endif 
