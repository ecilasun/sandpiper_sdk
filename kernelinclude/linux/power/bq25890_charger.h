/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _BQ25890_CHARGER_H_
#define _BQ25890_CHARGER_H_

struct regulator_init_data;

struct bq25890_platform_data {
	const struct regulator_init_data *regulator_init_data;
};

#endif
