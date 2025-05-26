/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_MSI_H
#define __ASM_GENERIC_MSI_H

#include <linux/types.h>

#ifdef CONFIG_GENERIC_MSI_IRQ

#ifndef NUM_MSI_ALLOC_SCRATCHPAD_REGS
# define NUM_MSI_ALLOC_SCRATCHPAD_REGS	2
#endif

struct msi_desc;


typedef struct msi_alloc_info {
	struct msi_desc			*desc;
	irq_hw_number_t			hwirq;
	unsigned long			flags;
	union {
		unsigned long		ul;
		void			*ptr;
	} scratchpad[NUM_MSI_ALLOC_SCRATCHPAD_REGS];
} msi_alloc_info_t;


#define MSI_ALLOC_FLAGS_PROXY_DEVICE	(1UL << 0)

#define GENERIC_MSI_DOMAIN_OPS		1

#endif 

#endif
