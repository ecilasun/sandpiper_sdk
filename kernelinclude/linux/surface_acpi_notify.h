/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_SURFACE_ACPI_NOTIFY_H
#define _LINUX_SURFACE_ACPI_NOTIFY_H

#include <linux/notifier.h>
#include <linux/types.h>


struct san_dgpu_event {
	u8 category;
	u8 target;
	u8 command;
	u8 instance;
	u16 length;
	u8 *payload;
};

int san_client_link(struct device *client);
int san_dgpu_notifier_register(struct notifier_block *nb);
int san_dgpu_notifier_unregister(struct notifier_block *nb);

#endif 
