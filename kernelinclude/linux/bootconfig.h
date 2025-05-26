/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_XBC_H
#define _LINUX_XBC_H


#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/types.h>
#else 

#endif

#define BOOTCONFIG_MAGIC	"#BOOTCONFIG\n"
#define BOOTCONFIG_MAGIC_LEN	12
#define BOOTCONFIG_ALIGN_SHIFT	2
#define BOOTCONFIG_ALIGN	(1 << BOOTCONFIG_ALIGN_SHIFT)
#define BOOTCONFIG_ALIGN_MASK	(BOOTCONFIG_ALIGN - 1)


static inline __init uint32_t xbc_calc_checksum(void *data, uint32_t size)
{
	unsigned char *p = data;
	uint32_t ret = 0;

	while (size--)
		ret += *p++;

	return ret;
}


struct xbc_node {
	uint16_t next;
	uint16_t child;
	uint16_t parent;
	uint16_t data;
} __attribute__ ((__packed__));

#define XBC_KEY		0
#define XBC_VALUE	(1 << 15)

#define XBC_DATA_MAX	(XBC_VALUE - 1)

#define XBC_NODE_MAX	8192
#define XBC_KEYLEN_MAX	256
#define XBC_DEPTH_MAX	16


struct xbc_node * __init xbc_root_node(void);
int __init xbc_node_index(struct xbc_node *node);
struct xbc_node * __init xbc_node_get_parent(struct xbc_node *node);
struct xbc_node * __init xbc_node_get_child(struct xbc_node *node);
struct xbc_node * __init xbc_node_get_next(struct xbc_node *node);
const char * __init xbc_node_get_data(struct xbc_node *node);


static inline __init bool xbc_node_is_value(struct xbc_node *node)
{
	return node->data & XBC_VALUE;
}


static inline __init bool xbc_node_is_key(struct xbc_node *node)
{
	return !xbc_node_is_value(node);
}


static inline __init bool xbc_node_is_array(struct xbc_node *node)
{
	return xbc_node_is_value(node) && node->child != 0;
}


static inline __init bool xbc_node_is_leaf(struct xbc_node *node)
{
	return xbc_node_is_key(node) &&
		(!node->child || xbc_node_is_value(xbc_node_get_child(node)));
}


struct xbc_node * __init xbc_node_find_subkey(struct xbc_node *parent,
					     const char *key);

const char * __init xbc_node_find_value(struct xbc_node *parent,
					const char *key,
					struct xbc_node **vnode);

struct xbc_node * __init xbc_node_find_next_leaf(struct xbc_node *root,
						 struct xbc_node *leaf);

const char * __init xbc_node_find_next_key_value(struct xbc_node *root,
						 struct xbc_node **leaf);


static inline const char * __init
xbc_find_value(const char *key, struct xbc_node **vnode)
{
	return xbc_node_find_value(NULL, key, vnode);
}


static inline struct xbc_node * __init xbc_find_node(const char *key)
{
	return xbc_node_find_subkey(NULL, key);
}


static inline struct xbc_node * __init xbc_node_get_subkey(struct xbc_node *node)
{
	struct xbc_node *child = xbc_node_get_child(node);

	if (child && xbc_node_is_value(child))
		return xbc_node_get_next(child);
	else
		return child;
}


#define xbc_array_for_each_value(anode, value)				\
	for (value = xbc_node_get_data(anode); anode != NULL ;		\
	     anode = xbc_node_get_child(anode),				\
	     value = anode ? xbc_node_get_data(anode) : NULL)


#define xbc_node_for_each_child(parent, child)				\
	for (child = xbc_node_get_child(parent); child != NULL ;	\
	     child = xbc_node_get_next(child))


#define xbc_node_for_each_subkey(parent, child)				\
	for (child = xbc_node_get_subkey(parent); child != NULL ;	\
	     child = xbc_node_get_next(child))


#define xbc_node_for_each_array_value(node, key, anode, value)		\
	for (value = xbc_node_find_value(node, key, &anode); value != NULL; \
	     anode = xbc_node_get_child(anode),				\
	     value = anode ? xbc_node_get_data(anode) : NULL)


#define xbc_node_for_each_key_value(node, knode, value)			\
	for (knode = NULL, value = xbc_node_find_next_key_value(node, &knode);\
	     knode != NULL; value = xbc_node_find_next_key_value(node, &knode))


#define xbc_for_each_key_value(knode, value)				\
	xbc_node_for_each_key_value(NULL, knode, value)


int __init xbc_node_compose_key_after(struct xbc_node *root,
			struct xbc_node *node, char *buf, size_t size);


static inline int __init xbc_node_compose_key(struct xbc_node *node,
					      char *buf, size_t size)
{
	return xbc_node_compose_key_after(NULL, node, buf, size);
}


int __init xbc_init(const char *buf, size_t size, const char **emsg, int *epos);


int __init xbc_get_info(int *node_size, size_t *data_size);


void __init _xbc_exit(bool early);

static inline void xbc_exit(void)
{
	_xbc_exit(false);
}


#ifdef CONFIG_BOOT_CONFIG_EMBED
const char * __init xbc_get_embedded_bootconfig(size_t *size);
#else
static inline const char *xbc_get_embedded_bootconfig(size_t *size)
{
	return NULL;
}
#endif

#endif
