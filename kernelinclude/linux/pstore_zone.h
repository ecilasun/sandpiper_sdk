/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PSTORE_ZONE_H_
#define __PSTORE_ZONE_H_

#include <linux/types.h>

typedef ssize_t (*pstore_zone_read_op)(char *, size_t, loff_t);
typedef ssize_t (*pstore_zone_write_op)(const char *, size_t, loff_t);
typedef ssize_t (*pstore_zone_erase_op)(size_t, loff_t);

struct pstore_zone_info {
	struct module *owner;
	const char *name;

	unsigned long total_size;
	unsigned long kmsg_size;
	int max_reason;
	unsigned long pmsg_size;
	unsigned long console_size;
	unsigned long ftrace_size;
	pstore_zone_read_op read;
	pstore_zone_write_op write;
	pstore_zone_erase_op erase;
	pstore_zone_write_op panic_write;
};

extern int register_pstore_zone(struct pstore_zone_info *info);
extern void unregister_pstore_zone(struct pstore_zone_info *info);

#endif
