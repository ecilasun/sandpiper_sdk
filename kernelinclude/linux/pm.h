/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_PM_H
#define _LINUX_PM_H

#include <linux/export.h>
#include <linux/list.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/completion.h>


extern void (*pm_power_off)(void);

struct device; 
#ifdef CONFIG_VT_CONSOLE_SLEEP
extern void pm_vt_switch_required(struct device *dev, bool required);
extern void pm_vt_switch_unregister(struct device *dev);
#else
static inline void pm_vt_switch_required(struct device *dev, bool required)
{
}
static inline void pm_vt_switch_unregister(struct device *dev)
{
}
#endif 

#ifdef CONFIG_CXL_SUSPEND
bool cxl_mem_active(void);
#else
static inline bool cxl_mem_active(void)
{
	return false;
}
#endif




#ifdef CONFIG_PM
extern const char power_group_name[];		
#else
#define power_group_name	NULL
#endif

typedef struct pm_message {
	int event;
} pm_message_t;


struct dev_pm_ops {
	int (*prepare)(struct device *dev);
	void (*complete)(struct device *dev);
	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);
	int (*freeze)(struct device *dev);
	int (*thaw)(struct device *dev);
	int (*poweroff)(struct device *dev);
	int (*restore)(struct device *dev);
	int (*suspend_late)(struct device *dev);
	int (*resume_early)(struct device *dev);
	int (*freeze_late)(struct device *dev);
	int (*thaw_early)(struct device *dev);
	int (*poweroff_late)(struct device *dev);
	int (*restore_early)(struct device *dev);
	int (*suspend_noirq)(struct device *dev);
	int (*resume_noirq)(struct device *dev);
	int (*freeze_noirq)(struct device *dev);
	int (*thaw_noirq)(struct device *dev);
	int (*poweroff_noirq)(struct device *dev);
	int (*restore_noirq)(struct device *dev);
	int (*runtime_suspend)(struct device *dev);
	int (*runtime_resume)(struct device *dev);
	int (*runtime_idle)(struct device *dev);
};

#define SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	.suspend = pm_sleep_ptr(suspend_fn), \
	.resume = pm_sleep_ptr(resume_fn), \
	.freeze = pm_sleep_ptr(suspend_fn), \
	.thaw = pm_sleep_ptr(resume_fn), \
	.poweroff = pm_sleep_ptr(suspend_fn), \
	.restore = pm_sleep_ptr(resume_fn),

#define LATE_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	.suspend_late = pm_sleep_ptr(suspend_fn), \
	.resume_early = pm_sleep_ptr(resume_fn), \
	.freeze_late = pm_sleep_ptr(suspend_fn), \
	.thaw_early = pm_sleep_ptr(resume_fn), \
	.poweroff_late = pm_sleep_ptr(suspend_fn), \
	.restore_early = pm_sleep_ptr(resume_fn),

#define NOIRQ_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	.suspend_noirq = pm_sleep_ptr(suspend_fn), \
	.resume_noirq = pm_sleep_ptr(resume_fn), \
	.freeze_noirq = pm_sleep_ptr(suspend_fn), \
	.thaw_noirq = pm_sleep_ptr(resume_fn), \
	.poweroff_noirq = pm_sleep_ptr(suspend_fn), \
	.restore_noirq = pm_sleep_ptr(resume_fn),

#define RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn) \
	.runtime_suspend = suspend_fn, \
	.runtime_resume = resume_fn, \
	.runtime_idle = idle_fn,

#ifdef CONFIG_PM_SLEEP
#define SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#else
#define SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#endif

#ifdef CONFIG_PM_SLEEP
#define SET_LATE_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	LATE_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#else
#define SET_LATE_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#endif

#ifdef CONFIG_PM_SLEEP
#define SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	NOIRQ_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#else
#define SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn)
#endif

#ifdef CONFIG_PM
#define SET_RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn) \
	RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn)
#else
#define SET_RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn)
#endif

