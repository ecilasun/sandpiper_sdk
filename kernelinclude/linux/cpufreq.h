/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _LINUX_CPUFREQ_H
#define _LINUX_CPUFREQ_H

#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/completion.h>
#include <linux/kobject.h>
#include <linux/notifier.h>
#include <linux/of.h>
#include <linux/pm_opp.h>
#include <linux/pm_qos.h>
#include <linux/spinlock.h>
#include <linux/sysfs.h>
#include <linux/minmax.h>




#define CPUFREQ_ETERNAL			(-1)
#define CPUFREQ_NAME_LEN		16

#define CPUFREQ_NAME_PLEN		(CPUFREQ_NAME_LEN + 1)

struct cpufreq_governor;

enum cpufreq_table_sorting {
	CPUFREQ_TABLE_UNSORTED,
	CPUFREQ_TABLE_SORTED_ASCENDING,
	CPUFREQ_TABLE_SORTED_DESCENDING
};

struct cpufreq_cpuinfo {
	unsigned int		max_freq;
	unsigned int		min_freq;

	
	unsigned int		transition_latency;
};

struct cpufreq_policy {
	
	cpumask_var_t		cpus;	
	cpumask_var_t		related_cpus; 
	cpumask_var_t		real_cpus; 

	unsigned int		shared_type; 
	unsigned int		cpu;    

	struct clk		*clk;
	struct cpufreq_cpuinfo	cpuinfo;

	unsigned int		min;    
	unsigned int		max;    
	unsigned int		cur;    
	unsigned int		suspend_freq; 

	unsigned int		policy; 
	unsigned int		last_policy; 
	struct cpufreq_governor	*governor; 
	void			*governor_data;
	char			last_governor[CPUFREQ_NAME_LEN]; 

	struct work_struct	update; 

	struct freq_constraints	constraints;
	struct freq_qos_request	*min_freq_req;
	struct freq_qos_request	*max_freq_req;

	struct cpufreq_frequency_table	*freq_table;
	enum cpufreq_table_sorting freq_table_sorted;

	struct list_head        policy_list;
	struct kobject		kobj;
	struct completion	kobj_unregister;

	
	struct rw_semaphore	rwsem;

	
	bool			fast_switch_possible;
	bool			fast_switch_enabled;

	
	bool			strict_target;

	
	bool			efficiencies_available;

	
	unsigned int		transition_delay_us;

	
	bool			dvfs_possible_from_any_cpu;

	
	bool			boost_enabled;

	 
	unsigned int cached_target_freq;
	unsigned int cached_resolved_idx;

	
	bool			transition_ongoing; 
	spinlock_t		transition_lock;
	wait_queue_head_t	transition_wait;
	struct task_struct	*transition_task; 

	
	struct cpufreq_stats	*stats;

	
	void			*driver_data;

	
	struct thermal_cooling_device *cdev;

	struct notifier_block nb_min;
	struct notifier_block nb_max;
};


struct cpufreq_policy_data {
	struct cpufreq_cpuinfo		cpuinfo;
	struct cpufreq_frequency_table	*freq_table;
	unsigned int			cpu;
	unsigned int			min;    
	unsigned int			max;    
};

struct cpufreq_freqs {
	struct cpufreq_policy *policy;
	unsigned int old;
	unsigned int new;
	u8 flags;		
};


#define CPUFREQ_SHARED_TYPE_NONE (0) 
#define CPUFREQ_SHARED_TYPE_HW	 (1) 
#define CPUFREQ_SHARED_TYPE_ALL	 (2) 
#define CPUFREQ_SHARED_TYPE_ANY	 (3) 

#ifdef CONFIG_CPU_FREQ
struct cpufreq_policy *cpufreq_cpu_get_raw(unsigned int cpu);
struct cpufreq_policy *cpufreq_cpu_get(unsigned int cpu);
void cpufreq_cpu_put(struct cpufreq_policy *policy);
#else
static inline struct cpufreq_policy *cpufreq_cpu_get_raw(unsigned int cpu)
{
	return NULL;
}
static inline struct cpufreq_policy *cpufreq_cpu_get(unsigned int cpu)
{
	return NULL;
}
static inline void cpufreq_cpu_put(struct cpufreq_policy *policy) { }
#endif

