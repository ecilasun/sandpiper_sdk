
#ifndef __DRM_MODES_H__
#define __DRM_MODES_H__

#include <linux/hdmi.h>

#include <drm/drm_mode_object.h>
#include <drm/drm_connector.h>

struct videomode;




enum drm_mode_status {
	MODE_OK = 0,
	MODE_HSYNC,
	MODE_VSYNC,
	MODE_H_ILLEGAL,
	MODE_V_ILLEGAL,
	MODE_BAD_WIDTH,
	MODE_NOMODE,
	MODE_NO_INTERLACE,
	MODE_NO_DBLESCAN,
	MODE_NO_VSCAN,
	MODE_MEM,
	MODE_VIRTUAL_X,
	MODE_VIRTUAL_Y,
	MODE_MEM_VIRT,
	MODE_NOCLOCK,
	MODE_CLOCK_HIGH,
	MODE_CLOCK_LOW,
	MODE_CLOCK_RANGE,
	MODE_BAD_HVALUE,
	MODE_BAD_VVALUE,
	MODE_BAD_VSCAN,
	MODE_HSYNC_NARROW,
	MODE_HSYNC_WIDE,
	MODE_HBLANK_NARROW,
	MODE_HBLANK_WIDE,
	MODE_VSYNC_NARROW,
	MODE_VSYNC_WIDE,
	MODE_VBLANK_NARROW,
	MODE_VBLANK_WIDE,
	MODE_PANEL,
	MODE_INTERLACE_WIDTH,
	MODE_ONE_WIDTH,
	MODE_ONE_HEIGHT,
	MODE_ONE_SIZE,
	MODE_NO_REDUCED,
	MODE_NO_STEREO,
	MODE_NO_420,
	MODE_STALE = -3,
	MODE_BAD = -2,
	MODE_ERROR = -1
};

#define DRM_MODE(nm, t, c, hd, hss, hse, ht, hsk, vd, vss, vse, vt, vs, f) \
	.name = nm, .status = 0, .type = (t), .clock = (c), \
	.hdisplay = (hd), .hsync_start = (hss), .hsync_end = (hse), \
	.htotal = (ht), .hskew = (hsk), .vdisplay = (vd), \
	.vsync_start = (vss), .vsync_end = (vse), .vtotal = (vt), \
	.vscan = (vs), .flags = (f)


#define DRM_MODE_RES_MM(res, dpi)	\
	(((res) * 254ul) / ((dpi) * 10ul))

#define __DRM_MODE_INIT(pix, hd, vd, hd_mm, vd_mm) \
	.type = DRM_MODE_TYPE_DRIVER, .clock = (pix), \
	.hdisplay = (hd), .hsync_start = (hd), .hsync_end = (hd), \
	.htotal = (hd), .vdisplay = (vd), .vsync_start = (vd), \
	.vsync_end = (vd), .vtotal = (vd), .width_mm = (hd_mm), \
	.height_mm = (vd_mm)


#define DRM_MODE_INIT(hz, hd, vd, hd_mm, vd_mm) \
	__DRM_MODE_INIT((hd) * (vd) * (hz) / 1000 , hd, vd, hd_mm, vd_mm)


#define DRM_SIMPLE_MODE(hd, vd, hd_mm, vd_mm) \
	__DRM_MODE_INIT(1 , hd, vd, hd_mm, vd_mm)

#define CRTC_INTERLACE_HALVE_V	(1 << 0) 
#define CRTC_STEREO_DOUBLE	(1 << 1) 
#define CRTC_NO_DBLSCAN		(1 << 2) 
#define CRTC_NO_VSCAN		(1 << 3) 
#define CRTC_STEREO_DOUBLE_ONLY	(CRTC_STEREO_DOUBLE | CRTC_NO_DBLSCAN | CRTC_NO_VSCAN)

#define DRM_MODE_FLAG_3D_MAX	DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF

