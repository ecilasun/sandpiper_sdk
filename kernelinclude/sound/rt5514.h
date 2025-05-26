/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_SND_RT5514_H
#define __LINUX_SND_RT5514_H

struct rt5514_platform_data {
	unsigned int dmic_init_delay;
	const char *dsp_calib_clk_name;
	unsigned int dsp_calib_clk_rate;
};

#endif

