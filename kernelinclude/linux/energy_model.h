/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ENERGY_MODEL_H
#define _LINUX_ENERGY_MODEL_H
#include <linux/cpumask.h>
#include <linux/device.h>
#include <linux/jump_label.h>
#include <linux/kobject.h>
#include <linux/rcupdate.h>
#include <linux/sched/cpufreq.h>
#include <linux/sched/topology.h>
#include <linux/types.h>


struct em_perf_state {
	unsigned long frequency;
	unsigned long power;
	unsigned long cost;
	unsigned long flags;
};


#define EM_PERF_STATE_INEFFICIENT BIT(0)


struct em_perf_domain {
	struct em_perf_state *table;
	int nr_perf_states;
	unsigned long flags;
	unsigned long cpus[];
};


#define EM_PERF_DOMAIN_MICROWATTS BIT(0)
#define EM_PERF_DOMAIN_SKIP_INEFFICIENCIES BIT(1)
#define EM_PERF_DOMAIN_ARTIFICIAL BIT(2)

#define em_span_cpus(em) (to_cpumask((em)->cpus))
#define em_is_artificial(em) ((em)->flags & EM_PERF_DOMAIN_ARTIFICIAL)

#ifdef CONFIG_ENERGY_MODEL

#define EM_MAX_POWER (64000000) 


#ifdef CONFIG_64BIT
#define EM_MAX_NUM_CPUS 4096
#else
#define EM_MAX_NUM_CPUS 16
#endif


#ifdef CONFIG_64BIT
#define em_estimate_energy(cost, sum_util, scale_cpu) \
	(((cost) * (sum_util)) / (scale_cpu))
#else
#define em_estimate_energy(cost, sum_util, scale_cpu) \
	(((cost) / (scale_cpu)) * (sum_util))
#endif

struct em_data_callback {
	
	int (*active_power)(struct device *dev, unsigned long *power,
			    unsigned long *freq);

	
	int (*get_cost)(struct device *dev, unsigned long freq,
			unsigned long *cost);
};
#define EM_SET_ACTIVE_POWER_CB(em_cb, cb) ((em_cb).active_power = cb)
#define EM_ADV_DATA_CB(_active_power_cb, _cost_cb)	\
	{ .active_power = _active_power_cb,		\
	  .get_cost = _cost_cb }
#define EM_DATA_CB(_active_power_cb)			\
		EM_ADV_DATA_CB(_active_power_cb, NULL)

struct em_perf_domain *em_cpu_get(int cpu);
struct em_perf_domain *em_pd_get(struct device *dev);
int em_dev_register_perf_domain(struct device *dev, unsigned int nr_states,
				struct em_data_callback *cb, cpumask_t *span,
				bool microwatts);
void em_dev_unregister_perf_domain(struct device *dev);


static inline
struct em_perf_state *em_pd_get_efficient_state(struct em_perf_domain *pd,
						unsigned long freq)
{
	struct em_perf_state *ps;
	int i;

	for (i = 0; i < pd->nr_perf_states; i++) {
		ps = &pd->table[i];
		if (ps->frequency >= freq) {
			if (pd->flags & EM_PERF_DOMAIN_SKIP_INEFFICIENCIES &&
			    ps->flags & EM_PERF_STATE_INEFFICIENT)
				continue;
			break;
		}
	}

	return ps;
}


static inline unsigned long em_cpu_energy(struct em_perf_domain *pd,
				unsigned long max_util, unsigned long sum_util,
				unsigned long allowed_cpu_cap)
{
	unsigned long freq, scale_cpu;
	struct em_perf_state *ps;
	int cpu;

	if (!sum_util)
		return 0;

	
	cpu = cpumask_first(to_cpumask(pd->cpus));
	scale_cpu = arch_scale_cpu_capacity(cpu);
	ps = &pd->table[pd->nr_perf_states - 1];

	max_util = map_util_perf(max_util);
	max_util = min(max_util, allowed_cpu_cap);
	freq = map_util_freq(max_util, ps->frequency, scale_cpu);

	
	ps = em_pd_get_efficient_state(pd, freq);

	
	return em_estimate_energy(ps->cost, sum_util, scale_cpu);
}


static inline int em_pd_nr_perf_states(struct em_perf_domain *pd)
{
	return pd->nr_perf_states;
}

#else
struct em_data_callback {};
#define EM_ADV_DATA_CB(_active_power_cb, _cost_cb) { }
#define EM_DATA_CB(_active_power_cb) { }
#define EM_SET_ACTIVE_POWER_CB(em_cb, cb) do { } while (0)

static inline
int em_dev_register_perf_domain(struct device *dev, unsigned int nr_states,
				struct em_data_callback *cb, cpumask_t *span,
				bool microwatts)
{
	return -EINVAL;
}
static inline void em_dev_unregister_perf_domain(struct device *dev)
{
}
static inline struct em_perf_domain *em_cpu_get(int cpu)
{
	return NULL;
}
static inline struct em_perf_domain *em_pd_get(struct device *dev)
{
	return NULL;
}
static inline unsigned long em_cpu_energy(struct em_perf_domain *pd,
			unsigned long max_util, unsigned long sum_util,
			unsigned long allowed_cpu_cap)
{
	return 0;
}
static inline int em_pd_nr_perf_states(struct em_perf_domain *pd)
{
	return 0;
}
#endif

#endif