static inline bool policy_is_inactive(struct cpufreq_policy *policy)
{
	return cpumask_empty(policy->cpus);
}

static inline bool policy_is_shared(struct cpufreq_policy *policy)
{
	return cpumask_weight(policy->cpus) > 1;
}

#ifdef CONFIG_CPU_FREQ
unsigned int cpufreq_get(unsigned int cpu);
unsigned int cpufreq_quick_get(unsigned int cpu);
unsigned int cpufreq_quick_get_max(unsigned int cpu);
unsigned int cpufreq_get_hw_max_freq(unsigned int cpu);
void disable_cpufreq(void);

u64 get_cpu_idle_time(unsigned int cpu, u64 *wall, int io_busy);

struct cpufreq_policy *cpufreq_cpu_acquire(unsigned int cpu);
void cpufreq_cpu_release(struct cpufreq_policy *policy);
int cpufreq_get_policy(struct cpufreq_policy *policy, unsigned int cpu);
void refresh_frequency_limits(struct cpufreq_policy *policy);
void cpufreq_update_policy(unsigned int cpu);
void cpufreq_update_limits(unsigned int cpu);
bool have_governor_per_policy(void);
bool cpufreq_supports_freq_invariance(void);
struct kobject *get_governor_parent_kobj(struct cpufreq_policy *policy);
void cpufreq_enable_fast_switch(struct cpufreq_policy *policy);
void cpufreq_disable_fast_switch(struct cpufreq_policy *policy);
bool has_target_index(void);
#else
static inline unsigned int cpufreq_get(unsigned int cpu)
{
	return 0;
}
static inline unsigned int cpufreq_quick_get(unsigned int cpu)
{
	return 0;
}
static inline unsigned int cpufreq_quick_get_max(unsigned int cpu)
{
	return 0;
}
static inline unsigned int cpufreq_get_hw_max_freq(unsigned int cpu)
{
	return 0;
}
static inline bool cpufreq_supports_freq_invariance(void)
{
	return false;
}
static inline void disable_cpufreq(void) { }
#endif

#ifdef CONFIG_CPU_FREQ_STAT
void cpufreq_stats_create_table(struct cpufreq_policy *policy);
void cpufreq_stats_free_table(struct cpufreq_policy *policy);
void cpufreq_stats_record_transition(struct cpufreq_policy *policy,
				     unsigned int new_freq);
#else
static inline void cpufreq_stats_create_table(struct cpufreq_policy *policy) { }
static inline void cpufreq_stats_free_table(struct cpufreq_policy *policy) { }
static inline void cpufreq_stats_record_transition(struct cpufreq_policy *policy,
						   unsigned int new_freq) { }
#endif 



#define CPUFREQ_RELATION_L 0  
#define CPUFREQ_RELATION_H 1  
#define CPUFREQ_RELATION_C 2  

#define CPUFREQ_RELATION_E BIT(2) 

#define CPUFREQ_RELATION_LE (CPUFREQ_RELATION_L | CPUFREQ_RELATION_E)
#define CPUFREQ_RELATION_HE (CPUFREQ_RELATION_H | CPUFREQ_RELATION_E)
#define CPUFREQ_RELATION_CE (CPUFREQ_RELATION_C | CPUFREQ_RELATION_E)

struct freq_attr {
	struct attribute attr;
	ssize_t (*show)(struct cpufreq_policy *, char *);
	ssize_t (*store)(struct cpufreq_policy *, const char *, size_t count);
};

