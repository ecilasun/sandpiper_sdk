/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_DEVFREQ_H__
#define __LINUX_DEVFREQ_H__

#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/pm_opp.h>
#include <linux/pm_qos.h>


#define DEVFREQ_GOV_SIMPLE_ONDEMAND	"simple_ondemand"
#define DEVFREQ_GOV_PERFORMANCE		"performance"
#define DEVFREQ_GOV_POWERSAVE		"powersave"
#define DEVFREQ_GOV_USERSPACE		"userspace"
#define DEVFREQ_GOV_PASSIVE		"passive"


#define DEVFREQ_TRANSITION_NOTIFIER	(0)


#define	DEVFREQ_PRECHANGE		(0)
#define DEVFREQ_POSTCHANGE		(1)


enum devfreq_timer {
	DEVFREQ_TIMER_DEFERRABLE = 0,
	DEVFREQ_TIMER_DELAYED,
	DEVFREQ_TIMER_NUM,
};

struct devfreq;
struct devfreq_governor;
struct devfreq_cpu_data;
struct thermal_cooling_device;


struct devfreq_dev_status {
	
	unsigned long total_time;
	unsigned long busy_time;
	unsigned long current_frequency;
	void *private_data;
};


#define DEVFREQ_FLAG_LEAST_UPPER_BOUND		0x1


struct devfreq_dev_profile {
	unsigned long initial_freq;
	unsigned int polling_ms;
	enum devfreq_timer timer;

	int (*target)(struct device *dev, unsigned long *freq, u32 flags);
	int (*get_dev_status)(struct device *dev,
			      struct devfreq_dev_status *stat);
	int (*get_cur_freq)(struct device *dev, unsigned long *freq);
	void (*exit)(struct device *dev);

	unsigned long *freq_table;
	unsigned int max_state;

	bool is_cooling_device;
};


struct devfreq_stats {
	unsigned int total_trans;
	unsigned int *trans_table;
	u64 *time_in_state;
	u64 last_update;
};


struct devfreq {
	struct list_head node;

	struct mutex lock;
	struct device dev;
	struct devfreq_dev_profile *profile;
	const struct devfreq_governor *governor;
	struct opp_table *opp_table;
	struct notifier_block nb;
	struct delayed_work work;

	unsigned long *freq_table;
	unsigned int max_state;

	unsigned long previous_freq;
	struct devfreq_dev_status last_status;

	void *data;
	void *governor_data;

	struct dev_pm_qos_request user_min_freq_req;
	struct dev_pm_qos_request user_max_freq_req;
	unsigned long scaling_min_freq;
	unsigned long scaling_max_freq;
	bool stop_polling;

	unsigned long suspend_freq;
	unsigned long resume_freq;
	atomic_t suspend_count;

	
	struct devfreq_stats stats;

	struct srcu_notifier_head transition_notifier_list;

	
	struct thermal_cooling_device *cdev;

	struct notifier_block nb_min;
	struct notifier_block nb_max;
};

struct devfreq_freqs {
	unsigned long old;
	unsigned long new;
};

#if defined(CONFIG_PM_DEVFREQ)
struct devfreq *devfreq_add_device(struct device *dev,
				struct devfreq_dev_profile *profile,
				const char *governor_name,
				void *data);
int devfreq_remove_device(struct devfreq *devfreq);
struct devfreq *devm_devfreq_add_device(struct device *dev,
				struct devfreq_dev_profile *profile,
				const char *governor_name,
				void *data);
void devm_devfreq_remove_device(struct device *dev, struct devfreq *devfreq);


int devfreq_suspend_device(struct devfreq *devfreq);
int devfreq_resume_device(struct devfreq *devfreq);

void devfreq_suspend(void);
void devfreq_resume(void);


int update_devfreq(struct devfreq *devfreq);


struct dev_pm_opp *devfreq_recommended_opp(struct device *dev,
				unsigned long *freq, u32 flags);
int devfreq_register_opp_notifier(struct device *dev,
				struct devfreq *devfreq);
int devfreq_unregister_opp_notifier(struct device *dev,
				struct devfreq *devfreq);
int devm_devfreq_register_opp_notifier(struct device *dev,
				struct devfreq *devfreq);
