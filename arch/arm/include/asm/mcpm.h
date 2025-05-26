/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef MCPM_H
#define MCPM_H


#define MAX_CPUS_PER_CLUSTER	4

#ifdef CONFIG_MCPM_QUAD_CLUSTER
#define MAX_NR_CLUSTERS		4
#else
#define MAX_NR_CLUSTERS		2
#endif

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <asm/cacheflush.h>


extern void mcpm_entry_point(void);


void mcpm_set_entry_vector(unsigned cpu, unsigned cluster, void *ptr);


void mcpm_set_early_poke(unsigned cpu, unsigned cluster,
			 unsigned long poke_phys_addr, unsigned long poke_val);




bool mcpm_is_available(void);


int mcpm_cpu_power_up(unsigned int cpu, unsigned int cluster);


void mcpm_cpu_power_down(void);


int mcpm_wait_for_cpu_powerdown(unsigned int cpu, unsigned int cluster);


void mcpm_cpu_suspend(void);


int mcpm_cpu_powered_up(void);


struct mcpm_platform_ops {
	int (*cpu_powerup)(unsigned int cpu, unsigned int cluster);
	int (*cluster_powerup)(unsigned int cluster);
	void (*cpu_suspend_prepare)(unsigned int cpu, unsigned int cluster);
	void (*cpu_powerdown_prepare)(unsigned int cpu, unsigned int cluster);
	void (*cluster_powerdown_prepare)(unsigned int cluster);
	void (*cpu_cache_disable)(void);
	void (*cluster_cache_disable)(void);
	void (*cpu_is_up)(unsigned int cpu, unsigned int cluster);
	void (*cluster_is_up)(unsigned int cluster);
	int (*wait_for_powerdown)(unsigned int cpu, unsigned int cluster);
};


int __init mcpm_platform_register(const struct mcpm_platform_ops *ops);


int __init mcpm_sync_init(
	void (*power_up_setup)(unsigned int affinity_level));


int __init mcpm_loopback(void (*cache_disable)(void));

void __init mcpm_smp_set_ops(void);


struct mcpm_sync_struct {
	
	struct {
		s8 cpu __aligned(__CACHE_WRITEBACK_GRANULE);
	} cpus[MAX_CPUS_PER_CLUSTER];

	
	s8 cluster __aligned(__CACHE_WRITEBACK_GRANULE);

	
	s8 inbound __aligned(__CACHE_WRITEBACK_GRANULE);
};

struct sync_struct {
	struct mcpm_sync_struct clusters[MAX_NR_CLUSTERS];
};

#else


#include <asm/asm-offsets.h>
#define __CACHE_WRITEBACK_GRANULE CACHE_WRITEBACK_GRANULE

#endif 


#define CPU_DOWN		0x11
#define CPU_COMING_UP		0x12
#define CPU_UP			0x13
#define CPU_GOING_DOWN		0x14

#define CLUSTER_DOWN		0x21
#define CLUSTER_UP		0x22
#define CLUSTER_GOING_DOWN	0x23

#define INBOUND_NOT_COMING_UP	0x31
#define INBOUND_COMING_UP	0x32


#define MCPM_SYNC_CLUSTER_CPUS	0
#define MCPM_SYNC_CPU_SIZE	__CACHE_WRITEBACK_GRANULE
#define MCPM_SYNC_CLUSTER_CLUSTER \
	(MCPM_SYNC_CLUSTER_CPUS + MCPM_SYNC_CPU_SIZE * MAX_CPUS_PER_CLUSTER)
#define MCPM_SYNC_CLUSTER_INBOUND \
	(MCPM_SYNC_CLUSTER_CLUSTER + __CACHE_WRITEBACK_GRANULE)
#define MCPM_SYNC_CLUSTER_SIZE \
	(MCPM_SYNC_CLUSTER_INBOUND + __CACHE_WRITEBACK_GRANULE)

#endif