#define cpufreq_freq_attr_ro(_name)		\
static struct freq_attr _name =			\
__ATTR(_name, 0444, show_##_name, NULL)

#define cpufreq_freq_attr_ro_perm(_name, _perm)	\
static struct freq_attr _name =			\
__ATTR(_name, _perm, show_##_name, NULL)

#define cpufreq_freq_attr_rw(_name)		\
static struct freq_attr _name =			\
__ATTR(_name, 0644, show_##_name, store_##_name)

#define cpufreq_freq_attr_wo(_name)		\
static struct freq_attr _name =			\
__ATTR(_name, 0200, NULL, store_##_name)

#define define_one_global_ro(_name)		\
static struct kobj_attribute _name =		\
__ATTR(_name, 0444, show_##_name, NULL)

#define define_one_global_rw(_name)		\
static struct kobj_attribute _name =		\
__ATTR(_name, 0644, show_##_name, store_##_name)


struct cpufreq_driver {
	char		name[CPUFREQ_NAME_LEN];
	u16		flags;
	void		*driver_data;

	
	int		(*init)(struct cpufreq_policy *policy);
	int		(*verify)(struct cpufreq_policy_data *policy);

	
	int		(*setpolicy)(struct cpufreq_policy *policy);

	int		(*target)(struct cpufreq_policy *policy,
				  unsigned int target_freq,
				  unsigned int relation);	
	int		(*target_index)(struct cpufreq_policy *policy,
					unsigned int index);
	unsigned int	(*fast_switch)(struct cpufreq_policy *policy,
				       unsigned int target_freq);
	
	void		(*adjust_perf)(unsigned int cpu,
				       unsigned long min_perf,
				       unsigned long target_perf,
				       unsigned long capacity);

	
	unsigned int	(*get_intermediate)(struct cpufreq_policy *policy,
					    unsigned int index);
	int		(*target_intermediate)(struct cpufreq_policy *policy,
					       unsigned int index);

	
	unsigned int	(*get)(unsigned int cpu);

	
	void		(*update_limits)(unsigned int cpu);

	
	int		(*bios_limit)(int cpu, unsigned int *limit);

	int		(*online)(struct cpufreq_policy *policy);
	int		(*offline)(struct cpufreq_policy *policy);
	int		(*exit)(struct cpufreq_policy *policy);
	int		(*suspend)(struct cpufreq_policy *policy);
	int		(*resume)(struct cpufreq_policy *policy);

	
	void		(*ready)(struct cpufreq_policy *policy);

	struct freq_attr **attr;

	
	bool		boost_enabled;
	int		(*set_boost)(struct cpufreq_policy *policy, int state);

	
	void		(*register_em)(struct cpufreq_policy *policy);
};




#define CPUFREQ_NEED_UPDATE_LIMITS		BIT(0)


#define CPUFREQ_CONST_LOOPS			BIT(1)


#define CPUFREQ_IS_COOLING_DEV			BIT(2)


#define CPUFREQ_HAVE_GOVERNOR_PER_POLICY	BIT(3)


#define CPUFREQ_ASYNC_NOTIFICATION		BIT(4)


#define CPUFREQ_NEED_INITIAL_FREQ_CHECK	BIT(5)


#define CPUFREQ_NO_AUTO_DYNAMIC_SWITCHING	BIT(6)

int cpufreq_register_driver(struct cpufreq_driver *driver_data);
void cpufreq_unregister_driver(struct cpufreq_driver *driver_data);

bool cpufreq_driver_test_flags(u16 flags);
const char *cpufreq_get_current_driver(void);
void *cpufreq_get_driver_data(void);

static inline int cpufreq_thermal_control_enabled(struct cpufreq_driver *drv)
{
	return IS_ENABLED(CONFIG_CPU_THERMAL) &&
		(drv->flags & CPUFREQ_IS_COOLING_DEV);
}

static inline void cpufreq_verify_within_limits(struct cpufreq_policy_data *policy,
						unsigned int min,
						unsigned int max)
{
	policy->max = clamp(policy->max, min, max);
	policy->min = clamp(policy->min, min, policy->max);
}

static inline void
cpufreq_verify_within_cpu_limits(struct cpufreq_policy_data *policy)
{
	cpufreq_verify_within_limits(policy, policy->cpuinfo.min_freq,
				     policy->cpuinfo.max_freq);
}

#ifdef CONFIG_CPU_FREQ
void cpufreq_suspend(void);
void cpufreq_resume(void);
int cpufreq_generic_suspend(struct cpufreq_policy *policy);
#else
static inline void cpufreq_suspend(void) {}
static inline void cpufreq_resume(void) {}
#endif



#define CPUFREQ_TRANSITION_NOTIFIER	(0)
#define CPUFREQ_POLICY_NOTIFIER		(1)


#define CPUFREQ_PRECHANGE		(0)
#define CPUFREQ_POSTCHANGE		(1)


#define CPUFREQ_CREATE_POLICY		(0)
#define CPUFREQ_REMOVE_POLICY		(1)

#ifdef CONFIG_CPU_FREQ
int cpufreq_register_notifier(struct notifier_block *nb, unsigned int list);
int cpufreq_unregister_notifier(struct notifier_block *nb, unsigned int list);

void cpufreq_freq_transition_begin(struct cpufreq_policy *policy,
		struct cpufreq_freqs *freqs);
void cpufreq_freq_transition_end(struct cpufreq_policy *policy,
		struct cpufreq_freqs *freqs, int transition_failed);

#else 
static inline int cpufreq_register_notifier(struct notifier_block *nb,
						unsigned int list)
{
	return 0;
}
static inline int cpufreq_unregister_notifier(struct notifier_block *nb,
						unsigned int list)
{
	return 0;
}
#endif 


static inline unsigned long cpufreq_scale(unsigned long old, u_int div,
		u_int mult)
{
#if BITS_PER_LONG == 32
	u64 result = ((u64) old) * ((u64) mult);
	do_div(result, div);
	return (unsigned long) result;

#elif BITS_PER_LONG == 64
	unsigned long result = old * ((u64) mult);
	result /= div;
	return result;
#endif
}



#define CPUFREQ_POLICY_UNKNOWN		(0)

#define CPUFREQ_POLICY_POWERSAVE	(1)
#define CPUFREQ_POLICY_PERFORMANCE	(2)


#define LATENCY_MULTIPLIER		(1000)

struct cpufreq_governor {
	char	name[CPUFREQ_NAME_LEN];
	int	(*init)(struct cpufreq_policy *policy);
	void	(*exit)(struct cpufreq_policy *policy);
	int	(*start)(struct cpufreq_policy *policy);
	void	(*stop)(struct cpufreq_policy *policy);
	void	(*limits)(struct cpufreq_policy *policy);
	ssize_t	(*show_setspeed)	(struct cpufreq_policy *policy,
					 char *buf);
	int	(*store_setspeed)	(struct cpufreq_policy *policy,
					 unsigned int freq);
	struct list_head	governor_list;
	struct module		*owner;
	u8			flags;
};




#define CPUFREQ_GOV_DYNAMIC_SWITCHING	BIT(0)


#define CPUFREQ_GOV_STRICT_TARGET	BIT(1)



unsigned int cpufreq_driver_fast_switch(struct cpufreq_policy *policy,
					unsigned int target_freq);
void cpufreq_driver_adjust_perf(unsigned int cpu,
				unsigned long min_perf,
				unsigned long target_perf,
				unsigned long capacity);
bool cpufreq_driver_has_adjust_perf(void);
int cpufreq_driver_target(struct cpufreq_policy *policy,
				 unsigned int target_freq,
				 unsigned int relation);
int __cpufreq_driver_target(struct cpufreq_policy *policy,
				   unsigned int target_freq,
				   unsigned int relation);
unsigned int cpufreq_driver_resolve_freq(struct cpufreq_policy *policy,
					 unsigned int target_freq);
unsigned int cpufreq_policy_transition_delay_us(struct cpufreq_policy *policy);
int cpufreq_register_governor(struct cpufreq_governor *governor);
void cpufreq_unregister_governor(struct cpufreq_governor *governor);
int cpufreq_start_governor(struct cpufreq_policy *policy);
void cpufreq_stop_governor(struct cpufreq_policy *policy);

#define cpufreq_governor_init(__governor)			\
static int __init __governor##_init(void)			\
{								\
	return cpufreq_register_governor(&__governor);	\
}								\
core_initcall(__governor##_init)

#define cpufreq_governor_exit(__governor)			\
static void __exit __governor##_exit(void)			\
{								\
	return cpufreq_unregister_governor(&__governor);	\
}								\
module_exit(__governor##_exit)

struct cpufreq_governor *cpufreq_default_governor(void);
struct cpufreq_governor *cpufreq_fallback_governor(void);

static inline void cpufreq_policy_apply_limits(struct cpufreq_policy *policy)
{
	if (policy->max < policy->cur)
		__cpufreq_driver_target(policy, policy->max,
					CPUFREQ_RELATION_HE);
	else if (policy->min > policy->cur)
		__cpufreq_driver_target(policy, policy->min,
					CPUFREQ_RELATION_LE);
}


struct gov_attr_set {
	struct kobject kobj;
	struct list_head policy_list;
	struct mutex update_lock;
	int usage_count;
};


extern const struct sysfs_ops governor_sysfs_ops;

static inline struct gov_attr_set *to_gov_attr_set(struct kobject *kobj)
{
	return container_of(kobj, struct gov_attr_set, kobj);
}

void gov_attr_set_init(struct gov_attr_set *attr_set, struct list_head *list_node);
void gov_attr_set_get(struct gov_attr_set *attr_set, struct list_head *list_node);
unsigned int gov_attr_set_put(struct gov_attr_set *attr_set, struct list_head *list_node);


struct governor_attr {
	struct attribute attr;
	ssize_t (*show)(struct gov_attr_set *attr_set, char *buf);
	ssize_t (*store)(struct gov_attr_set *attr_set, const char *buf,
			 size_t count);
};




#define CPUFREQ_ENTRY_INVALID		~0u
#define CPUFREQ_TABLE_END		~1u

#define CPUFREQ_BOOST_FREQ		(1 << 0)
#define CPUFREQ_INEFFICIENT_FREQ	(1 << 1)

struct cpufreq_frequency_table {
	unsigned int	flags;
	unsigned int	driver_data; 
	unsigned int	frequency; 
};

#if defined(CONFIG_CPU_FREQ) && defined(CONFIG_PM_OPP)
int dev_pm_opp_init_cpufreq_table(struct device *dev,
				  struct cpufreq_frequency_table **table);
void dev_pm_opp_free_cpufreq_table(struct device *dev,
				   struct cpufreq_frequency_table **table);
#else
static inline int dev_pm_opp_init_cpufreq_table(struct device *dev,
						struct cpufreq_frequency_table
						**table)
{
	return -EINVAL;
}

static inline void dev_pm_opp_free_cpufreq_table(struct device *dev,
						 struct cpufreq_frequency_table
						 **table)
{
}
#endif



#define cpufreq_for_each_entry(pos, table)	\
	for (pos = table; pos->frequency != CPUFREQ_TABLE_END; pos++)



#define cpufreq_for_each_entry_idx(pos, table, idx)	\
	for (pos = table, idx = 0; pos->frequency != CPUFREQ_TABLE_END; \
		pos++, idx++)



#define cpufreq_for_each_valid_entry(pos, table)			\
	for (pos = table; pos->frequency != CPUFREQ_TABLE_END; pos++)	\
		if (pos->frequency == CPUFREQ_ENTRY_INVALID)		\
			continue;					\
		else



#define cpufreq_for_each_valid_entry_idx(pos, table, idx)		\
	cpufreq_for_each_entry_idx(pos, table, idx)			\
		if (pos->frequency == CPUFREQ_ENTRY_INVALID)		\
			continue;					\
		else



#define cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies)	\
	cpufreq_for_each_valid_entry_idx(pos, table, idx)			\
		if (efficiencies && (pos->flags & CPUFREQ_INEFFICIENT_FREQ))	\
			continue;						\
		else


int cpufreq_frequency_table_cpuinfo(struct cpufreq_policy *policy,
				    struct cpufreq_frequency_table *table);

int cpufreq_frequency_table_verify(struct cpufreq_policy_data *policy,
				   struct cpufreq_frequency_table *table);
int cpufreq_generic_frequency_table_verify(struct cpufreq_policy_data *policy);

int cpufreq_table_index_unsorted(struct cpufreq_policy *policy,
				 unsigned int target_freq,
				 unsigned int relation);
int cpufreq_frequency_table_get_index(struct cpufreq_policy *policy,
		unsigned int freq);

ssize_t cpufreq_show_cpus(const struct cpumask *mask, char *buf);

#ifdef CONFIG_CPU_FREQ
int cpufreq_boost_trigger_state(int state);
int cpufreq_boost_enabled(void);
int cpufreq_enable_boost_support(void);
bool policy_has_boost_freq(struct cpufreq_policy *policy);


static inline int cpufreq_table_find_index_al(struct cpufreq_policy *policy,
					      unsigned int target_freq,
					      bool efficiencies)
{
	struct cpufreq_frequency_table *table = policy->freq_table;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	int idx, best = -1;

	cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies) {
		freq = pos->frequency;

		if (freq >= target_freq)
			return idx;

		best = idx;
	}

	return best;
}


static inline int cpufreq_table_find_index_dl(struct cpufreq_policy *policy,
					      unsigned int target_freq,
					      bool efficiencies)
{
	struct cpufreq_frequency_table *table = policy->freq_table;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	int idx, best = -1;

	cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies) {
		freq = pos->frequency;

		if (freq == target_freq)
			return idx;

		if (freq > target_freq) {
			best = idx;
			continue;
		}

		
		if (best == -1)
			return idx;

		return best;
	}

	return best;
}


static inline int cpufreq_table_find_index_l(struct cpufreq_policy *policy,
					     unsigned int target_freq,
					     bool efficiencies)
{
	target_freq = clamp_val(target_freq, policy->min, policy->max);

	if (policy->freq_table_sorted == CPUFREQ_TABLE_SORTED_ASCENDING)
		return cpufreq_table_find_index_al(policy, target_freq,
						   efficiencies);
	else
		return cpufreq_table_find_index_dl(policy, target_freq,
						   efficiencies);
}


static inline int cpufreq_table_find_index_ah(struct cpufreq_policy *policy,
					      unsigned int target_freq,
					      bool efficiencies)
{
	struct cpufreq_frequency_table *table = policy->freq_table;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	int idx, best = -1;

	cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies) {
		freq = pos->frequency;

		if (freq == target_freq)
			return idx;

		if (freq < target_freq) {
			best = idx;
			continue;
		}

		
		if (best == -1)
			return idx;

		return best;
	}

	return best;
}


static inline int cpufreq_table_find_index_dh(struct cpufreq_policy *policy,
					      unsigned int target_freq,
					      bool efficiencies)
{
	struct cpufreq_frequency_table *table = policy->freq_table;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	int idx, best = -1;

	cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies) {
		freq = pos->frequency;

		if (freq <= target_freq)
			return idx;

		best = idx;
	}

	return best;
}


static inline int cpufreq_table_find_index_h(struct cpufreq_policy *policy,
					     unsigned int target_freq,
					     bool efficiencies)
{
	target_freq = clamp_val(target_freq, policy->min, policy->max);

	if (policy->freq_table_sorted == CPUFREQ_TABLE_SORTED_ASCENDING)
		return cpufreq_table_find_index_ah(policy, target_freq,
						   efficiencies);
	else
		return cpufreq_table_find_index_dh(policy, target_freq,
						   efficiencies);
}


static inline int cpufreq_table_find_index_ac(struct cpufreq_policy *policy,
					      unsigned int target_freq,
					      bool efficiencies)
{
	struct cpufreq_frequency_table *table = policy->freq_table;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	int idx, best = -1;

	cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies) {
		freq = pos->frequency;

		if (freq == target_freq)
			return idx;

		if (freq < target_freq) {
			best = idx;
			continue;
		}

		
		if (best == -1)
			return idx;

		
		if (target_freq - table[best].frequency > freq - target_freq)
			return idx;

		return best;
	}

	return best;
}


static inline int cpufreq_table_find_index_dc(struct cpufreq_policy *policy,
					      unsigned int target_freq,
					      bool efficiencies)
{
	struct cpufreq_frequency_table *table = policy->freq_table;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	int idx, best = -1;

	cpufreq_for_each_efficient_entry_idx(pos, table, idx, efficiencies) {
		freq = pos->frequency;

		if (freq == target_freq)
			return idx;

		if (freq > target_freq) {
			best = idx;
			continue;
		}

		
		if (best == -1)
			return idx;

		
		if (table[best].frequency - target_freq > target_freq - freq)
			return idx;

		return best;
	}

	return best;
}


static inline int cpufreq_table_find_index_c(struct cpufreq_policy *policy,
					     unsigned int target_freq,
					     bool efficiencies)
{
	target_freq = clamp_val(target_freq, policy->min, policy->max);

	if (policy->freq_table_sorted == CPUFREQ_TABLE_SORTED_ASCENDING)
		return cpufreq_table_find_index_ac(policy, target_freq,
						   efficiencies);
	else
		return cpufreq_table_find_index_dc(policy, target_freq,
						   efficiencies);
}

static inline bool cpufreq_is_in_limits(struct cpufreq_policy *policy, int idx)
{
	unsigned int freq;

	if (idx < 0)
		return false;

	freq = policy->freq_table[idx].frequency;

	return freq == clamp_val(freq, policy->min, policy->max);
}

static inline int cpufreq_frequency_table_target(struct cpufreq_policy *policy,
						 unsigned int target_freq,
						 unsigned int relation)
{
	bool efficiencies = policy->efficiencies_available &&
			    (relation & CPUFREQ_RELATION_E);
	int idx;

	
	relation &= ~CPUFREQ_RELATION_E;

	if (unlikely(policy->freq_table_sorted == CPUFREQ_TABLE_UNSORTED))
		return cpufreq_table_index_unsorted(policy, target_freq,
						    relation);
retry:
	switch (relation) {
	case CPUFREQ_RELATION_L:
		idx = cpufreq_table_find_index_l(policy, target_freq,
						 efficiencies);
		break;
	case CPUFREQ_RELATION_H:
		idx = cpufreq_table_find_index_h(policy, target_freq,
						 efficiencies);
		break;
	case CPUFREQ_RELATION_C:
		idx = cpufreq_table_find_index_c(policy, target_freq,
						 efficiencies);
		break;
	default:
		WARN_ON_ONCE(1);
		return 0;
	}

	
	if (!cpufreq_is_in_limits(policy, idx) && efficiencies) {
		efficiencies = false;
		goto retry;
	}

	return idx;
}

static inline int cpufreq_table_count_valid_entries(const struct cpufreq_policy *policy)
{
	struct cpufreq_frequency_table *pos;
	int count = 0;

	if (unlikely(!policy->freq_table))
		return 0;

	cpufreq_for_each_valid_entry(pos, policy->freq_table)
		count++;

	return count;
}



static inline int
cpufreq_table_set_inefficient(struct cpufreq_policy *policy,
			      unsigned int frequency)
{
	struct cpufreq_frequency_table *pos;

	
	if (policy->freq_table_sorted == CPUFREQ_TABLE_UNSORTED)
		return -EINVAL;

	cpufreq_for_each_valid_entry(pos, policy->freq_table) {
		if (pos->frequency == frequency) {
			pos->flags |= CPUFREQ_INEFFICIENT_FREQ;
			policy->efficiencies_available = true;
			return 0;
		}
	}

	return -EINVAL;
}

static inline int parse_perf_domain(int cpu, const char *list_name,
				    const char *cell_name,
				    struct of_phandle_args *args)
{
	struct device_node *cpu_np;
	int ret;

	cpu_np = of_cpu_device_node_get(cpu);
	if (!cpu_np)
		return -ENODEV;

	ret = of_parse_phandle_with_args(cpu_np, list_name, cell_name, 0,
					 args);
	if (ret < 0)
		return ret;

	of_node_put(cpu_np);

	return 0;
}

static inline int of_perf_domain_get_sharing_cpumask(int pcpu, const char *list_name,
						     const char *cell_name, struct cpumask *cpumask,
						     struct of_phandle_args *pargs)
{
	int cpu, ret;
	struct of_phandle_args args;

	ret = parse_perf_domain(pcpu, list_name, cell_name, pargs);
	if (ret < 0)
		return ret;

	cpumask_set_cpu(pcpu, cpumask);

	for_each_possible_cpu(cpu) {
		if (cpu == pcpu)
			continue;

		ret = parse_perf_domain(cpu, list_name, cell_name, &args);
		if (ret < 0)
			continue;

		if (pargs->np == args.np && pargs->args_count == args.args_count &&
		    !memcmp(pargs->args, args.args, sizeof(args.args[0]) * args.args_count))
			cpumask_set_cpu(cpu, cpumask);

		of_node_put(args.np);
	}

	return 0;
}
#else
static inline int cpufreq_boost_trigger_state(int state)
{
	return 0;
}
static inline int cpufreq_boost_enabled(void)
{
	return 0;
}

static inline int cpufreq_enable_boost_support(void)
{
	return -EINVAL;
}

static inline bool policy_has_boost_freq(struct cpufreq_policy *policy)
{
	return false;
}

static inline int
cpufreq_table_set_inefficient(struct cpufreq_policy *policy,
			      unsigned int frequency)
{
	return -EINVAL;
}

static inline int of_perf_domain_get_sharing_cpumask(int pcpu, const char *list_name,
						     const char *cell_name, struct cpumask *cpumask,
						     struct of_phandle_args *pargs)
{
	return -EOPNOTSUPP;
}
#endif

#if defined(CONFIG_ENERGY_MODEL) && defined(CONFIG_CPU_FREQ_GOV_SCHEDUTIL)
void sched_cpufreq_governor_change(struct cpufreq_policy *policy,
			struct cpufreq_governor *old_gov);
#else
static inline void sched_cpufreq_governor_change(struct cpufreq_policy *policy,
			struct cpufreq_governor *old_gov) { }
#endif

extern unsigned int arch_freq_get_on_cpu(int cpu);

#ifndef arch_set_freq_scale
static __always_inline
void arch_set_freq_scale(const struct cpumask *cpus,
			 unsigned long cur_freq,
			 unsigned long max_freq)
{
}
#endif

extern struct freq_attr cpufreq_freq_attr_scaling_available_freqs;
extern struct freq_attr cpufreq_freq_attr_scaling_boost_freqs;
extern struct freq_attr *cpufreq_generic_attr[];
int cpufreq_table_validate_and_sort(struct cpufreq_policy *policy);

unsigned int cpufreq_generic_get(unsigned int cpu);
void cpufreq_generic_init(struct cpufreq_policy *policy,
		struct cpufreq_frequency_table *table,
		unsigned int transition_latency);

static inline void cpufreq_register_em_with_opp(struct cpufreq_policy *policy)
{
	dev_pm_opp_of_register_em(get_cpu_device(policy->cpu),
				  policy->related_cpus);
}
#endif 
