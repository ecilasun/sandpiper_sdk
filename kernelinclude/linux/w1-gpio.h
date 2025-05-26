/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _LINUX_W1_GPIO_H
#define _LINUX_W1_GPIO_H

struct gpio_desc;


struct w1_gpio_platform_data {
	struct gpio_desc *gpiod;
	struct gpio_desc *pullup_gpiod;
	void (*enable_external_pullup)(int enable);
	unsigned int pullup_duration;
};

#endif 
