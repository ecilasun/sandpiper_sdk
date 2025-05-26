/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SUSPEND_H
#define _LINUX_SUSPEND_H

#include <linux/swap.h>
#include <linux/notifier.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/mm.h>
#include <linux/freezer.h>
#include <asm/errno.h>

#ifdef CONFIG_VT
extern void pm_set_vt_switch(int);
#else
static inline void pm_set_vt_switch(int do_switch)
{
}
#endif

#ifdef CONFIG_VT_CONSOLE_SLEEP
extern void pm_prepare_console(void);
extern void pm_restore_console(void);
#else
static inline void pm_prepare_console(void)
{
}

static inline void pm_restore_console(void)
{
}
#endif

typedef int __bitwise suspend_state_t;

#define PM_SUSPEND_ON		((__force suspend_state_t) 0)
#define PM_SUSPEND_TO_IDLE	((__force suspend_state_t) 1)
#define PM_SUSPEND_STANDBY	((__force suspend_state_t) 2)
#define PM_SUSPEND_MEM		((__force suspend_state_t) 3)
#define PM_SUSPEND_MIN		PM_SUSPEND_TO_IDLE
#define PM_SUSPEND_MAX		((__force suspend_state_t) 4)

enum suspend_stat_step {
	SUSPEND_FREEZE = 1,
	SUSPEND_PREPARE,
	SUSPEND_SUSPEND,
	SUSPEND_SUSPEND_LATE,
	SUSPEND_SUSPEND_NOIRQ,
	SUSPEND_RESUME_NOIRQ,
	SUSPEND_RESUME_EARLY,
	SUSPEND_RESUME
};

struct suspend_stats {
	int	success;
	int	fail;
	int	failed_freeze;
	int	failed_prepare;
	int	failed_suspend;
	int	failed_suspend_late;
	int	failed_suspend_noirq;
	int	failed_resume;
	int	failed_resume_early;
	int	failed_resume_noirq;
#define	REC_FAILED_NUM	2
	int	last_failed_dev;
	char	failed_devs[REC_FAILED_NUM][40];
	int	last_failed_errno;
	int	errno[REC_FAILED_NUM];
	int	last_failed_step;
	u64	last_hw_sleep;
	u64	total_hw_sleep;
	u64	max_hw_sleep;
	enum suspend_stat_step	failed_steps[REC_FAILED_NUM];
};

extern struct suspend_stats suspend_stats;

static inline void dpm_save_failed_dev(const char *name)
{
	strscpy(suspend_stats.failed_devs[suspend_stats.last_failed_dev],
		name,
		sizeof(suspend_stats.failed_devs[0]));
	suspend_stats.last_failed_dev++;
	suspend_stats.last_failed_dev %= REC_FAILED_NUM;
}

static inline void dpm_save_failed_errno(int err)
{
	suspend_stats.errno[suspend_stats.last_failed_errno] = err;
	suspend_stats.last_failed_errno++;
	suspend_stats.last_failed_errno %= REC_FAILED_NUM;
}

static inline void dpm_save_failed_step(enum suspend_stat_step step)
{
	suspend_stats.failed_steps[suspend_stats.last_failed_step] = step;
	suspend_stats.last_failed_step++;
	suspend_stats.last_failed_step %= REC_FAILED_NUM;
}


struct platform_suspend_ops {
	int (*valid)(suspend_state_t state);
	int (*begin)(suspend_state_t state);
	int (*prepare)(void);
	int (*prepare_late)(void);
	int (*enter)(suspend_state_t state);
	void (*wake)(void);
	void (*finish)(void);
	bool (*suspend_again)(void);
	void (*end)(void);
	void (*recover)(void);
};

struct platform_s2idle_ops {
	int (*begin)(void);
	int (*prepare)(void);
	int (*prepare_late)(void);
	void (*check)(void);
	bool (*wake)(void);
	void (*restore_early)(void);
	void (*restore)(void);
	void (*end)(void);
};

#ifdef CONFIG_SUSPEND
extern suspend_state_t pm_suspend_target_state;
extern suspend_state_t mem_sleep_current;
extern suspend_state_t mem_sleep_default;


extern void suspend_set_ops(const struct platform_suspend_ops *ops);
extern int suspend_valid_only_mem(suspend_state_t state);

extern unsigned int pm_suspend_global_flags;

#define PM_SUSPEND_FLAG_FW_SUSPEND	BIT(0)
#define PM_SUSPEND_FLAG_FW_RESUME	BIT(1)
#define PM_SUSPEND_FLAG_NO_PLATFORM	BIT(2)

static inline void pm_suspend_clear_flags(void)
{
	pm_suspend_global_flags = 0;
}

