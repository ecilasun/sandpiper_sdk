/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_STACKTRACE_H
#define __LINUX_STACKTRACE_H

#include <linux/types.h>
#include <asm/errno.h>

struct task_struct;
struct pt_regs;

#ifdef CONFIG_ARCH_STACKWALK


typedef bool (*stack_trace_consume_fn)(void *cookie, unsigned long addr);

void arch_stack_walk(stack_trace_consume_fn consume_entry, void *cookie,
		     struct task_struct *task, struct pt_regs *regs);


int arch_stack_walk_reliable(stack_trace_consume_fn consume_entry, void *cookie,
			     struct task_struct *task);

void arch_stack_walk_user(stack_trace_consume_fn consume_entry, void *cookie,
			  const struct pt_regs *regs);
#endif 

#ifdef CONFIG_STACKTRACE
void stack_trace_print(const unsigned long *trace, unsigned int nr_entries,
		       int spaces);
int stack_trace_snprint(char *buf, size_t size, const unsigned long *entries,
			unsigned int nr_entries, int spaces);
unsigned int stack_trace_save(unsigned long *store, unsigned int size,
			      unsigned int skipnr);
unsigned int stack_trace_save_tsk(struct task_struct *task,
				  unsigned long *store, unsigned int size,
				  unsigned int skipnr);
unsigned int stack_trace_save_regs(struct pt_regs *regs, unsigned long *store,
				   unsigned int size, unsigned int skipnr);
unsigned int stack_trace_save_user(unsigned long *store, unsigned int size);
unsigned int filter_irq_stacks(unsigned long *entries, unsigned int nr_entries);

#ifndef CONFIG_ARCH_STACKWALK

struct stack_trace {
	unsigned int nr_entries, max_entries;
	unsigned long *entries;
	unsigned int skip;	
};

extern void save_stack_trace(struct stack_trace *trace);
extern void save_stack_trace_regs(struct pt_regs *regs,
				  struct stack_trace *trace);
extern void save_stack_trace_tsk(struct task_struct *tsk,
				struct stack_trace *trace);
extern int save_stack_trace_tsk_reliable(struct task_struct *tsk,
					 struct stack_trace *trace);
extern void save_stack_trace_user(struct stack_trace *trace);
#endif 
#endif 

#if defined(CONFIG_STACKTRACE) && defined(CONFIG_HAVE_RELIABLE_STACKTRACE)
int stack_trace_save_tsk_reliable(struct task_struct *tsk, unsigned long *store,
				  unsigned int size);
#else
static inline int stack_trace_save_tsk_reliable(struct task_struct *tsk,
						unsigned long *store,
						unsigned int size)
{
	return -ENOSYS;
}
#endif

#endif 
