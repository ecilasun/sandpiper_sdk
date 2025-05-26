// SPDX-License-Identifier: GPL-2.0


#ifndef __LINUX_PLATFORM_DATA_PHY_DA8XX_USB_H__
#define __LINUX_PLATFORM_DATA_PHY_DA8XX_USB_H__

#include <linux/regmap.h>


struct da8xx_usb_phy_platform_data {
	struct regmap *cfgchip;
};

#endif 
