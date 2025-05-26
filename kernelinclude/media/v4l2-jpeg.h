/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _V4L2_JPEG_H
#define _V4L2_JPEG_H

#include <linux/v4l2-controls.h>

#define V4L2_JPEG_MAX_COMPONENTS	4
#define V4L2_JPEG_MAX_TABLES		4


struct v4l2_jpeg_reference {
	u8 *start;
	size_t length;
};




struct v4l2_jpeg_frame_component_spec {
	u8 component_identifier;
	u8 horizontal_sampling_factor;
	u8 vertical_sampling_factor;
	u8 quantization_table_selector;
};


struct v4l2_jpeg_frame_header {
	u16 height;
	u16 width;
	u8 precision;
	u8 num_components;
	struct v4l2_jpeg_frame_component_spec component[V4L2_JPEG_MAX_COMPONENTS];
	enum v4l2_jpeg_chroma_subsampling subsampling;
};




struct v4l2_jpeg_scan_component_spec {
	u8 component_selector;
	u8 dc_entropy_coding_table_selector;
	u8 ac_entropy_coding_table_selector;
};


struct v4l2_jpeg_scan_header {
	u8 num_components;				
	struct v4l2_jpeg_scan_component_spec component[V4L2_JPEG_MAX_COMPONENTS];
	
};


enum v4l2_jpeg_app14_tf {
	V4L2_JPEG_APP14_TF_CMYK_RGB	= 0,
	V4L2_JPEG_APP14_TF_YCBCR	= 1,
	V4L2_JPEG_APP14_TF_YCCK		= 2,
	V4L2_JPEG_APP14_TF_UNKNOWN	= -1,
};


struct v4l2_jpeg_header {
	struct v4l2_jpeg_reference sof;
	struct v4l2_jpeg_reference sos;
	unsigned int num_dht;
	struct v4l2_jpeg_reference dht[V4L2_JPEG_MAX_TABLES];
	unsigned int num_dqt;
	struct v4l2_jpeg_reference dqt[V4L2_JPEG_MAX_TABLES];

	struct v4l2_jpeg_frame_header frame;
	struct v4l2_jpeg_scan_header *scan;
	struct v4l2_jpeg_reference *quantization_tables;
	struct v4l2_jpeg_reference *huffman_tables;
	u16 restart_interval;
	size_t ecs_offset;
	enum v4l2_jpeg_app14_tf app14_tf;
};

int v4l2_jpeg_parse_header(void *buf, size_t len, struct v4l2_jpeg_header *out);

int v4l2_jpeg_parse_frame_header(void *buf, size_t len,
				 struct v4l2_jpeg_frame_header *frame_header);
int v4l2_jpeg_parse_scan_header(void *buf, size_t len,
				struct v4l2_jpeg_scan_header *scan_header);
int v4l2_jpeg_parse_quantization_tables(void *buf, size_t len, u8 precision,
					struct v4l2_jpeg_reference *q_tables);
int v4l2_jpeg_parse_huffman_tables(void *buf, size_t len,
				   struct v4l2_jpeg_reference *huffman_tables);

#endif
