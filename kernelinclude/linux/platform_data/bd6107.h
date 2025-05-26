/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BD6107_H__
#define __BD6107_H__

struct device;

struct bd6107_platform_data {
	struct device *dev;
	unsigned int def_value;
};

#endif
