/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __CLK_LPSS_H
#define __CLK_LPSS_H

struct lpss_clk_data {
	const char *name;
	struct clk *clk;
};

extern int lpss_atom_clk_init(void);

#endif 
