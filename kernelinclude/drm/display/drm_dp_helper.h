

#ifndef _DRM_DP_HELPER_H_
#define _DRM_DP_HELPER_H_

#include <linux/delay.h>
#include <linux/i2c.h>

#include <drm/display/drm_dp.h>
#include <drm/drm_connector.h>

struct drm_device;
struct drm_dp_aux;
struct drm_panel;

bool drm_dp_channel_eq_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
			  int lane_count);
bool drm_dp_clock_recovery_ok(const u8 link_status[DP_LINK_STATUS_SIZE],
			      int lane_count);
u8 drm_dp_get_adjust_request_voltage(const u8 link_status[DP_LINK_STATUS_SIZE],
				     int lane);
u8 drm_dp_get_adjust_request_pre_emphasis(const u8 link_status[DP_LINK_STATUS_SIZE],
					  int lane);
u8 drm_dp_get_adjust_tx_ffe_preset(const u8 link_status[DP_LINK_STATUS_SIZE],
				   int lane);

int drm_dp_read_clock_recovery_delay(struct drm_dp_aux *aux, const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				     enum drm_dp_phy dp_phy, bool uhbr);
int drm_dp_read_channel_eq_delay(struct drm_dp_aux *aux, const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				 enum drm_dp_phy dp_phy, bool uhbr);

void drm_dp_link_train_clock_recovery_delay(const struct drm_dp_aux *aux,
					    const u8 dpcd[DP_RECEIVER_CAP_SIZE]);
void drm_dp_lttpr_link_train_clock_recovery_delay(void);
void drm_dp_link_train_channel_eq_delay(const struct drm_dp_aux *aux,
					const u8 dpcd[DP_RECEIVER_CAP_SIZE]);
void drm_dp_lttpr_link_train_channel_eq_delay(const struct drm_dp_aux *aux,
					      const u8 caps[DP_LTTPR_PHY_CAP_SIZE]);

int drm_dp_128b132b_read_aux_rd_interval(struct drm_dp_aux *aux);
bool drm_dp_128b132b_lane_channel_eq_done(const u8 link_status[DP_LINK_STATUS_SIZE],
					  int lane_count);
bool drm_dp_128b132b_lane_symbol_locked(const u8 link_status[DP_LINK_STATUS_SIZE],
					int lane_count);
bool drm_dp_128b132b_eq_interlane_align_done(const u8 link_status[DP_LINK_STATUS_SIZE]);
bool drm_dp_128b132b_cds_interlane_align_done(const u8 link_status[DP_LINK_STATUS_SIZE]);
bool drm_dp_128b132b_link_training_failed(const u8 link_status[DP_LINK_STATUS_SIZE]);

u8 drm_dp_link_rate_to_bw_code(int link_rate);
int drm_dp_bw_code_to_link_rate(u8 link_bw);

const char *drm_dp_phy_name(enum drm_dp_phy dp_phy);


struct drm_dp_vsc_sdp {
	unsigned char sdp_type;
	unsigned char revision;
	unsigned char length;
	enum dp_pixelformat pixelformat;
	enum dp_colorimetry colorimetry;
	int bpc;
	enum dp_dynamic_range dynamic_range;
	enum dp_content_type content_type;
};

void drm_dp_vsc_sdp_log(const char *level, struct device *dev,
			const struct drm_dp_vsc_sdp *vsc);

int drm_dp_psr_setup_time(const u8 psr_cap[EDP_PSR_RECEIVER_CAP_SIZE]);

static inline int
drm_dp_max_link_rate(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return drm_dp_bw_code_to_link_rate(dpcd[DP_MAX_LINK_RATE]);
}

static inline u8
drm_dp_max_lane_count(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;
}

static inline bool
drm_dp_enhanced_frame_cap(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DPCD_REV] >= 0x11 &&
		(dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP);
}

static inline bool
drm_dp_fast_training_cap(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DPCD_REV] >= 0x11 &&
		(dpcd[DP_MAX_DOWNSPREAD] & DP_NO_AUX_HANDSHAKE_LINK_TRAINING);
}

