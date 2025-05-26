/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_CLOCK_H
#define _LINUX_SCHED_CLOCK_H

#include <linux/smp.h>


extern u64 sched_clock(void);

#if defined(CONFIG_ARCH_WANTS_NO_INSTR) || defined(CONFIG_GENERIC_SCHED_CLOCK)
extern u64 sched_clock_noinstr(void);
#else
static __always_inline u64 sched_clock_noinstr(void)
{
	return sched_clock();
}
#endif


extern u64 running_clock(void);
extern u64 sched_clock_cpu(int cpu);


extern void sched_clock_init(void);

#ifndef CONFIG_HAVE_UNSTABLE_SCHED_CLOCK
static inline void sched_clock_tick(void)
{
}

static inline void clear_sched_clock_stable(void)
{
}

static inline void sched_clock_idle_sleep_event(void)
{
}

static inline void sched_clock_idle_wakeup_event(void)
{
}

static inline u64 cpu_clock(int cpu)
{
	return sched_clock();
}

static __always_inline u64 local_clock_noinstr(void)
{
	return sched_clock_noinstr();
}

static __always_inline u64 local_clock(void)
{
	return sched_clock();
}
#else
extern int sched_clock_stable(void);
extern void clear_sched_clock_stable(void);


extern u64 __sched_clock_offset;

extern void sched_clock_tick(void);
extern void sched_clock_tick_stable(void);
extern void sched_clock_idle_sleep_event(void);
extern void sched_clock_idle_wakeup_event(void);


static inline u64 cpu_clock(int cpu)
{
	return sched_clock_cpu(cpu);
}

extern u64 local_clock_noinstr(void);
extern u64 local_clock(void);

#endif

#ifdef CONFIG_IRQ_TIME_ACCOUNTING

extern void enable_sched_clock_irqtime(void);
extern void disable_sched_clock_irqtime(void);
#else
static inline void enable_sched_clock_irqtime(void) {}
static inline void disable_sched_clock_irqtime(void) {}
#endif

#endif 
