/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */


#ifndef _UAPI_REMOTEPROC_CDEV_H_
#define _UAPI_REMOTEPROC_CDEV_H_

#include <linux/ioctl.h>
#include <linux/types.h>

#define RPROC_MAGIC	0xB7


#define RPROC_SET_SHUTDOWN_ON_RELEASE _IOW(RPROC_MAGIC, 1, __s32)


#define RPROC_GET_SHUTDOWN_ON_RELEASE _IOR(RPROC_MAGIC, 2, __s32)

#endif
