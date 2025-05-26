/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef NTXEC_H
#define NTXEC_H

#include <linux/types.h>

struct device;
struct regmap;

struct ntxec {
	struct device *dev;
	struct regmap *regmap;
};


static inline u16 ntxec_reg8(u8 value)
{
	return value << 8;
}


#define NTXEC_VERSION_KOBO_AURA	0xd726	
#define NTXEC_VERSION_TOLINO_SHINE2 0xf110 
#define NTXEC_VERSION_TOLINO_VISION 0xe135 
#endif