#define DRM_MODE_MATCH_TIMINGS (1 << 0)
#define DRM_MODE_MATCH_CLOCK (1 << 1)
#define DRM_MODE_MATCH_FLAGS (1 << 2)
#define DRM_MODE_MATCH_3D_FLAGS (1 << 3)
#define DRM_MODE_MATCH_ASPECT_RATIO (1 << 4)


struct drm_display_mode {
	
	int clock;		
	u16 hdisplay;
	u16 hsync_start;
	u16 hsync_end;
	u16 htotal;
	u16 hskew;
	u16 vdisplay;
	u16 vsync_start;
	u16 vsync_end;
	u16 vtotal;
	u16 vscan;
	
	u32 flags;

	
	int crtc_clock;
	u16 crtc_hdisplay;
	u16 crtc_hblank_start;
	u16 crtc_hblank_end;
	u16 crtc_hsync_start;
	u16 crtc_hsync_end;
	u16 crtc_htotal;
	u16 crtc_hskew;
	u16 crtc_vdisplay;
	u16 crtc_vblank_start;
	u16 crtc_vblank_end;
	u16 crtc_vsync_start;
	u16 crtc_vsync_end;
	u16 crtc_vtotal;

	
	u16 width_mm;

	
	u16 height_mm;

	
	u8 type;

	
	bool expose_to_userspace;

	
	struct list_head head;

	
	char name[DRM_DISPLAY_MODE_LEN];

	
	enum drm_mode_status status;

	
	enum hdmi_picture_aspect picture_aspect_ratio;

};


#define DRM_MODE_FMT    "\"%s\": %d %d %d %d %d %d %d %d %d %d 0x%x 0x%x"


#define DRM_MODE_ARG(m) \
	(m)->name, drm_mode_vrefresh(m), (m)->clock, \
	(m)->hdisplay, (m)->hsync_start, (m)->hsync_end, (m)->htotal, \
	(m)->vdisplay, (m)->vsync_start, (m)->vsync_end, (m)->vtotal, \
	(m)->type, (m)->flags

#define obj_to_mode(x) container_of(x, struct drm_display_mode, base)


static inline bool drm_mode_is_stereo(const struct drm_display_mode *mode)
{
	return mode->flags & DRM_MODE_FLAG_3D_MASK;
}

struct drm_connector;
struct drm_cmdline_mode;

struct drm_display_mode *drm_mode_create(struct drm_device *dev);
void drm_mode_destroy(struct drm_device *dev, struct drm_display_mode *mode);
void drm_mode_convert_to_umode(struct drm_mode_modeinfo *out,
			       const struct drm_display_mode *in);
int drm_mode_convert_umode(struct drm_device *dev,
			   struct drm_display_mode *out,
			   const struct drm_mode_modeinfo *in);
void drm_mode_probed_add(struct drm_connector *connector, struct drm_display_mode *mode);
void drm_mode_debug_printmodeline(const struct drm_display_mode *mode);
bool drm_mode_is_420_only(const struct drm_display_info *display,
			  const struct drm_display_mode *mode);
bool drm_mode_is_420_also(const struct drm_display_info *display,
			  const struct drm_display_mode *mode);
bool drm_mode_is_420(const struct drm_display_info *display,
		     const struct drm_display_mode *mode);

struct drm_display_mode *drm_analog_tv_mode(struct drm_device *dev,
					    enum drm_connector_tv_mode mode,
					    unsigned long pixel_clock_hz,
					    unsigned int hdisplay,
					    unsigned int vdisplay,
					    bool interlace);

static inline struct drm_display_mode *drm_mode_analog_ntsc_480i(struct drm_device *dev)
{
	return drm_analog_tv_mode(dev, DRM_MODE_TV_MODE_NTSC, 13500000, 720, 480, true);
}

static inline struct drm_display_mode *drm_mode_analog_pal_576i(struct drm_device *dev)
{
	return drm_analog_tv_mode(dev, DRM_MODE_TV_MODE_PAL, 13500000, 720, 576, true);
}

