/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_JOBCTL_H
#define _LINUX_SCHED_JOBCTL_H

#include <linux/types.h>

struct task_struct;


#define JOBCTL_STOP_SIGMASK	0xffff	

#define JOBCTL_STOP_DEQUEUED_BIT 16	
#define JOBCTL_STOP_PENDING_BIT	17	
#define JOBCTL_STOP_CONSUME_BIT	18	
#define JOBCTL_TRAP_STOP_BIT	19	
#define JOBCTL_TRAP_NOTIFY_BIT	20	
#define JOBCTL_TRAPPING_BIT	21	
#define JOBCTL_LISTENING_BIT	22	
#define JOBCTL_TRAP_FREEZE_BIT	23	
#define JOBCTL_PTRACE_FROZEN_BIT	24	

#define JOBCTL_STOPPED_BIT	26	
#define JOBCTL_TRACED_BIT	27	

#define JOBCTL_STOP_DEQUEUED	(1UL << JOBCTL_STOP_DEQUEUED_BIT)
#define JOBCTL_STOP_PENDING	(1UL << JOBCTL_STOP_PENDING_BIT)
#define JOBCTL_STOP_CONSUME	(1UL << JOBCTL_STOP_CONSUME_BIT)
#define JOBCTL_TRAP_STOP	(1UL << JOBCTL_TRAP_STOP_BIT)
#define JOBCTL_TRAP_NOTIFY	(1UL << JOBCTL_TRAP_NOTIFY_BIT)
#define JOBCTL_TRAPPING		(1UL << JOBCTL_TRAPPING_BIT)
#define JOBCTL_LISTENING	(1UL << JOBCTL_LISTENING_BIT)
#define JOBCTL_TRAP_FREEZE	(1UL << JOBCTL_TRAP_FREEZE_BIT)
#define JOBCTL_PTRACE_FROZEN	(1UL << JOBCTL_PTRACE_FROZEN_BIT)

#define JOBCTL_STOPPED		(1UL << JOBCTL_STOPPED_BIT)
#define JOBCTL_TRACED		(1UL << JOBCTL_TRACED_BIT)

#define JOBCTL_TRAP_MASK	(JOBCTL_TRAP_STOP | JOBCTL_TRAP_NOTIFY)
#define JOBCTL_PENDING_MASK	(JOBCTL_STOP_PENDING | JOBCTL_TRAP_MASK)

extern bool task_set_jobctl_pending(struct task_struct *task, unsigned long mask);
extern void task_clear_jobctl_trapping(struct task_struct *task);
extern void task_clear_jobctl_pending(struct task_struct *task, unsigned long mask);

#endif 