static inline bool
drm_dp_tps3_supported(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DPCD_REV] >= 0x12 &&
		dpcd[DP_MAX_LANE_COUNT] & DP_TPS3_SUPPORTED;
}

static inline bool
drm_dp_max_downspread(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DPCD_REV] >= 0x11 ||
		dpcd[DP_MAX_DOWNSPREAD] & DP_MAX_DOWNSPREAD_0_5;
}

static inline bool
drm_dp_tps4_supported(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DPCD_REV] >= 0x14 &&
		dpcd[DP_MAX_DOWNSPREAD] & DP_TPS4_SUPPORTED;
}

static inline u8
drm_dp_training_pattern_mask(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return (dpcd[DP_DPCD_REV] >= 0x14) ? DP_TRAINING_PATTERN_MASK_1_4 :
		DP_TRAINING_PATTERN_MASK;
}

static inline bool
drm_dp_is_branch(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DOWNSTREAMPORT_PRESENT] & DP_DWN_STRM_PORT_PRESENT;
}


u8 drm_dp_dsc_sink_max_slice_count(const u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE],
				   bool is_edp);
u8 drm_dp_dsc_sink_line_buf_depth(const u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE]);
int drm_dp_dsc_sink_supported_input_bpcs(const u8 dsc_dpc[DP_DSC_RECEIVER_CAP_SIZE],
					 u8 dsc_bpc[3]);

static inline bool
drm_dp_sink_supports_dsc(const u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE])
{
	return dsc_dpcd[DP_DSC_SUPPORT - DP_DSC_SUPPORT] &
		DP_DSC_DECOMPRESSION_IS_SUPPORTED;
}

static inline u16
drm_edp_dsc_sink_output_bpp(const u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE])
{
	return dsc_dpcd[DP_DSC_MAX_BITS_PER_PIXEL_LOW - DP_DSC_SUPPORT] |
		((dsc_dpcd[DP_DSC_MAX_BITS_PER_PIXEL_HI - DP_DSC_SUPPORT] &
		  DP_DSC_MAX_BITS_PER_PIXEL_HI_MASK) << 8);
}

static inline u32
drm_dp_dsc_sink_max_slice_width(const u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE])
{
	
	return dsc_dpcd[DP_DSC_MAX_SLICE_WIDTH - DP_DSC_SUPPORT] *
		DP_DSC_SLICE_WIDTH_MULTIPLIER;
}


static inline bool
drm_dp_dsc_sink_supports_format(const u8 dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE], u8 output_format)
{
	return dsc_dpcd[DP_DSC_DEC_COLOR_FORMAT_CAP - DP_DSC_SUPPORT] & output_format;
}


static inline bool
drm_dp_sink_supports_fec(const u8 fec_capable)
{
	return fec_capable & DP_FEC_CAPABLE;
}

static inline bool
drm_dp_channel_coding_supported(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_MAIN_LINK_CHANNEL_CODING] & DP_CAP_ANSI_8B10B;
}

static inline bool
drm_dp_alternate_scrambler_reset_cap(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_EDP_CONFIGURATION_CAP] &
			DP_ALTERNATE_SCRAMBLER_RESET_CAP;
}


static inline bool
drm_dp_sink_can_do_video_without_timing_msa(const u8 dpcd[DP_RECEIVER_CAP_SIZE])
{
	return dpcd[DP_DOWN_STREAM_PORT_COUNT] &
		DP_MSA_TIMING_PAR_IGNORED;
}


static inline bool
drm_edp_backlight_supported(const u8 edp_dpcd[EDP_DISPLAY_CTL_CAP_SIZE])
{
	return !!(edp_dpcd[1] & DP_EDP_TCON_BACKLIGHT_ADJUSTMENT_CAP);
}




struct drm_dp_aux_msg {
	unsigned int address;
	u8 request;
	u8 reply;
	void *buffer;
	size_t size;
};

struct cec_adapter;
struct edid;
struct drm_connector;


struct drm_dp_aux_cec {
	struct mutex lock;
	struct cec_adapter *adap;
	struct drm_connector *connector;
	struct delayed_work unregister_work;
};


