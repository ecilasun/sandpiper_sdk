/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LP872X_REGULATOR_H__
#define __LP872X_REGULATOR_H__

#include <linux/regulator/machine.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>

#define LP872X_MAX_REGULATORS		9

#define LP8720_ENABLE_DELAY		200
#define LP8725_ENABLE_DELAY		30000

enum lp872x_regulator_id {
	LP8720_ID_BASE,
	LP8720_ID_LDO1 = LP8720_ID_BASE,
	LP8720_ID_LDO2,
	LP8720_ID_LDO3,
	LP8720_ID_LDO4,
	LP8720_ID_LDO5,
	LP8720_ID_BUCK,

	LP8725_ID_BASE,
	LP8725_ID_LDO1 = LP8725_ID_BASE,
	LP8725_ID_LDO2,
	LP8725_ID_LDO3,
	LP8725_ID_LDO4,
	LP8725_ID_LDO5,
	LP8725_ID_LILO1,
	LP8725_ID_LILO2,
	LP8725_ID_BUCK1,
	LP8725_ID_BUCK2,

	LP872X_ID_MAX,
};

enum lp872x_dvs_sel {
	SEL_V1,
	SEL_V2,
};


struct lp872x_dvs {
	struct gpio_desc *gpio;
	enum lp872x_dvs_sel vsel;
	enum gpiod_flags init_state;
};


struct lp872x_regulator_data {
	enum lp872x_regulator_id id;
	struct regulator_init_data *init_data;
};


struct lp872x_platform_data {
	u8 general_config;
	bool update_config;
	struct lp872x_regulator_data regulator_data[LP872X_MAX_REGULATORS];
	struct lp872x_dvs *dvs;
	struct gpio_desc *enable_gpio;
};

#endif
