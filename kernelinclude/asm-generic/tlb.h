/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ASM_GENERIC__TLB_H
#define _ASM_GENERIC__TLB_H

#include <linux/mmu_notifier.h>
#include <linux/swap.h>
#include <linux/hugetlb_inline.h>
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>


#ifndef nmi_uaccess_okay
# define nmi_uaccess_okay() true
#endif

#ifdef CONFIG_MMU



#ifdef CONFIG_MMU_GATHER_TABLE_FREE

struct mmu_table_batch {
#ifdef CONFIG_MMU_GATHER_RCU_TABLE_FREE
	struct rcu_head		rcu;
#endif
	unsigned int		nr;
	void			*tables[];
};

#define MAX_TABLE_BATCH		\
	((PAGE_SIZE - sizeof(struct mmu_table_batch)) / sizeof(void *))

extern void tlb_remove_table(struct mmu_gather *tlb, void *table);

#else 


#define tlb_remove_table(tlb, page) tlb_remove_page((tlb), (page))

#endif 

#ifdef CONFIG_MMU_GATHER_RCU_TABLE_FREE

#ifndef tlb_needs_table_invalidate
#define tlb_needs_table_invalidate() (true)
#endif

void tlb_remove_table_sync_one(void);

#else

#ifdef tlb_needs_table_invalidate
#error tlb_needs_table_invalidate() requires MMU_GATHER_RCU_TABLE_FREE
#endif

static inline void tlb_remove_table_sync_one(void) { }

#endif 


#ifndef CONFIG_MMU_GATHER_NO_GATHER

#define MMU_GATHER_BUNDLE	8

struct mmu_gather_batch {
	struct mmu_gather_batch	*next;
	unsigned int		nr;
	unsigned int		max;
	struct encoded_page	*encoded_pages[];
};

#define MAX_GATHER_BATCH	\
	((PAGE_SIZE - sizeof(struct mmu_gather_batch)) / sizeof(void *))


#define MAX_GATHER_BATCH_COUNT	(10000UL/MAX_GATHER_BATCH)

extern bool __tlb_remove_page_size(struct mmu_gather *tlb,
				   struct encoded_page *page,
				   int page_size);

#ifdef CONFIG_SMP

#define tlb_delay_rmap(tlb) (((tlb)->delayed_rmap = 1), true)
extern void tlb_flush_rmaps(struct mmu_gather *tlb, struct vm_area_struct *vma);
#endif

#endif


#ifndef tlb_delay_rmap
#define tlb_delay_rmap(tlb) (false)
static inline void tlb_flush_rmaps(struct mmu_gather *tlb, struct vm_area_struct *vma) { }
#endif


struct mmu_gather {
	struct mm_struct	*mm;

#ifdef CONFIG_MMU_GATHER_TABLE_FREE
	struct mmu_table_batch	*batch;
#endif

	unsigned long		start;
	unsigned long		end;
	
	unsigned int		fullmm : 1;

	
	unsigned int		need_flush_all : 1;

	
	unsigned int		freed_tables : 1;

	
	unsigned int		delayed_rmap : 1;

	
	unsigned int		cleared_ptes : 1;
	unsigned int		cleared_pmds : 1;
	unsigned int		cleared_puds : 1;
	unsigned int		cleared_p4ds : 1;

	
	unsigned int		vma_exec : 1;
	unsigned int		vma_huge : 1;
	unsigned int		vma_pfn  : 1;

	unsigned int		batch_count;

#ifndef CONFIG_MMU_GATHER_NO_GATHER
	struct mmu_gather_batch *active;
	struct mmu_gather_batch	local;
	struct page		*__pages[MMU_GATHER_BUNDLE];

#ifdef CONFIG_MMU_GATHER_PAGE_SIZE
	unsigned int page_size;
#endif
#endif
};

void tlb_flush_mmu(struct mmu_gather *tlb);

static inline void __tlb_adjust_range(struct mmu_gather *tlb,
				      unsigned long address,
				      unsigned int range_size)
{
	tlb->start = min(tlb->start, address);
	tlb->end = max(tlb->end, address + range_size);
}

static inline void __tlb_reset_range(struct mmu_gather *tlb)
{
	if (tlb->fullmm) {
		tlb->start = tlb->end = ~0;
	} else {
		tlb->start = TASK_SIZE;
		tlb->end = 0;
	}
	tlb->freed_tables = 0;
	tlb->cleared_ptes = 0;
	tlb->cleared_pmds = 0;
	tlb->cleared_puds = 0;
	tlb->cleared_p4ds = 0;
	
}

