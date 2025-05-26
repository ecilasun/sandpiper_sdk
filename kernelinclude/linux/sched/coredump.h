/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_COREDUMP_H
#define _LINUX_SCHED_COREDUMP_H

#include <linux/mm_types.h>

#define SUID_DUMP_DISABLE	0	
#define SUID_DUMP_USER		1	
#define SUID_DUMP_ROOT		2	




#define MMF_DUMPABLE_BITS 2
#define MMF_DUMPABLE_MASK ((1 << MMF_DUMPABLE_BITS) - 1)

extern void set_dumpable(struct mm_struct *mm, int value);

static inline int __get_dumpable(unsigned long mm_flags)
{
	return mm_flags & MMF_DUMPABLE_MASK;
}

static inline int get_dumpable(struct mm_struct *mm)
{
	return __get_dumpable(mm->flags);
}


#define MMF_DUMP_ANON_PRIVATE	2
#define MMF_DUMP_ANON_SHARED	3
#define MMF_DUMP_MAPPED_PRIVATE	4
#define MMF_DUMP_MAPPED_SHARED	5
#define MMF_DUMP_ELF_HEADERS	6
#define MMF_DUMP_HUGETLB_PRIVATE 7
#define MMF_DUMP_HUGETLB_SHARED  8
#define MMF_DUMP_DAX_PRIVATE	9
#define MMF_DUMP_DAX_SHARED	10

#define MMF_DUMP_FILTER_SHIFT	MMF_DUMPABLE_BITS
#define MMF_DUMP_FILTER_BITS	9
#define MMF_DUMP_FILTER_MASK \
	(((1 << MMF_DUMP_FILTER_BITS) - 1) << MMF_DUMP_FILTER_SHIFT)
#define MMF_DUMP_FILTER_DEFAULT \
	((1 << MMF_DUMP_ANON_PRIVATE) |	(1 << MMF_DUMP_ANON_SHARED) |\
	 (1 << MMF_DUMP_HUGETLB_PRIVATE) | MMF_DUMP_MASK_DEFAULT_ELF)

#ifdef CONFIG_CORE_DUMP_DEFAULT_ELF_HEADERS
# define MMF_DUMP_MASK_DEFAULT_ELF	(1 << MMF_DUMP_ELF_HEADERS)
#else
# define MMF_DUMP_MASK_DEFAULT_ELF	0
#endif
					
#define MMF_VM_MERGEABLE	16	
#define MMF_VM_HUGEPAGE		17	

//#define MMF_EXE_FILE_CHANGED	18	

#define MMF_HAS_UPROBES		19	
#define MMF_RECALC_UPROBES	20	
#define MMF_OOM_SKIP		21	
#define MMF_UNSTABLE		22	
#define MMF_HUGE_ZERO_PAGE	23      
#define MMF_DISABLE_THP		24	
#define MMF_OOM_REAP_QUEUED	25	
#define MMF_MULTIPROCESS	26	

#define MMF_HAS_PINNED		27	

#define MMF_HAS_MDWE		28
#define MMF_HAS_MDWE_MASK	(1 << MMF_HAS_MDWE)

#define MMF_DISABLE_THP_MASK	(1 << MMF_DISABLE_THP)

#define MMF_INIT_MASK		(MMF_DUMPABLE_MASK | MMF_DUMP_FILTER_MASK |\
				 MMF_DISABLE_THP_MASK | MMF_HAS_MDWE_MASK)

#define MMF_VM_MERGE_ANY	29
#define MMF_HAS_MDWE_NO_INHERIT	30

static inline unsigned long mmf_init_flags(unsigned long flags)
{
	if (flags & (1UL << MMF_HAS_MDWE_NO_INHERIT))
		flags &= ~((1UL << MMF_HAS_MDWE) |
			   (1UL << MMF_HAS_MDWE_NO_INHERIT));
	return flags & MMF_INIT_MASK;
}

#endif 
