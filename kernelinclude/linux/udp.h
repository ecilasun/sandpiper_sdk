/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_UDP_H
#define _LINUX_UDP_H

#include <net/inet_sock.h>
#include <linux/skbuff.h>
#include <net/netns/hash.h>
#include <uapi/linux/udp.h>

static inline struct udphdr *udp_hdr(const struct sk_buff *skb)
{
	return (struct udphdr *)skb_transport_header(skb);
}

#define UDP_HTABLE_SIZE_MIN_PERNET	128
#define UDP_HTABLE_SIZE_MIN		(CONFIG_BASE_SMALL ? 128 : 256)
#define UDP_HTABLE_SIZE_MAX		65536

static inline u32 udp_hashfn(const struct net *net, u32 num, u32 mask)
{
	return (num + net_hash_mix(net)) & mask;
}

enum {
	UDP_FLAGS_CORK,		
	UDP_FLAGS_NO_CHECK6_TX, 
	UDP_FLAGS_NO_CHECK6_RX, 
	UDP_FLAGS_GRO_ENABLED,	
	UDP_FLAGS_ACCEPT_FRAGLIST,
	UDP_FLAGS_ACCEPT_L4,
	UDP_FLAGS_ENCAP_ENABLED, 
	UDP_FLAGS_UDPLITE_SEND_CC, 
	UDP_FLAGS_UDPLITE_RECV_CC, 
};

struct udp_sock {
	
	struct inet_sock inet;
#define udp_port_hash		inet.sk.__sk_common.skc_u16hashes[0]
#define udp_portaddr_hash	inet.sk.__sk_common.skc_u16hashes[1]
#define udp_portaddr_node	inet.sk.__sk_common.skc_portaddr_node

	unsigned long	 udp_flags;

	int		 pending;	
	__u8		 encap_type;	

	
	__u16		 len;		
	__u16		 gso_size;
	
	__u16		 pcslen;
	__u16		 pcrlen;
	
	int (*encap_rcv)(struct sock *sk, struct sk_buff *skb);
	void (*encap_err_rcv)(struct sock *sk, struct sk_buff *skb, int err,
			      __be16 port, u32 info, u8 *payload);
	int (*encap_err_lookup)(struct sock *sk, struct sk_buff *skb);
	void (*encap_destroy)(struct sock *sk);

	
	struct sk_buff *	(*gro_receive)(struct sock *sk,
					       struct list_head *head,
					       struct sk_buff *skb);
	int			(*gro_complete)(struct sock *sk,
						struct sk_buff *skb,
						int nhoff);

	
	struct sk_buff_head	reader_queue ____cacheline_aligned_in_smp;

	
	int		forward_deficit;

	
	int		forward_threshold;
};

#define udp_test_bit(nr, sk)			\
	test_bit(UDP_FLAGS_##nr, &udp_sk(sk)->udp_flags)
#define udp_set_bit(nr, sk)			\
	set_bit(UDP_FLAGS_##nr, &udp_sk(sk)->udp_flags)
#define udp_test_and_set_bit(nr, sk)		\
	test_and_set_bit(UDP_FLAGS_##nr, &udp_sk(sk)->udp_flags)
#define udp_clear_bit(nr, sk)			\
	clear_bit(UDP_FLAGS_##nr, &udp_sk(sk)->udp_flags)
#define udp_assign_bit(nr, sk, val)		\
	assign_bit(UDP_FLAGS_##nr, &udp_sk(sk)->udp_flags, val)

#define UDP_MAX_SEGMENTS	(1 << 7UL)

#define udp_sk(ptr) container_of_const(ptr, struct udp_sock, inet.sk)

static inline void udp_set_no_check6_tx(struct sock *sk, bool val)
{
	udp_assign_bit(NO_CHECK6_TX, sk, val);
}

static inline void udp_set_no_check6_rx(struct sock *sk, bool val)
{
	udp_assign_bit(NO_CHECK6_RX, sk, val);
}

static inline bool udp_get_no_check6_tx(const struct sock *sk)
{
	return udp_test_bit(NO_CHECK6_TX, sk);
}

static inline bool udp_get_no_check6_rx(const struct sock *sk)
{
	return udp_test_bit(NO_CHECK6_RX, sk);
}

static inline void udp_cmsg_recv(struct msghdr *msg, struct sock *sk,
				 struct sk_buff *skb)
{
	int gso_size;

	if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4) {
		gso_size = skb_shinfo(skb)->gso_size;
		put_cmsg(msg, SOL_UDP, UDP_GRO, sizeof(gso_size), &gso_size);
	}
}

DECLARE_STATIC_KEY_FALSE(udp_encap_needed_key);
#if IS_ENABLED(CONFIG_IPV6)
DECLARE_STATIC_KEY_FALSE(udpv6_encap_needed_key);
#endif

static inline bool udp_encap_needed(void)
{
	if (static_branch_unlikely(&udp_encap_needed_key))
		return true;

#if IS_ENABLED(CONFIG_IPV6)
	if (static_branch_unlikely(&udpv6_encap_needed_key))
		return true;
#endif

	return false;
}

static inline bool udp_unexpected_gso(struct sock *sk, struct sk_buff *skb)
{
	if (!skb_is_gso(skb))
		return false;

	if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4 &&
	    !udp_test_bit(ACCEPT_L4, sk))
		return true;

	if (skb_shinfo(skb)->gso_type & SKB_GSO_FRAGLIST &&
	    !udp_test_bit(ACCEPT_FRAGLIST, sk))
		return true;

	
	if (udp_encap_needed() &&
	    READ_ONCE(udp_sk(sk)->encap_rcv) &&
	    !(skb_shinfo(skb)->gso_type &
	      (SKB_GSO_UDP_TUNNEL | SKB_GSO_UDP_TUNNEL_CSUM)))
		return true;

	return false;
}

static inline void udp_allow_gso(struct sock *sk)
{
	udp_set_bit(ACCEPT_L4, sk);
	udp_set_bit(ACCEPT_FRAGLIST, sk);
}

#define udp_portaddr_for_each_entry(__sk, list) \
	hlist_for_each_entry(__sk, list, __sk_common.skc_portaddr_node)

#define udp_portaddr_for_each_entry_rcu(__sk, list) \
	hlist_for_each_entry_rcu(__sk, list, __sk_common.skc_portaddr_node)

#define IS_UDPLITE(__sk) (__sk->sk_protocol == IPPROTO_UDPLITE)

#endif	