struct drm_dp_aux {
	
	const char *name;

	
	struct i2c_adapter ddc;

	
	struct device *dev;

	
	struct drm_device *drm_dev;

	
	struct drm_crtc *crtc;

	
	struct mutex hw_mutex;

	
	struct work_struct crc_work;

	
	u8 crc_count;

	
	ssize_t (*transfer)(struct drm_dp_aux *aux,
			    struct drm_dp_aux_msg *msg);

	
	int (*wait_hpd_asserted)(struct drm_dp_aux *aux, unsigned long wait_us);

	
	unsigned i2c_nack_count;
	
	unsigned i2c_defer_count;
	
	struct drm_dp_aux_cec cec;
	
	bool is_remote;

	
	bool powered_down;
};

int drm_dp_dpcd_probe(struct drm_dp_aux *aux, unsigned int offset);
void drm_dp_dpcd_set_powered(struct drm_dp_aux *aux, bool powered);
ssize_t drm_dp_dpcd_read(struct drm_dp_aux *aux, unsigned int offset,
			 void *buffer, size_t size);
ssize_t drm_dp_dpcd_write(struct drm_dp_aux *aux, unsigned int offset,
			  void *buffer, size_t size);


static inline ssize_t drm_dp_dpcd_readb(struct drm_dp_aux *aux,
					unsigned int offset, u8 *valuep)
{
	return drm_dp_dpcd_read(aux, offset, valuep, 1);
}


static inline ssize_t drm_dp_dpcd_writeb(struct drm_dp_aux *aux,
					 unsigned int offset, u8 value)
{
	return drm_dp_dpcd_write(aux, offset, &value, 1);
}

int drm_dp_read_dpcd_caps(struct drm_dp_aux *aux,
			  u8 dpcd[DP_RECEIVER_CAP_SIZE]);

int drm_dp_dpcd_read_link_status(struct drm_dp_aux *aux,
				 u8 status[DP_LINK_STATUS_SIZE]);

int drm_dp_dpcd_read_phy_link_status(struct drm_dp_aux *aux,
				     enum drm_dp_phy dp_phy,
				     u8 link_status[DP_LINK_STATUS_SIZE]);

bool drm_dp_send_real_edid_checksum(struct drm_dp_aux *aux,
				    u8 real_edid_checksum);

int drm_dp_read_downstream_info(struct drm_dp_aux *aux,
				const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				u8 downstream_ports[DP_MAX_DOWNSTREAM_PORTS]);
bool drm_dp_downstream_is_type(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			       const u8 port_cap[4], u8 type);
bool drm_dp_downstream_is_tmds(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			       const u8 port_cap[4],
			       const struct edid *edid);
int drm_dp_downstream_max_dotclock(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				   const u8 port_cap[4]);
int drm_dp_downstream_max_tmds_clock(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				     const u8 port_cap[4],
				     const struct edid *edid);
int drm_dp_downstream_min_tmds_clock(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				     const u8 port_cap[4],
				     const struct edid *edid);
int drm_dp_downstream_max_bpc(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			      const u8 port_cap[4],
			      const struct edid *edid);
bool drm_dp_downstream_420_passthrough(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				       const u8 port_cap[4]);
bool drm_dp_downstream_444_to_420_conversion(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
					     const u8 port_cap[4]);
struct drm_display_mode *drm_dp_downstream_mode(struct drm_device *dev,
						const u8 dpcd[DP_RECEIVER_CAP_SIZE],
						const u8 port_cap[4]);
int drm_dp_downstream_id(struct drm_dp_aux *aux, char id[6]);
void drm_dp_downstream_debug(struct seq_file *m,
			     const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			     const u8 port_cap[4],
			     const struct edid *edid,
			     struct drm_dp_aux *aux);
enum drm_mode_subconnector
drm_dp_subconnector_type(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			 const u8 port_cap[4]);
void drm_dp_set_subconnector_property(struct drm_connector *connector,
				      enum drm_connector_status status,
				      const u8 *dpcd,
				      const u8 port_cap[4]);

