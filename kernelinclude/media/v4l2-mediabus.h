/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef V4L2_MEDIABUS_H
#define V4L2_MEDIABUS_H

#include <linux/v4l2-mediabus.h>
#include <linux/bitops.h>





#define V4L2_MBUS_MASTER			BIT(0)
#define V4L2_MBUS_SLAVE				BIT(1)

#define V4L2_MBUS_HSYNC_ACTIVE_HIGH		BIT(2)
#define V4L2_MBUS_HSYNC_ACTIVE_LOW		BIT(3)
#define V4L2_MBUS_VSYNC_ACTIVE_HIGH		BIT(4)
#define V4L2_MBUS_VSYNC_ACTIVE_LOW		BIT(5)
#define V4L2_MBUS_PCLK_SAMPLE_RISING		BIT(6)
#define V4L2_MBUS_PCLK_SAMPLE_FALLING		BIT(7)
#define V4L2_MBUS_PCLK_SAMPLE_DUALEDGE		BIT(8)
#define V4L2_MBUS_DATA_ACTIVE_HIGH		BIT(9)
#define V4L2_MBUS_DATA_ACTIVE_LOW		BIT(10)

#define V4L2_MBUS_FIELD_EVEN_HIGH		BIT(11)

#define V4L2_MBUS_FIELD_EVEN_LOW		BIT(12)

#define V4L2_MBUS_VIDEO_SOG_ACTIVE_HIGH		BIT(13)
#define V4L2_MBUS_VIDEO_SOG_ACTIVE_LOW		BIT(14)
#define V4L2_MBUS_DATA_ENABLE_HIGH		BIT(15)
#define V4L2_MBUS_DATA_ENABLE_LOW		BIT(16)



#define V4L2_MBUS_CSI2_NONCONTINUOUS_CLOCK	BIT(0)

#define V4L2_MBUS_CSI2_MAX_DATA_LANES		8


struct v4l2_mbus_config_mipi_csi2 {
	unsigned int flags;
	unsigned char data_lanes[V4L2_MBUS_CSI2_MAX_DATA_LANES];
	unsigned char clock_lane;
	unsigned char num_data_lanes;
	bool lane_polarities[1 + V4L2_MBUS_CSI2_MAX_DATA_LANES];
};


struct v4l2_mbus_config_parallel {
	unsigned int flags;
	unsigned char bus_width;
	unsigned char data_shift;
};


struct v4l2_mbus_config_mipi_csi1 {
	unsigned char clock_inv:1;
	unsigned char strobe:1;
	bool lane_polarity[2];
	unsigned char data_lane;
	unsigned char clock_lane;
};


enum v4l2_mbus_type {
	V4L2_MBUS_UNKNOWN,
	V4L2_MBUS_PARALLEL,
	V4L2_MBUS_BT656,
	V4L2_MBUS_CSI1,
	V4L2_MBUS_CCP2,
	V4L2_MBUS_CSI2_DPHY,
	V4L2_MBUS_CSI2_CPHY,
	V4L2_MBUS_DPI,
	V4L2_MBUS_INVALID,
};


struct v4l2_mbus_config {
	enum v4l2_mbus_type type;
	union {
		struct v4l2_mbus_config_parallel parallel;
		struct v4l2_mbus_config_mipi_csi1 mipi_csi1;
		struct v4l2_mbus_config_mipi_csi2 mipi_csi2;
	} bus;
};


static inline void
v4l2_fill_pix_format(struct v4l2_pix_format *pix_fmt,
		     const struct v4l2_mbus_framefmt *mbus_fmt)
{
	pix_fmt->width = mbus_fmt->width;
	pix_fmt->height = mbus_fmt->height;
	pix_fmt->field = mbus_fmt->field;
	pix_fmt->colorspace = mbus_fmt->colorspace;
	pix_fmt->ycbcr_enc = mbus_fmt->ycbcr_enc;
	pix_fmt->quantization = mbus_fmt->quantization;
	pix_fmt->xfer_func = mbus_fmt->xfer_func;
}


static inline void v4l2_fill_mbus_format(struct v4l2_mbus_framefmt *mbus_fmt,
					 const struct v4l2_pix_format *pix_fmt,
			   u32 code)
{
	mbus_fmt->width = pix_fmt->width;
	mbus_fmt->height = pix_fmt->height;
	mbus_fmt->field = pix_fmt->field;
	mbus_fmt->colorspace = pix_fmt->colorspace;
	mbus_fmt->ycbcr_enc = pix_fmt->ycbcr_enc;
	mbus_fmt->quantization = pix_fmt->quantization;
	mbus_fmt->xfer_func = pix_fmt->xfer_func;
	mbus_fmt->code = code;
}


static inline void
v4l2_fill_pix_format_mplane(struct v4l2_pix_format_mplane *pix_mp_fmt,
			    const struct v4l2_mbus_framefmt *mbus_fmt)
{
	pix_mp_fmt->width = mbus_fmt->width;
	pix_mp_fmt->height = mbus_fmt->height;
	pix_mp_fmt->field = mbus_fmt->field;
	pix_mp_fmt->colorspace = mbus_fmt->colorspace;
	pix_mp_fmt->ycbcr_enc = mbus_fmt->ycbcr_enc;
	pix_mp_fmt->quantization = mbus_fmt->quantization;
	pix_mp_fmt->xfer_func = mbus_fmt->xfer_func;
}


static inline void
v4l2_fill_mbus_format_mplane(struct v4l2_mbus_framefmt *mbus_fmt,
			     const struct v4l2_pix_format_mplane *pix_mp_fmt)
{
	mbus_fmt->width = pix_mp_fmt->width;
	mbus_fmt->height = pix_mp_fmt->height;
	mbus_fmt->field = pix_mp_fmt->field;
	mbus_fmt->colorspace = pix_mp_fmt->colorspace;
	mbus_fmt->ycbcr_enc = pix_mp_fmt->ycbcr_enc;
	mbus_fmt->quantization = pix_mp_fmt->quantization;
	mbus_fmt->xfer_func = pix_mp_fmt->xfer_func;
}

#endif
