/* SPDX-License-Identifier: GPL-2.0 */


#include <media/v4l2-mediabus.h>

enum dphy3_algo {
	DPHY3_ALGO_DEFAULT = 0,
	DPHY3_ALGO_PXA910,
	DPHY3_ALGO_PXA2128
};

struct mmp_camera_platform_data {
	enum v4l2_mbus_type bus_type;
	int mclk_src;	
	int mclk_div;	
	
	int dphy[3];		
	enum dphy3_algo dphy3_algo;	
	int lane;		
	int lane_clk;
};
