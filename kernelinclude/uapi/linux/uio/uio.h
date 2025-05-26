/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */


#ifndef _UAPI_UIO_UIO_H_
#define _UAPI_UIO_UIO_H_

#include <linux/ioctl.h>
#include <linux/types.h>


enum uio_dmabuf_dir {
	UIO_DMABUF_DIR_BIDIR	= 1,
	UIO_DMABUF_DIR_TO_DEV	= 2,
	UIO_DMABUF_DIR_FROM_DEV	= 3,
	UIO_DMABUF_DIR_NONE	= 4,
};


struct uio_dmabuf_args {
	__s32	dbuf_fd;
	__u64	dma_addr;
	__u64	size;
	__u8	dir;
};

#define UIO_IOC_BASE		'U'


#define	UIO_IOC_MAP_DMABUF	_IOWR(UIO_IOC_BASE, 0x1, struct uio_dmabuf_args)


#define	UIO_IOC_UNMAP_DMABUF	_IOWR(UIO_IOC_BASE, 0x2, struct uio_dmabuf_args)

#endif
