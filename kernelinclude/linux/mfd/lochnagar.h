/* SPDX-License-Identifier: GPL-2.0 */


#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/regmap.h>

#ifndef CIRRUS_LOCHNAGAR_H
#define CIRRUS_LOCHNAGAR_H

enum lochnagar_type {
	LOCHNAGAR1,
	LOCHNAGAR2,
};


struct lochnagar {
	enum lochnagar_type type;
	struct device *dev;
	struct regmap *regmap;

	
	struct mutex analogue_config_lock;
};


#define LOCHNAGAR_SOFTWARE_RESET                             0x00
#define LOCHNAGAR_FIRMWARE_ID1                               0x01
#define LOCHNAGAR_FIRMWARE_ID2                               0x02


#define LOCHNAGAR_DEVICE_ID_MASK                           0xFFFC
#define LOCHNAGAR_DEVICE_ID_SHIFT                               2
#define LOCHNAGAR_REV_ID_MASK                              0x0003
#define LOCHNAGAR_REV_ID_SHIFT                                  0

int lochnagar_update_config(struct lochnagar *lochnagar);

#endif
