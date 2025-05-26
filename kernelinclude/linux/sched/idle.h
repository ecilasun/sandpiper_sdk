/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_IDLE_H
#define _LINUX_SCHED_IDLE_H

#include <linux/sched.h>

enum cpu_idle_type {
	CPU_IDLE,
	CPU_NOT_IDLE,
	CPU_NEWLY_IDLE,
	CPU_MAX_IDLE_TYPES
};

#ifdef CONFIG_SMP
extern void wake_up_if_idle(int cpu);
#else
static inline void wake_up_if_idle(int cpu) { }
#endif


#ifdef TIF_POLLING_NRFLAG

#ifdef _ASM_GENERIC_BITOPS_INSTRUMENTED_ATOMIC_H

static __always_inline void __current_set_polling(void)
{
	arch_set_bit(TIF_POLLING_NRFLAG,
		     (unsigned long *)(&current_thread_info()->flags));
}

static __always_inline void __current_clr_polling(void)
{
	arch_clear_bit(TIF_POLLING_NRFLAG,
		       (unsigned long *)(&current_thread_info()->flags));
}

#else

static __always_inline void __current_set_polling(void)
{
	set_bit(TIF_POLLING_NRFLAG,
		(unsigned long *)(&current_thread_info()->flags));
}

static __always_inline void __current_clr_polling(void)
{
	clear_bit(TIF_POLLING_NRFLAG,
		  (unsigned long *)(&current_thread_info()->flags));
}

#endif 

static __always_inline bool __must_check current_set_polling_and_test(void)
{
	__current_set_polling();

	
	smp_mb__after_atomic();

	return unlikely(tif_need_resched());
}

static __always_inline bool __must_check current_clr_polling_and_test(void)
{
	__current_clr_polling();

	
	smp_mb__after_atomic();

	return unlikely(tif_need_resched());
}

#else
static inline void __current_set_polling(void) { }
static inline void __current_clr_polling(void) { }

static inline bool __must_check current_set_polling_and_test(void)
{
	return unlikely(tif_need_resched());
}
static inline bool __must_check current_clr_polling_and_test(void)
{
	return unlikely(tif_need_resched());
}
#endif

static __always_inline void current_clr_polling(void)
{
	__current_clr_polling();

	
	smp_mb(); 

	preempt_fold_need_resched();
}

#endif 
