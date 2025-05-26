/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */



#ifndef __XLNX_MPG2TSMUX_INTERFACE_H__
#define __XLNX_MPG2TSMUX_INTERFACE_H__

#include <linux/types.h>
#include <linux/ioctl.h>


enum ts_mux_command {
	CREATE_TS_MISC = 0,
	CREATE_TS_VIDEO_KEYFRAME,
	CREATE_TS_VIDEO_NON_KEYFRAME,
	CREATE_TS_AUDIO,
	WRITE_PAT,
	WRITE_PMT,
	WRITE_SI,
	INVALID
};


struct stream_context_in {
	enum ts_mux_command command;
	__u8 stream_id;
	__u8 extended_stream_id;
	int is_pcr_stream;
	int is_valid_pts;
	int is_valid_dts;
	int is_dmabuf;
	__u16 pid;
	__u64 size_data_in;
	__u64 pts;
	__u64 dts;
	__u32 srcbuf_id;
	int insert_pcr;
	__u16 pcr_extension;
	__u64 pcr_base;
};


struct muxer_context_in {
	int is_dmabuf;
	__u32 dstbuf_id;
	__u32 dmabuf_size;
};


enum xlnx_tsmux_status {
	MPG2MUX_BUSY = 0,
	MPG2MUX_READY,
	MPG2MUX_ERROR
};


struct strc_bufs_info {
	__u32 num_buf;
	__u32 buf_size;
};


struct out_buffer {
	__u32 buf_id;
	__u32 buf_write;
};


enum strmtbl_cnxt {
	NO_UPDATE = 0,
	ADD_TO_TBL,
	DEL_FR_TBL,
};


struct strc_strminfo {
	enum strmtbl_cnxt strmtbl_ctxt;
	__u16 pid;
};


enum xlnx_tsmux_dma_dir {
	DMA_TO_MPG2MUX = 1,
	DMA_FROM_MPG2MUX,
};


enum xlnx_tsmux_dmabuf_flags {
	DMABUF_ERROR = 1,
	DMABUF_CONTIG = 2,
	DMABUF_NON_CONTIG = 4,
	DMABUF_ATTACHED = 8,
};


struct xlnx_tsmux_dmabuf_info {
	int buf_fd;
	enum xlnx_tsmux_dma_dir dir;
	enum xlnx_tsmux_dmabuf_flags flags;
};



#define MPG2MUX_MAGIC 'M'


#define MPG2MUX_INBUFALLOC _IOWR(MPG2MUX_MAGIC, 1, struct strc_bufs_info *)


#define MPG2MUX_INBUFDEALLOC _IO(MPG2MUX_MAGIC, 2)


#define MPG2MUX_OUTBUFALLOC _IOWR(MPG2MUX_MAGIC, 3, struct strc_bufs_info *)


#define MPG2MUX_OUTBUFDEALLOC _IO(MPG2MUX_MAGIC, 4)


#define MPG2MUX_STBLALLOC _IOW(MPG2MUX_MAGIC, 5, unsigned short *)


#define MPG2MUX_STBLDEALLOC _IO(MPG2MUX_MAGIC, 6)


#define MPG2MUX_TBLUPDATE _IOW(MPG2MUX_MAGIC, 7, struct strc_strminfo *)


#define MPG2MUX_SETSTRM _IOW(MPG2MUX_MAGIC, 8, struct stream_context_in *)


#define MPG2MUX_START _IO(MPG2MUX_MAGIC, 9)


#define MPG2MUX_STOP _IO(MPG2MUX_MAGIC, 10)


#define MPG2MUX_STATUS _IOR(MPG2MUX_MAGIC, 11, unsigned short *)


#define MPG2MUX_GETOUTBUF _IOW(MPG2MUX_MAGIC, 12, struct out_buffer *)


#define MPG2MUX_SETMUX _IOW(MPG2MUX_MAGIC, 13, struct muxer_context_in *)


#define MPG2MUX_VDBUF _IOWR(MPG2MUX_MAGIC, 14, struct xlnx_tsmux_dmabuf_info *)

#endif
