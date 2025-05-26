/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_INTERRUPT_H
#define _LINUX_INTERRUPT_H

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/cpumask.h>
#include <linux/irqreturn.h>
#include <linux/irqnr.h>
#include <linux/hardirq.h>
#include <linux/irqflags.h>
#include <linux/hrtimer.h>
#include <linux/kref.h>
#include <linux/workqueue.h>
#include <linux/jump_label.h>

#include <linux/atomic.h>
#include <asm/ptrace.h>
#include <asm/irq.h>
#include <asm/sections.h>


#define IRQF_TRIGGER_NONE	0x00000000
#define IRQF_TRIGGER_RISING	0x00000001
#define IRQF_TRIGGER_FALLING	0x00000002
#define IRQF_TRIGGER_HIGH	0x00000004
#define IRQF_TRIGGER_LOW	0x00000008
#define IRQF_TRIGGER_MASK	(IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
				 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define IRQF_TRIGGER_PROBE	0x00000010


#define IRQF_SHARED		0x00000080
#define IRQF_PROBE_SHARED	0x00000100
#define __IRQF_TIMER		0x00000200
#define IRQF_PERCPU		0x00000400
#define IRQF_NOBALANCING	0x00000800
#define IRQF_IRQPOLL		0x00001000
#define IRQF_ONESHOT		0x00002000
#define IRQF_NO_SUSPEND		0x00004000
#define IRQF_FORCE_RESUME	0x00008000
#define IRQF_NO_THREAD		0x00010000
#define IRQF_EARLY_RESUME	0x00020000
#define IRQF_COND_SUSPEND	0x00040000
#define IRQF_NO_AUTOEN		0x00080000
#define IRQF_NO_DEBUG		0x00100000

#define IRQF_TIMER		(__IRQF_TIMER | IRQF_NO_SUSPEND | IRQF_NO_THREAD)


enum {
	IRQC_IS_HARDIRQ	= 0,
	IRQC_IS_NESTED,
};

typedef irqreturn_t (*irq_handler_t)(int, void *);


struct irqaction {
	irq_handler_t		handler;
	void			*dev_id;
	void __percpu		*percpu_dev_id;
	struct irqaction	*next;
	irq_handler_t		thread_fn;
	struct task_struct	*thread;
	struct irqaction	*secondary;
	unsigned int		irq;
	unsigned int		flags;
	unsigned long		thread_flags;
	unsigned long		thread_mask;
	const char		*name;
	struct proc_dir_entry	*dir;
} ____cacheline_internodealigned_in_smp;

extern irqreturn_t no_action(int cpl, void *dev_id);


#define IRQ_NOTCONNECTED	(1U << 31)

extern int __must_check
request_threaded_irq(unsigned int irq, irq_handler_t handler,
		     irq_handler_t thread_fn,
		     unsigned long flags, const char *name, void *dev);


static inline int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
{
	return request_threaded_irq(irq, handler, NULL, flags, name, dev);
}

extern int __must_check
request_any_context_irq(unsigned int irq, irq_handler_t handler,
			unsigned long flags, const char *name, void *dev_id);

extern int __must_check
__request_percpu_irq(unsigned int irq, irq_handler_t handler,
		     unsigned long flags, const char *devname,
		     void __percpu *percpu_dev_id);

extern int __must_check
request_nmi(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev);

static inline int __must_check
request_percpu_irq(unsigned int irq, irq_handler_t handler,
		   const char *devname, void __percpu *percpu_dev_id)
{
	return __request_percpu_irq(irq, handler, 0,
				    devname, percpu_dev_id);
}

extern int __must_check
request_percpu_nmi(unsigned int irq, irq_handler_t handler,
		   const char *devname, void __percpu *dev);

extern const void *free_irq(unsigned int, void *);
extern void free_percpu_irq(unsigned int, void __percpu *);

extern const void *free_nmi(unsigned int irq, void *dev_id);
extern void free_percpu_nmi(unsigned int irq, void __percpu *percpu_dev_id);

struct device;

extern int __must_check
devm_request_threaded_irq(struct device *dev, unsigned int irq,
			  irq_handler_t handler, irq_handler_t thread_fn,
			  unsigned long irqflags, const char *devname,
			  void *dev_id);

static inline int __must_check
devm_request_irq(struct device *dev, unsigned int irq, irq_handler_t handler,
		 unsigned long irqflags, const char *devname, void *dev_id)
{
	return devm_request_threaded_irq(dev, irq, handler, NULL, irqflags,
					 devname, dev_id);
}

extern int __must_check
devm_request_any_context_irq(struct device *dev, unsigned int irq,
		 irq_handler_t handler, unsigned long irqflags,
		 const char *devname, void *dev_id);

