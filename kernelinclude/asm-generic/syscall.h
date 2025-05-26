/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _ASM_SYSCALL_H
#define _ASM_SYSCALL_H	1

struct task_struct;
struct pt_regs;


int syscall_get_nr(struct task_struct *task, struct pt_regs *regs);


void syscall_rollback(struct task_struct *task, struct pt_regs *regs);


long syscall_get_error(struct task_struct *task, struct pt_regs *regs);


long syscall_get_return_value(struct task_struct *task, struct pt_regs *regs);


void syscall_set_return_value(struct task_struct *task, struct pt_regs *regs,
			      int error, long val);


void syscall_get_arguments(struct task_struct *task, struct pt_regs *regs,
			   unsigned long *args);


int syscall_get_arch(struct task_struct *task);
#endif	
