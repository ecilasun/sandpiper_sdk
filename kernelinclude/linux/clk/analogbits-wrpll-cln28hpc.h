/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_CLK_ANALOGBITS_WRPLL_CLN28HPC_H
#define __LINUX_CLK_ANALOGBITS_WRPLL_CLN28HPC_H

#include <linux/types.h>


#define DIVQ_VALUES				6


#define WRPLL_FLAGS_BYPASS_SHIFT		0
#define WRPLL_FLAGS_BYPASS_MASK		BIT(WRPLL_FLAGS_BYPASS_SHIFT)
#define WRPLL_FLAGS_RESET_SHIFT		1
#define WRPLL_FLAGS_RESET_MASK		BIT(WRPLL_FLAGS_RESET_SHIFT)
#define WRPLL_FLAGS_INT_FEEDBACK_SHIFT	2
#define WRPLL_FLAGS_INT_FEEDBACK_MASK	BIT(WRPLL_FLAGS_INT_FEEDBACK_SHIFT)
#define WRPLL_FLAGS_EXT_FEEDBACK_SHIFT	3
#define WRPLL_FLAGS_EXT_FEEDBACK_MASK	BIT(WRPLL_FLAGS_EXT_FEEDBACK_SHIFT)


struct wrpll_cfg {
	u8 divr;
	u8 divq;
	u8 range;
	u8 flags;
	u16 divf;

	u32 output_rate_cache[DIVQ_VALUES];
	unsigned long parent_rate;
	u8 max_r;
	u8 init_r;
};

int wrpll_configure_for_rate(struct wrpll_cfg *c, u32 target_rate,
			     unsigned long parent_rate);

unsigned int wrpll_calc_max_lock_us(const struct wrpll_cfg *c);

unsigned long wrpll_calc_output_rate(const struct wrpll_cfg *c,
				     unsigned long parent_rate);

#endif 