extern void devm_free_irq(struct device *dev, unsigned int irq, void *dev_id);

bool irq_has_action(unsigned int irq);
extern void disable_irq_nosync(unsigned int irq);
extern bool disable_hardirq(unsigned int irq);
extern void disable_irq(unsigned int irq);
extern void disable_percpu_irq(unsigned int irq);
extern void enable_irq(unsigned int irq);
extern void enable_percpu_irq(unsigned int irq, unsigned int type);
extern bool irq_percpu_is_enabled(unsigned int irq);
extern void irq_wake_thread(unsigned int irq, void *dev_id);

extern void disable_nmi_nosync(unsigned int irq);
extern void disable_percpu_nmi(unsigned int irq);
extern void enable_nmi(unsigned int irq);
extern void enable_percpu_nmi(unsigned int irq, unsigned int type);
extern int prepare_percpu_nmi(unsigned int irq);
extern void teardown_percpu_nmi(unsigned int irq);

extern int irq_inject_interrupt(unsigned int irq);


extern void suspend_device_irqs(void);
extern void resume_device_irqs(void);
extern void rearm_wake_irq(unsigned int irq);


struct irq_affinity_notify {
	unsigned int irq;
	struct kref kref;
	struct work_struct work;
	void (*notify)(struct irq_affinity_notify *, const cpumask_t *mask);
	void (*release)(struct kref *ref);
};

#define	IRQ_AFFINITY_MAX_SETS  4


struct irq_affinity {
	unsigned int	pre_vectors;
	unsigned int	post_vectors;
	unsigned int	nr_sets;
	unsigned int	set_size[IRQ_AFFINITY_MAX_SETS];
	void		(*calc_sets)(struct irq_affinity *, unsigned int nvecs);
	void		*priv;
};


struct irq_affinity_desc {
	struct cpumask	mask;
	unsigned int	is_managed : 1;
};

#if defined(CONFIG_SMP)

extern cpumask_var_t irq_default_affinity;

extern int irq_set_affinity(unsigned int irq, const struct cpumask *cpumask);
extern int irq_force_affinity(unsigned int irq, const struct cpumask *cpumask);

extern int irq_can_set_affinity(unsigned int irq);
extern int irq_select_affinity(unsigned int irq);

extern int __irq_apply_affinity_hint(unsigned int irq, const struct cpumask *m,
				     bool setaffinity);


static inline int
irq_update_affinity_hint(unsigned int irq, const struct cpumask *m)
{
	return __irq_apply_affinity_hint(irq, m, false);
}


static inline int
irq_set_affinity_and_hint(unsigned int irq, const struct cpumask *m)
{
	return __irq_apply_affinity_hint(irq, m, true);
}


static inline int irq_set_affinity_hint(unsigned int irq, const struct cpumask *m)
{
	return irq_set_affinity_and_hint(irq, m);
}

extern int irq_update_affinity_desc(unsigned int irq,
				    struct irq_affinity_desc *affinity);

extern int
irq_set_affinity_notifier(unsigned int irq, struct irq_affinity_notify *notify);

struct irq_affinity_desc *
irq_create_affinity_masks(unsigned int nvec, struct irq_affinity *affd);

unsigned int irq_calc_affinity_vectors(unsigned int minvec, unsigned int maxvec,
				       const struct irq_affinity *affd);

#else 

static inline int irq_set_affinity(unsigned int irq, const struct cpumask *m)
{
	return -EINVAL;
}

static inline int irq_force_affinity(unsigned int irq, const struct cpumask *cpumask)
{
	return 0;
}

static inline int irq_can_set_affinity(unsigned int irq)
{
	return 0;
}

static inline int irq_select_affinity(unsigned int irq)  { return 0; }

static inline int irq_update_affinity_hint(unsigned int irq,
					   const struct cpumask *m)
{
	return -EINVAL;
}

static inline int irq_set_affinity_and_hint(unsigned int irq,
					    const struct cpumask *m)
{
	return -EINVAL;
}

static inline int irq_set_affinity_hint(unsigned int irq,
					const struct cpumask *m)
{
	return -EINVAL;
}

static inline int irq_update_affinity_desc(unsigned int irq,
					   struct irq_affinity_desc *affinity)
{
	return -EINVAL;
}

static inline int
irq_set_affinity_notifier(unsigned int irq, struct irq_affinity_notify *notify)
{
	return 0;
}

static inline struct irq_affinity_desc *
irq_create_affinity_masks(unsigned int nvec, struct irq_affinity *affd)
{
	return NULL;
}

