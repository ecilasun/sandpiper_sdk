/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _RDMA_NETLINK_H
#define _RDMA_NETLINK_H

#include <linux/netlink.h>
#include <uapi/rdma/rdma_netlink.h>

enum {
	RDMA_NLDEV_ATTR_EMPTY_STRING = 1,
	RDMA_NLDEV_ATTR_ENTRY_STRLEN = 16,
	RDMA_NLDEV_ATTR_CHARDEV_TYPE_SIZE = 32,
};

struct rdma_nl_cbs {
	int (*doit)(struct sk_buff *skb, struct nlmsghdr *nlh,
		    struct netlink_ext_ack *extack);
	int (*dump)(struct sk_buff *skb, struct netlink_callback *nlcb);
	u8 flags;
};

enum rdma_nl_flags {
	
	RDMA_NL_ADMIN_PERM	= 1 << 0,
};


#define MODULE_ALIAS_RDMA_NETLINK(_index, _val)                                \
	static inline void __maybe_unused __chk_##_index(void)                 \
	{                                                                      \
		BUILD_BUG_ON(_index != _val);                                  \
	}                                                                      \
	MODULE_ALIAS("rdma-netlink-subsys-" __stringify(_val))


void rdma_nl_register(unsigned int index,
		      const struct rdma_nl_cbs cb_table[]);


void rdma_nl_unregister(unsigned int index);


void *ibnl_put_msg(struct sk_buff *skb, struct nlmsghdr **nlh, int seq,
		   int len, int client, int op, int flags);

int ibnl_put_attr(struct sk_buff *skb, struct nlmsghdr *nlh,
		  int len, void *data, int type);


int rdma_nl_unicast(struct net *net, struct sk_buff *skb, u32 pid);


int rdma_nl_unicast_wait(struct net *net, struct sk_buff *skb, __u32 pid);


int rdma_nl_multicast(struct net *net, struct sk_buff *skb,
		      unsigned int group, gfp_t flags);


bool rdma_nl_chk_listeners(unsigned int group);

struct rdma_link_ops {
	struct list_head list;
	const char *type;
	int (*newlink)(const char *ibdev_name, struct net_device *ndev);
};

void rdma_link_register(struct rdma_link_ops *ops);
void rdma_link_unregister(struct rdma_link_ops *ops);

#define MODULE_ALIAS_RDMA_LINK(type) MODULE_ALIAS("rdma-link-" type)
#define MODULE_ALIAS_RDMA_CLIENT(type) MODULE_ALIAS("rdma-client-" type)

#endif 
