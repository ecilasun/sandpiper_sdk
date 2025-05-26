/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_ENTRYCOMMON_H
#define __LINUX_ENTRYCOMMON_H

#include <linux/static_call_types.h>
#include <linux/ptrace.h>
#include <linux/syscalls.h>
#include <linux/seccomp.h>
#include <linux/sched.h>

#include <asm/entry-common.h>


#ifndef _TIF_PATCH_PENDING
# define _TIF_PATCH_PENDING		(0)
#endif

#ifndef _TIF_UPROBE
# define _TIF_UPROBE			(0)
#endif


#ifndef ARCH_SYSCALL_WORK_ENTER
# define ARCH_SYSCALL_WORK_ENTER	(0)
#endif


#ifndef ARCH_SYSCALL_WORK_EXIT
# define ARCH_SYSCALL_WORK_EXIT		(0)
#endif

#define SYSCALL_WORK_ENTER	(SYSCALL_WORK_SECCOMP |			\
				 SYSCALL_WORK_SYSCALL_TRACEPOINT |	\
				 SYSCALL_WORK_SYSCALL_TRACE |		\
				 SYSCALL_WORK_SYSCALL_EMU |		\
				 SYSCALL_WORK_SYSCALL_AUDIT |		\
				 SYSCALL_WORK_SYSCALL_USER_DISPATCH |	\
				 ARCH_SYSCALL_WORK_ENTER)
#define SYSCALL_WORK_EXIT	(SYSCALL_WORK_SYSCALL_TRACEPOINT |	\
				 SYSCALL_WORK_SYSCALL_TRACE |		\
				 SYSCALL_WORK_SYSCALL_AUDIT |		\
				 SYSCALL_WORK_SYSCALL_USER_DISPATCH |	\
				 SYSCALL_WORK_SYSCALL_EXIT_TRAP	|	\
				 ARCH_SYSCALL_WORK_EXIT)


#ifndef ARCH_EXIT_TO_USER_MODE_WORK
# define ARCH_EXIT_TO_USER_MODE_WORK		(0)
#endif

#define EXIT_TO_USER_MODE_WORK						\
	(_TIF_SIGPENDING | _TIF_NOTIFY_RESUME | _TIF_UPROBE |		\
	 _TIF_NEED_RESCHED | _TIF_PATCH_PENDING | _TIF_NOTIFY_SIGNAL |	\
	 ARCH_EXIT_TO_USER_MODE_WORK)


static __always_inline void arch_enter_from_user_mode(struct pt_regs *regs);

#ifndef arch_enter_from_user_mode
static __always_inline void arch_enter_from_user_mode(struct pt_regs *regs) {}
#endif


void enter_from_user_mode(struct pt_regs *regs);


void syscall_enter_from_user_mode_prepare(struct pt_regs *regs);


long syscall_enter_from_user_mode_work(struct pt_regs *regs, long syscall);


long syscall_enter_from_user_mode(struct pt_regs *regs, long syscall);


static inline void local_irq_enable_exit_to_user(unsigned long ti_work);

#ifndef local_irq_enable_exit_to_user
static inline void local_irq_enable_exit_to_user(unsigned long ti_work)
{
	local_irq_enable();
}
#endif


static inline void local_irq_disable_exit_to_user(void);

#ifndef local_irq_disable_exit_to_user
static inline void local_irq_disable_exit_to_user(void)
{
	local_irq_disable();
}
#endif


static inline void arch_exit_to_user_mode_work(struct pt_regs *regs,
					       unsigned long ti_work);

#ifndef arch_exit_to_user_mode_work
static inline void arch_exit_to_user_mode_work(struct pt_regs *regs,
					       unsigned long ti_work)
{
}
#endif


static inline void arch_exit_to_user_mode_prepare(struct pt_regs *regs,
						  unsigned long ti_work);

#ifndef arch_exit_to_user_mode_prepare
static inline void arch_exit_to_user_mode_prepare(struct pt_regs *regs,
						  unsigned long ti_work)
{
}
#endif


static __always_inline void arch_exit_to_user_mode(void);

#ifndef arch_exit_to_user_mode
static __always_inline void arch_exit_to_user_mode(void) { }
#endif


void arch_do_signal_or_restart(struct pt_regs *regs);


void exit_to_user_mode(void);


void syscall_exit_to_user_mode_work(struct pt_regs *regs);


void syscall_exit_to_user_mode(struct pt_regs *regs);


void irqentry_enter_from_user_mode(struct pt_regs *regs);


void irqentry_exit_to_user_mode(struct pt_regs *regs);

#ifndef irqentry_state

typedef struct irqentry_state {
	union {
		bool	exit_rcu;
		bool	lockdep;
	};
} irqentry_state_t;
#endif


irqentry_state_t noinstr irqentry_enter(struct pt_regs *regs);


void raw_irqentry_exit_cond_resched(void);
#ifdef CONFIG_PREEMPT_DYNAMIC
#if defined(CONFIG_HAVE_PREEMPT_DYNAMIC_CALL)
#define irqentry_exit_cond_resched_dynamic_enabled	raw_irqentry_exit_cond_resched
#define irqentry_exit_cond_resched_dynamic_disabled	NULL
DECLARE_STATIC_CALL(irqentry_exit_cond_resched, raw_irqentry_exit_cond_resched);
#define irqentry_exit_cond_resched()	static_call(irqentry_exit_cond_resched)()
#elif defined(CONFIG_HAVE_PREEMPT_DYNAMIC_KEY)
DECLARE_STATIC_KEY_TRUE(sk_dynamic_irqentry_exit_cond_resched);
void dynamic_irqentry_exit_cond_resched(void);
#define irqentry_exit_cond_resched()	dynamic_irqentry_exit_cond_resched()
#endif
#else 
#define irqentry_exit_cond_resched()	raw_irqentry_exit_cond_resched()
#endif 


void noinstr irqentry_exit(struct pt_regs *regs, irqentry_state_t state);


irqentry_state_t noinstr irqentry_nmi_enter(struct pt_regs *regs);


void noinstr irqentry_nmi_exit(struct pt_regs *regs, irqentry_state_t irq_state);

#endif
