/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */

#ifndef __XILINX_HDMIRXSS_H__
#define __XILINX_HDMIRXSS_H__

#include <linux/types.h>

#define XHDCP_IOCTL	'X'


struct xhdmirxss_hdcp1x_keys_ioctl {
	__u32 size;
	void const *keys;
};



struct xhdmirxss_hdcp2x_keys_ioctl {
	void const *lc128key;
	void const *privatekey;
};


#define XILINX_HDMIRXSS_HDCP_KEY_WRITE \
	_IOW(XHDCP_IOCTL, BASE_VIDIOC_PRIVATE + 1, struct xhdmirxss_hdcp1x_keys_ioctl)


#define XILINX_HDMIRXSS_HDCP22_KEY_WRITE \
	_IOW(XHDCP_IOCTL, BASE_VIDIOC_PRIVATE + 2, struct xhdmirxss_hdcp2x_keys_ioctl)

#endif
