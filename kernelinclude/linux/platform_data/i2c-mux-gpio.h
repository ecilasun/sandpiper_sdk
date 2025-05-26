/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_I2C_MUX_GPIO_H
#define _LINUX_I2C_MUX_GPIO_H


#define I2C_MUX_GPIO_NO_IDLE	((unsigned)-1)


struct i2c_mux_gpio_platform_data {
	int parent;
	int base_nr;
	const unsigned *values;
	int n_values;
	const unsigned *classes;
	unsigned idle;
};

#endif 