#define _DEFINE_DEV_PM_OPS(name, \
			   suspend_fn, resume_fn, \
			   runtime_suspend_fn, runtime_resume_fn, idle_fn) \
const struct dev_pm_ops name = { \
	SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	RUNTIME_PM_OPS(runtime_suspend_fn, runtime_resume_fn, idle_fn) \
}

#define _EXPORT_PM_OPS(name, license, ns)				\
	const struct dev_pm_ops name;					\
	__EXPORT_SYMBOL(name, license, ns);				\
	const struct dev_pm_ops name

#define _DISCARD_PM_OPS(name, license, ns)				\
	static __maybe_unused const struct dev_pm_ops __static_##name

#ifdef CONFIG_PM
#define _EXPORT_DEV_PM_OPS(name, license, ns)		_EXPORT_PM_OPS(name, license, ns)
#define EXPORT_PM_FN_GPL(name)				EXPORT_SYMBOL_GPL(name)
#define EXPORT_PM_FN_NS_GPL(name, ns)			EXPORT_SYMBOL_NS_GPL(name, ns)
#else
#define _EXPORT_DEV_PM_OPS(name, license, ns)		_DISCARD_PM_OPS(name, license, ns)
#define EXPORT_PM_FN_GPL(name)
#define EXPORT_PM_FN_NS_GPL(name, ns)
#endif

#ifdef CONFIG_PM_SLEEP
#define _EXPORT_DEV_SLEEP_PM_OPS(name, license, ns)	_EXPORT_PM_OPS(name, license, ns)
#else
#define _EXPORT_DEV_SLEEP_PM_OPS(name, license, ns)	_DISCARD_PM_OPS(name, license, ns)
#endif

#define EXPORT_DEV_PM_OPS(name)				_EXPORT_DEV_PM_OPS(name, "", "")
#define EXPORT_GPL_DEV_PM_OPS(name)			_EXPORT_DEV_PM_OPS(name, "GPL", "")
#define EXPORT_NS_DEV_PM_OPS(name, ns)			_EXPORT_DEV_PM_OPS(name, "", #ns)
#define EXPORT_NS_GPL_DEV_PM_OPS(name, ns)		_EXPORT_DEV_PM_OPS(name, "GPL", #ns)

#define EXPORT_DEV_SLEEP_PM_OPS(name)			_EXPORT_DEV_SLEEP_PM_OPS(name, "", "")
#define EXPORT_GPL_DEV_SLEEP_PM_OPS(name)		_EXPORT_DEV_SLEEP_PM_OPS(name, "GPL", "")
#define EXPORT_NS_DEV_SLEEP_PM_OPS(name, ns)		_EXPORT_DEV_SLEEP_PM_OPS(name, "", #ns)
#define EXPORT_NS_GPL_DEV_SLEEP_PM_OPS(name, ns)	_EXPORT_DEV_SLEEP_PM_OPS(name, "GPL", #ns)


#define DEFINE_SIMPLE_DEV_PM_OPS(name, suspend_fn, resume_fn) \
	_DEFINE_DEV_PM_OPS(name, suspend_fn, resume_fn, NULL, NULL, NULL)

#define EXPORT_SIMPLE_DEV_PM_OPS(name, suspend_fn, resume_fn) \
	EXPORT_DEV_SLEEP_PM_OPS(name) = { \
		SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	}
#define EXPORT_GPL_SIMPLE_DEV_PM_OPS(name, suspend_fn, resume_fn) \
	EXPORT_GPL_DEV_SLEEP_PM_OPS(name) = { \
		SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	}
#define EXPORT_NS_SIMPLE_DEV_PM_OPS(name, suspend_fn, resume_fn, ns)	\
	EXPORT_NS_DEV_SLEEP_PM_OPS(name, ns) = { \
		SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	}
#define EXPORT_NS_GPL_SIMPLE_DEV_PM_OPS(name, suspend_fn, resume_fn, ns)	\
	EXPORT_NS_GPL_DEV_SLEEP_PM_OPS(name, ns) = { \
		SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	}