struct drm_display_mode *drm_cvt_mode(struct drm_device *dev,
				      int hdisplay, int vdisplay, int vrefresh,
				      bool reduced, bool interlaced,
				      bool margins);
struct drm_display_mode *drm_gtf_mode(struct drm_device *dev,
				      int hdisplay, int vdisplay, int vrefresh,
				      bool interlaced, int margins);
struct drm_display_mode *drm_gtf_mode_complex(struct drm_device *dev,
					      int hdisplay, int vdisplay,
					      int vrefresh, bool interlaced,
					      int margins,
					      int GTF_M, int GTF_2C,
					      int GTF_K, int GTF_2J);
void drm_display_mode_from_videomode(const struct videomode *vm,
				     struct drm_display_mode *dmode);
void drm_display_mode_to_videomode(const struct drm_display_mode *dmode,
				   struct videomode *vm);
void drm_bus_flags_from_videomode(const struct videomode *vm, u32 *bus_flags);

#if defined(CONFIG_OF)
int of_get_drm_display_mode(struct device_node *np,
			    struct drm_display_mode *dmode, u32 *bus_flags,
			    int index);
int of_get_drm_panel_display_mode(struct device_node *np,
				  struct drm_display_mode *dmode, u32 *bus_flags);
#else
static inline int of_get_drm_display_mode(struct device_node *np,
					  struct drm_display_mode *dmode,
					  u32 *bus_flags, int index)
{
	return -EINVAL;
}

static inline int of_get_drm_panel_display_mode(struct device_node *np,
						struct drm_display_mode *dmode, u32 *bus_flags)
{
	return -EINVAL;
}
#endif

void drm_mode_set_name(struct drm_display_mode *mode);
int drm_mode_vrefresh(const struct drm_display_mode *mode);
void drm_mode_get_hv_timing(const struct drm_display_mode *mode,
			    int *hdisplay, int *vdisplay);

void drm_mode_set_crtcinfo(struct drm_display_mode *p,
			   int adjust_flags);
void drm_mode_copy(struct drm_display_mode *dst,
		   const struct drm_display_mode *src);
void drm_mode_init(struct drm_display_mode *dst,
		   const struct drm_display_mode *src);
struct drm_display_mode *drm_mode_duplicate(struct drm_device *dev,
					    const struct drm_display_mode *mode);
bool drm_mode_match(const struct drm_display_mode *mode1,
		    const struct drm_display_mode *mode2,
		    unsigned int match_flags);
bool drm_mode_equal(const struct drm_display_mode *mode1,
		    const struct drm_display_mode *mode2);
bool drm_mode_equal_no_clocks(const struct drm_display_mode *mode1,
			      const struct drm_display_mode *mode2);
bool drm_mode_equal_no_clocks_no_stereo(const struct drm_display_mode *mode1,
					const struct drm_display_mode *mode2);


enum drm_mode_status drm_mode_validate_driver(struct drm_device *dev,
					      const struct drm_display_mode *mode);
enum drm_mode_status drm_mode_validate_size(const struct drm_display_mode *mode,
					    int maxX, int maxY);
enum drm_mode_status
drm_mode_validate_ycbcr420(const struct drm_display_mode *mode,
			   struct drm_connector *connector);
void drm_mode_prune_invalid(struct drm_device *dev,
			    struct list_head *mode_list, bool verbose);
void drm_mode_sort(struct list_head *mode_list);
void drm_connector_list_update(struct drm_connector *connector);


bool
drm_mode_parse_command_line_for_connector(const char *mode_option,
					  const struct drm_connector *connector,
					  struct drm_cmdline_mode *mode);
struct drm_display_mode *
drm_mode_create_from_cmdline_mode(struct drm_device *dev,
				  struct drm_cmdline_mode *cmd);

#endif 
