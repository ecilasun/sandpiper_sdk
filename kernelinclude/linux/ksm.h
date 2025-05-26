/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_KSM_H
#define __LINUX_KSM_H


#include <linux/bitops.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/rmap.h>
#include <linux/sched.h>
#include <linux/sched/coredump.h>

#ifdef CONFIG_KSM
int ksm_madvise(struct vm_area_struct *vma, unsigned long start,
		unsigned long end, int advice, unsigned long *vm_flags);

void ksm_add_vma(struct vm_area_struct *vma);
int ksm_enable_merge_any(struct mm_struct *mm);
int ksm_disable_merge_any(struct mm_struct *mm);
int ksm_disable(struct mm_struct *mm);

int __ksm_enter(struct mm_struct *mm);
void __ksm_exit(struct mm_struct *mm);

#define is_ksm_zero_pte(pte)	(is_zero_pfn(pte_pfn(pte)) && pte_dirty(pte))

extern atomic_long_t ksm_zero_pages;

static inline void ksm_map_zero_page(struct mm_struct *mm)
{
	atomic_long_inc(&ksm_zero_pages);
	atomic_long_inc(&mm->ksm_zero_pages);
}

static inline void ksm_might_unmap_zero_page(struct mm_struct *mm, pte_t pte)
{
	if (is_ksm_zero_pte(pte)) {
		atomic_long_dec(&ksm_zero_pages);
		atomic_long_dec(&mm->ksm_zero_pages);
	}
}

static inline long mm_ksm_zero_pages(struct mm_struct *mm)
{
	return atomic_long_read(&mm->ksm_zero_pages);
}

static inline int ksm_fork(struct mm_struct *mm, struct mm_struct *oldmm)
{
	int ret;

	if (test_bit(MMF_VM_MERGEABLE, &oldmm->flags)) {
		ret = __ksm_enter(mm);
		if (ret)
			return ret;
	}

	if (test_bit(MMF_VM_MERGE_ANY, &oldmm->flags))
		set_bit(MMF_VM_MERGE_ANY, &mm->flags);

	return 0;
}

static inline void ksm_exit(struct mm_struct *mm)
{
	if (test_bit(MMF_VM_MERGEABLE, &mm->flags))
		__ksm_exit(mm);
}


struct page *ksm_might_need_to_copy(struct page *page,
			struct vm_area_struct *vma, unsigned long address);

void rmap_walk_ksm(struct folio *folio, struct rmap_walk_control *rwc);
void folio_migrate_ksm(struct folio *newfolio, struct folio *folio);

#ifdef CONFIG_MEMORY_FAILURE
void collect_procs_ksm(struct page *page, struct list_head *to_kill,
		       int force_early);
#endif

#ifdef CONFIG_PROC_FS
long ksm_process_profit(struct mm_struct *);
#endif 

#else  

static inline void ksm_add_vma(struct vm_area_struct *vma)
{
}

static inline int ksm_disable(struct mm_struct *mm)
{
	return 0;
}

static inline int ksm_fork(struct mm_struct *mm, struct mm_struct *oldmm)
{
	return 0;
}

static inline void ksm_exit(struct mm_struct *mm)
{
}

static inline void ksm_might_unmap_zero_page(struct mm_struct *mm, pte_t pte)
{
}

#ifdef CONFIG_MEMORY_FAILURE
static inline void collect_procs_ksm(struct page *page,
				     struct list_head *to_kill, int force_early)
{
}
#endif

#ifdef CONFIG_MMU
static inline int ksm_madvise(struct vm_area_struct *vma, unsigned long start,
		unsigned long end, int advice, unsigned long *vm_flags)
{
	return 0;
}

static inline struct page *ksm_might_need_to_copy(struct page *page,
			struct vm_area_struct *vma, unsigned long address)
{
	return page;
}

static inline void rmap_walk_ksm(struct folio *folio,
			struct rmap_walk_control *rwc)
{
}

static inline void folio_migrate_ksm(struct folio *newfolio, struct folio *old)
{
}
#endif 
#endif 

#endif 
