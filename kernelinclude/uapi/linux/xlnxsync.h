/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */

#ifndef __XLNXSYNC_H__
#define __XLNXSYNC_H__

#include <linux/types.h>

#define XLNXSYNC_IOCTL_HDR_VER		0x10004


#define XLNXSYNC_AUTO_SEARCH		0xFF

#define XLNXSYNC_MAX_ENC_CHAN		4
#define XLNXSYNC_MAX_DEC_CHAN		2
#define XLNXSYNC_BUF_PER_CHAN		3

#define XLNXSYNC_PROD			0
#define XLNXSYNC_CONS			1
#define XLNXSYNC_IO			2

#define XLNXSYNC_MAX_CORES		4


struct xlnxsync_err_intr {
	__u8 prod_sync : 1;
	__u8 prod_wdg : 1;
	__u8 cons_sync : 1;
	__u8 cons_wdg : 1;
	__u8 ldiff : 1;
	__u8 cdiff : 1;
};


struct xlnxsync_intr {
	__u64 hdr_ver;
	struct xlnxsync_err_intr err;
	__u8 prod_lfbdone : 1;
	__u8 prod_cfbdone : 1;
	__u8 cons_lfbdone : 1;
	__u8 cons_cfbdone : 1;
};


struct xlnxsync_chan_config {
	__u64 hdr_ver;
	__u64 luma_start_offset[XLNXSYNC_IO];
	__u64 chroma_start_offset[XLNXSYNC_IO];
	__u64 luma_end_offset[XLNXSYNC_IO];
	__u64 chroma_end_offset[XLNXSYNC_IO];
	__u32 luma_margin;
	__u32 chroma_margin;
	__u32 luma_core_offset[XLNXSYNC_MAX_CORES];
	__u32 chroma_core_offset[XLNXSYNC_MAX_CORES];
	__u32 dma_fd;
	__u8 fb_id[XLNXSYNC_IO];
	__u8 ismono[XLNXSYNC_IO];
};


struct xlnxsync_clr_err {
	__u64 hdr_ver;
	struct xlnxsync_err_intr err;
};


struct xlnxsync_fbdone {
	__u64 hdr_ver;
	__u8 status[XLNXSYNC_BUF_PER_CHAN][XLNXSYNC_IO];
};


struct xlnxsync_config {
	__u64	hdr_ver;
	__u8	encode;
	__u8	max_channels;
	__u8	active_channels;
	__u8	reserved_id;
	__u32	reserved[10];
};


struct xlnxsync_stat {
	__u64 hdr_ver;
	__u8 fbdone[XLNXSYNC_BUF_PER_CHAN][XLNXSYNC_IO];
	__u8 enable;
	struct xlnxsync_err_intr err;
};

#define XLNXSYNC_MAGIC			'X'


#define XLNXSYNC_GET_CFG		_IOR(XLNXSYNC_MAGIC, 1,\
					     struct xlnxsync_config *)

#define XLNXSYNC_CHAN_GET_STATUS	_IOR(XLNXSYNC_MAGIC, 2, __u32 *)

#define XLNXSYNC_CHAN_SET_CONFIG	_IOW(XLNXSYNC_MAGIC, 3,\
					     struct xlnxsync_chan_config *)

#define XLNXSYNC_CHAN_ENABLE		_IO(XLNXSYNC_MAGIC, 4)

#define XLNXSYNC_CHAN_DISABLE		_IO(XLNXSYNC_MAGIC, 5)

#define XLNXSYNC_CHAN_CLR_ERR		_IOW(XLNXSYNC_MAGIC, 6,\
					     struct xlnxsync_clr_err *)

#define XLNXSYNC_CHAN_GET_FBDONE_STAT	_IOR(XLNXSYNC_MAGIC, 7,\
					     struct xlnxsync_fbdone *)

#define XLNXSYNC_CHAN_CLR_FBDONE_STAT	_IOW(XLNXSYNC_MAGIC, 8,\
					     struct xlnxsync_fbdone *)

#define XLNXSYNC_CHAN_SET_INTR_MASK	_IOW(XLNXSYNC_MAGIC, 9,\
					     struct xlnxsync_intr *)

#define XLNXSYNC_RESET_SLOT		_IO(XLNXSYNC_MAGIC, 10)
#endif
