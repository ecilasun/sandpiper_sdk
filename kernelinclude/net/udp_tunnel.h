/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_UDP_TUNNEL_H
#define __NET_UDP_TUNNEL_H

#include <net/ip_tunnels.h>
#include <net/udp.h>

#if IS_ENABLED(CONFIG_IPV6)
#include <net/ipv6.h>
#include <net/ipv6_stubs.h>
#endif

struct udp_port_cfg {
	u8			family;

	
	union {
		struct in_addr		local_ip;
#if IS_ENABLED(CONFIG_IPV6)
		struct in6_addr		local_ip6;
#endif
	};

	union {
		struct in_addr		peer_ip;
#if IS_ENABLED(CONFIG_IPV6)
		struct in6_addr		peer_ip6;
#endif
	};

	__be16			local_udp_port;
	__be16			peer_udp_port;
	int			bind_ifindex;
	unsigned int		use_udp_checksums:1,
				use_udp6_tx_checksums:1,
				use_udp6_rx_checksums:1,
				ipv6_v6only:1;
};

int udp_sock_create4(struct net *net, struct udp_port_cfg *cfg,
		     struct socket **sockp);

#if IS_ENABLED(CONFIG_IPV6)
int udp_sock_create6(struct net *net, struct udp_port_cfg *cfg,
		     struct socket **sockp);
#else
static inline int udp_sock_create6(struct net *net, struct udp_port_cfg *cfg,
				   struct socket **sockp)
{
	return 0;
}
#endif

static inline int udp_sock_create(struct net *net,
				  struct udp_port_cfg *cfg,
				  struct socket **sockp)
{
	if (cfg->family == AF_INET)
		return udp_sock_create4(net, cfg, sockp);

	if (cfg->family == AF_INET6)
		return udp_sock_create6(net, cfg, sockp);

	return -EPFNOSUPPORT;
}

typedef int (*udp_tunnel_encap_rcv_t)(struct sock *sk, struct sk_buff *skb);
typedef int (*udp_tunnel_encap_err_lookup_t)(struct sock *sk,
					     struct sk_buff *skb);
typedef void (*udp_tunnel_encap_err_rcv_t)(struct sock *sk,
					   struct sk_buff *skb, int err,
					   __be16 port, u32 info, u8 *payload);
typedef void (*udp_tunnel_encap_destroy_t)(struct sock *sk);
typedef struct sk_buff *(*udp_tunnel_gro_receive_t)(struct sock *sk,
						    struct list_head *head,
						    struct sk_buff *skb);
typedef int (*udp_tunnel_gro_complete_t)(struct sock *sk, struct sk_buff *skb,
					 int nhoff);

struct udp_tunnel_sock_cfg {
	void *sk_user_data;     
	
	__u8  encap_type;
	udp_tunnel_encap_rcv_t encap_rcv;
	udp_tunnel_encap_err_lookup_t encap_err_lookup;
	udp_tunnel_encap_err_rcv_t encap_err_rcv;
	udp_tunnel_encap_destroy_t encap_destroy;
	udp_tunnel_gro_receive_t gro_receive;
	udp_tunnel_gro_complete_t gro_complete;
};


void setup_udp_tunnel_sock(struct net *net, struct socket *sock,
			   struct udp_tunnel_sock_cfg *sock_cfg);


enum udp_parsable_tunnel_type {
	UDP_TUNNEL_TYPE_VXLAN	  = BIT(0), 
	UDP_TUNNEL_TYPE_GENEVE	  = BIT(1), 
	UDP_TUNNEL_TYPE_VXLAN_GPE = BIT(2), 
};

struct udp_tunnel_info {
	unsigned short type;
	sa_family_t sa_family;
	__be16 port;
	u8 hw_priv;
};


void udp_tunnel_push_rx_port(struct net_device *dev, struct socket *sock,
			     unsigned short type);
void udp_tunnel_drop_rx_port(struct net_device *dev, struct socket *sock,
			     unsigned short type);
void udp_tunnel_notify_add_rx_port(struct socket *sock, unsigned short type);
void udp_tunnel_notify_del_rx_port(struct socket *sock, unsigned short type);

static inline void udp_tunnel_get_rx_info(struct net_device *dev)
{
	ASSERT_RTNL();
	if (!(dev->features & NETIF_F_RX_UDP_TUNNEL_PORT))
		return;
	call_netdevice_notifiers(NETDEV_UDP_TUNNEL_PUSH_INFO, dev);
}

static inline void udp_tunnel_drop_rx_info(struct net_device *dev)
{
	ASSERT_RTNL();
	if (!(dev->features & NETIF_F_RX_UDP_TUNNEL_PORT))
		return;
	call_netdevice_notifiers(NETDEV_UDP_TUNNEL_DROP_INFO, dev);
}


void udp_tunnel_xmit_skb(struct rtable *rt, struct sock *sk, struct sk_buff *skb,
			 __be32 src, __be32 dst, __u8 tos, __u8 ttl,
			 __be16 df, __be16 src_port, __be16 dst_port,
			 bool xnet, bool nocheck);

int udp_tunnel6_xmit_skb(struct dst_entry *dst, struct sock *sk,
			 struct sk_buff *skb,
			 struct net_device *dev, struct in6_addr *saddr,
			 struct in6_addr *daddr,
			 __u8 prio, __u8 ttl, __be32 label,
			 __be16 src_port, __be16 dst_port, bool nocheck);

void udp_tunnel_sock_release(struct socket *sock);

