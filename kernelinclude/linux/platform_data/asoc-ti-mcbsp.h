/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASOC_TI_MCBSP_H
#define __ASOC_TI_MCBSP_H

#include <linux/spinlock.h>
#include <linux/clk.h>


struct omap_mcbsp_ops {
	void (*request)(unsigned int);
	void (*free)(unsigned int);
};

struct omap_mcbsp_platform_data {
	struct omap_mcbsp_ops *ops;
	u16 buffer_size;
	u8 reg_size;
	u8 reg_step;

	
	bool has_wakeup; 
	bool has_ccr; 
	int (*force_ick_on)(struct clk *clk, bool force_on);
};

void omap3_mcbsp_init_pdata_callback(struct omap_mcbsp_platform_data *pdata);

#endif
