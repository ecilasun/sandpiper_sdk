/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef MADERA_PDATA_H
#define MADERA_PDATA_H

#include <linux/kernel.h>
#include <linux/regulator/arizona-ldo1.h>
#include <linux/regulator/arizona-micsupp.h>
#include <linux/regulator/machine.h>
#include <sound/madera-pdata.h>

#define MADERA_MAX_MICBIAS		4
#define MADERA_MAX_CHILD_MICBIAS	4

#define MADERA_MAX_GPSW			2

struct gpio_desc;
struct pinctrl_map;


struct madera_pdata {
	struct gpio_desc *reset;

	struct arizona_ldo1_pdata ldo1;
	struct arizona_micsupp_pdata micvdd;

	unsigned int irq_flags;
	int gpio_base;

	const struct pinctrl_map *gpio_configs;
	int n_gpio_configs;

	u32 gpsw[MADERA_MAX_GPSW];

	struct madera_codec_pdata codec;
};

#endif
