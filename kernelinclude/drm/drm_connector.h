

#ifndef __DRM_CONNECTOR_H__
#define __DRM_CONNECTOR_H__

#include <linux/list.h>
#include <linux/llist.h>
#include <linux/ctype.h>
#include <linux/hdmi.h>
#include <linux/notifier.h>
#include <drm/drm_mode_object.h>
#include <drm/drm_util.h>
#include <drm/drm_property.h>

#include <uapi/drm/drm_mode.h>

struct drm_connector_helper_funcs;
struct drm_modeset_acquire_ctx;
struct drm_device;
struct drm_crtc;
struct drm_encoder;
struct drm_panel;
struct drm_property;
struct drm_property_blob;
struct drm_printer;
struct drm_privacy_screen;
struct edid;
struct i2c_adapter;

enum drm_connector_force {
	DRM_FORCE_UNSPECIFIED,
	DRM_FORCE_OFF,
	DRM_FORCE_ON,         
	DRM_FORCE_ON_DIGITAL, 
};


enum drm_connector_status {
	
	connector_status_connected = 1,
	
	connector_status_disconnected = 2,
	
	connector_status_unknown = 3,
};


enum drm_connector_registration_state {
	
	DRM_CONNECTOR_INITIALIZING = 0,

	
	DRM_CONNECTOR_REGISTERED = 1,

	
	DRM_CONNECTOR_UNREGISTERED = 2,
};

enum subpixel_order {
	SubPixelUnknown = 0,
	SubPixelHorizontalRGB,
	SubPixelHorizontalBGR,
	SubPixelVerticalRGB,
	SubPixelVerticalBGR,
	SubPixelNone,

};


enum drm_connector_tv_mode {
	
	DRM_MODE_TV_MODE_NTSC,

	
	DRM_MODE_TV_MODE_NTSC_443,

	
	DRM_MODE_TV_MODE_NTSC_J,

	
	DRM_MODE_TV_MODE_PAL,

	
	DRM_MODE_TV_MODE_PAL_M,

	
	DRM_MODE_TV_MODE_PAL_N,

	
	DRM_MODE_TV_MODE_SECAM,

	
	DRM_MODE_TV_MODE_MAX,
};


struct drm_scrambling {
	
	bool supported;
	
	bool low_rates;
};


struct drm_scdc {
	
	bool supported;
	
	bool read_request;
	
	struct drm_scrambling scrambling;
};


struct drm_hdmi_dsc_cap {
	
	bool v_1p2;

	
	bool native_420;

	
	bool all_bpp;

	
	u8 bpc_supported;

	
	u8 max_slices;

	
	int clk_per_slice;

	
	u8 max_lanes;

	
	u8 max_frl_rate_per_lane;

	
	u8 total_chunk_kbytes;
};


struct drm_hdmi_info {
	
	struct drm_scdc scdc;

	
	unsigned long y420_vdb_modes[BITS_TO_LONGS(256)];

	
	unsigned long y420_cmdb_modes[BITS_TO_LONGS(256)];

	
	u8 y420_dc_modes;

	
	u8 max_frl_rate_per_lane;

	
	u8 max_lanes;

	
	struct drm_hdmi_dsc_cap dsc_cap;
};


enum drm_link_status {
	DRM_LINK_STATUS_GOOD = DRM_MODE_LINK_STATUS_GOOD,
	DRM_LINK_STATUS_BAD = DRM_MODE_LINK_STATUS_BAD,
};


enum drm_panel_orientation {
	DRM_MODE_PANEL_ORIENTATION_UNKNOWN = -1,
	DRM_MODE_PANEL_ORIENTATION_NORMAL = 0,
	DRM_MODE_PANEL_ORIENTATION_BOTTOM_UP,
	DRM_MODE_PANEL_ORIENTATION_LEFT_UP,
	DRM_MODE_PANEL_ORIENTATION_RIGHT_UP,
};


struct drm_monitor_range_info {
	u16 min_vfreq;
	u16 max_vfreq;
};


struct drm_luminance_range_info {
	u32 min_luminance;
	u32 max_luminance;
};


enum drm_privacy_screen_status {
	PRIVACY_SCREEN_DISABLED = 0,
	PRIVACY_SCREEN_ENABLED,
	PRIVACY_SCREEN_DISABLED_LOCKED,
	PRIVACY_SCREEN_ENABLED_LOCKED,
};


enum drm_colorspace {
	
	DRM_MODE_COLORIMETRY_DEFAULT 		= 0,
	
