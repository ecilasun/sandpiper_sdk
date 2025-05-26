/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _PDATA_MAX197_H
#define _PDATA_MAX197_H


struct max197_platform_data {
	int (*convert)(u8 ctrl);
};

#endif 
