/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _FPGA_REGION_H
#define _FPGA_REGION_H

#include <linux/device.h>
#include <linux/fpga/fpga-mgr.h>
#include <linux/fpga/fpga-bridge.h>

struct fpga_region;


struct fpga_region_info {
	struct fpga_manager *mgr;
	struct fpga_compat_id *compat_id;
	void *priv;
	int (*get_bridges)(struct fpga_region *region);
};


struct fpga_region {
	struct device dev;
	struct mutex mutex; 
	struct list_head bridge_list;
	struct fpga_manager *mgr;
	struct fpga_image_info *info;
	struct fpga_compat_id *compat_id;
	struct module *ops_owner;
	void *priv;
	int (*get_bridges)(struct fpga_region *region);
};

#define to_fpga_region(d) container_of(d, struct fpga_region, dev)

struct fpga_region *
fpga_region_class_find(struct device *start, const void *data,
		       int (*match)(struct device *, const void *));

int fpga_region_program_fpga(struct fpga_region *region);

#define fpga_region_register_full(parent, info) \
	__fpga_region_register_full(parent, info, THIS_MODULE)
struct fpga_region *
__fpga_region_register_full(struct device *parent, const struct fpga_region_info *info,
			    struct module *owner);

#define fpga_region_register(parent, mgr, get_bridges) \
	__fpga_region_register(parent, mgr, get_bridges, THIS_MODULE)
struct fpga_region *
__fpga_region_register(struct device *parent, struct fpga_manager *mgr,
		       int (*get_bridges)(struct fpga_region *), struct module *owner);
void fpga_region_unregister(struct fpga_region *region);

#endif 