	DRM_MODE_COLORIMETRY_NO_DATA		= 0,
	DRM_MODE_COLORIMETRY_SMPTE_170M_YCC	= 1,
	DRM_MODE_COLORIMETRY_BT709_YCC		= 2,
	
	DRM_MODE_COLORIMETRY_XVYCC_601		= 3,
	DRM_MODE_COLORIMETRY_XVYCC_709		= 4,
	DRM_MODE_COLORIMETRY_SYCC_601		= 5,
	DRM_MODE_COLORIMETRY_OPYCC_601		= 6,
	DRM_MODE_COLORIMETRY_OPRGB		= 7,
	DRM_MODE_COLORIMETRY_BT2020_CYCC	= 8,
	DRM_MODE_COLORIMETRY_BT2020_RGB		= 9,
	DRM_MODE_COLORIMETRY_BT2020_YCC		= 10,
	
	DRM_MODE_COLORIMETRY_DCI_P3_RGB_D65	= 11,
	DRM_MODE_COLORIMETRY_DCI_P3_RGB_THEATER	= 12,
	
	DRM_MODE_COLORIMETRY_RGB_WIDE_FIXED	= 13,
	DRM_MODE_COLORIMETRY_RGB_WIDE_FLOAT	= 14,
	DRM_MODE_COLORIMETRY_BT601_YCC		= 15,
	
	DRM_MODE_COLORIMETRY_COUNT
};


enum drm_bus_flags {
	
	DRM_BUS_FLAG_DE_LOW = BIT(0),

	
	DRM_BUS_FLAG_DE_HIGH = BIT(1),

	
	DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE = BIT(2),

	
	DRM_BUS_FLAG_PIXDATA_DRIVE_NEGEDGE = BIT(3),

	
	DRM_BUS_FLAG_PIXDATA_SAMPLE_POSEDGE = DRM_BUS_FLAG_PIXDATA_DRIVE_NEGEDGE,

	
	DRM_BUS_FLAG_PIXDATA_SAMPLE_NEGEDGE = DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE,

	
	DRM_BUS_FLAG_DATA_MSB_TO_LSB = BIT(4),

	
	DRM_BUS_FLAG_DATA_LSB_TO_MSB = BIT(5),

	
	DRM_BUS_FLAG_SYNC_DRIVE_POSEDGE = BIT(6),

	
	DRM_BUS_FLAG_SYNC_DRIVE_NEGEDGE = BIT(7),

	
	DRM_BUS_FLAG_SYNC_SAMPLE_POSEDGE = DRM_BUS_FLAG_SYNC_DRIVE_NEGEDGE,

	
	DRM_BUS_FLAG_SYNC_SAMPLE_NEGEDGE = DRM_BUS_FLAG_SYNC_DRIVE_POSEDGE,

	
	DRM_BUS_FLAG_SHARP_SIGNALS = BIT(8),
};


struct drm_display_info {
	
	unsigned int width_mm;

	
	unsigned int height_mm;

	
	unsigned int bpc;

	
	enum subpixel_order subpixel_order;

#define DRM_COLOR_FORMAT_RGB444		(1<<0)
#define DRM_COLOR_FORMAT_YCBCR444	(1<<1)
#define DRM_COLOR_FORMAT_YCBCR422	(1<<2)
#define DRM_COLOR_FORMAT_YCBCR420	(1<<3)

	
	int panel_orientation;

	
	u32 color_formats;

	
	const u32 *bus_formats;
	
	unsigned int num_bus_formats;

	
	u32 bus_flags;

	
	int max_tmds_clock;

	
	bool dvi_dual;

	
	bool is_hdmi;

	
	bool has_audio;

	
	bool has_hdmi_infoframe;

	
	bool rgb_quant_range_selectable;

	
	u8 edid_hdmi_rgb444_dc_modes;

	
	u8 edid_hdmi_ycbcr444_dc_modes;

	
	u8 cea_rev;

	
	struct drm_hdmi_info hdmi;

	
	bool non_desktop;

	
	struct drm_monitor_range_info monitor_range;

	
	struct drm_luminance_range_info luminance_range;

	
	u8 mso_stream_count;

	
	u8 mso_pixel_overlap;

	
	u32 max_dsc_bpp;

	
	u8 *vics;

	
	int vics_len;

	
	u32 quirks;
};

int drm_display_info_set_bus_formats(struct drm_display_info *info,
				     const u32 *formats,
				     unsigned int num_formats);


struct drm_connector_tv_margins {
	
	unsigned int bottom;

	
	unsigned int left;

	
	unsigned int right;

	
	unsigned int top;
};


