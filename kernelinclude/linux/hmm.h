/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef LINUX_HMM_H
#define LINUX_HMM_H

#include <linux/mm.h>

struct mmu_interval_notifier;


enum hmm_pfn_flags {
	
	HMM_PFN_VALID = 1UL << (BITS_PER_LONG - 1),
	HMM_PFN_WRITE = 1UL << (BITS_PER_LONG - 2),
	HMM_PFN_ERROR = 1UL << (BITS_PER_LONG - 3),
	HMM_PFN_ORDER_SHIFT = (BITS_PER_LONG - 8),

	
	HMM_PFN_REQ_FAULT = HMM_PFN_VALID,
	HMM_PFN_REQ_WRITE = HMM_PFN_WRITE,

	HMM_PFN_FLAGS = 0xFFUL << HMM_PFN_ORDER_SHIFT,
};


static inline struct page *hmm_pfn_to_page(unsigned long hmm_pfn)
{
	return pfn_to_page(hmm_pfn & ~HMM_PFN_FLAGS);
}


static inline unsigned int hmm_pfn_to_map_order(unsigned long hmm_pfn)
{
	return (hmm_pfn >> HMM_PFN_ORDER_SHIFT) & 0x1F;
}


struct hmm_range {
	struct mmu_interval_notifier *notifier;
	unsigned long		notifier_seq;
	unsigned long		start;
	unsigned long		end;
	unsigned long		*hmm_pfns;
	unsigned long		default_flags;
	unsigned long		pfn_flags_mask;
	void			*dev_private_owner;
};


int hmm_range_fault(struct hmm_range *range);


#define HMM_RANGE_DEFAULT_TIMEOUT 1000

#endif 
