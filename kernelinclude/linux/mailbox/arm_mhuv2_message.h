// SPDX-License-Identifier: GPL-2.0


#ifndef _LINUX_ARM_MHUV2_MESSAGE_H_
#define _LINUX_ARM_MHUV2_MESSAGE_H_

#include <linux/types.h>


struct arm_mhuv2_mbox_msg {
	void *data;
	size_t len;
};

#endif 