struct drm_tv_connector_state {
	enum drm_mode_subconnector select_subconnector;
	enum drm_mode_subconnector subconnector;
	struct drm_connector_tv_margins margins;
	unsigned int legacy_mode;
	unsigned int mode;
	unsigned int brightness;
	unsigned int contrast;
	unsigned int flicker_reduction;
	unsigned int overscan;
	unsigned int saturation;
	unsigned int hue;
};


struct drm_connector_state {
	
	struct drm_connector *connector;

	
	struct drm_crtc *crtc;

	
	struct drm_encoder *best_encoder;

	
	enum drm_link_status link_status;

	
	struct drm_atomic_state *state;

	
	struct drm_crtc_commit *commit;

	
	struct drm_tv_connector_state tv;

	
	bool self_refresh_aware;

	
	enum hdmi_picture_aspect picture_aspect_ratio;

	
	unsigned int content_type;

	
	unsigned int hdcp_content_type;

	
	unsigned int scaling_mode;

	
	unsigned int content_protection;

	
	enum drm_colorspace colorspace;

	
	struct drm_writeback_job *writeback_job;

	
	u8 max_requested_bpc;

	
	u8 max_bpc;

	
	enum drm_privacy_screen_status privacy_screen_sw_state;

	
	struct drm_property_blob *hdr_output_metadata;

	
	struct drm_property_blob *gen_hdr_output_metadata;

};


struct drm_connector_funcs {
	
	int (*dpms)(struct drm_connector *connector, int mode);

	
	void (*reset)(struct drm_connector *connector);

	
	enum drm_connector_status (*detect)(struct drm_connector *connector,
					    bool force);

	
	void (*force)(struct drm_connector *connector);

	
	int (*fill_modes)(struct drm_connector *connector, uint32_t max_width, uint32_t max_height);

	
	int (*set_property)(struct drm_connector *connector, struct drm_property *property,
			     uint64_t val);

	
	int (*late_register)(struct drm_connector *connector);

	
	void (*early_unregister)(struct drm_connector *connector);

	
	void (*destroy)(struct drm_connector *connector);

	
	struct drm_connector_state *(*atomic_duplicate_state)(struct drm_connector *connector);

	
	void (*atomic_destroy_state)(struct drm_connector *connector,
				     struct drm_connector_state *state);

	
	int (*atomic_set_property)(struct drm_connector *connector,
				   struct drm_connector_state *state,
				   struct drm_property *property,
				   uint64_t val);

	
	int (*atomic_get_property)(struct drm_connector *connector,
				   const struct drm_connector_state *state,
				   struct drm_property *property,
				   uint64_t *val);

	
	void (*atomic_print_state)(struct drm_printer *p,
				   const struct drm_connector_state *state);

	
	void (*oob_hotplug_event)(struct drm_connector *connector);

	
	void (*debugfs_init)(struct drm_connector *connector, struct dentry *root);
};


struct drm_cmdline_mode {
	
	char name[DRM_DISPLAY_MODE_LEN];

	
	bool specified;

	
	bool refresh_specified;

	
	bool bpp_specified;

	
	unsigned int pixel_clock;

	
	int xres;

	
	int yres;

	
	int bpp;

	
	int refresh;

	
	bool rb;

	
	bool interlace;

	
	bool cvt;

	
	bool margins;

	
	enum drm_connector_force force;

	
	unsigned int rotation_reflection;

	
	enum drm_panel_orientation panel_orientation;

	
	struct drm_connector_tv_margins tv_margins;

	
	enum drm_connector_tv_mode tv_mode;

	
	bool tv_mode_specified;
};


struct drm_connector {
	
	struct drm_device *dev;
	
	struct device *kdev;
	
	struct device_attribute *attr;
	
	struct fwnode_handle *fwnode;

	
	struct list_head head;

	
	struct list_head global_connector_list_entry;

	
	struct drm_mode_object base;

	
	char *name;

	
	struct mutex mutex;

	
	unsigned index;

	
	int connector_type;
	
	int connector_type_id;
	
	bool interlace_allowed;
	
	bool doublescan_allowed;
	