#define SIMPLE_DEV_PM_OPS(name, suspend_fn, resume_fn) \
const struct dev_pm_ops __maybe_unused name = { \
	SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
}


#define UNIVERSAL_DEV_PM_OPS(name, suspend_fn, resume_fn, idle_fn) \
const struct dev_pm_ops __maybe_unused name = { \
	SET_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
	SET_RUNTIME_PM_OPS(suspend_fn, resume_fn, idle_fn) \
}


#define DEFINE_NOIRQ_DEV_PM_OPS(name, suspend_fn, resume_fn) \
const struct dev_pm_ops name = { \
	NOIRQ_SYSTEM_SLEEP_PM_OPS(suspend_fn, resume_fn) \
}

#define pm_ptr(_ptr) PTR_IF(IS_ENABLED(CONFIG_PM), (_ptr))
#define pm_sleep_ptr(_ptr) PTR_IF(IS_ENABLED(CONFIG_PM_SLEEP), (_ptr))



#define PM_EVENT_INVALID	(-1)
#define PM_EVENT_ON		0x0000
#define PM_EVENT_FREEZE		0x0001
#define PM_EVENT_SUSPEND	0x0002
#define PM_EVENT_HIBERNATE	0x0004
#define PM_EVENT_QUIESCE	0x0008
#define PM_EVENT_RESUME		0x0010
#define PM_EVENT_THAW		0x0020
#define PM_EVENT_RESTORE	0x0040
#define PM_EVENT_RECOVER	0x0080
#define PM_EVENT_USER		0x0100
#define PM_EVENT_REMOTE		0x0200
#define PM_EVENT_AUTO		0x0400

#define PM_EVENT_SLEEP		(PM_EVENT_SUSPEND | PM_EVENT_HIBERNATE)
#define PM_EVENT_USER_SUSPEND	(PM_EVENT_USER | PM_EVENT_SUSPEND)
#define PM_EVENT_USER_RESUME	(PM_EVENT_USER | PM_EVENT_RESUME)
#define PM_EVENT_REMOTE_RESUME	(PM_EVENT_REMOTE | PM_EVENT_RESUME)
#define PM_EVENT_AUTO_SUSPEND	(PM_EVENT_AUTO | PM_EVENT_SUSPEND)
#define PM_EVENT_AUTO_RESUME	(PM_EVENT_AUTO | PM_EVENT_RESUME)

#define PMSG_INVALID	((struct pm_message){ .event = PM_EVENT_INVALID, })
#define PMSG_ON		((struct pm_message){ .event = PM_EVENT_ON, })
#define PMSG_FREEZE	((struct pm_message){ .event = PM_EVENT_FREEZE, })
#define PMSG_QUIESCE	((struct pm_message){ .event = PM_EVENT_QUIESCE, })
#define PMSG_SUSPEND	((struct pm_message){ .event = PM_EVENT_SUSPEND, })
#define PMSG_HIBERNATE	((struct pm_message){ .event = PM_EVENT_HIBERNATE, })
#define PMSG_RESUME	((struct pm_message){ .event = PM_EVENT_RESUME, })
#define PMSG_THAW	((struct pm_message){ .event = PM_EVENT_THAW, })
#define PMSG_RESTORE	((struct pm_message){ .event = PM_EVENT_RESTORE, })
#define PMSG_RECOVER	((struct pm_message){ .event = PM_EVENT_RECOVER, })
#define PMSG_USER_SUSPEND	((struct pm_message) \
					{ .event = PM_EVENT_USER_SUSPEND, })
#define PMSG_USER_RESUME	((struct pm_message) \
					{ .event = PM_EVENT_USER_RESUME, })
#define PMSG_REMOTE_RESUME	((struct pm_message) \
					{ .event = PM_EVENT_REMOTE_RESUME, })
#define PMSG_AUTO_SUSPEND	((struct pm_message) \
					{ .event = PM_EVENT_AUTO_SUSPEND, })
#define PMSG_AUTO_RESUME	((struct pm_message) \
					{ .event = PM_EVENT_AUTO_RESUME, })

