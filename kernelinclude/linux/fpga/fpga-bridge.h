/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_FPGA_BRIDGE_H
#define _LINUX_FPGA_BRIDGE_H

#include <linux/device.h>
#include <linux/fpga/fpga-mgr.h>

struct fpga_bridge;


struct fpga_bridge_ops {
	int (*enable_show)(struct fpga_bridge *bridge);
	int (*enable_set)(struct fpga_bridge *bridge, bool enable);
	void (*fpga_bridge_remove)(struct fpga_bridge *bridge);
	const struct attribute_group **groups;
};


struct fpga_bridge_info {
	const char *name;
	const struct fpga_bridge_ops *br_ops;
	void *priv;
};


struct fpga_bridge {
	const char *name;
	struct device dev;
	struct mutex mutex; 
	const struct fpga_bridge_ops *br_ops;
	struct module *br_ops_owner;
	struct fpga_image_info *info;
	struct list_head node;
	void *priv;
};

#define to_fpga_bridge(d) container_of(d, struct fpga_bridge, dev)

struct fpga_bridge *of_fpga_bridge_get(struct device_node *node,
				       struct fpga_image_info *info);
struct fpga_bridge *fpga_bridge_get(struct device *dev,
				    struct fpga_image_info *info);
void fpga_bridge_put(struct fpga_bridge *bridge);
int fpga_bridge_enable(struct fpga_bridge *bridge);
int fpga_bridge_disable(struct fpga_bridge *bridge);

int fpga_bridges_enable(struct list_head *bridge_list);
int fpga_bridges_disable(struct list_head *bridge_list);
void fpga_bridges_put(struct list_head *bridge_list);
int fpga_bridge_get_to_list(struct device *dev,
			    struct fpga_image_info *info,
			    struct list_head *bridge_list);
int of_fpga_bridge_get_to_list(struct device_node *np,
			       struct fpga_image_info *info,
			       struct list_head *bridge_list);

#define fpga_bridge_register(parent, name, br_ops, priv) \
	__fpga_bridge_register(parent, name, br_ops, priv, THIS_MODULE)
struct fpga_bridge *
__fpga_bridge_register(struct device *parent, const char *name,
		       const struct fpga_bridge_ops *br_ops, void *priv,
		       struct module *owner);
void fpga_bridge_unregister(struct fpga_bridge *br);

#endif 