	bool stereo_allowed;

	
	bool ycbcr_420_allowed;

	
	enum drm_connector_registration_state registration_state;

	
	struct list_head modes;

	
	enum drm_connector_status status;

	
	struct list_head probed_modes;

	
	struct drm_display_info display_info;

	
	const struct drm_connector_funcs *funcs;

	
	struct drm_property_blob *edid_blob_ptr;

	
	struct drm_object_properties properties;

	
	struct drm_property *scaling_mode_property;

	
	struct drm_property *vrr_capable_property;

	
	struct drm_property *colorspace_property;

	
	struct drm_property_blob *path_blob_ptr;

	
	struct drm_property *max_bpc_property;

	
	struct drm_privacy_screen *privacy_screen;

	
	struct notifier_block privacy_screen_notifier;

	
	struct drm_property *privacy_screen_sw_state_property;

	
	struct drm_property *privacy_screen_hw_state_property;

#define DRM_CONNECTOR_POLL_HPD (1 << 0)
#define DRM_CONNECTOR_POLL_CONNECT (1 << 1)
#define DRM_CONNECTOR_POLL_DISCONNECT (1 << 2)

	
	uint8_t polled;

	
	int dpms;

	
	const struct drm_connector_helper_funcs *helper_private;

	
	struct drm_cmdline_mode cmdline_mode;
	
	enum drm_connector_force force;

	
	const struct drm_edid *edid_override;

	
	struct mutex edid_override_mutex;

	
	u64 epoch_counter;

	
	u32 possible_encoders;

	
	struct drm_encoder *encoder;

#define MAX_ELD_BYTES	128
	
	uint8_t eld[MAX_ELD_BYTES];
	
	bool latency_present[2];
	
	int video_latency[2];
	
	int audio_latency[2];

	
	struct i2c_adapter *ddc;

	
	int null_edid_counter;

	
	unsigned bad_edid_counter;

	
	bool edid_corrupt;
	
	u8 real_edid_checksum;

	
	struct dentry *debugfs_entry;

	
	struct drm_connector_state *state;

	

	
	struct drm_property_blob *tile_blob_ptr;

	
	bool has_tile;
	
	struct drm_tile_group *tile_group;
	
	bool tile_is_single_monitor;

	
	
	uint8_t num_h_tile, num_v_tile;
	
	
	uint8_t tile_h_loc, tile_v_loc;
	
	
	uint16_t tile_h_size, tile_v_size;

	
	struct llist_node free_node;

	
	struct hdr_sink_metadata hdr_sink_metadata;
};

#define obj_to_connector(x) container_of(x, struct drm_connector, base)

int drm_connector_init(struct drm_device *dev,
		       struct drm_connector *connector,
		       const struct drm_connector_funcs *funcs,
		       int connector_type);
int drm_connector_init_with_ddc(struct drm_device *dev,
				struct drm_connector *connector,
				const struct drm_connector_funcs *funcs,
				int connector_type,
				struct i2c_adapter *ddc);
int drmm_connector_init(struct drm_device *dev,
			struct drm_connector *connector,
			const struct drm_connector_funcs *funcs,
			int connector_type,
			struct i2c_adapter *ddc);
void drm_connector_attach_edid_property(struct drm_connector *connector);
int drm_connector_register(struct drm_connector *connector);
void drm_connector_unregister(struct drm_connector *connector);
int drm_connector_attach_encoder(struct drm_connector *connector,
				      struct drm_encoder *encoder);

void drm_connector_cleanup(struct drm_connector *connector);

static inline unsigned int drm_connector_index(const struct drm_connector *connector)
{
	return connector->index;
}

static inline u32 drm_connector_mask(const struct drm_connector *connector)
{
	return 1 << connector->index;
}


static inline struct drm_connector *drm_connector_lookup(struct drm_device *dev,
		struct drm_file *file_priv,
		uint32_t id)
{
	struct drm_mode_object *mo;
	mo = drm_mode_object_find(dev, file_priv, id, DRM_MODE_OBJECT_CONNECTOR);
	return mo ? obj_to_connector(mo) : NULL;
}


static inline void drm_connector_get(struct drm_connector *connector)
{
	drm_mode_object_get(&connector->base);
}


static inline void drm_connector_put(struct drm_connector *connector)
{
	drm_mode_object_put(&connector->base);
}


static inline bool
drm_connector_is_unregistered(struct drm_connector *connector)
{
	return READ_ONCE(connector->registration_state) ==
		DRM_CONNECTOR_UNREGISTERED;
}

void drm_connector_oob_hotplug_event(struct fwnode_handle *connector_fwnode);
const char *drm_get_connector_type_name(unsigned int connector_type);
const char *drm_get_connector_status_name(enum drm_connector_status status);
const char *drm_get_subpixel_order_name(enum subpixel_order order);
const char *drm_get_dpms_name(int val);
const char *drm_get_dvi_i_subconnector_name(int val);
const char *drm_get_dvi_i_select_name(int val);
const char *drm_get_tv_mode_name(int val);
const char *drm_get_tv_subconnector_name(int val);
const char *drm_get_tv_select_name(int val);
const char *drm_get_dp_subconnector_name(int val);
const char *drm_get_content_protection_name(int val);
const char *drm_get_hdcp_content_type_name(int val);

