/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __LINUX_REGULATOR_DA9121_H
#define __LINUX_REGULATOR_DA9121_H

#include <linux/regulator/machine.h>

struct gpio_desc;

enum {
	DA9121_IDX_BUCK1,
	DA9121_IDX_BUCK2,
	DA9121_IDX_MAX
};

struct da9121_pdata {
	int num_buck;
	struct gpio_desc *gpiod_ren[DA9121_IDX_MAX];
	struct device_node *reg_node[DA9121_IDX_MAX];
	struct regulator_init_data *init_data[DA9121_IDX_MAX];
};

#endif
