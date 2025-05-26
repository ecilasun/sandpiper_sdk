/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __PHY_MIPI_DPHY_H_
#define __PHY_MIPI_DPHY_H_


struct phy_configure_opts_mipi_dphy {
	
	unsigned int		clk_miss;

	
	unsigned int		clk_post;

	
	unsigned int		clk_pre;

	
	unsigned int		clk_prepare;

	
	unsigned int		clk_settle;

	
	unsigned int		clk_term_en;

	
	unsigned int		clk_trail;

	
	unsigned int		clk_zero;

	
	unsigned int		d_term_en;

	
	unsigned int		eot;

	
	unsigned int		hs_exit;

	
	unsigned int		hs_prepare;

	
	unsigned int		hs_settle;

	
	unsigned int		hs_skip;

	
	unsigned int		hs_trail;

	
	unsigned int		hs_zero;

	
	unsigned int		init;

	
	unsigned int		lpx;

	
	unsigned int		ta_get;

	
	unsigned int		ta_go;

	
	unsigned int		ta_sure;

	
	unsigned int		wakeup;

	
	unsigned long		hs_clk_rate;

	
	unsigned long		lp_clk_rate;

	
	unsigned char		lanes;
};

int phy_mipi_dphy_get_default_config(unsigned long pixel_clock,
				     unsigned int bpp,
				     unsigned int lanes,
				     struct phy_configure_opts_mipi_dphy *cfg);
int phy_mipi_dphy_get_default_config_for_hsclk(unsigned long long hs_clk_rate,
					       unsigned int lanes,
					       struct phy_configure_opts_mipi_dphy *cfg);
int phy_mipi_dphy_config_validate(struct phy_configure_opts_mipi_dphy *cfg);

#endif 
