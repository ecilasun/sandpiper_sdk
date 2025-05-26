// SPDX-License-Identifier: GPL-2.0


#ifndef __LINUX_CLK_DAVINCI_PLL_H___
#define __LINUX_CLK_DAVINCI_PLL_H___

#include <linux/device.h>
#include <linux/regmap.h>



#ifdef CONFIG_ARCH_DAVINCI_DA830
int da830_pll_init(struct device *dev, void __iomem *base, struct regmap *cfgchip);
#endif
#ifdef CONFIG_ARCH_DAVINCI_DA850
int da850_pll0_init(struct device *dev, void __iomem *base, struct regmap *cfgchip);
#endif

#endif 
