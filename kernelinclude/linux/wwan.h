/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __WWAN_H
#define __WWAN_H

#include <linux/poll.h>
#include <linux/netdevice.h>
#include <linux/types.h>


enum wwan_port_type {
	WWAN_PORT_AT,
	WWAN_PORT_MBIM,
	WWAN_PORT_QMI,
	WWAN_PORT_QCDM,
	WWAN_PORT_FIREHOSE,
	WWAN_PORT_XMMRPC,

	

	__WWAN_PORT_MAX,
	WWAN_PORT_MAX = __WWAN_PORT_MAX - 1,
	WWAN_PORT_UNKNOWN,
};

struct device;
struct file;
struct netlink_ext_ack;
struct sk_buff;
struct wwan_port;


struct wwan_port_ops {
	int (*start)(struct wwan_port *port);
	void (*stop)(struct wwan_port *port);
	int (*tx)(struct wwan_port *port, struct sk_buff *skb);

	
	int (*tx_blocking)(struct wwan_port *port, struct sk_buff *skb);
	__poll_t (*tx_poll)(struct wwan_port *port, struct file *filp,
			    poll_table *wait);
};


struct wwan_port_caps {
	size_t frag_len;
	unsigned int headroom_len;
};


struct wwan_port *wwan_create_port(struct device *parent,
				   enum wwan_port_type type,
				   const struct wwan_port_ops *ops,
				   struct wwan_port_caps *caps,
				   void *drvdata);


void wwan_remove_port(struct wwan_port *port);


void wwan_port_rx(struct wwan_port *port, struct sk_buff *skb);


void wwan_port_txoff(struct wwan_port *port);



void wwan_port_txon(struct wwan_port *port);


void *wwan_port_get_drvdata(struct wwan_port *port);


struct wwan_netdev_priv {
	u32 link_id;

	
	u8 drv_priv[] __aligned(sizeof(void *));
};

static inline void *wwan_netdev_drvpriv(struct net_device *dev)
{
	return ((struct wwan_netdev_priv *)netdev_priv(dev))->drv_priv;
}


#define WWAN_NO_DEFAULT_LINK		U32_MAX


struct wwan_ops {
	unsigned int priv_size;
	void (*setup)(struct net_device *dev);
	int (*newlink)(void *ctxt, struct net_device *dev,
		       u32 if_id, struct netlink_ext_ack *extack);
	void (*dellink)(void *ctxt, struct net_device *dev,
			struct list_head *head);
};

int wwan_register_ops(struct device *parent, const struct wwan_ops *ops,
		      void *ctxt, u32 def_link_id);

void wwan_unregister_ops(struct device *parent);

#ifdef CONFIG_WWAN_DEBUGFS
struct dentry *wwan_get_debugfs_dir(struct device *parent);
void wwan_put_debugfs_dir(struct dentry *dir);
#else
static inline struct dentry *wwan_get_debugfs_dir(struct device *parent)
{
	return ERR_PTR(-ENODEV);
}
static inline void wwan_put_debugfs_dir(struct dentry *dir) {}
#endif

#endif 
