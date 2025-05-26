/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __PHY_LVDS_H_
#define __PHY_LVDS_H_


struct phy_configure_opts_lvds {
	unsigned int	bits_per_lane_and_dclk_cycle;
	unsigned long	differential_clk_rate;
	unsigned int	lanes;
	bool		is_slave;
};

#endif 
