/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __NET_MCTP_H
#define __NET_MCTP_H

#include <linux/bits.h>
#include <linux/mctp.h>
#include <linux/netdevice.h>
#include <net/net_namespace.h>
#include <net/sock.h>


struct mctp_hdr {
	u8	ver;
	u8	dest;
	u8	src;
	u8	flags_seq_tag;
};

#define MCTP_VER_MIN	1
#define MCTP_VER_MAX	1


#define MCTP_HDR_FLAG_SOM	BIT(7)
#define MCTP_HDR_FLAG_EOM	BIT(6)
#define MCTP_HDR_FLAG_TO	BIT(3)
#define MCTP_HDR_FLAGS		GENMASK(5, 3)
#define MCTP_HDR_SEQ_SHIFT	4
#define MCTP_HDR_SEQ_MASK	GENMASK(1, 0)
#define MCTP_HDR_TAG_SHIFT	0
#define MCTP_HDR_TAG_MASK	GENMASK(2, 0)

#define MCTP_INITIAL_DEFAULT_NET	1

static inline bool mctp_address_unicast(mctp_eid_t eid)
{
	return eid >= 8 && eid < 255;
}

static inline bool mctp_address_broadcast(mctp_eid_t eid)
{
	return eid == 255;
}

static inline bool mctp_address_null(mctp_eid_t eid)
{
	return eid == 0;
}

static inline bool mctp_address_matches(mctp_eid_t match, mctp_eid_t eid)
{
	return match == eid || match == MCTP_ADDR_ANY;
}

static inline struct mctp_hdr *mctp_hdr(struct sk_buff *skb)
{
	return (struct mctp_hdr *)skb_network_header(skb);
}


struct mctp_sock {
	struct sock	sk;

	
	unsigned int	bind_net;
	mctp_eid_t	bind_addr;
	__u8		bind_type;

	
	bool		addr_ext;

	
	struct hlist_head keys;

	
	struct timer_list key_expiry;
};


struct mctp_sk_key {
	mctp_eid_t	peer_addr;
	mctp_eid_t	local_addr; 
	__u8		tag; 

	
	struct sock	*sk;

	
	struct hlist_node hlist;

	
	struct hlist_node sklist;

	
	spinlock_t	lock;

	
	refcount_t	refs;

	
	struct sk_buff	*reasm_head;
	struct sk_buff	**reasm_tailp;
	bool		reasm_dead;
	u8		last_seq;

	
	bool		valid;

	
	unsigned long	expiry;

	
	unsigned long	dev_flow_state;
	struct mctp_dev	*dev;

	
	bool		manual_alloc;
};

struct mctp_skb_cb {
	unsigned int	magic;
	unsigned int	net;
	int		ifindex; 
	mctp_eid_t	src;
	unsigned char	halen;
	unsigned char	haddr[MAX_ADDR_LEN];
};


static inline struct mctp_skb_cb *__mctp_cb(struct sk_buff *skb)
{
	struct mctp_skb_cb *cb = (void *)skb->cb;

	cb->magic = 0x4d435450;
	return cb;
}

static inline struct mctp_skb_cb *mctp_cb(struct sk_buff *skb)
{
	struct mctp_skb_cb *cb = (void *)skb->cb;

	BUILD_BUG_ON(sizeof(struct mctp_skb_cb) > sizeof(skb->cb));
	WARN_ON(cb->magic != 0x4d435450);
	return (void *)(skb->cb);
}


struct mctp_flow {
	struct mctp_sk_key *key;
};


struct mctp_route {
	mctp_eid_t		min, max;

	unsigned char		type;
	unsigned int		mtu;
	struct mctp_dev		*dev;
	int			(*output)(struct mctp_route *route,
					  struct sk_buff *skb);

	struct list_head	list;
	refcount_t		refs;
	struct rcu_head		rcu;
};


struct mctp_route *mctp_route_lookup(struct net *net, unsigned int dnet,
				     mctp_eid_t daddr);


int mctp_local_output(struct sock *sk, struct mctp_route *rt,
		      struct sk_buff *skb, mctp_eid_t daddr, u8 req_tag);

void mctp_key_unref(struct mctp_sk_key *key);
struct mctp_sk_key *mctp_alloc_local_tag(struct mctp_sock *msk,
					 mctp_eid_t daddr, mctp_eid_t saddr,
					 bool manual, u8 *tagp);


unsigned int mctp_default_net(struct net *net);
int mctp_default_net_set(struct net *net, unsigned int index);
int mctp_route_add_local(struct mctp_dev *mdev, mctp_eid_t addr);
int mctp_route_remove_local(struct mctp_dev *mdev, mctp_eid_t addr);
void mctp_route_remove_dev(struct mctp_dev *mdev);


enum mctp_neigh_source {
	MCTP_NEIGH_STATIC,
	MCTP_NEIGH_DISCOVER,
};

struct mctp_neigh {
	struct mctp_dev		*dev;
	mctp_eid_t		eid;
	enum mctp_neigh_source	source;

	unsigned char		ha[MAX_ADDR_LEN];

	struct list_head	list;
	struct rcu_head		rcu;
};

int mctp_neigh_init(void);
void mctp_neigh_exit(void);

// ret_hwaddr may be NULL, otherwise must have space for MAX_ADDR_LEN
int mctp_neigh_lookup(struct mctp_dev *dev, mctp_eid_t eid,
		      void *ret_hwaddr);
void mctp_neigh_remove_dev(struct mctp_dev *mdev);

int mctp_routes_init(void);
void mctp_routes_exit(void);

void mctp_device_init(void);
void mctp_device_exit(void);

#endif 