int drm_get_tv_mode_from_name(const char *name, size_t len);

int drm_mode_create_dvi_i_properties(struct drm_device *dev);
void drm_connector_attach_dp_subconnector_property(struct drm_connector *connector);

int drm_mode_create_tv_margin_properties(struct drm_device *dev);
int drm_mode_create_tv_properties_legacy(struct drm_device *dev,
					 unsigned int num_modes,
					 const char * const modes[]);
int drm_mode_create_tv_properties(struct drm_device *dev,
				  unsigned int supported_tv_modes);
void drm_connector_attach_tv_margin_properties(struct drm_connector *conn);
int drm_mode_create_scaling_mode_property(struct drm_device *dev);
int drm_connector_attach_content_type_property(struct drm_connector *dev);
int drm_connector_attach_scaling_mode_property(struct drm_connector *connector,
					       u32 scaling_mode_mask);
int drm_connector_attach_vrr_capable_property(
		struct drm_connector *connector);
int drm_connector_attach_colorspace_property(struct drm_connector *connector);
int drm_connector_attach_hdr_output_metadata_property(struct drm_connector *connector);
bool drm_connector_atomic_hdr_metadata_equal(struct drm_connector_state *old_state,
					     struct drm_connector_state *new_state);
int drm_mode_create_aspect_ratio_property(struct drm_device *dev);
int drm_mode_create_hdmi_colorspace_property(struct drm_connector *connector,
					     u32 supported_colorspaces);
int drm_mode_create_dp_colorspace_property(struct drm_connector *connector,
					   u32 supported_colorspaces);
int drm_mode_create_content_type_property(struct drm_device *dev);
int drm_mode_create_suggested_offset_properties(struct drm_device *dev);

int drm_connector_set_path_property(struct drm_connector *connector,
				    const char *path);
int drm_connector_set_tile_property(struct drm_connector *connector);
int drm_connector_update_edid_property(struct drm_connector *connector,
				       const struct edid *edid);
void drm_connector_set_link_status_property(struct drm_connector *connector,
					    uint64_t link_status);
void drm_connector_set_vrr_capable_property(
		struct drm_connector *connector, bool capable);
int drm_connector_set_panel_orientation(
	struct drm_connector *connector,
	enum drm_panel_orientation panel_orientation);
int drm_connector_set_panel_orientation_with_quirk(
	struct drm_connector *connector,
	enum drm_panel_orientation panel_orientation,
	int width, int height);
int drm_connector_set_orientation_from_panel(
	struct drm_connector *connector,
	struct drm_panel *panel);
int drm_connector_attach_max_bpc_property(struct drm_connector *connector,
					  int min, int max);
void drm_connector_create_privacy_screen_properties(struct drm_connector *conn);
void drm_connector_attach_privacy_screen_properties(struct drm_connector *conn);
void drm_connector_attach_privacy_screen_provider(
	struct drm_connector *connector, struct drm_privacy_screen *priv);
void drm_connector_update_privacy_screen(const struct drm_connector_state *connector_state);


struct drm_tile_group {
	struct kref refcount;
	struct drm_device *dev;
	int id;
	u8 group_data[8];
};

struct drm_tile_group *drm_mode_create_tile_group(struct drm_device *dev,
						  const char topology[8]);
struct drm_tile_group *drm_mode_get_tile_group(struct drm_device *dev,
					       const char topology[8]);
void drm_mode_put_tile_group(struct drm_device *dev,
			     struct drm_tile_group *tg);


struct drm_connector_list_iter {

	struct drm_device *dev;
	struct drm_connector *conn;
};

void drm_connector_list_iter_begin(struct drm_device *dev,
				   struct drm_connector_list_iter *iter);
struct drm_connector *
drm_connector_list_iter_next(struct drm_connector_list_iter *iter);
void drm_connector_list_iter_end(struct drm_connector_list_iter *iter);

bool drm_connector_has_possible_encoder(struct drm_connector *connector,
					struct drm_encoder *encoder);
const char *drm_get_colorspace_name(enum drm_colorspace colorspace);


#define drm_for_each_connector_iter(connector, iter) \
	while ((connector = drm_connector_list_iter_next(iter)))


#define drm_connector_for_each_possible_encoder(connector, encoder) \
	drm_for_each_encoder_mask(encoder, (connector)->dev, \
				  (connector)->possible_encoders)

#endif
