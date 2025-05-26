/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _TDA1997X_
#define _TDA1997X_


struct tda1997x_platform_data {
	enum v4l2_mbus_type vidout_bus_type;
	u32 vidout_bus_width;
	u8 vidout_port_cfg[9];
	
	bool vidout_inv_de;
	bool vidout_inv_hs;
	bool vidout_inv_vs;
	bool vidout_inv_pclk;
	
	u8 vidout_delay_hs;
	u8 vidout_delay_vs;
	u8 vidout_delay_de;
	u8 vidout_delay_pclk;
	
	u8 vidout_sel_hs;
	u8 vidout_sel_vs;
	u8 vidout_sel_de;

	
	int audout_format;
	u32 audout_mclk_fs;	
	u32 audout_width;	
	u32 audout_layout;	
	bool audout_layoutauto;	
	bool audout_invert_clk;	
	bool audio_auto_mute;	
};

#endif
