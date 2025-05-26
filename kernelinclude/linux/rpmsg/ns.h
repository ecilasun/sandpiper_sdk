/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_RPMSG_NS_H
#define _LINUX_RPMSG_NS_H

#include <linux/mod_devicetable.h>
#include <linux/rpmsg.h>
#include <linux/rpmsg/byteorder.h>
#include <linux/types.h>


struct rpmsg_ns_msg {
	char name[RPMSG_NAME_SIZE];
	__rpmsg32 addr;
	__rpmsg32 flags;
} __packed;


enum rpmsg_ns_flags {
	RPMSG_NS_CREATE		= 0,
	RPMSG_NS_DESTROY	= 1,
};


#define RPMSG_NS_ADDR			(53)

int rpmsg_ns_register_device(struct rpmsg_device *rpdev);

#endif
