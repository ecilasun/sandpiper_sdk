/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */


#ifndef _CAN_CORE_H
#define _CAN_CORE_H

#include <linux/can.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>

#define DNAME(dev) ((dev) ? (dev)->name : "any")


struct can_proto {
	int type;
	int protocol;
	const struct proto_ops *ops;
	struct proto *prot;
};


#define CAN_REQUIRED_SIZE(struct_type, member) \
	(offsetof(typeof(struct_type), member) + \
	 sizeof(((typeof(struct_type) *)(NULL))->member))



extern int  can_proto_register(const struct can_proto *cp);
extern void can_proto_unregister(const struct can_proto *cp);

int can_rx_register(struct net *net, struct net_device *dev,
		    canid_t can_id, canid_t mask,
		    void (*func)(struct sk_buff *, void *),
		    void *data, char *ident, struct sock *sk);

extern void can_rx_unregister(struct net *net, struct net_device *dev,
			      canid_t can_id, canid_t mask,
			      void (*func)(struct sk_buff *, void *),
			      void *data);

extern int can_send(struct sk_buff *skb, int loop);
void can_sock_destruct(struct sock *sk);

#endif 
