// SPDX-License-Identifier: GPL-2.0

#ifndef _LINUX_HRTIMER_H
#define _LINUX_HRTIMER_H

#include <linux/hrtimer_defs.h>
#include <linux/rbtree.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/percpu.h>
#include <linux/seqlock.h>
#include <linux/timer.h>
#include <linux/timerqueue.h>

struct hrtimer_clock_base;
struct hrtimer_cpu_base;


enum hrtimer_mode {
	HRTIMER_MODE_ABS	= 0x00,
	HRTIMER_MODE_REL	= 0x01,
	HRTIMER_MODE_PINNED	= 0x02,
	HRTIMER_MODE_SOFT	= 0x04,
	HRTIMER_MODE_HARD	= 0x08,

	HRTIMER_MODE_ABS_PINNED = HRTIMER_MODE_ABS | HRTIMER_MODE_PINNED,
	HRTIMER_MODE_REL_PINNED = HRTIMER_MODE_REL | HRTIMER_MODE_PINNED,

	HRTIMER_MODE_ABS_SOFT	= HRTIMER_MODE_ABS | HRTIMER_MODE_SOFT,
	HRTIMER_MODE_REL_SOFT	= HRTIMER_MODE_REL | HRTIMER_MODE_SOFT,

	HRTIMER_MODE_ABS_PINNED_SOFT = HRTIMER_MODE_ABS_PINNED | HRTIMER_MODE_SOFT,
	HRTIMER_MODE_REL_PINNED_SOFT = HRTIMER_MODE_REL_PINNED | HRTIMER_MODE_SOFT,

	HRTIMER_MODE_ABS_HARD	= HRTIMER_MODE_ABS | HRTIMER_MODE_HARD,
	HRTIMER_MODE_REL_HARD	= HRTIMER_MODE_REL | HRTIMER_MODE_HARD,

	HRTIMER_MODE_ABS_PINNED_HARD = HRTIMER_MODE_ABS_PINNED | HRTIMER_MODE_HARD,
	HRTIMER_MODE_REL_PINNED_HARD = HRTIMER_MODE_REL_PINNED | HRTIMER_MODE_HARD,
};


enum hrtimer_restart {
	HRTIMER_NORESTART,	
	HRTIMER_RESTART,	
};


#define HRTIMER_STATE_INACTIVE	0x00
#define HRTIMER_STATE_ENQUEUED	0x01


struct hrtimer {
	struct timerqueue_node		node;
	ktime_t				_softexpires;
	enum hrtimer_restart		(*function)(struct hrtimer *);
	struct hrtimer_clock_base	*base;
	u8				state;
	u8				is_rel;
	u8				is_soft;
	u8				is_hard;
};


struct hrtimer_sleeper {
	struct hrtimer timer;
	struct task_struct *task;
};

#ifdef CONFIG_64BIT
# define __hrtimer_clock_base_align	____cacheline_aligned
#else
# define __hrtimer_clock_base_align
#endif


struct hrtimer_clock_base {
	struct hrtimer_cpu_base	*cpu_base;
	unsigned int		index;
	clockid_t		clockid;
	seqcount_raw_spinlock_t	seq;
	struct hrtimer		*running;
	struct timerqueue_head	active;
	ktime_t			(*get_time)(void);
	ktime_t			offset;
} __hrtimer_clock_base_align;

enum  hrtimer_base_type {
	HRTIMER_BASE_MONOTONIC,
	HRTIMER_BASE_REALTIME,
	HRTIMER_BASE_BOOTTIME,
	HRTIMER_BASE_TAI,
	HRTIMER_BASE_MONOTONIC_SOFT,
	HRTIMER_BASE_REALTIME_SOFT,
	HRTIMER_BASE_BOOTTIME_SOFT,
	HRTIMER_BASE_TAI_SOFT,
	HRTIMER_MAX_CLOCK_BASES,
};


struct hrtimer_cpu_base {
	raw_spinlock_t			lock;
	unsigned int			cpu;
	unsigned int			active_bases;
	unsigned int			clock_was_set_seq;
	unsigned int			hres_active		: 1,
					in_hrtirq		: 1,
					hang_detected		: 1,
					softirq_activated       : 1,
					online			: 1;
#ifdef CONFIG_HIGH_RES_TIMERS
	unsigned int			nr_events;
	unsigned short			nr_retries;
	unsigned short			nr_hangs;
	unsigned int			max_hang_time;
#endif
#ifdef CONFIG_PREEMPT_RT
	spinlock_t			softirq_expiry_lock;
	atomic_t			timer_waiters;
#endif
	ktime_t				expires_next;
	struct hrtimer			*next_timer;
	ktime_t				softirq_expires_next;
	struct hrtimer			*softirq_next_timer;
	struct hrtimer_clock_base	clock_base[HRTIMER_MAX_CLOCK_BASES];
} ____cacheline_aligned;

