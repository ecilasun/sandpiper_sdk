/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _LINUX_GPIO_REGMAP_H
#define _LINUX_GPIO_REGMAP_H

struct device;
struct fwnode_handle;
struct gpio_regmap;
struct irq_domain;
struct regmap;

#define GPIO_REGMAP_ADDR_ZERO ((unsigned int)(-1))
#define GPIO_REGMAP_ADDR(addr) ((addr) ? : GPIO_REGMAP_ADDR_ZERO)


struct gpio_regmap_config {
	struct device *parent;
	struct regmap *regmap;
	struct fwnode_handle *fwnode;

	const char *label;
	int ngpio;
	const char *const *names;

	unsigned int reg_dat_base;
	unsigned int reg_set_base;
	unsigned int reg_clr_base;
	unsigned int reg_dir_in_base;
	unsigned int reg_dir_out_base;
	int reg_stride;
	int ngpio_per_reg;
	struct irq_domain *irq_domain;

	int (*reg_mask_xlate)(struct gpio_regmap *gpio, unsigned int base,
			      unsigned int offset, unsigned int *reg,
			      unsigned int *mask);

	void *drvdata;
};

struct gpio_regmap *gpio_regmap_register(const struct gpio_regmap_config *config);
void gpio_regmap_unregister(struct gpio_regmap *gpio);
struct gpio_regmap *devm_gpio_regmap_register(struct device *dev,
					      const struct gpio_regmap_config *config);
void *gpio_regmap_get_drvdata(struct gpio_regmap *gpio);

#endif 