struct drm_dp_desc;
bool drm_dp_read_sink_count_cap(struct drm_connector *connector,
				const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				const struct drm_dp_desc *desc);
int drm_dp_read_sink_count(struct drm_dp_aux *aux);

int drm_dp_read_lttpr_common_caps(struct drm_dp_aux *aux,
				  const u8 dpcd[DP_RECEIVER_CAP_SIZE],
				  u8 caps[DP_LTTPR_COMMON_CAP_SIZE]);
int drm_dp_read_lttpr_phy_caps(struct drm_dp_aux *aux,
			       const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			       enum drm_dp_phy dp_phy,
			       u8 caps[DP_LTTPR_PHY_CAP_SIZE]);
int drm_dp_lttpr_count(const u8 cap[DP_LTTPR_COMMON_CAP_SIZE]);
int drm_dp_lttpr_max_link_rate(const u8 caps[DP_LTTPR_COMMON_CAP_SIZE]);
int drm_dp_lttpr_max_lane_count(const u8 caps[DP_LTTPR_COMMON_CAP_SIZE]);
bool drm_dp_lttpr_voltage_swing_level_3_supported(const u8 caps[DP_LTTPR_PHY_CAP_SIZE]);
bool drm_dp_lttpr_pre_emphasis_level_3_supported(const u8 caps[DP_LTTPR_PHY_CAP_SIZE]);

void drm_dp_remote_aux_init(struct drm_dp_aux *aux);
void drm_dp_aux_init(struct drm_dp_aux *aux);
int drm_dp_aux_register(struct drm_dp_aux *aux);
void drm_dp_aux_unregister(struct drm_dp_aux *aux);

int drm_dp_start_crc(struct drm_dp_aux *aux, struct drm_crtc *crtc);
int drm_dp_stop_crc(struct drm_dp_aux *aux);

struct drm_dp_dpcd_ident {
	u8 oui[3];
	u8 device_id[6];
	u8 hw_rev;
	u8 sw_major_rev;
	u8 sw_minor_rev;
} __packed;


struct drm_dp_desc {
	struct drm_dp_dpcd_ident ident;
	u32 quirks;
};

int drm_dp_read_desc(struct drm_dp_aux *aux, struct drm_dp_desc *desc,
		     bool is_branch);


enum drm_dp_quirk {
	
	DP_DPCD_QUIRK_CONSTANT_N,
	
	DP_DPCD_QUIRK_NO_PSR,
	
	DP_DPCD_QUIRK_NO_SINK_COUNT,
	
	DP_DPCD_QUIRK_DSC_WITHOUT_VIRTUAL_DPCD,
	
	DP_DPCD_QUIRK_CAN_DO_MAX_LINK_RATE_3_24_GBPS,
};


static inline bool
drm_dp_has_quirk(const struct drm_dp_desc *desc, enum drm_dp_quirk quirk)
{
	return desc->quirks & BIT(quirk);
}


struct drm_edp_backlight_info {
	u8 pwmgen_bit_count;
	u8 pwm_freq_pre_divider;
	u16 max;

	bool lsb_reg_used : 1;
	bool aux_enable : 1;
	bool aux_set : 1;
};

int
drm_edp_backlight_init(struct drm_dp_aux *aux, struct drm_edp_backlight_info *bl,
		       u16 driver_pwm_freq_hz, const u8 edp_dpcd[EDP_DISPLAY_CTL_CAP_SIZE],
		       u16 *current_level, u8 *current_mode);
int drm_edp_backlight_set_level(struct drm_dp_aux *aux, const struct drm_edp_backlight_info *bl,
				u16 level);
int drm_edp_backlight_enable(struct drm_dp_aux *aux, const struct drm_edp_backlight_info *bl,
			     u16 level);
int drm_edp_backlight_disable(struct drm_dp_aux *aux, const struct drm_edp_backlight_info *bl);

