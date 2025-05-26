/* SPDX-License-Identifier: MIT */


#ifndef __CLK_FCH_H
#define __CLK_FCH_H

#include <linux/compiler.h>

struct fch_clk_data {
	void __iomem *base;
	char *name;
};

#endif 
