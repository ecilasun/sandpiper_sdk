/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_CC_PLATFORM_H
#define _LINUX_CC_PLATFORM_H

#include <linux/types.h>
#include <linux/stddef.h>


enum cc_attr {
	
	CC_ATTR_MEM_ENCRYPT,

	
	CC_ATTR_HOST_MEM_ENCRYPT,

	
	CC_ATTR_GUEST_MEM_ENCRYPT,

	
	CC_ATTR_GUEST_STATE_ENCRYPT,

	
	CC_ATTR_GUEST_UNROLL_STRING_IO,

	
	CC_ATTR_GUEST_SEV_SNP,

	
	CC_ATTR_HOTPLUG_DISABLED,
};

#ifdef CONFIG_ARCH_HAS_CC_PLATFORM


bool cc_platform_has(enum cc_attr attr);

#else	

static inline bool cc_platform_has(enum cc_attr attr) { return false; }

#endif	

#endif	
