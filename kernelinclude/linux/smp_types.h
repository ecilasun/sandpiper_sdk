/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SMP_TYPES_H
#define __LINUX_SMP_TYPES_H

#include <linux/llist.h>

enum {
	CSD_FLAG_LOCK		= 0x01,

	IRQ_WORK_PENDING	= 0x01,
	IRQ_WORK_BUSY		= 0x02,
	IRQ_WORK_LAZY		= 0x04, 
	IRQ_WORK_HARD_IRQ	= 0x08, 

	IRQ_WORK_CLAIMED	= (IRQ_WORK_PENDING | IRQ_WORK_BUSY),

	CSD_TYPE_ASYNC		= 0x00,
	CSD_TYPE_SYNC		= 0x10,
	CSD_TYPE_IRQ_WORK	= 0x20,
	CSD_TYPE_TTWU		= 0x30,

	CSD_FLAG_TYPE_MASK	= 0xF0,
};



struct __call_single_node {
	struct llist_node	llist;
	union {
		unsigned int	u_flags;
		atomic_t	a_flags;
	};
#ifdef CONFIG_64BIT
	u16 src, dst;
#endif
};

#endif 