static inline void pm_set_suspend_via_firmware(void)
{
	pm_suspend_global_flags |= PM_SUSPEND_FLAG_FW_SUSPEND;
}

static inline void pm_set_resume_via_firmware(void)
{
	pm_suspend_global_flags |= PM_SUSPEND_FLAG_FW_RESUME;
}

static inline void pm_set_suspend_no_platform(void)
{
	pm_suspend_global_flags |= PM_SUSPEND_FLAG_NO_PLATFORM;
}


static inline bool pm_suspend_via_firmware(void)
{
	return !!(pm_suspend_global_flags & PM_SUSPEND_FLAG_FW_SUSPEND);
}


static inline bool pm_resume_via_firmware(void)
{
	return !!(pm_suspend_global_flags & PM_SUSPEND_FLAG_FW_RESUME);
}


static inline bool pm_suspend_no_platform(void)
{
	return !!(pm_suspend_global_flags & PM_SUSPEND_FLAG_NO_PLATFORM);
}


enum s2idle_states {
	S2IDLE_STATE_NONE,      
	S2IDLE_STATE_ENTER,     
	S2IDLE_STATE_WAKE,      
};

extern enum s2idle_states __read_mostly s2idle_state;

static inline bool idle_should_enter_s2idle(void)
{
	return unlikely(s2idle_state == S2IDLE_STATE_ENTER);
}

extern bool pm_suspend_default_s2idle(void);
extern void __init pm_states_init(void);
extern void s2idle_set_ops(const struct platform_s2idle_ops *ops);
extern void s2idle_wake(void);


extern void arch_suspend_disable_irqs(void);


extern void arch_suspend_enable_irqs(void);

extern int pm_suspend(suspend_state_t state);
extern bool sync_on_suspend_enabled;
#else 
#define suspend_valid_only_mem	NULL

#define pm_suspend_target_state	(PM_SUSPEND_ON)

static inline void pm_suspend_clear_flags(void) {}
static inline void pm_set_suspend_via_firmware(void) {}
static inline void pm_set_resume_via_firmware(void) {}
static inline bool pm_suspend_via_firmware(void) { return false; }
static inline bool pm_resume_via_firmware(void) { return false; }
static inline bool pm_suspend_no_platform(void) { return false; }
static inline bool pm_suspend_default_s2idle(void) { return false; }

static inline void suspend_set_ops(const struct platform_suspend_ops *ops) {}
static inline int pm_suspend(suspend_state_t state) { return -ENOSYS; }
static inline bool sync_on_suspend_enabled(void) { return true; }
static inline bool idle_should_enter_s2idle(void) { return false; }
static inline void __init pm_states_init(void) {}
static inline void s2idle_set_ops(const struct platform_s2idle_ops *ops) {}
static inline void s2idle_wake(void) {}
#endif 


struct pbe {
	void *address;		
	void *orig_address;	
	struct pbe *next;
};


struct platform_hibernation_ops {
	int (*begin)(pm_message_t stage);
	void (*end)(void);
	int (*pre_snapshot)(void);
	void (*finish)(void);
	int (*prepare)(void);
	int (*enter)(void);
	void (*leave)(void);
	int (*pre_restore)(void);
	void (*restore_cleanup)(void);
	void (*recover)(void);
};

#ifdef CONFIG_HIBERNATION

extern void register_nosave_region(unsigned long b, unsigned long e);
extern int swsusp_page_is_forbidden(struct page *);
extern void swsusp_set_page_free(struct page *);
extern void swsusp_unset_page_free(struct page *);
extern unsigned long get_safe_page(gfp_t gfp_mask);
extern asmlinkage int swsusp_arch_suspend(void);
extern asmlinkage int swsusp_arch_resume(void);

extern u32 swsusp_hardware_signature;
extern void hibernation_set_ops(const struct platform_hibernation_ops *ops);
extern int hibernate(void);
extern bool system_entering_hibernation(void);
extern bool hibernation_available(void);
asmlinkage int swsusp_save(void);
extern struct pbe *restore_pblist;
int pfn_is_nosave(unsigned long pfn);

int hibernate_quiet_exec(int (*func)(void *data), void *data);
int hibernate_resume_nonboot_cpu_disable(void);
int arch_hibernation_header_save(void *addr, unsigned int max_size);
int arch_hibernation_header_restore(void *addr);

#else 
static inline void register_nosave_region(unsigned long b, unsigned long e) {}
static inline int swsusp_page_is_forbidden(struct page *p) { return 0; }
static inline void swsusp_set_page_free(struct page *p) {}
static inline void swsusp_unset_page_free(struct page *p) {}

static inline void hibernation_set_ops(const struct platform_hibernation_ops *ops) {}
static inline int hibernate(void) { return -ENOSYS; }
static inline bool system_entering_hibernation(void) { return false; }
static inline bool hibernation_available(void) { return false; }

