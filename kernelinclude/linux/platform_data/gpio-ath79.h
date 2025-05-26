/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_PLATFORM_DATA_GPIO_ATH79_H
#define __LINUX_PLATFORM_DATA_GPIO_ATH79_H

struct ath79_gpio_platform_data {
	unsigned ngpios;
	bool oe_inverted;
};

#endif
