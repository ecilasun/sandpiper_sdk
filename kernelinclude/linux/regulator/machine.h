/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_REGULATOR_MACHINE_H_
#define __LINUX_REGULATOR_MACHINE_H_

#include <linux/regulator/consumer.h>
#include <linux/suspend.h>

struct regulator;



#define REGULATOR_CHANGE_VOLTAGE	0x1
#define REGULATOR_CHANGE_CURRENT	0x2
#define REGULATOR_CHANGE_MODE		0x4
#define REGULATOR_CHANGE_STATUS		0x8
#define REGULATOR_CHANGE_DRMS		0x10
#define REGULATOR_CHANGE_BYPASS		0x20


#define DO_NOTHING_IN_SUSPEND	0
#define DISABLE_IN_SUSPEND	1
#define ENABLE_IN_SUSPEND	2


enum regulator_active_discharge {
	REGULATOR_ACTIVE_DISCHARGE_DEFAULT,
	REGULATOR_ACTIVE_DISCHARGE_DISABLE,
	REGULATOR_ACTIVE_DISCHARGE_ENABLE,
};


struct regulator_state {
	int uV;
	int min_uV;
	int max_uV;
	unsigned int mode;
	int enabled;
	bool changeable;
};

#define REGULATOR_NOTIF_LIMIT_DISABLE -1
#define REGULATOR_NOTIF_LIMIT_ENABLE -2
struct notification_limit {
	int prot;
	int err;
	int warn;
};


struct regulation_constraints {

	const char *name;

	
	int min_uV;
	int max_uV;

	int uV_offset;

	
	int min_uA;
	int max_uA;
	int ilim_uA;

	int system_load;

	
	u32 *max_spread;

	
	int max_uV_step;

	
	unsigned int valid_modes_mask;

	
	unsigned int valid_ops_mask;

	
	int input_uV;

	
	struct regulator_state state_disk;
	struct regulator_state state_mem;
	struct regulator_state state_standby;
	struct notification_limit over_curr_limits;
	struct notification_limit over_voltage_limits;
	struct notification_limit under_voltage_limits;
	struct notification_limit temp_limits;
	suspend_state_t initial_state; 

	
	unsigned int initial_mode;

	unsigned int ramp_delay;
	unsigned int settling_time;
	unsigned int settling_time_up;
	unsigned int settling_time_down;
	unsigned int enable_time;

	unsigned int active_discharge;

	
	unsigned always_on:1;	
	unsigned boot_on:1;	
	unsigned apply_uV:1;	
	unsigned ramp_disable:1; 
	unsigned soft_start:1;	
	unsigned pull_down:1;	
	unsigned over_current_protection:1; 
	unsigned over_current_detection:1; 
	unsigned over_voltage_detection:1; 
	unsigned under_voltage_detection:1; 
	unsigned over_temp_detection:1; 
};


struct regulator_consumer_supply {
	const char *dev_name;   
	const char *supply;	
};


#define REGULATOR_SUPPLY(_name, _dev_name)			\
{								\
	.supply		= _name,				\
	.dev_name	= _dev_name,				\
}


struct regulator_init_data {
	const char *supply_regulator;        

	struct regulation_constraints constraints;

	int num_consumer_supplies;
	struct regulator_consumer_supply *consumer_supplies;

	
	int (*regulator_init)(void *driver_data);
	void *driver_data;	
};

#ifdef CONFIG_REGULATOR
void regulator_has_full_constraints(void);
#else
static inline void regulator_has_full_constraints(void)
{
}
#endif

static inline int regulator_suspend_prepare(suspend_state_t state)
{
	return 0;
}
static inline int regulator_suspend_finish(void)
{
	return 0;
}

#endif