struct metadata_dst *udp_tun_rx_dst(struct sk_buff *skb, unsigned short family,
				    __be16 flags, __be64 tunnel_id,
				    int md_size);

#ifdef CONFIG_INET
static inline int udp_tunnel_handle_offloads(struct sk_buff *skb, bool udp_csum)
{
	int type = udp_csum ? SKB_GSO_UDP_TUNNEL_CSUM : SKB_GSO_UDP_TUNNEL;

	return iptunnel_handle_offloads(skb, type);
}
#endif

static inline void udp_tunnel_encap_enable(struct sock *sk)
{
	if (udp_test_and_set_bit(ENCAP_ENABLED, sk))
		return;

#if IS_ENABLED(CONFIG_IPV6)
	if (READ_ONCE(sk->sk_family) == PF_INET6)
		ipv6_stub->udpv6_encap_enable();
#endif
	udp_encap_enable();
}

#define UDP_TUNNEL_NIC_MAX_TABLES	4

enum udp_tunnel_nic_info_flags {
	
	UDP_TUNNEL_NIC_INFO_MAY_SLEEP	= BIT(0),
	
	UDP_TUNNEL_NIC_INFO_OPEN_ONLY	= BIT(1),
	
	UDP_TUNNEL_NIC_INFO_IPV4_ONLY	= BIT(2),
	
	UDP_TUNNEL_NIC_INFO_STATIC_IANA_VXLAN	= BIT(3),
};

struct udp_tunnel_nic;

#define UDP_TUNNEL_NIC_MAX_SHARING_DEVICES	(U16_MAX / 2)

struct udp_tunnel_nic_shared {
	struct udp_tunnel_nic *udp_tunnel_nic_info;

	struct list_head devices;
};

struct udp_tunnel_nic_shared_node {
	struct net_device *dev;
	struct list_head list;
};


struct udp_tunnel_nic_info {
	
	int (*set_port)(struct net_device *dev,
			unsigned int table, unsigned int entry,
			struct udp_tunnel_info *ti);
	int (*unset_port)(struct net_device *dev,
			  unsigned int table, unsigned int entry,
			  struct udp_tunnel_info *ti);

	
	int (*sync_table)(struct net_device *dev, unsigned int table);

	struct udp_tunnel_nic_shared *shared;

	unsigned int flags;

	struct udp_tunnel_nic_table_info {
		unsigned int n_entries;
		unsigned int tunnel_types;
	} tables[UDP_TUNNEL_NIC_MAX_TABLES];
};


struct udp_tunnel_nic_ops {
	void (*get_port)(struct net_device *dev, unsigned int table,
			 unsigned int idx, struct udp_tunnel_info *ti);
	void (*set_port_priv)(struct net_device *dev, unsigned int table,
			      unsigned int idx, u8 priv);
	void (*add_port)(struct net_device *dev, struct udp_tunnel_info *ti);
	void (*del_port)(struct net_device *dev, struct udp_tunnel_info *ti);
	void (*reset_ntf)(struct net_device *dev);

	size_t (*dump_size)(struct net_device *dev, unsigned int table);
	int (*dump_write)(struct net_device *dev, unsigned int table,
			  struct sk_buff *skb);
};

#ifdef CONFIG_INET
extern const struct udp_tunnel_nic_ops *udp_tunnel_nic_ops;
#else
#define udp_tunnel_nic_ops	((struct udp_tunnel_nic_ops *)NULL)
#endif

static inline void
udp_tunnel_nic_get_port(struct net_device *dev, unsigned int table,
			unsigned int idx, struct udp_tunnel_info *ti)
{
	
	memset(ti, 0, sizeof(*ti));

	if (udp_tunnel_nic_ops)
		udp_tunnel_nic_ops->get_port(dev, table, idx, ti);
}

static inline void
udp_tunnel_nic_set_port_priv(struct net_device *dev, unsigned int table,
			     unsigned int idx, u8 priv)
{
	if (udp_tunnel_nic_ops)
		udp_tunnel_nic_ops->set_port_priv(dev, table, idx, priv);
}

static inline void
udp_tunnel_nic_add_port(struct net_device *dev, struct udp_tunnel_info *ti)
{
	if (!(dev->features & NETIF_F_RX_UDP_TUNNEL_PORT))
		return;
	if (udp_tunnel_nic_ops)
		udp_tunnel_nic_ops->add_port(dev, ti);
}

static inline void
udp_tunnel_nic_del_port(struct net_device *dev, struct udp_tunnel_info *ti)
{
	if (!(dev->features & NETIF_F_RX_UDP_TUNNEL_PORT))
		return;
	if (udp_tunnel_nic_ops)
		udp_tunnel_nic_ops->del_port(dev, ti);
}


static inline void udp_tunnel_nic_reset_ntf(struct net_device *dev)
{
	if (udp_tunnel_nic_ops)
		udp_tunnel_nic_ops->reset_ntf(dev);
}

static inline size_t
udp_tunnel_nic_dump_size(struct net_device *dev, unsigned int table)
{
	if (!udp_tunnel_nic_ops)
		return 0;
	return udp_tunnel_nic_ops->dump_size(dev, table);
}

static inline int
udp_tunnel_nic_dump_write(struct net_device *dev, unsigned int table,
			  struct sk_buff *skb)
{
	if (!udp_tunnel_nic_ops)
		return 0;
	return udp_tunnel_nic_ops->dump_write(dev, table, skb);
}
#endif