#ifdef CONFIG_MMU_GATHER_NO_RANGE

#if defined(tlb_flush)
#error MMU_GATHER_NO_RANGE relies on default tlb_flush()
#endif


static inline void tlb_flush(struct mmu_gather *tlb)
{
	if (tlb->end)
		flush_tlb_mm(tlb->mm);
}

#else 

#ifndef tlb_flush

static inline void tlb_flush(struct mmu_gather *tlb)
{
	if (tlb->fullmm || tlb->need_flush_all) {
		flush_tlb_mm(tlb->mm);
	} else if (tlb->end) {
		struct vm_area_struct vma = {
			.vm_mm = tlb->mm,
			.vm_flags = (tlb->vma_exec ? VM_EXEC    : 0) |
				    (tlb->vma_huge ? VM_HUGETLB : 0),
		};

		flush_tlb_range(&vma, tlb->start, tlb->end);
	}
}
#endif

#endif 

static inline void
tlb_update_vma_flags(struct mmu_gather *tlb, struct vm_area_struct *vma)
{
	
	tlb->vma_huge = is_vm_hugetlb_page(vma);
	tlb->vma_exec = !!(vma->vm_flags & VM_EXEC);
	tlb->vma_pfn  = !!(vma->vm_flags & (VM_PFNMAP|VM_MIXEDMAP));
}

static inline void tlb_flush_mmu_tlbonly(struct mmu_gather *tlb)
{
	
	if (!(tlb->freed_tables || tlb->cleared_ptes || tlb->cleared_pmds ||
	      tlb->cleared_puds || tlb->cleared_p4ds))
		return;

	tlb_flush(tlb);
	__tlb_reset_range(tlb);
}

static inline void tlb_remove_page_size(struct mmu_gather *tlb,
					struct page *page, int page_size)
{
	if (__tlb_remove_page_size(tlb, encode_page(page, 0), page_size))
		tlb_flush_mmu(tlb);
}

static __always_inline bool __tlb_remove_page(struct mmu_gather *tlb, struct page *page, unsigned int flags)
{
	return __tlb_remove_page_size(tlb, encode_page(page, flags), PAGE_SIZE);
}


static inline void tlb_remove_page(struct mmu_gather *tlb, struct page *page)
{
	return tlb_remove_page_size(tlb, page, PAGE_SIZE);
}

static inline void tlb_remove_ptdesc(struct mmu_gather *tlb, void *pt)
{
	tlb_remove_table(tlb, pt);
}


static inline void tlb_remove_page_ptdesc(struct mmu_gather *tlb, struct ptdesc *pt)
{
	tlb_remove_page(tlb, ptdesc_page(pt));
}

static inline void tlb_change_page_size(struct mmu_gather *tlb,
						     unsigned int page_size)
{
#ifdef CONFIG_MMU_GATHER_PAGE_SIZE
	if (tlb->page_size && tlb->page_size != page_size) {
		if (!tlb->fullmm && !tlb->need_flush_all)
			tlb_flush_mmu(tlb);
	}

	tlb->page_size = page_size;
#endif
}

static inline unsigned long tlb_get_unmap_shift(struct mmu_gather *tlb)
{
	if (tlb->cleared_ptes)
		return PAGE_SHIFT;
	if (tlb->cleared_pmds)
		return PMD_SHIFT;
	if (tlb->cleared_puds)
		return PUD_SHIFT;
	if (tlb->cleared_p4ds)
		return P4D_SHIFT;

	return PAGE_SHIFT;
}

static inline unsigned long tlb_get_unmap_size(struct mmu_gather *tlb)
{
	return 1UL << tlb_get_unmap_shift(tlb);
}


static inline void tlb_start_vma(struct mmu_gather *tlb, struct vm_area_struct *vma)
{
	if (tlb->fullmm)
		return;

	tlb_update_vma_flags(tlb, vma);
#ifndef CONFIG_MMU_GATHER_NO_FLUSH_CACHE
	flush_cache_range(vma, vma->vm_start, vma->vm_end);
#endif
}

static inline void tlb_end_vma(struct mmu_gather *tlb, struct vm_area_struct *vma)
{
	if (tlb->fullmm)
		return;

	
	if (tlb->vma_pfn || !IS_ENABLED(CONFIG_MMU_GATHER_MERGE_VMAS)) {
		
		tlb_flush_mmu_tlbonly(tlb);
	}
}


