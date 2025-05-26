/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TRACE_RECURSION_H
#define _LINUX_TRACE_RECURSION_H

#include <linux/interrupt.h>
#include <linux/sched.h>

#ifdef CONFIG_TRACING




enum {
	
	TRACE_FTRACE_BIT,
	TRACE_FTRACE_NMI_BIT,
	TRACE_FTRACE_IRQ_BIT,
	TRACE_FTRACE_SIRQ_BIT,
	TRACE_FTRACE_TRANSITION_BIT,

	
	TRACE_INTERNAL_BIT,
	TRACE_INTERNAL_NMI_BIT,
	TRACE_INTERNAL_IRQ_BIT,
	TRACE_INTERNAL_SIRQ_BIT,
	TRACE_INTERNAL_TRANSITION_BIT,

	TRACE_BRANCH_BIT,

	TRACE_IRQ_BIT,

	
	TRACE_GRAPH_BIT,

	

	TRACE_GRAPH_DEPTH_START_BIT,
	TRACE_GRAPH_DEPTH_END_BIT,

	
	TRACE_GRAPH_NOTRACE_BIT,

	
	TRACE_RECORD_RECURSION_BIT,
};

#define trace_recursion_set(bit)	do { (current)->trace_recursion |= (1<<(bit)); } while (0)
#define trace_recursion_clear(bit)	do { (current)->trace_recursion &= ~(1<<(bit)); } while (0)
#define trace_recursion_test(bit)	((current)->trace_recursion & (1<<(bit)))

#define trace_recursion_depth() \
	(((current)->trace_recursion >> TRACE_GRAPH_DEPTH_START_BIT) & 3)
#define trace_recursion_set_depth(depth) \
	do {								\
		current->trace_recursion &=				\
			~(3 << TRACE_GRAPH_DEPTH_START_BIT);		\
		current->trace_recursion |=				\
			((depth) & 3) << TRACE_GRAPH_DEPTH_START_BIT;	\
	} while (0)

#define TRACE_CONTEXT_BITS	4

#define TRACE_FTRACE_START	TRACE_FTRACE_BIT

#define TRACE_LIST_START	TRACE_INTERNAL_BIT

#define TRACE_CONTEXT_MASK	((1 << (TRACE_LIST_START + TRACE_CONTEXT_BITS)) - 1)


enum {
	TRACE_CTX_NMI,
	TRACE_CTX_IRQ,
	TRACE_CTX_SOFTIRQ,
	TRACE_CTX_NORMAL,
	TRACE_CTX_TRANSITION,
};

static __always_inline int trace_get_context_bit(void)
{
	unsigned char bit = interrupt_context_level();

	return TRACE_CTX_NORMAL - bit;
}

#ifdef CONFIG_FTRACE_RECORD_RECURSION
extern void ftrace_record_recursion(unsigned long ip, unsigned long parent_ip);
# define do_ftrace_record_recursion(ip, pip)				\
	do {								\
		if (!trace_recursion_test(TRACE_RECORD_RECURSION_BIT)) { \
			trace_recursion_set(TRACE_RECORD_RECURSION_BIT); \
			ftrace_record_recursion(ip, pip);		\
			trace_recursion_clear(TRACE_RECORD_RECURSION_BIT); \
		}							\
	} while (0)
#else
# define do_ftrace_record_recursion(ip, pip)	do { } while (0)
#endif

#ifdef CONFIG_ARCH_WANTS_NO_INSTR
# define trace_warn_on_no_rcu(ip)					\
	({								\
		bool __ret = !rcu_is_watching();			\
		if (__ret && !trace_recursion_test(TRACE_RECORD_RECURSION_BIT)) { \
			trace_recursion_set(TRACE_RECORD_RECURSION_BIT); \
			WARN_ONCE(true, "RCU not on for: %pS\n", (void *)ip); \
			trace_recursion_clear(TRACE_RECORD_RECURSION_BIT); \
		}							\
		__ret;							\
	})
#else
# define trace_warn_on_no_rcu(ip)	false
#endif


static __always_inline int trace_test_and_set_recursion(unsigned long ip, unsigned long pip,
							int start)
{
	unsigned int val = READ_ONCE(current->trace_recursion);
	int bit;

	if (trace_warn_on_no_rcu(ip))
		return -1;

	bit = trace_get_context_bit() + start;
	if (unlikely(val & (1 << bit))) {
		
		bit = TRACE_CTX_TRANSITION + start;
		if (val & (1 << bit)) {
			do_ftrace_record_recursion(ip, pip);
			return -1;
		}
	}

	val |= 1 << bit;
	current->trace_recursion = val;
	barrier();

	preempt_disable_notrace();

	return bit;
}


static __always_inline void trace_clear_recursion(int bit)
{
	preempt_enable_notrace();
	barrier();
	trace_recursion_clear(bit);
}


static __always_inline int ftrace_test_recursion_trylock(unsigned long ip,
							 unsigned long parent_ip)
{
	return trace_test_and_set_recursion(ip, parent_ip, TRACE_FTRACE_START);
}


static __always_inline void ftrace_test_recursion_unlock(int bit)
{
	trace_clear_recursion(bit);
}

#endif 
#endif 
