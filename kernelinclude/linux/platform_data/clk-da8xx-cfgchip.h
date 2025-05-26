// SPDX-License-Identifier: GPL-2.0


#ifndef __LINUX_PLATFORM_DATA_CLK_DA8XX_CFGCHIP_H__
#define __LINUX_PLATFORM_DATA_CLK_DA8XX_CFGCHIP_H__

#include <linux/regmap.h>


struct da8xx_cfgchip_clk_platform_data {
	struct regmap *cfgchip;
};

#endif 