#if IS_ENABLED(CONFIG_DRM_KMS_HELPER) && (IS_BUILTIN(CONFIG_BACKLIGHT_CLASS_DEVICE) || \
	(IS_MODULE(CONFIG_DRM_KMS_HELPER) && IS_MODULE(CONFIG_BACKLIGHT_CLASS_DEVICE)))

int drm_panel_dp_aux_backlight(struct drm_panel *panel, struct drm_dp_aux *aux);

#else

static inline int drm_panel_dp_aux_backlight(struct drm_panel *panel,
					     struct drm_dp_aux *aux)
{
	return 0;
}

#endif

#ifdef CONFIG_DRM_DP_CEC
void drm_dp_cec_irq(struct drm_dp_aux *aux);
void drm_dp_cec_register_connector(struct drm_dp_aux *aux,
				   struct drm_connector *connector);
void drm_dp_cec_unregister_connector(struct drm_dp_aux *aux);
void drm_dp_cec_set_edid(struct drm_dp_aux *aux, const struct edid *edid);
void drm_dp_cec_unset_edid(struct drm_dp_aux *aux);
#else
static inline void drm_dp_cec_irq(struct drm_dp_aux *aux)
{
}

static inline void
drm_dp_cec_register_connector(struct drm_dp_aux *aux,
			      struct drm_connector *connector)
{
}

static inline void drm_dp_cec_unregister_connector(struct drm_dp_aux *aux)
{
}

static inline void drm_dp_cec_set_edid(struct drm_dp_aux *aux,
				       const struct edid *edid)
{
}

static inline void drm_dp_cec_unset_edid(struct drm_dp_aux *aux)
{
}

#endif


struct drm_dp_phy_test_params {
	int link_rate;
	u8 num_lanes;
	u8 phy_pattern;
	u8 hbr2_reset[2];
	u8 custom80[10];
	bool enhanced_frame_cap;
};

int drm_dp_get_phy_test_pattern(struct drm_dp_aux *aux,
				struct drm_dp_phy_test_params *data);
int drm_dp_set_phy_test_pattern(struct drm_dp_aux *aux,
				struct drm_dp_phy_test_params *data, u8 dp_rev);
int drm_dp_get_pcon_max_frl_bw(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
			       const u8 port_cap[4]);
int drm_dp_pcon_frl_prepare(struct drm_dp_aux *aux, bool enable_frl_ready_hpd);
bool drm_dp_pcon_is_frl_ready(struct drm_dp_aux *aux);
int drm_dp_pcon_frl_configure_1(struct drm_dp_aux *aux, int max_frl_gbps,
				u8 frl_mode);
int drm_dp_pcon_frl_configure_2(struct drm_dp_aux *aux, int max_frl_mask,
				u8 frl_type);
int drm_dp_pcon_reset_frl_config(struct drm_dp_aux *aux);
int drm_dp_pcon_frl_enable(struct drm_dp_aux *aux);

bool drm_dp_pcon_hdmi_link_active(struct drm_dp_aux *aux);
int drm_dp_pcon_hdmi_link_mode(struct drm_dp_aux *aux, u8 *frl_trained_mask);
void drm_dp_pcon_hdmi_frl_link_error_count(struct drm_dp_aux *aux,
					   struct drm_connector *connector);
bool drm_dp_pcon_enc_is_dsc_1_2(const u8 pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]);
int drm_dp_pcon_dsc_max_slices(const u8 pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]);
int drm_dp_pcon_dsc_max_slice_width(const u8 pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]);
int drm_dp_pcon_dsc_bpp_incr(const u8 pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]);
int drm_dp_pcon_pps_default(struct drm_dp_aux *aux);
int drm_dp_pcon_pps_override_buf(struct drm_dp_aux *aux, u8 pps_buf[128]);
int drm_dp_pcon_pps_override_param(struct drm_dp_aux *aux, u8 pps_param[6]);
bool drm_dp_downstream_rgb_to_ycbcr_conversion(const u8 dpcd[DP_RECEIVER_CAP_SIZE],
					       const u8 port_cap[4], u8 color_spc);
int drm_dp_pcon_convert_rgb_to_ycbcr(struct drm_dp_aux *aux, u8 color_spc);

#endif 
