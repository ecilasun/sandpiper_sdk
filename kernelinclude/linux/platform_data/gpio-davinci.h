/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __DAVINCI_GPIO_PLATFORM_H
#define __DAVINCI_GPIO_PLATFORM_H

struct davinci_gpio_platform_data {
	bool	no_auto_base;
	u32	base;
	u32	ngpio;
	u32	gpio_unbanked;
};


#define GPIO_TO_PIN(bank, gpio)	(16 * (bank) + (gpio))

#endif
