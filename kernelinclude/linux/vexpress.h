/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_VEXPRESS_H
#define _LINUX_VEXPRESS_H

#include <linux/device.h>
#include <linux/regmap.h>



struct regmap *devm_regmap_init_vexpress_config(struct device *dev);

#endif
