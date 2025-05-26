/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _WM8960_PDATA_H
#define _WM8960_PDATA_H

#define WM8960_DRES_400R 0
#define WM8960_DRES_200R 1
#define WM8960_DRES_600R 2
#define WM8960_DRES_150R 3
#define WM8960_DRES_MAX  3

struct wm8960_data {
	bool capless;  

	bool shared_lrclk;  

	
	u32 hp_cfg[3];

	
	u32 gpio_cfg[2];
};

#endif
