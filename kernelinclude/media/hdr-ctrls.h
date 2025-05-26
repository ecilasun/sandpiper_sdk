/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */


#ifndef _HDR_CTRLS_H_
#define _HDR_CTRLS_H_

#include <linux/types.h>

#define V4L2_CTRL_CLASS_METADATA 0x00b00000

#define V4L2_CID_METADATA_BASE (V4L2_CTRL_CLASS_METADATA | 0x900)
#define V4L2_CID_METADATA_CLASS (V4L2_CTRL_CLASS_METADATA | 1)

#define V4L2_CID_METADATA_HDR (V4L2_CID_METADATA_BASE + 1)

enum v4l2_eotf {
	
	V4L2_EOTF_TRADITIONAL_GAMMA_SDR,
	V4L2_EOTF_TRADITIONAL_GAMMA_HDR,
	V4L2_EOTF_SMPTE_ST2084,
	V4L2_EOTF_BT_2100_HLG,
};

enum v4l2_hdr_type {
	

	
	V4L2_HDR_TYPE_HDR10     = 0x0000,

	
	V4L2_HDR_TYPE_HDR10P    = 1 << 15 | V4L2_HDR_TYPE_HDR10,
};


struct v4l2_hdr10_payload {
	__u8 eotf;
	__u8 metadata_type;
	struct {
		__u16 x;
		__u16 y;
	} display_primaries[3];
	struct {
		__u16 x;
		__u16 y;
	} white_point;
	__u16 max_mdl;
	__u16 min_mdl;
	__u16 max_cll;
	__u16 max_fall;
};


struct v4l2_metadata_hdr {
	__u16 metadata_type;
	__u16 size;
	
	__u8 payload[4000];
};

#endif