void devm_devfreq_unregister_opp_notifier(struct device *dev,
				struct devfreq *devfreq);
int devfreq_register_notifier(struct devfreq *devfreq,
				struct notifier_block *nb,
				unsigned int list);
int devfreq_unregister_notifier(struct devfreq *devfreq,
				struct notifier_block *nb,
				unsigned int list);
int devm_devfreq_register_notifier(struct device *dev,
				struct devfreq *devfreq,
				struct notifier_block *nb,
				unsigned int list);
void devm_devfreq_unregister_notifier(struct device *dev,
				struct devfreq *devfreq,
				struct notifier_block *nb,
				unsigned int list);
struct devfreq *devfreq_get_devfreq_by_node(struct device_node *node);
struct devfreq *devfreq_get_devfreq_by_phandle(struct device *dev,
				const char *phandle_name, int index);
#endif 


struct devfreq_simple_ondemand_data {
	unsigned int upthreshold;
	unsigned int downdifferential;
};

enum devfreq_parent_dev_type {
	DEVFREQ_PARENT_DEV,
	CPUFREQ_PARENT_DEV,
};


struct devfreq_passive_data {
	
	struct devfreq *parent;

	
	int (*get_target_freq)(struct devfreq *this, unsigned long *freq);

	
	enum devfreq_parent_dev_type parent_type;

	
	struct devfreq *this;
	struct notifier_block nb;
	struct list_head cpu_data_list;
};

#if !defined(CONFIG_PM_DEVFREQ)
static inline struct devfreq *devfreq_add_device(struct device *dev,
					struct devfreq_dev_profile *profile,
					const char *governor_name,
					void *data)
{
	return ERR_PTR(-ENOSYS);
}

static inline int devfreq_remove_device(struct devfreq *devfreq)
{
	return 0;
}

static inline struct devfreq *devm_devfreq_add_device(struct device *dev,
					struct devfreq_dev_profile *profile,
					const char *governor_name,
					void *data)
{
	return ERR_PTR(-ENOSYS);
}

static inline void devm_devfreq_remove_device(struct device *dev,
					struct devfreq *devfreq)
{
}

static inline int devfreq_suspend_device(struct devfreq *devfreq)
{
	return 0;
}

static inline int devfreq_resume_device(struct devfreq *devfreq)
{
	return 0;
}

static inline void devfreq_suspend(void) {}
static inline void devfreq_resume(void) {}

static inline struct dev_pm_opp *devfreq_recommended_opp(struct device *dev,
					unsigned long *freq, u32 flags)
{
	return ERR_PTR(-EINVAL);
}

static inline int devfreq_register_opp_notifier(struct device *dev,
					struct devfreq *devfreq)
{
	return -EINVAL;
}

static inline int devfreq_unregister_opp_notifier(struct device *dev,
					struct devfreq *devfreq)
{
	return -EINVAL;
}

static inline int devm_devfreq_register_opp_notifier(struct device *dev,
					struct devfreq *devfreq)
{
	return -EINVAL;
}

static inline void devm_devfreq_unregister_opp_notifier(struct device *dev,
					struct devfreq *devfreq)
{
}

static inline int devfreq_register_notifier(struct devfreq *devfreq,
					struct notifier_block *nb,
					unsigned int list)
{
	return 0;
}

static inline int devfreq_unregister_notifier(struct devfreq *devfreq,
					struct notifier_block *nb,
					unsigned int list)
{
	return 0;
}

static inline int devm_devfreq_register_notifier(struct device *dev,
					struct devfreq *devfreq,
					struct notifier_block *nb,
					unsigned int list)
{
	return 0;
}

static inline void devm_devfreq_unregister_notifier(struct device *dev,
					struct devfreq *devfreq,
					struct notifier_block *nb,
					unsigned int list)
{
}

static inline struct devfreq *devfreq_get_devfreq_by_node(struct device_node *node)
{
	return ERR_PTR(-ENODEV);
}

static inline struct devfreq *devfreq_get_devfreq_by_phandle(struct device *dev,
					const char *phandle_name, int index)
{
	return ERR_PTR(-ENODEV);
}

static inline int devfreq_update_stats(struct devfreq *df)
{
	return -EINVAL;
}
#endif 

#endif 
