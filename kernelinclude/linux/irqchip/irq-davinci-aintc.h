/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_IRQ_DAVINCI_AINTC_
#define _LINUX_IRQ_DAVINCI_AINTC_

#include <linux/ioport.h>


struct davinci_aintc_config {
	struct resource reg;
	unsigned int num_irqs;
	u8 *prios;
};

void davinci_aintc_init(const struct davinci_aintc_config *config);

#endif 
