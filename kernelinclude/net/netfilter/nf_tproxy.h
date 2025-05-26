#ifndef _NF_TPROXY_H_
#define _NF_TPROXY_H_

#include <net/tcp.h>

enum nf_tproxy_lookup_t {
	 NF_TPROXY_LOOKUP_LISTENER,
	 NF_TPROXY_LOOKUP_ESTABLISHED,
};

static inline bool nf_tproxy_sk_is_transparent(struct sock *sk)
{
	if (inet_sk_transparent(sk))
		return true;

	sock_gen_put(sk);
	return false;
}

static inline void nf_tproxy_twsk_deschedule_put(struct inet_timewait_sock *tw)
{
	local_bh_disable();
	inet_twsk_deschedule_put(tw);
	local_bh_enable();
}


static inline void nf_tproxy_assign_sock(struct sk_buff *skb, struct sock *sk)
{
	skb_orphan(skb);
	skb->sk = sk;
	skb->destructor = sock_edemux;
}

__be32 nf_tproxy_laddr4(struct sk_buff *skb, __be32 user_laddr, __be32 daddr);


struct sock *
nf_tproxy_handle_time_wait4(struct net *net, struct sk_buff *skb,
			    __be32 laddr, __be16 lport, struct sock *sk);


struct sock *
nf_tproxy_get_sock_v4(struct net *net, struct sk_buff *skb,
		      const u8 protocol,
		      const __be32 saddr, const __be32 daddr,
		      const __be16 sport, const __be16 dport,
		      const struct net_device *in,
		      const enum nf_tproxy_lookup_t lookup_type);

const struct in6_addr *
nf_tproxy_laddr6(struct sk_buff *skb, const struct in6_addr *user_laddr,
		 const struct in6_addr *daddr);


struct sock *
nf_tproxy_handle_time_wait6(struct sk_buff *skb, int tproto, int thoff,
			    struct net *net,
			    const struct in6_addr *laddr,
			    const __be16 lport,
			    struct sock *sk);

struct sock *
nf_tproxy_get_sock_v6(struct net *net, struct sk_buff *skb, int thoff,
		      const u8 protocol,
		      const struct in6_addr *saddr, const struct in6_addr *daddr,
		      const __be16 sport, const __be16 dport,
		      const struct net_device *in,
		      const enum nf_tproxy_lookup_t lookup_type);

#endif 
