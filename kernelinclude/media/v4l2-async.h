/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef V4L2_ASYNC_H
#define V4L2_ASYNC_H

#include <linux/list.h>
#include <linux/mutex.h>

struct dentry;
struct device;
struct device_node;
struct v4l2_device;
struct v4l2_subdev;
struct v4l2_async_notifier;


enum v4l2_async_match_type {
	V4L2_ASYNC_MATCH_TYPE_I2C,
	V4L2_ASYNC_MATCH_TYPE_FWNODE,
};


struct v4l2_async_match_desc {
	enum v4l2_async_match_type type;
	union {
		struct fwnode_handle *fwnode;
		struct {
			int adapter_id;
			unsigned short address;
		} i2c;
	};
};


struct v4l2_async_connection {
	struct v4l2_async_match_desc match;
	struct v4l2_async_notifier *notifier;
	struct list_head asc_entry;
	struct list_head asc_subdev_entry;
	struct v4l2_subdev *sd;
};


struct v4l2_async_notifier_operations {
	int (*bound)(struct v4l2_async_notifier *notifier,
		     struct v4l2_subdev *subdev,
		     struct v4l2_async_connection *asc);
	int (*complete)(struct v4l2_async_notifier *notifier);
	void (*unbind)(struct v4l2_async_notifier *notifier,
		       struct v4l2_subdev *subdev,
		       struct v4l2_async_connection *asc);
	void (*destroy)(struct v4l2_async_connection *asc);
};


struct v4l2_async_notifier {
	const struct v4l2_async_notifier_operations *ops;
	struct v4l2_device *v4l2_dev;
	struct v4l2_subdev *sd;
	struct v4l2_async_notifier *parent;
	struct list_head waiting_list;
	struct list_head done_list;
	struct list_head notifier_entry;
};


struct v4l2_async_subdev_endpoint {
	struct list_head async_subdev_endpoint_entry;
	struct fwnode_handle *endpoint;
};


void v4l2_async_debug_init(struct dentry *debugfs_dir);


void v4l2_async_nf_init(struct v4l2_async_notifier *notifier,
			struct v4l2_device *v4l2_dev);


void v4l2_async_subdev_nf_init(struct v4l2_async_notifier *notifier,
			       struct v4l2_subdev *sd);

struct v4l2_async_connection *
__v4l2_async_nf_add_fwnode(struct v4l2_async_notifier *notifier,
			   struct fwnode_handle *fwnode,
			   unsigned int asc_struct_size);

#define v4l2_async_nf_add_fwnode(notifier, fwnode, type)		\
	((type *)__v4l2_async_nf_add_fwnode(notifier, fwnode, sizeof(type)))

struct v4l2_async_connection *
__v4l2_async_nf_add_fwnode_remote(struct v4l2_async_notifier *notif,
				  struct fwnode_handle *endpoint,
				  unsigned int asc_struct_size);

#define v4l2_async_nf_add_fwnode_remote(notifier, ep, type) \
	((type *)__v4l2_async_nf_add_fwnode_remote(notifier, ep, sizeof(type)))

struct v4l2_async_connection *
__v4l2_async_nf_add_i2c(struct v4l2_async_notifier *notifier,
			int adapter_id, unsigned short address,
			unsigned int asc_struct_size);

#define v4l2_async_nf_add_i2c(notifier, adapter, address, type) \
	((type *)__v4l2_async_nf_add_i2c(notifier, adapter, address, \
					 sizeof(type)))


int v4l2_async_subdev_endpoint_add(struct v4l2_subdev *sd,
				   struct fwnode_handle *fwnode);


struct v4l2_async_connection *
v4l2_async_connection_unique(struct v4l2_subdev *sd);


int v4l2_async_nf_register(struct v4l2_async_notifier *notifier);


void v4l2_async_nf_unregister(struct v4l2_async_notifier *notifier);


void v4l2_async_nf_cleanup(struct v4l2_async_notifier *notifier);


int v4l2_async_register_subdev(struct v4l2_subdev *sd);


int __must_check
v4l2_async_register_subdev_sensor(struct v4l2_subdev *sd);


void v4l2_async_unregister_subdev(struct v4l2_subdev *sd);
#endif
