/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __LINUX_GPIO_LEGACY_OF_MM_GPIO_CHIP_H
#define __LINUX_GPIO_LEGACY_OF_MM_GPIO_CHIP_H

#include <linux/gpio/driver.h>
#include <linux/of.h>


struct of_mm_gpio_chip {
	struct gpio_chip gc;
	void (*save_regs)(struct of_mm_gpio_chip *mm_gc);
	void __iomem *regs;
};

static inline struct of_mm_gpio_chip *to_of_mm_gpio_chip(struct gpio_chip *gc)
{
	return container_of(gc, struct of_mm_gpio_chip, gc);
}

extern int of_mm_gpiochip_add_data(struct device_node *np,
				   struct of_mm_gpio_chip *mm_gc,
				   void *data);
extern void of_mm_gpiochip_remove(struct of_mm_gpio_chip *mm_gc);

#endif 
