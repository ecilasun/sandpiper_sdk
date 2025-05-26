/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __V4L2_DV_TIMINGS_H
#define __V4L2_DV_TIMINGS_H

#include <linux/videodev2.h>


struct v4l2_fract v4l2_calc_timeperframe(const struct v4l2_dv_timings *t);


extern const struct v4l2_dv_timings v4l2_dv_timings_presets[];


typedef bool v4l2_check_dv_timings_fnc(const struct v4l2_dv_timings *t, void *handle);


bool v4l2_valid_dv_timings(const struct v4l2_dv_timings *t,
			   const struct v4l2_dv_timings_cap *cap,
			   v4l2_check_dv_timings_fnc fnc,
			   void *fnc_handle);


int v4l2_enum_dv_timings_cap(struct v4l2_enum_dv_timings *t,
			     const struct v4l2_dv_timings_cap *cap,
			     v4l2_check_dv_timings_fnc fnc,
			     void *fnc_handle);


bool v4l2_find_dv_timings_cap(struct v4l2_dv_timings *t,
			      const struct v4l2_dv_timings_cap *cap,
			      unsigned pclock_delta,
			      v4l2_check_dv_timings_fnc fnc,
			      void *fnc_handle);


bool v4l2_find_dv_timings_cea861_vic(struct v4l2_dv_timings *t, u8 vic);


bool v4l2_match_dv_timings(const struct v4l2_dv_timings *measured,
			   const struct v4l2_dv_timings *standard,
			   unsigned pclock_delta, bool match_reduced_fps);


void v4l2_print_dv_timings(const char *dev_prefix, const char *prefix,
			   const struct v4l2_dv_timings *t, bool detailed);


bool v4l2_detect_cvt(unsigned frame_height, unsigned hfreq, unsigned vsync,
		unsigned active_width, u32 polarities, bool interlaced,
		struct v4l2_dv_timings *fmt);


bool v4l2_detect_gtf(unsigned frame_height, unsigned hfreq, unsigned vsync,
		u32 polarities, bool interlaced, struct v4l2_fract aspect,
		struct v4l2_dv_timings *fmt);


struct v4l2_fract v4l2_calc_aspect_ratio(u8 hor_landscape, u8 vert_portrait);


struct v4l2_fract v4l2_dv_timings_aspect_ratio(const struct v4l2_dv_timings *t);


static inline  bool can_reduce_fps(struct v4l2_bt_timings *bt)
{
	if ((bt->standards & V4L2_DV_BT_STD_CVT) && (bt->vsync == 8))
		return true;

	if ((bt->standards & V4L2_DV_BT_STD_CEA861) &&
	    (bt->flags & V4L2_DV_FL_CAN_REDUCE_FPS))
		return true;

	return false;
}


struct v4l2_hdmi_colorimetry {
	enum v4l2_colorspace colorspace;
	enum v4l2_ycbcr_encoding ycbcr_enc;
	enum v4l2_quantization quantization;
	enum v4l2_xfer_func xfer_func;
};

struct hdmi_avi_infoframe;
struct hdmi_vendor_infoframe;

struct v4l2_hdmi_colorimetry
v4l2_hdmi_rx_colorimetry(const struct hdmi_avi_infoframe *avi,
			 const struct hdmi_vendor_infoframe *hdmi,
			 unsigned int height);

u16 v4l2_get_edid_phys_addr(const u8 *edid, unsigned int size,
			    unsigned int *offset);
void v4l2_set_edid_phys_addr(u8 *edid, unsigned int size, u16 phys_addr);
u16 v4l2_phys_addr_for_input(u16 phys_addr, u8 input);
int v4l2_phys_addr_validate(u16 phys_addr, u16 *parent, u16 *port);

#endif