#define PMSG_IS_AUTO(msg)	(((msg).event & PM_EVENT_AUTO) != 0)



enum rpm_status {
	RPM_INVALID = -1,
	RPM_ACTIVE = 0,
	RPM_RESUMING,
	RPM_SUSPENDED,
	RPM_SUSPENDING,
};



enum rpm_request {
	RPM_REQ_NONE = 0,
	RPM_REQ_IDLE,
	RPM_REQ_SUSPEND,
	RPM_REQ_AUTOSUSPEND,
	RPM_REQ_RESUME,
};

struct wakeup_source;
struct wake_irq;
struct pm_domain_data;

struct pm_subsys_data {
	spinlock_t lock;
	unsigned int refcount;
#ifdef CONFIG_PM_CLK
	unsigned int clock_op_might_sleep;
	struct mutex clock_mutex;
	struct list_head clock_list;
#endif
#ifdef CONFIG_PM_GENERIC_DOMAINS
	struct pm_domain_data *domain_data;
#endif
};


#define DPM_FLAG_NO_DIRECT_COMPLETE	BIT(0)
#define DPM_FLAG_SMART_PREPARE		BIT(1)
#define DPM_FLAG_SMART_SUSPEND		BIT(2)
#define DPM_FLAG_MAY_SKIP_RESUME	BIT(3)

struct dev_pm_info {
	pm_message_t		power_state;
	unsigned int		can_wakeup:1;
	unsigned int		async_suspend:1;
	bool			in_dpm_list:1;	
	bool			is_prepared:1;	
	bool			is_suspended:1;	
	bool			is_noirq_suspended:1;
	bool			is_late_suspended:1;
	bool			no_pm:1;
	bool			early_init:1;	
	bool			direct_complete:1;	
	u32			driver_flags;
	spinlock_t		lock;
#ifdef CONFIG_PM_SLEEP
	struct list_head	entry;
	struct completion	completion;
	struct wakeup_source	*wakeup;
	bool			wakeup_path:1;
	bool			syscore:1;
	bool			no_pm_callbacks:1;	
	unsigned int		must_resume:1;	
	unsigned int		may_skip_resume:1;	
#else
	unsigned int		should_wakeup:1;
#endif
#ifdef CONFIG_PM
	struct hrtimer		suspend_timer;
	u64			timer_expires;
	struct work_struct	work;
	wait_queue_head_t	wait_queue;
	struct wake_irq		*wakeirq;
	atomic_t		usage_count;
	atomic_t		child_count;
	unsigned int		disable_depth:3;
	unsigned int		idle_notification:1;
	unsigned int		request_pending:1;
	unsigned int		deferred_resume:1;
	unsigned int		needs_force_resume:1;
	unsigned int		runtime_auto:1;
	bool			ignore_children:1;
	unsigned int		no_callbacks:1;
	unsigned int		irq_safe:1;
	unsigned int		use_autosuspend:1;
	unsigned int		timer_autosuspends:1;
	unsigned int		memalloc_noio:1;
	unsigned int		links_count;
	enum rpm_request	request;
	enum rpm_status		runtime_status;
	enum rpm_status		last_status;
	int			runtime_error;
	int			autosuspend_delay;
	u64			last_busy;
	u64			active_time;
	u64			suspended_time;
	u64			accounting_timestamp;
#endif
	struct pm_subsys_data	*subsys_data;  
	void (*set_latency_tolerance)(struct device *, s32);
	struct dev_pm_qos	*qos;
};

extern int dev_pm_get_subsys_data(struct device *dev);
extern void dev_pm_put_subsys_data(struct device *dev);


struct dev_pm_domain {
	struct dev_pm_ops	ops;
	int (*start)(struct device *dev);
	void (*detach)(struct device *dev, bool power_off);
	int (*activate)(struct device *dev);
	void (*sync)(struct device *dev);
	void (*dismiss)(struct device *dev);
};




#define PM_EVENT_PRETHAW PM_EVENT_QUIESCE



