/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PAGEBLOCK_FLAGS_H
#define PAGEBLOCK_FLAGS_H

#include <linux/types.h>

#define PB_migratetype_bits 3

enum pageblock_bits {
	PB_migrate,
	PB_migrate_end = PB_migrate + PB_migratetype_bits - 1,
			
	PB_migrate_skip,

	
	NR_PAGEBLOCK_BITS
};

#ifdef CONFIG_HUGETLB_PAGE

#ifdef CONFIG_HUGETLB_PAGE_SIZE_VARIABLE


extern unsigned int pageblock_order;

#else 


#define pageblock_order		min_t(unsigned int, HUGETLB_PAGE_ORDER, MAX_ORDER)

#endif 

#else 


#define pageblock_order		MAX_ORDER

#endif 

#define pageblock_nr_pages	(1UL << pageblock_order)
#define pageblock_align(pfn)	ALIGN((pfn), pageblock_nr_pages)
#define pageblock_aligned(pfn)	IS_ALIGNED((pfn), pageblock_nr_pages)
#define pageblock_start_pfn(pfn)	ALIGN_DOWN((pfn), pageblock_nr_pages)
#define pageblock_end_pfn(pfn)		ALIGN((pfn) + 1, pageblock_nr_pages)


struct page;

unsigned long get_pfnblock_flags_mask(const struct page *page,
				unsigned long pfn,
				unsigned long mask);

void set_pfnblock_flags_mask(struct page *page,
				unsigned long flags,
				unsigned long pfn,
				unsigned long mask);


#ifdef CONFIG_COMPACTION
#define get_pageblock_skip(page) \
	get_pfnblock_flags_mask(page, page_to_pfn(page),	\
			(1 << (PB_migrate_skip)))
#define clear_pageblock_skip(page) \
	set_pfnblock_flags_mask(page, 0, page_to_pfn(page),	\
			(1 << PB_migrate_skip))
#define set_pageblock_skip(page) \
	set_pfnblock_flags_mask(page, (1 << PB_migrate_skip),	\
			page_to_pfn(page),			\
			(1 << PB_migrate_skip))
#else
static inline bool get_pageblock_skip(struct page *page)
{
	return false;
}
static inline void clear_pageblock_skip(struct page *page)
{
}
static inline void set_pageblock_skip(struct page *page)
{
}
#endif 

#endif	
