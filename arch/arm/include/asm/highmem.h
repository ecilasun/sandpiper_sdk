/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_HIGHMEM_H
#define _ASM_HIGHMEM_H

#include <asm/cachetype.h>
#include <asm/fixmap.h>

#define PKMAP_BASE		(PAGE_OFFSET - PMD_SIZE)
#define LAST_PKMAP		PTRS_PER_PTE
#define LAST_PKMAP_MASK		(LAST_PKMAP - 1)
#define PKMAP_NR(virt)		(((virt) - PKMAP_BASE) >> PAGE_SHIFT)
#define PKMAP_ADDR(nr)		(PKMAP_BASE + ((nr) << PAGE_SHIFT))

#define flush_cache_kmaps() \
	do { \
		if (cache_is_vivt()) \
			flush_cache_all(); \
	} while (0)

extern pte_t *pkmap_page_table;


#define ARCH_NEEDS_KMAP_HIGH_GET
#if defined(CONFIG_SMP) && defined(CONFIG_CPU_TLB_V6)
#undef ARCH_NEEDS_KMAP_HIGH_GET
#if defined(CONFIG_HIGHMEM) && defined(CONFIG_CPU_CACHE_VIVT)
#error "The sum of features in your kernel config cannot be supported together"
#endif
#endif


#ifdef CONFIG_ARM_ERRATA_798181
#undef ARCH_NEEDS_KMAP_HIGH_GET
#endif

#ifdef ARCH_NEEDS_KMAP_HIGH_GET
extern void *kmap_high_get(struct page *page);

static inline void *arch_kmap_local_high_get(struct page *page)
{
	if (IS_ENABLED(CONFIG_DEBUG_HIGHMEM) && !cache_is_vivt())
		return NULL;
	return kmap_high_get(page);
}
#define arch_kmap_local_high_get arch_kmap_local_high_get

#else 
static inline void *kmap_high_get(struct page *page)
{
	return NULL;
}
#endif 

#define arch_kmap_local_post_map(vaddr, pteval)				\
	local_flush_tlb_kernel_page(vaddr)

#define arch_kmap_local_pre_unmap(vaddr)				\
do {									\
	if (cache_is_vivt())						\
		__cpuc_flush_dcache_area((void *)vaddr, PAGE_SIZE);	\
} while (0)

#define arch_kmap_local_post_unmap(vaddr)				\
	local_flush_tlb_kernel_page(vaddr)

#endif
