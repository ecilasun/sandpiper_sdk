/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_ARM_TOPOLOGY_H
#define _ASM_ARM_TOPOLOGY_H

#ifdef CONFIG_ARM_CPU_TOPOLOGY

#include <linux/cpumask.h>
#include <linux/arch_topology.h>


#ifndef CONFIG_BL_SWITCHER

#define arch_set_freq_scale topology_set_freq_scale
#define arch_scale_freq_capacity topology_get_freq_scale
#define arch_scale_freq_invariant topology_scale_freq_invariant
#endif


#define arch_scale_cpu_capacity topology_get_cpu_scale


#define arch_update_cpu_topology topology_update_cpu_topology


#define arch_scale_thermal_pressure topology_get_thermal_pressure
#define arch_update_thermal_pressure	topology_update_thermal_pressure

#else

static inline void init_cpu_topology(void) { }
static inline void store_cpu_topology(unsigned int cpuid) { }

#endif

#include <asm-generic/topology.h>

#endif 
