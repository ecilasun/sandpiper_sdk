/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __CPU_COOLING_H__
#define __CPU_COOLING_H__

#include <linux/of.h>
#include <linux/thermal.h>
#include <linux/cpumask.h>

struct cpufreq_policy;

#ifdef CONFIG_CPU_FREQ_THERMAL

struct thermal_cooling_device *
cpufreq_cooling_register(struct cpufreq_policy *policy);


void cpufreq_cooling_unregister(struct thermal_cooling_device *cdev);


struct thermal_cooling_device *
of_cpufreq_cooling_register(struct cpufreq_policy *policy);

#else 
static inline struct thermal_cooling_device *
cpufreq_cooling_register(struct cpufreq_policy *policy)
{
	return ERR_PTR(-ENOSYS);
}

static inline
void cpufreq_cooling_unregister(struct thermal_cooling_device *cdev)
{
	return;
}

static inline struct thermal_cooling_device *
of_cpufreq_cooling_register(struct cpufreq_policy *policy)
{
	return NULL;
}
#endif 

struct cpuidle_driver;

#ifdef CONFIG_CPU_IDLE_THERMAL
void cpuidle_cooling_register(struct cpuidle_driver *drv);
#else 
static inline void cpuidle_cooling_register(struct cpuidle_driver *drv)
{
}
#endif 

#endif 
