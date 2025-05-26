/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __DW_HDMI__
#define __DW_HDMI__

#include <sound/hdmi-codec.h>

struct drm_display_info;
struct drm_display_mode;
struct drm_encoder;
struct dw_hdmi;
struct platform_device;



enum {
	DW_HDMI_RES_8,
	DW_HDMI_RES_10,
	DW_HDMI_RES_12,
	DW_HDMI_RES_MAX,
};

enum dw_hdmi_phy_type {
	DW_HDMI_PHY_DWC_HDMI_TX_PHY = 0x00,
	DW_HDMI_PHY_DWC_MHL_PHY_HEAC = 0xb2,
	DW_HDMI_PHY_DWC_MHL_PHY = 0xc2,
	DW_HDMI_PHY_DWC_HDMI_3D_TX_PHY_HEAC = 0xe2,
	DW_HDMI_PHY_DWC_HDMI_3D_TX_PHY = 0xf2,
	DW_HDMI_PHY_DWC_HDMI20_TX_PHY = 0xf3,
	DW_HDMI_PHY_VENDOR_PHY = 0xfe,
};

struct dw_hdmi_mpll_config {
	unsigned long mpixelclock;
	struct {
		u16 cpce;
		u16 gmp;
	} res[DW_HDMI_RES_MAX];
};

struct dw_hdmi_curr_ctrl {
	unsigned long mpixelclock;
	u16 curr[DW_HDMI_RES_MAX];
};

struct dw_hdmi_phy_config {
	unsigned long mpixelclock;
	u16 sym_ctr;    
	u16 term;       
	u16 vlev_ctr;   
};

struct dw_hdmi_phy_ops {
	int (*init)(struct dw_hdmi *hdmi, void *data,
		    const struct drm_display_info *display,
		    const struct drm_display_mode *mode);
	void (*disable)(struct dw_hdmi *hdmi, void *data);
	enum drm_connector_status (*read_hpd)(struct dw_hdmi *hdmi, void *data);
	void (*update_hpd)(struct dw_hdmi *hdmi, void *data,
			   bool force, bool disabled, bool rxsense);
	void (*setup_hpd)(struct dw_hdmi *hdmi, void *data);
};

struct dw_hdmi_plat_data {
	struct regmap *regm;

	unsigned int output_port;

	unsigned long input_bus_encoding;
	bool use_drm_infoframe;
	bool ycbcr_420_allowed;

	
	void *priv_data;

	
	enum drm_mode_status (*mode_valid)(struct dw_hdmi *hdmi, void *data,
					   const struct drm_display_info *info,
					   const struct drm_display_mode *mode);

	
	void (*enable_audio)(struct dw_hdmi *hdmi, int channel,
			     int width, int rate, int non_pcm);
	void (*disable_audio)(struct dw_hdmi *hdmi);

	
	const struct dw_hdmi_phy_ops *phy_ops;
	const char *phy_name;
	void *phy_data;
	unsigned int phy_force_vendor;

	
	const struct dw_hdmi_mpll_config *mpll_cfg;
	const struct dw_hdmi_curr_ctrl *cur_ctr;
	const struct dw_hdmi_phy_config *phy_config;
	int (*configure_phy)(struct dw_hdmi *hdmi, void *data,
			     unsigned long mpixelclock);

	unsigned int disable_cec : 1;
};

struct dw_hdmi *dw_hdmi_probe(struct platform_device *pdev,
			      const struct dw_hdmi_plat_data *plat_data);
void dw_hdmi_remove(struct dw_hdmi *hdmi);
void dw_hdmi_unbind(struct dw_hdmi *hdmi);
struct dw_hdmi *dw_hdmi_bind(struct platform_device *pdev,
			     struct drm_encoder *encoder,
			     const struct dw_hdmi_plat_data *plat_data);

void dw_hdmi_resume(struct dw_hdmi *hdmi);

void dw_hdmi_setup_rx_sense(struct dw_hdmi *hdmi, bool hpd, bool rx_sense);

int dw_hdmi_set_plugged_cb(struct dw_hdmi *hdmi, hdmi_codec_plugged_cb fn,
			   struct device *codec_dev);
void dw_hdmi_set_sample_non_pcm(struct dw_hdmi *hdmi, unsigned int non_pcm);
void dw_hdmi_set_sample_width(struct dw_hdmi *hdmi, unsigned int width);
void dw_hdmi_set_sample_rate(struct dw_hdmi *hdmi, unsigned int rate);
void dw_hdmi_set_channel_count(struct dw_hdmi *hdmi, unsigned int cnt);
void dw_hdmi_set_channel_status(struct dw_hdmi *hdmi, u8 *channel_status);
void dw_hdmi_set_channel_allocation(struct dw_hdmi *hdmi, unsigned int ca);
void dw_hdmi_audio_enable(struct dw_hdmi *hdmi);
void dw_hdmi_audio_disable(struct dw_hdmi *hdmi);
void dw_hdmi_set_high_tmds_clock_ratio(struct dw_hdmi *hdmi,
				       const struct drm_display_info *display);


void dw_hdmi_phy_i2c_set_addr(struct dw_hdmi *hdmi, u8 address);
void dw_hdmi_phy_i2c_write(struct dw_hdmi *hdmi, unsigned short data,
			   unsigned char addr);

void dw_hdmi_phy_gen1_reset(struct dw_hdmi *hdmi);

void dw_hdmi_phy_gen2_pddq(struct dw_hdmi *hdmi, u8 enable);
void dw_hdmi_phy_gen2_txpwron(struct dw_hdmi *hdmi, u8 enable);
void dw_hdmi_phy_gen2_reset(struct dw_hdmi *hdmi);

enum drm_connector_status dw_hdmi_phy_read_hpd(struct dw_hdmi *hdmi,
					       void *data);
void dw_hdmi_phy_update_hpd(struct dw_hdmi *hdmi, void *data,
			    bool force, bool disabled, bool rxsense);
void dw_hdmi_phy_setup_hpd(struct dw_hdmi *hdmi, void *data);

bool dw_hdmi_bus_fmt_is_420(struct dw_hdmi *hdmi);

#endif 
