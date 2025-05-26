/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __MANIFEST_H
#define __MANIFEST_H

#include <linux/types.h>

struct gb_interface;
bool gb_manifest_parse(struct gb_interface *intf, void *data, size_t size);

#endif 
