/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __TIMER_DAVINCI_H__
#define __TIMER_DAVINCI_H__

#include <linux/clk.h>
#include <linux/ioport.h>

enum {
	DAVINCI_TIMER_CLOCKEVENT_IRQ,
	DAVINCI_TIMER_CLOCKSOURCE_IRQ,
	DAVINCI_TIMER_NUM_IRQS,
};


struct davinci_timer_cfg {
	struct resource reg;
	struct resource irq[DAVINCI_TIMER_NUM_IRQS];
	unsigned int cmp_off;
};

int __init davinci_timer_register(struct clk *clk,
				  const struct davinci_timer_cfg *data);

#endif 
