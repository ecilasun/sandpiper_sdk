/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_PROCINFO_H
#define __ASM_PROCINFO_H

#ifdef __KERNEL__

struct cpu_tlb_fns;
struct cpu_user_fns;
struct cpu_cache_fns;
struct processor;


struct proc_info_list {
	unsigned int		cpu_val;
	unsigned int		cpu_mask;
	unsigned long		__cpu_mm_mmu_flags;	
	unsigned long		__cpu_io_mmu_flags;	
	unsigned long		__cpu_flush;		
	const char		*arch_name;
	const char		*elf_name;
	unsigned int		elf_hwcap;
	const char		*cpu_name;
	struct processor	*proc;
	struct cpu_tlb_fns	*tlb;
	struct cpu_user_fns	*user;
	struct cpu_cache_fns	*cache;
};

#else	
#include <asm/elf.h>
#warning "Please include asm/elf.h instead"
#endif	
#endif
