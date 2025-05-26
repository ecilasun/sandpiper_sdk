/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASMARM_TLB_H
#define __ASMARM_TLB_H

#include <asm/cacheflush.h>

#ifndef CONFIG_MMU

#include <linux/pagemap.h>

#define tlb_flush(tlb)	((void) tlb)

#include <asm-generic/tlb.h>

#else 

#include <linux/swap.h>
#include <asm/tlbflush.h>

static inline void __tlb_remove_table(void *_table)
{
	free_page_and_swap_cache((struct page *)_table);
}

#include <asm-generic/tlb.h>

static inline void
__pte_free_tlb(struct mmu_gather *tlb, pgtable_t pte, unsigned long addr)
{
	struct ptdesc *ptdesc = page_ptdesc(pte);

	pagetable_pte_dtor(ptdesc);

#ifndef CONFIG_ARM_LPAE
	
	addr = (addr & PMD_MASK) + SZ_1M;
	__tlb_adjust_range(tlb, addr - PAGE_SIZE, 2 * PAGE_SIZE);
#endif

	tlb_remove_ptdesc(tlb, ptdesc);
}

static inline void
__pmd_free_tlb(struct mmu_gather *tlb, pmd_t *pmdp, unsigned long addr)
{
#ifdef CONFIG_ARM_LPAE
	struct ptdesc *ptdesc = virt_to_ptdesc(pmdp);

	pagetable_pmd_dtor(ptdesc);
	tlb_remove_ptdesc(tlb, ptdesc);
#endif
}

#endif 
#endif
