/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PHY_HDMI_H_
#define __PHY_HDMI_H_

#include <linux/types.h>


enum callback_type {
	RX_INIT_CB,
	RX_READY_CB,
	TX_INIT_CB,
	TX_READY_CB,
};


struct hdmiphy_callback {
	void (*cb)(void *callback_func);
	void *data;
	u32 type;
};


struct phy_configure_opts_hdmi {
	
	u8 tmdsclock_ratio_flag : 1;
	
	u8 tmdsclock_ratio : 1;

	
	u8 ibufds : 1;
	
	u8 ibufds_en : 1;
	
	u8 clkout1_obuftds : 1;
	
	u8 clkout1_obuftds_en : 1;
	
	u8 config_hdmi20 : 1;
	
	u8 config_hdmi21 : 1;
	
	u64 linerate;
	
	u8 nchannels;
	
	u8 rx_get_refclk : 1;
	
	unsigned long rx_refclk_hz;
	
	u8 phycb : 1;
	
	struct hdmiphy_callback hdmiphycb;
	
	u8 tx_params : 1;
	
	u8 cal_mmcm_param : 1;
	
	u64 tx_tmdsclk;
	
	u8 ppc;
	
	u8 bpc;
	
	u8 fmt;
	
	u8 reset_gt : 1;
	
	u8 get_samplerate : 1;
	
	u8 samplerate;
	
	u8 resetgtpll : 1;
};

#endif 
