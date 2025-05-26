/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_FWNODE_H_
#define _LINUX_FWNODE_H_

#include <linux/types.h>
#include <linux/list.h>
#include <linux/bits.h>
#include <linux/err.h>

struct fwnode_operations;
struct device;


#define FWNODE_FLAG_LINKS_ADDED			BIT(0)
#define FWNODE_FLAG_NOT_DEVICE			BIT(1)
#define FWNODE_FLAG_INITIALIZED			BIT(2)
#define FWNODE_FLAG_NEEDS_CHILD_BOUND_ON_ADD	BIT(3)
#define FWNODE_FLAG_BEST_EFFORT			BIT(4)
#define FWNODE_FLAG_VISITED			BIT(5)

struct fwnode_handle {
	struct fwnode_handle *secondary;
	const struct fwnode_operations *ops;
	struct device *dev;
	struct list_head suppliers;
	struct list_head consumers;
	u8 flags;
};


#define FWLINK_FLAG_CYCLE			BIT(0)

struct fwnode_link {
	struct fwnode_handle *supplier;
	struct list_head s_hook;
	struct fwnode_handle *consumer;
	struct list_head c_hook;
	u8 flags;
};


struct fwnode_endpoint {
	unsigned int port;
	unsigned int id;
	const struct fwnode_handle *local_fwnode;
};


#define SWNODE_GRAPH_PORT_NAME_FMT		"port@%u"
#define SWNODE_GRAPH_ENDPOINT_NAME_FMT		"endpoint@%u"

#define NR_FWNODE_REFERENCE_ARGS	8


struct fwnode_reference_args {
	struct fwnode_handle *fwnode;
	unsigned int nargs;
	u64 args[NR_FWNODE_REFERENCE_ARGS];
};


struct fwnode_operations {
	struct fwnode_handle *(*get)(struct fwnode_handle *fwnode);
	void (*put)(struct fwnode_handle *fwnode);
	bool (*device_is_available)(const struct fwnode_handle *fwnode);
	const void *(*device_get_match_data)(const struct fwnode_handle *fwnode,
					     const struct device *dev);
	bool (*device_dma_supported)(const struct fwnode_handle *fwnode);
	enum dev_dma_attr
	(*device_get_dma_attr)(const struct fwnode_handle *fwnode);
	bool (*property_present)(const struct fwnode_handle *fwnode,
				 const char *propname);
	int (*property_read_int_array)(const struct fwnode_handle *fwnode,
				       const char *propname,
				       unsigned int elem_size, void *val,
				       size_t nval);
	int
	(*property_read_string_array)(const struct fwnode_handle *fwnode_handle,
				      const char *propname, const char **val,
				      size_t nval);
	const char *(*get_name)(const struct fwnode_handle *fwnode);
	const char *(*get_name_prefix)(const struct fwnode_handle *fwnode);
	struct fwnode_handle *(*get_parent)(const struct fwnode_handle *fwnode);
	struct fwnode_handle *
	(*get_next_child_node)(const struct fwnode_handle *fwnode,
			       struct fwnode_handle *child);
	struct fwnode_handle *
	(*get_named_child_node)(const struct fwnode_handle *fwnode,
				const char *name);
	int (*get_reference_args)(const struct fwnode_handle *fwnode,
				  const char *prop, const char *nargs_prop,
				  unsigned int nargs, unsigned int index,
				  struct fwnode_reference_args *args);
	struct fwnode_handle *
	(*graph_get_next_endpoint)(const struct fwnode_handle *fwnode,
				   struct fwnode_handle *prev);
	struct fwnode_handle *
	(*graph_get_remote_endpoint)(const struct fwnode_handle *fwnode);
	struct fwnode_handle *
	(*graph_get_port_parent)(struct fwnode_handle *fwnode);
	int (*graph_parse_endpoint)(const struct fwnode_handle *fwnode,
				    struct fwnode_endpoint *endpoint);
	void __iomem *(*iomap)(struct fwnode_handle *fwnode, int index);
	int (*irq_get)(const struct fwnode_handle *fwnode, unsigned int index);
	int (*add_links)(struct fwnode_handle *fwnode);
};

#define fwnode_has_op(fwnode, op)					\
	(!IS_ERR_OR_NULL(fwnode) && (fwnode)->ops && (fwnode)->ops->op)

#define fwnode_call_int_op(fwnode, op, ...)				\
	(fwnode_has_op(fwnode, op) ?					\
	 (fwnode)->ops->op(fwnode, ## __VA_ARGS__) : (IS_ERR_OR_NULL(fwnode) ? -EINVAL : -ENXIO))

#define fwnode_call_bool_op(fwnode, op, ...)		\
	(fwnode_has_op(fwnode, op) ?			\
	 (fwnode)->ops->op(fwnode, ## __VA_ARGS__) : false)

#define fwnode_call_ptr_op(fwnode, op, ...)		\
	(fwnode_has_op(fwnode, op) ?			\
	 (fwnode)->ops->op(fwnode, ## __VA_ARGS__) : NULL)
#define fwnode_call_void_op(fwnode, op, ...)				\
	do {								\
		if (fwnode_has_op(fwnode, op))				\
			(fwnode)->ops->op(fwnode, ## __VA_ARGS__);	\
	} while (false)
#define get_dev_from_fwnode(fwnode)	get_device((fwnode)->dev)

static inline void fwnode_init(struct fwnode_handle *fwnode,
			       const struct fwnode_operations *ops)
{
	fwnode->ops = ops;
	INIT_LIST_HEAD(&fwnode->consumers);
	INIT_LIST_HEAD(&fwnode->suppliers);
}

static inline void fwnode_dev_initialized(struct fwnode_handle *fwnode,
					  bool initialized)
{
	if (IS_ERR_OR_NULL(fwnode))
		return;

	if (initialized)
		fwnode->flags |= FWNODE_FLAG_INITIALIZED;
	else
		fwnode->flags &= ~FWNODE_FLAG_INITIALIZED;
}

extern bool fw_devlink_is_strict(void);
int fwnode_link_add(struct fwnode_handle *con, struct fwnode_handle *sup);
void fwnode_links_purge(struct fwnode_handle *fwnode);
void fw_devlink_purge_absent_suppliers(struct fwnode_handle *fwnode);

#endif
