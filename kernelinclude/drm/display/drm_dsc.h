/* SPDX-License-Identifier: MIT
 * Copyright (C) 2018 Intel Corp.
 *
 * Authors:
 * Manasi Navare <manasi.d.navare@intel.com>
 */

#ifndef DRM_DSC_H_
#define DRM_DSC_H_

#include <drm/display/drm_dp.h>


#define DSC_NUM_BUF_RANGES			15
#define DSC_MUX_WORD_SIZE_8_10_BPC		48
#define DSC_MUX_WORD_SIZE_12_BPC		64
#define DSC_RC_PIXELS_PER_GROUP			3
#define DSC_SCALE_DECREMENT_INTERVAL_MAX	4095
#define DSC_RANGE_BPG_OFFSET_MASK		0x3f


#define DSC_RC_MODEL_SIZE_CONST		    8192
#define DSC_RC_EDGE_FACTOR_CONST	    6
#define DSC_RC_TGT_OFFSET_HI_CONST	    3
#define DSC_RC_TGT_OFFSET_LO_CONST	    3


#define DSC_PPS_VERSION_MAJOR_SHIFT		4
#define DSC_PPS_BPC_SHIFT			4
#define DSC_PPS_MSB_SHIFT			8
#define DSC_PPS_LSB_MASK			(0xFF << 0)
#define DSC_PPS_BPP_HIGH_MASK			(0x3 << 8)
#define DSC_PPS_VBR_EN_SHIFT			2
#define DSC_PPS_SIMPLE422_SHIFT			3
#define DSC_PPS_CONVERT_RGB_SHIFT		4
#define DSC_PPS_BLOCK_PRED_EN_SHIFT		5
#define DSC_PPS_INIT_XMIT_DELAY_HIGH_MASK	(0x3 << 8)
#define DSC_PPS_SCALE_DEC_INT_HIGH_MASK		(0xF << 8)
#define DSC_PPS_RC_TGT_OFFSET_HI_SHIFT		4
#define DSC_PPS_RC_RANGE_MINQP_SHIFT		11
#define DSC_PPS_RC_RANGE_MAXQP_SHIFT		6
#define DSC_PPS_NATIVE_420_SHIFT		1
#define DSC_1_2_MAX_LINEBUF_DEPTH_BITS		16
#define DSC_1_2_MAX_LINEBUF_DEPTH_VAL		0
#define DSC_1_1_MAX_LINEBUF_DEPTH_BITS		13


struct drm_dsc_rc_range_parameters {
	
	u8 range_min_qp;
	
	u8 range_max_qp;
	
	u8 range_bpg_offset;
};


struct drm_dsc_config {
	
	u8 line_buf_depth;
	
	u8 bits_per_component;
	
	bool convert_rgb;
	
	u8 slice_count;
	
	u16 slice_width;
	
	u16 slice_height;
	
	bool simple_422;
	
	u16 pic_width;
	
	u16 pic_height;
	
	u8 rc_tgt_offset_high;
	
	u8 rc_tgt_offset_low;
	
	u16 bits_per_pixel;
	
	u8 rc_edge_factor;
	
	u8 rc_quant_incr_limit1;
	
	u8 rc_quant_incr_limit0;
	
	u16 initial_xmit_delay;
	
	u16  initial_dec_delay;
	
	bool block_pred_enable;
	
	u8 first_line_bpg_offset;
	
	u16 initial_offset;
	
	u16 rc_buf_thresh[DSC_NUM_BUF_RANGES - 1];
	
	struct drm_dsc_rc_range_parameters rc_range_params[DSC_NUM_BUF_RANGES];
	
	u16 rc_model_size;
	
	u8 flatness_min_qp;
	
	u8 flatness_max_qp;
	
	u8 initial_scale_value;
	
	u16 scale_decrement_interval;
	
	u16 scale_increment_interval;
	
	u16 nfl_bpg_offset;
	
	u16 slice_bpg_offset;
	
	u16 final_offset;
	
	bool vbr_enable;
	
	u8 mux_word_size;
	
	u16 slice_chunk_size;
	
	u16 rc_bits;
	
	u8 dsc_version_minor;
	
	u8 dsc_version_major;
	
	bool native_422;
	
	bool native_420;
	
	u8 second_line_bpg_offset;
	
	u16 nsl_bpg_offset;
	
	u16 second_line_offset_adj;
};


struct drm_dsc_picture_parameter_set {
	
	u8 dsc_version;
	
	u8 pps_identifier;
	
	u8 pps_reserved;
	
	u8 pps_3;
	
	u8 pps_4;
	
	u8 bits_per_pixel_low;
	
	__be16 pic_height;
	
	__be16 pic_width;
	
	__be16 slice_height;
	
	__be16 slice_width;
	
	__be16 chunk_size;
	
	u8 initial_xmit_delay_high;
	
	u8 initial_xmit_delay_low;
	
	__be16 initial_dec_delay;
	
	u8 pps20_reserved;
	
	u8 initial_scale_value;
	
	__be16 scale_increment_interval;
	
	u8 scale_decrement_interval_high;
	
	u8 scale_decrement_interval_low;
	
	u8 pps26_reserved;
	
	u8 first_line_bpg_offset;
	
	__be16 nfl_bpg_offset;
	
	__be16 slice_bpg_offset;
	
	__be16 initial_offset;
	
	__be16 final_offset;
	
	u8 flatness_min_qp;
	
	u8 flatness_max_qp;
	
	__be16 rc_model_size;
	
	u8 rc_edge_factor;
	
	u8 rc_quant_incr_limit0;
	
	u8 rc_quant_incr_limit1;
	
	u8 rc_tgt_offset;
	
	u8 rc_buf_thresh[DSC_NUM_BUF_RANGES - 1];
	
	__be16 rc_range_parameters[DSC_NUM_BUF_RANGES];
	
	u8 native_422_420;
	
	u8 second_line_bpg_offset;
	
	__be16 nsl_bpg_offset;
	
	__be16 second_line_offset_adj;
	
	u32 pps_long_94_reserved;
	
	u32 pps_long_98_reserved;
	
	u32 pps_long_102_reserved;
	
	u32 pps_long_106_reserved;
	
	u32 pps_long_110_reserved;
	
	u32 pps_long_114_reserved;
	
	u32 pps_long_118_reserved;
	
	u32 pps_long_122_reserved;
	
	__be16 pps_short_126_reserved;
} __packed;


struct drm_dsc_pps_infoframe {
	struct dp_sdp_header pps_header;
	struct drm_dsc_picture_parameter_set pps_payload;
} __packed;

#endif 