static inline void hrtimer_set_expires(struct hrtimer *timer, ktime_t time)
{
	timer->node.expires = time;
	timer->_softexpires = time;
}

static inline void hrtimer_set_expires_range(struct hrtimer *timer, ktime_t time, ktime_t delta)
{
	timer->_softexpires = time;
	timer->node.expires = ktime_add_safe(time, delta);
}

static inline void hrtimer_set_expires_range_ns(struct hrtimer *timer, ktime_t time, u64 delta)
{
	timer->_softexpires = time;
	timer->node.expires = ktime_add_safe(time, ns_to_ktime(delta));
}

static inline void hrtimer_set_expires_tv64(struct hrtimer *timer, s64 tv64)
{
	timer->node.expires = tv64;
	timer->_softexpires = tv64;
}

static inline void hrtimer_add_expires(struct hrtimer *timer, ktime_t time)
{
	timer->node.expires = ktime_add_safe(timer->node.expires, time);
	timer->_softexpires = ktime_add_safe(timer->_softexpires, time);
}

static inline void hrtimer_add_expires_ns(struct hrtimer *timer, u64 ns)
{
	timer->node.expires = ktime_add_ns(timer->node.expires, ns);
	timer->_softexpires = ktime_add_ns(timer->_softexpires, ns);
}

static inline ktime_t hrtimer_get_expires(const struct hrtimer *timer)
{
	return timer->node.expires;
}

static inline ktime_t hrtimer_get_softexpires(const struct hrtimer *timer)
{
	return timer->_softexpires;
}

static inline s64 hrtimer_get_expires_tv64(const struct hrtimer *timer)
{
	return timer->node.expires;
}
static inline s64 hrtimer_get_softexpires_tv64(const struct hrtimer *timer)
{
	return timer->_softexpires;
}

static inline s64 hrtimer_get_expires_ns(const struct hrtimer *timer)
{
	return ktime_to_ns(timer->node.expires);
}

static inline ktime_t hrtimer_expires_remaining(const struct hrtimer *timer)
{
	return ktime_sub(timer->node.expires, timer->base->get_time());
}

static inline ktime_t hrtimer_cb_get_time(struct hrtimer *timer)
{
	return timer->base->get_time();
}

static inline int hrtimer_is_hres_active(struct hrtimer *timer)
{
	return IS_ENABLED(CONFIG_HIGH_RES_TIMERS) ?
		timer->base->cpu_base->hres_active : 0;
}

#ifdef CONFIG_HIGH_RES_TIMERS
struct clock_event_device;

extern void hrtimer_interrupt(struct clock_event_device *dev);

extern unsigned int hrtimer_resolution;

#else

#define hrtimer_resolution	(unsigned int)LOW_RES_NSEC

#endif

static inline ktime_t
__hrtimer_expires_remaining_adjusted(const struct hrtimer *timer, ktime_t now)
{
	ktime_t rem = ktime_sub(timer->node.expires, now);

	
	if (IS_ENABLED(CONFIG_TIME_LOW_RES) && timer->is_rel)
		rem -= hrtimer_resolution;
	return rem;
}

static inline ktime_t
hrtimer_expires_remaining_adjusted(const struct hrtimer *timer)
{
	return __hrtimer_expires_remaining_adjusted(timer,
						    timer->base->get_time());
}

#ifdef CONFIG_TIMERFD
extern void timerfd_clock_was_set(void);
extern void timerfd_resume(void);
#else
static inline void timerfd_clock_was_set(void) { }
static inline void timerfd_resume(void) { }
#endif

DECLARE_PER_CPU(struct tick_device, tick_cpu_device);

#ifdef CONFIG_PREEMPT_RT
void hrtimer_cancel_wait_running(const struct hrtimer *timer);
#else
static inline void hrtimer_cancel_wait_running(struct hrtimer *timer)
{
	cpu_relax();
}
#endif