static inline unsigned int
irq_calc_affinity_vectors(unsigned int minvec, unsigned int maxvec,
			  const struct irq_affinity *affd)
{
	return maxvec;
}

#endif 


static inline void disable_irq_nosync_lockdep(unsigned int irq)
{
	disable_irq_nosync(irq);
#ifdef CONFIG_LOCKDEP
	local_irq_disable();
#endif
}

static inline void disable_irq_nosync_lockdep_irqsave(unsigned int irq, unsigned long *flags)
{
	disable_irq_nosync(irq);
#ifdef CONFIG_LOCKDEP
	local_irq_save(*flags);
#endif
}

static inline void disable_irq_lockdep(unsigned int irq)
{
	disable_irq(irq);
#ifdef CONFIG_LOCKDEP
	local_irq_disable();
#endif
}

static inline void enable_irq_lockdep(unsigned int irq)
{
#ifdef CONFIG_LOCKDEP
	local_irq_enable();
#endif
	enable_irq(irq);
}

static inline void enable_irq_lockdep_irqrestore(unsigned int irq, unsigned long *flags)
{
#ifdef CONFIG_LOCKDEP
	local_irq_restore(*flags);
#endif
	enable_irq(irq);
}


extern int irq_set_irq_wake(unsigned int irq, unsigned int on);

static inline int enable_irq_wake(unsigned int irq)
{
	return irq_set_irq_wake(irq, 1);
}

static inline int disable_irq_wake(unsigned int irq)
{
	return irq_set_irq_wake(irq, 0);
}


enum irqchip_irq_state {
	IRQCHIP_STATE_PENDING,		
	IRQCHIP_STATE_ACTIVE,		
	IRQCHIP_STATE_MASKED,		
	IRQCHIP_STATE_LINE_LEVEL,	
};

extern int irq_get_irqchip_state(unsigned int irq, enum irqchip_irq_state which,
				 bool *state);
extern int irq_set_irqchip_state(unsigned int irq, enum irqchip_irq_state which,
				 bool state);

#ifdef CONFIG_IRQ_FORCED_THREADING
# ifdef CONFIG_PREEMPT_RT
#  define force_irqthreads()	(true)
# else
DECLARE_STATIC_KEY_FALSE(force_irqthreads_key);
#  define force_irqthreads()	(static_branch_unlikely(&force_irqthreads_key))
# endif
#else
#define force_irqthreads()	(false)
#endif

#ifndef local_softirq_pending

#ifndef local_softirq_pending_ref
#define local_softirq_pending_ref irq_stat.__softirq_pending
#endif

#define local_softirq_pending()	(__this_cpu_read(local_softirq_pending_ref))
#define set_softirq_pending(x)	(__this_cpu_write(local_softirq_pending_ref, (x)))
#define or_softirq_pending(x)	(__this_cpu_or(local_softirq_pending_ref, (x)))

#endif 


#ifndef hard_irq_disable
#define hard_irq_disable()	do { } while(0)
#endif



enum
{
	HI_SOFTIRQ=0,
	TIMER_SOFTIRQ,
	NET_TX_SOFTIRQ,
	NET_RX_SOFTIRQ,
	BLOCK_SOFTIRQ,
	IRQ_POLL_SOFTIRQ,
	TASKLET_SOFTIRQ,
	SCHED_SOFTIRQ,
	HRTIMER_SOFTIRQ,
	RCU_SOFTIRQ,    

	NR_SOFTIRQS
};


#define SOFTIRQ_HOTPLUG_SAFE_MASK (BIT(TIMER_SOFTIRQ) | BIT(IRQ_POLL_SOFTIRQ) |\
				   BIT(HRTIMER_SOFTIRQ) | BIT(RCU_SOFTIRQ))



extern const char * const softirq_to_name[NR_SOFTIRQS];



struct softirq_action
{
	void	(*action)(struct softirq_action *);
};

asmlinkage void do_softirq(void);
asmlinkage void __do_softirq(void);

#ifdef CONFIG_PREEMPT_RT
extern void do_softirq_post_smp_call_flush(unsigned int was_pending);
#else
static inline void do_softirq_post_smp_call_flush(unsigned int unused)
{
	do_softirq();
}
#endif

extern void open_softirq(int nr, void (*action)(struct softirq_action *));
extern void softirq_init(void);
extern void __raise_softirq_irqoff(unsigned int nr);

extern void raise_softirq_irqoff(unsigned int nr);
extern void raise_softirq(unsigned int nr);

DECLARE_PER_CPU(struct task_struct *, ksoftirqd);

static inline struct task_struct *this_cpu_ksoftirqd(void)
{
	return this_cpu_read(ksoftirqd);
}



