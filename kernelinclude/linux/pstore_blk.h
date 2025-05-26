/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PSTORE_BLK_H_
#define __PSTORE_BLK_H_

#include <linux/types.h>
#include <linux/pstore.h>
#include <linux/pstore_zone.h>


struct pstore_device_info {
	unsigned int flags;
	struct pstore_zone_info zone;
};

int  register_pstore_device(struct pstore_device_info *dev);
void unregister_pstore_device(struct pstore_device_info *dev);


struct pstore_blk_config {
	char device[80];
	enum kmsg_dump_reason max_reason;
	unsigned long kmsg_size;
	unsigned long pmsg_size;
	unsigned long console_size;
	unsigned long ftrace_size;
};


int pstore_blk_get_config(struct pstore_blk_config *info);

#endif
