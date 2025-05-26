/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_STACKLEAK_H
#define _LINUX_STACKLEAK_H

#include <linux/sched.h>
#include <linux/sched/task_stack.h>


#define STACKLEAK_POISON -0xBEEF
#define STACKLEAK_SEARCH_DEPTH 128

#ifdef CONFIG_GCC_PLUGIN_STACKLEAK
#include <asm/stacktrace.h>


static __always_inline unsigned long
stackleak_task_low_bound(const struct task_struct *tsk)
{
	
	return (unsigned long)end_of_stack(tsk) + sizeof(unsigned long);
}


static __always_inline unsigned long
stackleak_task_high_bound(const struct task_struct *tsk)
{
	
	return (unsigned long)task_pt_regs(tsk);
}


static __always_inline unsigned long
stackleak_find_top_of_poison(const unsigned long low, const unsigned long high)
{
	const unsigned int depth = STACKLEAK_SEARCH_DEPTH / sizeof(unsigned long);
	unsigned int poison_count = 0;
	unsigned long poison_high = high;
	unsigned long sp = high;

	while (sp > low && poison_count < depth) {
		sp -= sizeof(unsigned long);

		if (*(unsigned long *)sp == STACKLEAK_POISON) {
			poison_count++;
		} else {
			poison_count = 0;
			poison_high = sp;
		}
	}

	return poison_high;
}

static inline void stackleak_task_init(struct task_struct *t)
{
	t->lowest_stack = stackleak_task_low_bound(t);
# ifdef CONFIG_STACKLEAK_METRICS
	t->prev_lowest_stack = t->lowest_stack;
# endif
}

#else 
static inline void stackleak_task_init(struct task_struct *t) { }
#endif

#endif
