/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __REGULATOR_GPIO_H
#define __REGULATOR_GPIO_H

#include <linux/gpio/consumer.h>

struct regulator_init_data;

enum regulator_type;


struct gpio_regulator_state {
	int value;
	int gpios;
};


struct gpio_regulator_config {
	const char *supply_name;
	const char *input_supply;

	unsigned enabled_at_boot:1;
	unsigned startup_delay;

	enum gpiod_flags *gflags;
	int ngpios;

	struct gpio_regulator_state *states;
	int nr_states;

	enum regulator_type type;
	struct regulator_init_data *init_data;
};

#endif
