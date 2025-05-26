/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _I8254_H_
#define _I8254_H_

struct device;
struct regmap;


struct i8254_regmap_config {
	struct device *parent;
	struct regmap *map;
};

int devm_i8254_regmap_register(struct device *dev, const struct i8254_regmap_config *config);

#endif 
