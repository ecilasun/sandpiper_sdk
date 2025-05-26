/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __LINUX_MFD_RSMU_H
#define __LINUX_MFD_RSMU_H

#define RSMU_MAX_WRITE_COUNT	(255)
#define RSMU_MAX_READ_COUNT	(255)


enum rsmu_type {
	RSMU_CM		= 0x34000,
	RSMU_SABRE	= 0x33810,
	RSMU_FC3	= 0x38312,
};


struct rsmu_ddata {
	struct device *dev;
	struct regmap *regmap;
	struct mutex lock;
	enum rsmu_type type;
	u32 page;
};
#endif 