static inline int hibernate_quiet_exec(int (*func)(void *data), void *data) {
	return -ENOTSUPP;
}
#endif 

int arch_resume_nosmt(void);

#ifdef CONFIG_HIBERNATION_SNAPSHOT_DEV
int is_hibernate_resume_dev(dev_t dev);
#else
static inline int is_hibernate_resume_dev(dev_t dev) { return 0; }
#endif


#define PM_HIBERNATION_PREPARE	0x0001 
#define PM_POST_HIBERNATION	0x0002 
#define PM_SUSPEND_PREPARE	0x0003 
#define PM_POST_SUSPEND		0x0004 
#define PM_RESTORE_PREPARE	0x0005 
#define PM_POST_RESTORE		0x0006 

extern struct mutex system_transition_mutex;

#ifdef CONFIG_PM_SLEEP
void save_processor_state(void);
void restore_processor_state(void);


extern int register_pm_notifier(struct notifier_block *nb);
extern int unregister_pm_notifier(struct notifier_block *nb);
extern void ksys_sync_helper(void);
extern void pm_report_hw_sleep_time(u64 t);
extern void pm_report_max_hw_sleep(u64 t);

#define pm_notifier(fn, pri) {				\
	static struct notifier_block fn##_nb =			\
		{ .notifier_call = fn, .priority = pri };	\
	register_pm_notifier(&fn##_nb);			\
}


extern bool events_check_enabled;

static inline bool pm_suspended_storage(void)
{
	return !gfp_has_io_fs(gfp_allowed_mask);
}

extern bool pm_wakeup_pending(void);
extern void pm_system_wakeup(void);
extern void pm_system_cancel_wakeup(void);
extern void pm_wakeup_clear(unsigned int irq_number);
extern void pm_system_irq_wakeup(unsigned int irq_number);
extern unsigned int pm_wakeup_irq(void);
extern bool pm_get_wakeup_count(unsigned int *count, bool block);
extern bool pm_save_wakeup_count(unsigned int count);
extern void pm_wakep_autosleep_enabled(bool set);
extern void pm_print_active_wakeup_sources(void);

extern unsigned int lock_system_sleep(void);
extern void unlock_system_sleep(unsigned int);

#else 

static inline int register_pm_notifier(struct notifier_block *nb)
{
	return 0;
}

static inline int unregister_pm_notifier(struct notifier_block *nb)
{
	return 0;
}

static inline void pm_report_hw_sleep_time(u64 t) {};
static inline void pm_report_max_hw_sleep(u64 t) {};

static inline void ksys_sync_helper(void) {}

#define pm_notifier(fn, pri)	do { (void)(fn); } while (0)

static inline bool pm_suspended_storage(void) { return false; }
static inline bool pm_wakeup_pending(void) { return false; }
static inline void pm_system_wakeup(void) {}
static inline void pm_wakeup_clear(bool reset) {}
static inline void pm_system_irq_wakeup(unsigned int irq_number) {}

static inline unsigned int lock_system_sleep(void) { return 0; }
static inline void unlock_system_sleep(unsigned int flags) {}

#endif 

#ifdef CONFIG_PM_SLEEP_DEBUG
extern bool pm_print_times_enabled;
extern bool pm_debug_messages_on;
extern bool pm_debug_messages_should_print(void);
static inline int pm_dyn_debug_messages_on(void)
{
#ifdef CONFIG_DYNAMIC_DEBUG
	return 1;
#else
	return 0;
#endif
}
#ifndef pr_fmt
#define pr_fmt(fmt) "PM: " fmt
#endif
#define __pm_pr_dbg(fmt, ...)					\
	do {							\
		if (pm_debug_messages_should_print())		\
			printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__);	\
		else if (pm_dyn_debug_messages_on())		\
			pr_debug(fmt, ##__VA_ARGS__);	\
	} while (0)
#define __pm_deferred_pr_dbg(fmt, ...)				\
	do {							\
		if (pm_debug_messages_should_print())		\
			printk_deferred(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__);	\
	} while (0)
#else
#define pm_print_times_enabled	(false)
#define pm_debug_messages_on	(false)

#include <linux/printk.h>

#define __pm_pr_dbg(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#define __pm_deferred_pr_dbg(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif


#define pm_pr_dbg(fmt, ...) \
	__pm_pr_dbg(fmt, ##__VA_ARGS__)

#define pm_deferred_pr_dbg(fmt, ...) \
	__pm_deferred_pr_dbg(fmt, ##__VA_ARGS__)

#ifdef CONFIG_PM_AUTOSLEEP


void queue_up_suspend_work(void);

#else 

static inline void queue_up_suspend_work(void) {}

#endif 

#endif 