static inline void tlb_flush_pte_range(struct mmu_gather *tlb,
				     unsigned long address, unsigned long size)
{
	__tlb_adjust_range(tlb, address, size);
	tlb->cleared_ptes = 1;
}

static inline void tlb_flush_pmd_range(struct mmu_gather *tlb,
				     unsigned long address, unsigned long size)
{
	__tlb_adjust_range(tlb, address, size);
	tlb->cleared_pmds = 1;
}

static inline void tlb_flush_pud_range(struct mmu_gather *tlb,
				     unsigned long address, unsigned long size)
{
	__tlb_adjust_range(tlb, address, size);
	tlb->cleared_puds = 1;
}

static inline void tlb_flush_p4d_range(struct mmu_gather *tlb,
				     unsigned long address, unsigned long size)
{
	__tlb_adjust_range(tlb, address, size);
	tlb->cleared_p4ds = 1;
}

#ifndef __tlb_remove_tlb_entry
#define __tlb_remove_tlb_entry(tlb, ptep, address) do { } while (0)
#endif


#define tlb_remove_tlb_entry(tlb, ptep, address)		\
	do {							\
		tlb_flush_pte_range(tlb, address, PAGE_SIZE);	\
		__tlb_remove_tlb_entry(tlb, ptep, address);	\
	} while (0)

#define tlb_remove_huge_tlb_entry(h, tlb, ptep, address)	\
	do {							\
		unsigned long _sz = huge_page_size(h);		\
		if (_sz >= P4D_SIZE)				\
			tlb_flush_p4d_range(tlb, address, _sz);	\
		else if (_sz >= PUD_SIZE)			\
			tlb_flush_pud_range(tlb, address, _sz);	\
		else if (_sz >= PMD_SIZE)			\
			tlb_flush_pmd_range(tlb, address, _sz);	\
		else						\
			tlb_flush_pte_range(tlb, address, _sz);	\
		__tlb_remove_tlb_entry(tlb, ptep, address);	\
	} while (0)


#ifndef __tlb_remove_pmd_tlb_entry
#define __tlb_remove_pmd_tlb_entry(tlb, pmdp, address) do {} while (0)
#endif

#define tlb_remove_pmd_tlb_entry(tlb, pmdp, address)			\
	do {								\
		tlb_flush_pmd_range(tlb, address, HPAGE_PMD_SIZE);	\
		__tlb_remove_pmd_tlb_entry(tlb, pmdp, address);		\
	} while (0)


#ifndef __tlb_remove_pud_tlb_entry
#define __tlb_remove_pud_tlb_entry(tlb, pudp, address) do {} while (0)
#endif

#define tlb_remove_pud_tlb_entry(tlb, pudp, address)			\
	do {								\
		tlb_flush_pud_range(tlb, address, HPAGE_PUD_SIZE);	\
		__tlb_remove_pud_tlb_entry(tlb, pudp, address);		\
	} while (0)



#ifndef pte_free_tlb
#define pte_free_tlb(tlb, ptep, address)			\
	do {							\
		tlb_flush_pmd_range(tlb, address, PAGE_SIZE);	\
		tlb->freed_tables = 1;				\
		__pte_free_tlb(tlb, ptep, address);		\
	} while (0)
#endif

#ifndef pmd_free_tlb
#define pmd_free_tlb(tlb, pmdp, address)			\
	do {							\
		tlb_flush_pud_range(tlb, address, PAGE_SIZE);	\
		tlb->freed_tables = 1;				\
		__pmd_free_tlb(tlb, pmdp, address);		\
	} while (0)
#endif

#ifndef pud_free_tlb
#define pud_free_tlb(tlb, pudp, address)			\
	do {							\
		tlb_flush_p4d_range(tlb, address, PAGE_SIZE);	\
		tlb->freed_tables = 1;				\
		__pud_free_tlb(tlb, pudp, address);		\
	} while (0)
#endif

#ifndef p4d_free_tlb
#define p4d_free_tlb(tlb, pudp, address)			\
	do {							\
		__tlb_adjust_range(tlb, address, PAGE_SIZE);	\
		tlb->freed_tables = 1;				\
		__p4d_free_tlb(tlb, pudp, address);		\
	} while (0)
#endif

#ifndef pte_needs_flush
static inline bool pte_needs_flush(pte_t oldpte, pte_t newpte)
{
	return true;
}
#endif

#ifndef huge_pmd_needs_flush
static inline bool huge_pmd_needs_flush(pmd_t oldpmd, pmd_t newpmd)
{
	return true;
}
#endif

#endif 

#endif 
