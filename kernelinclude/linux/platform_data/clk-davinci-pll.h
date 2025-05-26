// SPDX-License-Identifier: GPL-2.0


#ifndef __LINUX_PLATFORM_DATA_CLK_DAVINCI_PLL_H__
#define __LINUX_PLATFORM_DATA_CLK_DAVINCI_PLL_H__

#include <linux/regmap.h>


struct davinci_pll_platform_data {
	struct regmap *cfgchip;
};

#endif 
