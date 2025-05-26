/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */


#ifndef _UAPI_LINUX_RPL_IPTUNNEL_H
#define _UAPI_LINUX_RPL_IPTUNNEL_H

enum {
	RPL_IPTUNNEL_UNSPEC,
	RPL_IPTUNNEL_SRH,
	__RPL_IPTUNNEL_MAX,
};
#define RPL_IPTUNNEL_MAX (__RPL_IPTUNNEL_MAX - 1)

#define RPL_IPTUNNEL_SRH_SIZE(srh) (((srh)->hdrlen + 1) << 3)

#endif
