/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _MEDIA_V4L2_VP9_H
#define _MEDIA_V4L2_VP9_H

#include <media/v4l2-ctrls.h>


struct v4l2_vp9_frame_mv_context {
	u8 joint[3];
	u8 sign[2];
	u8 classes[2][10];
	u8 class0_bit[2];
	u8 bits[2][10];
	u8 class0_fr[2][2][3];
	u8 fr[2][3];
	u8 class0_hp[2];
	u8 hp[2];
};


struct v4l2_vp9_frame_context {
	u8 tx8[2][1];
	u8 tx16[2][2];
	u8 tx32[2][3];
	u8 coef[4][2][2][6][6][3];
	u8 skip[3];
	u8 inter_mode[7][3];
	u8 interp_filter[4][2];
	u8 is_inter[4];
	u8 comp_mode[5];
	u8 single_ref[5][2];
	u8 comp_ref[5];
	u8 y_mode[4][9];
	u8 uv_mode[10][9];
	u8 partition[16][3];

	struct v4l2_vp9_frame_mv_context mv;
};


struct v4l2_vp9_frame_symbol_counts {
	u32 (*partition)[16][4];
	u32 (*skip)[3][2];
	u32 (*intra_inter)[4][2];
	u32 (*tx32p)[2][4];
	u32 (*tx16p)[2][4];
	u32 (*tx8p)[2][2];
	u32 (*y_mode)[4][10];
	u32 (*uv_mode)[10][10];
	u32 (*comp)[5][2];
	u32 (*comp_ref)[5][2];
	u32 (*single_ref)[5][2][2];
	u32 (*mv_mode)[7][4];
	u32 (*filter)[4][3];
	u32 (*mv_joint)[4];
	u32 (*sign)[2][2];
	u32 (*classes)[2][11];
	u32 (*class0)[2][2];
	u32 (*bits)[2][10][2];
	u32 (*class0_fp)[2][2][4];
	u32 (*fp)[2][4];
	u32 (*class0_hp)[2][2];
	u32 (*hp)[2][2];
	u32 (*coeff[4][2][2][6][6])[3];
	u32 *eob[4][2][2][6][6][2];
};

extern const u8 v4l2_vp9_kf_y_mode_prob[10][10][9]; 
extern const u8 v4l2_vp9_kf_partition_probs[16][3]; 
extern const u8 v4l2_vp9_kf_uv_mode_prob[10][9]; 
extern const struct v4l2_vp9_frame_context v4l2_vp9_default_probs; 


void v4l2_vp9_fw_update_probs(struct v4l2_vp9_frame_context *probs,
			      const struct v4l2_ctrl_vp9_compressed_hdr *deltas,
			      const struct v4l2_ctrl_vp9_frame *dec_params);


u8 v4l2_vp9_reset_frame_ctx(const struct v4l2_ctrl_vp9_frame *dec_params,
			    struct v4l2_vp9_frame_context *frame_context);


void v4l2_vp9_adapt_coef_probs(struct v4l2_vp9_frame_context *probs,
			       struct v4l2_vp9_frame_symbol_counts *counts,
			       bool use_128,
			       bool frame_is_intra);


void v4l2_vp9_adapt_noncoef_probs(struct v4l2_vp9_frame_context *probs,
				  struct v4l2_vp9_frame_symbol_counts *counts,
				  u8 reference_mode, u8 interpolation_filter, u8 tx_mode,
				  u32 flags);


bool
v4l2_vp9_seg_feat_enabled(const u8 *feature_enabled,
			  unsigned int feature,
			  unsigned int segid);

#endif 
