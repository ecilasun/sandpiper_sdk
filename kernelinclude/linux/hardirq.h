/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_HARDIRQ_H
#define LINUX_HARDIRQ_H

#include <linux/context_tracking_state.h>
#include <linux/preempt.h>
#include <linux/lockdep.h>
#include <linux/ftrace_irq.h>
#include <linux/sched.h>
#include <linux/vtime.h>
#include <asm/hardirq.h>

extern void synchronize_irq(unsigned int irq);
extern bool synchronize_hardirq(unsigned int irq);

#ifdef CONFIG_NO_HZ_FULL
void __rcu_irq_enter_check_tick(void);
#else
static inline void __rcu_irq_enter_check_tick(void) { }
#endif

static __always_inline void rcu_irq_enter_check_tick(void)
{
	if (context_tracking_enabled())
		__rcu_irq_enter_check_tick();
}


#define __irq_enter()					\
	do {						\
		preempt_count_add(HARDIRQ_OFFSET);	\
		lockdep_hardirq_enter();		\
		account_hardirq_enter(current);		\
	} while (0)


#define __irq_enter_raw()				\
	do {						\
		preempt_count_add(HARDIRQ_OFFSET);	\
		lockdep_hardirq_enter();		\
	} while (0)


void irq_enter(void);

void irq_enter_rcu(void);


#define __irq_exit()					\
	do {						\
		account_hardirq_exit(current);		\
		lockdep_hardirq_exit();			\
		preempt_count_sub(HARDIRQ_OFFSET);	\
	} while (0)


#define __irq_exit_raw()				\
	do {						\
		lockdep_hardirq_exit();			\
		preempt_count_sub(HARDIRQ_OFFSET);	\
	} while (0)


void irq_exit(void);


void irq_exit_rcu(void);

#ifndef arch_nmi_enter
#define arch_nmi_enter()	do { } while (0)
#define arch_nmi_exit()		do { } while (0)
#endif




#define __nmi_enter()						\
	do {							\
		lockdep_off();					\
		arch_nmi_enter();				\
		BUG_ON(in_nmi() == NMI_MASK);			\
		__preempt_count_add(NMI_OFFSET + HARDIRQ_OFFSET);	\
	} while (0)

#define nmi_enter()						\
	do {							\
		__nmi_enter();					\
		lockdep_hardirq_enter();			\
		ct_nmi_enter();				\
		instrumentation_begin();			\
		ftrace_nmi_enter();				\
		instrumentation_end();				\
	} while (0)

#define __nmi_exit()						\
	do {							\
		BUG_ON(!in_nmi());				\
		__preempt_count_sub(NMI_OFFSET + HARDIRQ_OFFSET);	\
		arch_nmi_exit();				\
		lockdep_on();					\
	} while (0)

#define nmi_exit()						\
	do {							\
		instrumentation_begin();			\
		ftrace_nmi_exit();				\
		instrumentation_end();				\
		ct_nmi_exit();					\
		lockdep_hardirq_exit();				\
		__nmi_exit();					\
	} while (0)

#endif 