struct tasklet_struct
{
	struct tasklet_struct *next;
	unsigned long state;
	atomic_t count;
	bool use_callback;
	union {
		void (*func)(unsigned long data);
		void (*callback)(struct tasklet_struct *t);
	};
	unsigned long data;
};

#define DECLARE_TASKLET(name, _callback)		\
struct tasklet_struct name = {				\
	.count = ATOMIC_INIT(0),			\
	.callback = _callback,				\
	.use_callback = true,				\
}

#define DECLARE_TASKLET_DISABLED(name, _callback)	\
struct tasklet_struct name = {				\
	.count = ATOMIC_INIT(1),			\
	.callback = _callback,				\
	.use_callback = true,				\
}

#define from_tasklet(var, callback_tasklet, tasklet_fieldname)	\
	container_of(callback_tasklet, typeof(*var), tasklet_fieldname)

#define DECLARE_TASKLET_OLD(name, _func)		\
struct tasklet_struct name = {				\
	.count = ATOMIC_INIT(0),			\
	.func = _func,					\
}

#define DECLARE_TASKLET_DISABLED_OLD(name, _func)	\
struct tasklet_struct name = {				\
	.count = ATOMIC_INIT(1),			\
	.func = _func,					\
}

enum
{
	TASKLET_STATE_SCHED,	
	TASKLET_STATE_RUN	
};

#if defined(CONFIG_SMP) || defined(CONFIG_PREEMPT_RT)
static inline int tasklet_trylock(struct tasklet_struct *t)
{
	return !test_and_set_bit(TASKLET_STATE_RUN, &(t)->state);
}

void tasklet_unlock(struct tasklet_struct *t);
void tasklet_unlock_wait(struct tasklet_struct *t);
void tasklet_unlock_spin_wait(struct tasklet_struct *t);

#else
static inline int tasklet_trylock(struct tasklet_struct *t) { return 1; }
static inline void tasklet_unlock(struct tasklet_struct *t) { }
static inline void tasklet_unlock_wait(struct tasklet_struct *t) { }
static inline void tasklet_unlock_spin_wait(struct tasklet_struct *t) { }
#endif

extern void __tasklet_schedule(struct tasklet_struct *t);

static inline void tasklet_schedule(struct tasklet_struct *t)
{
	if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state))
		__tasklet_schedule(t);
}

extern void __tasklet_hi_schedule(struct tasklet_struct *t);

static inline void tasklet_hi_schedule(struct tasklet_struct *t)
{
	if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state))
		__tasklet_hi_schedule(t);
}

static inline void tasklet_disable_nosync(struct tasklet_struct *t)
{
	atomic_inc(&t->count);
	smp_mb__after_atomic();
}


static inline void tasklet_disable_in_atomic(struct tasklet_struct *t)
{
	tasklet_disable_nosync(t);
	tasklet_unlock_spin_wait(t);
	smp_mb();
}

static inline void tasklet_disable(struct tasklet_struct *t)
{
	tasklet_disable_nosync(t);
	tasklet_unlock_wait(t);
	smp_mb();
}

static inline void tasklet_enable(struct tasklet_struct *t)
{
	smp_mb__before_atomic();
	atomic_dec(&t->count);
}

extern void tasklet_kill(struct tasklet_struct *t);
extern void tasklet_init(struct tasklet_struct *t,
			 void (*func)(unsigned long), unsigned long data);
extern void tasklet_setup(struct tasklet_struct *t,
			  void (*callback)(struct tasklet_struct *));



#if !defined(CONFIG_GENERIC_IRQ_PROBE) 
static inline unsigned long probe_irq_on(void)
{
	return 0;
}
static inline int probe_irq_off(unsigned long val)
{
	return 0;
}
static inline unsigned int probe_irq_mask(unsigned long val)
{
	return 0;
}
#else
extern unsigned long probe_irq_on(void);	
extern int probe_irq_off(unsigned long);	
extern unsigned int probe_irq_mask(unsigned long);	
#endif

#ifdef CONFIG_PROC_FS

extern void init_irq_proc(void);
#else
static inline void init_irq_proc(void)
{
}
#endif

#ifdef CONFIG_IRQ_TIMINGS
void irq_timings_enable(void);
void irq_timings_disable(void);
u64 irq_timings_next_event(u64 now);
#endif

struct seq_file;
int show_interrupts(struct seq_file *p, void *v);
int arch_show_interrupts(struct seq_file *p, int prec);

extern int early_irq_init(void);
extern int arch_probe_nr_irqs(void);
extern int arch_early_irq_init(void);


#ifndef __irq_entry
# define __irq_entry	 __section(".irqentry.text")
#endif

#define __softirq_entry  __section(".softirqentry.text")

#endif