#ifdef CONFIG_PM_SLEEP
extern void device_pm_lock(void);
extern void dpm_resume_start(pm_message_t state);
extern void dpm_resume_end(pm_message_t state);
extern void dpm_resume_noirq(pm_message_t state);
extern void dpm_resume_early(pm_message_t state);
extern void dpm_resume(pm_message_t state);
extern void dpm_complete(pm_message_t state);

extern void device_pm_unlock(void);
extern int dpm_suspend_end(pm_message_t state);
extern int dpm_suspend_start(pm_message_t state);
extern int dpm_suspend_noirq(pm_message_t state);
extern int dpm_suspend_late(pm_message_t state);
extern int dpm_suspend(pm_message_t state);
extern int dpm_prepare(pm_message_t state);

extern void __suspend_report_result(const char *function, struct device *dev, void *fn, int ret);

#define suspend_report_result(dev, fn, ret)				\
	do {								\
		__suspend_report_result(__func__, dev, fn, ret);	\
	} while (0)

extern int device_pm_wait_for_dev(struct device *sub, struct device *dev);
extern void dpm_for_each_dev(void *data, void (*fn)(struct device *, void *));

extern int pm_generic_prepare(struct device *dev);
extern int pm_generic_suspend_late(struct device *dev);
extern int pm_generic_suspend_noirq(struct device *dev);
extern int pm_generic_suspend(struct device *dev);
extern int pm_generic_resume_early(struct device *dev);
extern int pm_generic_resume_noirq(struct device *dev);
extern int pm_generic_resume(struct device *dev);
extern int pm_generic_freeze_noirq(struct device *dev);
extern int pm_generic_freeze_late(struct device *dev);
extern int pm_generic_freeze(struct device *dev);
extern int pm_generic_thaw_noirq(struct device *dev);
extern int pm_generic_thaw_early(struct device *dev);
extern int pm_generic_thaw(struct device *dev);
extern int pm_generic_restore_noirq(struct device *dev);
extern int pm_generic_restore_early(struct device *dev);
extern int pm_generic_restore(struct device *dev);
extern int pm_generic_poweroff_noirq(struct device *dev);
extern int pm_generic_poweroff_late(struct device *dev);
extern int pm_generic_poweroff(struct device *dev);
extern void pm_generic_complete(struct device *dev);

extern bool dev_pm_skip_resume(struct device *dev);
extern bool dev_pm_skip_suspend(struct device *dev);

#else 

#define device_pm_lock() do {} while (0)
#define device_pm_unlock() do {} while (0)

static inline int dpm_suspend_start(pm_message_t state)
{
	return 0;
}

#define suspend_report_result(dev, fn, ret)	do {} while (0)

static inline int device_pm_wait_for_dev(struct device *a, struct device *b)
{
	return 0;
}

static inline void dpm_for_each_dev(void *data, void (*fn)(struct device *, void *))
{
}

#define pm_generic_prepare		NULL
#define pm_generic_suspend_late		NULL
#define pm_generic_suspend_noirq	NULL
#define pm_generic_suspend		NULL
#define pm_generic_resume_early		NULL
#define pm_generic_resume_noirq		NULL
#define pm_generic_resume		NULL
#define pm_generic_freeze_noirq		NULL
#define pm_generic_freeze_late		NULL
#define pm_generic_freeze		NULL
#define pm_generic_thaw_noirq		NULL
#define pm_generic_thaw_early		NULL
#define pm_generic_thaw			NULL
#define pm_generic_restore_noirq	NULL
#define pm_generic_restore_early	NULL
#define pm_generic_restore		NULL
#define pm_generic_poweroff_noirq	NULL
#define pm_generic_poweroff_late	NULL
#define pm_generic_poweroff		NULL
#define pm_generic_complete		NULL
#endif 


enum dpm_order {
	DPM_ORDER_NONE,
	DPM_ORDER_DEV_AFTER_PARENT,
	DPM_ORDER_PARENT_BEFORE_DEV,
	DPM_ORDER_DEV_LAST,
};

#endif 