extern void hrtimer_init(struct hrtimer *timer, clockid_t which_clock,
			 enum hrtimer_mode mode);
extern void hrtimer_init_sleeper(struct hrtimer_sleeper *sl, clockid_t clock_id,
				 enum hrtimer_mode mode);

#ifdef CONFIG_DEBUG_OBJECTS_TIMERS
extern void hrtimer_init_on_stack(struct hrtimer *timer, clockid_t which_clock,
				  enum hrtimer_mode mode);
extern void hrtimer_init_sleeper_on_stack(struct hrtimer_sleeper *sl,
					  clockid_t clock_id,
					  enum hrtimer_mode mode);

extern void destroy_hrtimer_on_stack(struct hrtimer *timer);
#else
static inline void hrtimer_init_on_stack(struct hrtimer *timer,
					 clockid_t which_clock,
					 enum hrtimer_mode mode)
{
	hrtimer_init(timer, which_clock, mode);
}

static inline void hrtimer_init_sleeper_on_stack(struct hrtimer_sleeper *sl,
						 clockid_t clock_id,
						 enum hrtimer_mode mode)
{
	hrtimer_init_sleeper(sl, clock_id, mode);
}

static inline void destroy_hrtimer_on_stack(struct hrtimer *timer) { }
#endif


extern void hrtimer_start_range_ns(struct hrtimer *timer, ktime_t tim,
				   u64 range_ns, const enum hrtimer_mode mode);


static inline void hrtimer_start(struct hrtimer *timer, ktime_t tim,
				 const enum hrtimer_mode mode)
{
	hrtimer_start_range_ns(timer, tim, 0, mode);
}

extern int hrtimer_cancel(struct hrtimer *timer);
extern int hrtimer_try_to_cancel(struct hrtimer *timer);

static inline void hrtimer_start_expires(struct hrtimer *timer,
					 enum hrtimer_mode mode)
{
	u64 delta;
	ktime_t soft, hard;
	soft = hrtimer_get_softexpires(timer);
	hard = hrtimer_get_expires(timer);
	delta = ktime_to_ns(ktime_sub(hard, soft));
	hrtimer_start_range_ns(timer, soft, delta, mode);
}

void hrtimer_sleeper_start_expires(struct hrtimer_sleeper *sl,
				   enum hrtimer_mode mode);

static inline void hrtimer_restart(struct hrtimer *timer)
{
	hrtimer_start_expires(timer, HRTIMER_MODE_ABS);
}


extern ktime_t __hrtimer_get_remaining(const struct hrtimer *timer, bool adjust);


static inline ktime_t hrtimer_get_remaining(const struct hrtimer *timer)
{
	return __hrtimer_get_remaining(timer, false);
}

extern u64 hrtimer_get_next_event(void);
extern u64 hrtimer_next_event_without(const struct hrtimer *exclude);

extern bool hrtimer_active(const struct hrtimer *timer);


static inline bool hrtimer_is_queued(struct hrtimer *timer)
{
	
	return !!(READ_ONCE(timer->state) & HRTIMER_STATE_ENQUEUED);
}


static inline int hrtimer_callback_running(struct hrtimer *timer)
{
	return timer->base->running == timer;
}


extern u64
hrtimer_forward(struct hrtimer *timer, ktime_t now, ktime_t interval);


static inline u64 hrtimer_forward_now(struct hrtimer *timer,
				      ktime_t interval)
{
	return hrtimer_forward(timer, timer->base->get_time(), interval);
}



extern int nanosleep_copyout(struct restart_block *, struct timespec64 *);
extern long hrtimer_nanosleep(ktime_t rqtp, const enum hrtimer_mode mode,
			      const clockid_t clockid);

extern int schedule_hrtimeout_range(ktime_t *expires, u64 delta,
				    const enum hrtimer_mode mode);
extern int schedule_hrtimeout_range_clock(ktime_t *expires,
					  u64 delta,
					  const enum hrtimer_mode mode,
					  clockid_t clock_id);
extern int schedule_hrtimeout(ktime_t *expires, const enum hrtimer_mode mode);


extern void hrtimer_run_queues(void);


extern void __init hrtimers_init(void);


extern void sysrq_timer_list_show(void);

int hrtimers_prepare_cpu(unsigned int cpu);
#ifdef CONFIG_HOTPLUG_CPU
int hrtimers_cpu_dying(unsigned int cpu);
#else
#define hrtimers_cpu_dying	NULL
#endif

#endif
