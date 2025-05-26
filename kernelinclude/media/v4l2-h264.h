/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _MEDIA_V4L2_H264_H
#define _MEDIA_V4L2_H264_H

#include <media/v4l2-ctrls.h>


struct v4l2_h264_reflist_builder {
	struct {
		s32 top_field_order_cnt;
		s32 bottom_field_order_cnt;
		int frame_num;
		u16 longterm : 1;
	} refs[V4L2_H264_NUM_DPB_ENTRIES];

	s32 cur_pic_order_count;
	u8 cur_pic_fields;

	struct v4l2_h264_reference unordered_reflist[V4L2_H264_REF_LIST_LEN];
	u8 num_valid;
};

void
v4l2_h264_init_reflist_builder(struct v4l2_h264_reflist_builder *b,
		const struct v4l2_ctrl_h264_decode_params *dec_params,
		const struct v4l2_ctrl_h264_sps *sps,
		const struct v4l2_h264_dpb_entry dpb[V4L2_H264_NUM_DPB_ENTRIES]);


void
v4l2_h264_build_b_ref_lists(const struct v4l2_h264_reflist_builder *builder,
			    struct v4l2_h264_reference *b0_reflist,
			    struct v4l2_h264_reference *b1_reflist);


void
v4l2_h264_build_p_ref_list(const struct v4l2_h264_reflist_builder *builder,
			   struct v4l2_h264_reference *reflist);

#endif 
