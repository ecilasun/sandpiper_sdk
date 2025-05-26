/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ELFCORE_H
#define _LINUX_ELFCORE_H

#include <linux/user.h>
#include <linux/bug.h>
#include <linux/sched/task_stack.h>
#include <linux/types.h>
#include <linux/signal.h>
#include <linux/time.h>
#include <linux/ptrace.h>
#include <linux/fs.h>
#include <linux/elf.h>

struct coredump_params;

struct elf_siginfo
{
	int	si_signo;			
	int	si_code;			
	int	si_errno;			
};


struct elf_prstatus_common
{
	struct elf_siginfo pr_info;	
	short	pr_cursig;		
	unsigned long pr_sigpend;	
	unsigned long pr_sighold;	
	pid_t	pr_pid;
	pid_t	pr_ppid;
	pid_t	pr_pgrp;
	pid_t	pr_sid;
	struct __kernel_old_timeval pr_utime;	
	struct __kernel_old_timeval pr_stime;	
	struct __kernel_old_timeval pr_cutime;	
	struct __kernel_old_timeval pr_cstime;	
};

struct elf_prstatus
{
	struct elf_prstatus_common common;
	elf_gregset_t pr_reg;	
	int pr_fpvalid;		
};

#define ELF_PRARGSZ	(80)	

struct elf_prpsinfo
{
	char	pr_state;	
	char	pr_sname;	
	char	pr_zomb;	
	char	pr_nice;	
	unsigned long pr_flag;	
	__kernel_uid_t	pr_uid;
	__kernel_gid_t	pr_gid;
	pid_t	pr_pid, pr_ppid, pr_pgrp, pr_sid;
	
	
	char	pr_fname[16];	
	char	pr_psargs[ELF_PRARGSZ];	
};

static inline void elf_core_copy_regs(elf_gregset_t *elfregs, struct pt_regs *regs)
{
#ifdef ELF_CORE_COPY_REGS
	ELF_CORE_COPY_REGS((*elfregs), regs)
#else
	BUG_ON(sizeof(*elfregs) != sizeof(*regs));
	*(struct pt_regs *)elfregs = *regs;
#endif
}

static inline int elf_core_copy_task_regs(struct task_struct *t, elf_gregset_t* elfregs)
{
#if defined (ELF_CORE_COPY_TASK_REGS)
	return ELF_CORE_COPY_TASK_REGS(t, elfregs);
#else
	elf_core_copy_regs(elfregs, task_pt_regs(t));
#endif
	return 0;
}

int elf_core_copy_task_fpregs(struct task_struct *t, elf_fpregset_t *fpu);

#ifdef CONFIG_ARCH_BINFMT_ELF_EXTRA_PHDRS

extern Elf_Half elf_core_extra_phdrs(struct coredump_params *cprm);
extern int
elf_core_write_extra_phdrs(struct coredump_params *cprm, loff_t offset);
extern int
elf_core_write_extra_data(struct coredump_params *cprm);
extern size_t elf_core_extra_data_size(struct coredump_params *cprm);
#else
static inline Elf_Half elf_core_extra_phdrs(struct coredump_params *cprm)
{
	return 0;
}

static inline int elf_core_write_extra_phdrs(struct coredump_params *cprm, loff_t offset)
{
	return 1;
}

static inline int elf_core_write_extra_data(struct coredump_params *cprm)
{
	return 1;
}

static inline size_t elf_core_extra_data_size(struct coredump_params *cprm)
{
	return 0;
}
#endif 

#endif 
