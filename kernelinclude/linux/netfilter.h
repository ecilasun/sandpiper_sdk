/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_NETFILTER_H
#define __LINUX_NETFILTER_H

#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/static_key.h>
#include <linux/module.h>
#include <linux/netfilter_defs.h>
#include <linux/netdevice.h>
#include <linux/sockptr.h>
#include <net/net_namespace.h>

static inline int NF_DROP_GETERR(int verdict)
{
	return -(verdict >> NF_VERDICT_QBITS);
}

static inline int nf_inet_addr_cmp(const union nf_inet_addr *a1,
				   const union nf_inet_addr *a2)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS) && BITS_PER_LONG == 64
	const unsigned long *ul1 = (const unsigned long *)a1;
	const unsigned long *ul2 = (const unsigned long *)a2;

	return ((ul1[0] ^ ul2[0]) | (ul1[1] ^ ul2[1])) == 0UL;
#else
	return a1->all[0] == a2->all[0] &&
	       a1->all[1] == a2->all[1] &&
	       a1->all[2] == a2->all[2] &&
	       a1->all[3] == a2->all[3];
#endif
}

static inline void nf_inet_addr_mask(const union nf_inet_addr *a1,
				     union nf_inet_addr *result,
				     const union nf_inet_addr *mask)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS) && BITS_PER_LONG == 64
	const unsigned long *ua = (const unsigned long *)a1;
	unsigned long *ur = (unsigned long *)result;
	const unsigned long *um = (const unsigned long *)mask;

	ur[0] = ua[0] & um[0];
	ur[1] = ua[1] & um[1];
#else
	result->all[0] = a1->all[0] & mask->all[0];
	result->all[1] = a1->all[1] & mask->all[1];
	result->all[2] = a1->all[2] & mask->all[2];
	result->all[3] = a1->all[3] & mask->all[3];
#endif
}

int netfilter_init(void);

struct sk_buff;

struct nf_hook_ops;

struct sock;

struct nf_hook_state {
	u8 hook;
	u8 pf;
	struct net_device *in;
	struct net_device *out;
	struct sock *sk;
	struct net *net;
	int (*okfn)(struct net *, struct sock *, struct sk_buff *);
};

typedef unsigned int nf_hookfn(void *priv,
			       struct sk_buff *skb,
			       const struct nf_hook_state *state);
enum nf_hook_ops_type {
	NF_HOOK_OP_UNDEFINED,
	NF_HOOK_OP_NF_TABLES,
	NF_HOOK_OP_BPF,
};

struct nf_hook_ops {
	
	nf_hookfn		*hook;
	struct net_device	*dev;
	void			*priv;
	u8			pf;
	enum nf_hook_ops_type	hook_ops_type:8;
	unsigned int		hooknum;
	
	int			priority;
};

struct nf_hook_entry {
	nf_hookfn			*hook;
	void				*priv;
};

struct nf_hook_entries_rcu_head {
	struct rcu_head head;
	void	*allocation;
};

struct nf_hook_entries {
	u16				num_hook_entries;
	
	struct nf_hook_entry		hooks[];

	
};

#ifdef CONFIG_NETFILTER
static inline struct nf_hook_ops **nf_hook_entries_get_hook_ops(const struct nf_hook_entries *e)
{
	unsigned int n = e->num_hook_entries;
	const void *hook_end;

	hook_end = &e->hooks[n]; 

	return (struct nf_hook_ops **)hook_end;
}

static inline int
nf_hook_entry_hookfn(const struct nf_hook_entry *entry, struct sk_buff *skb,
		     struct nf_hook_state *state)
{
	return entry->hook(entry->priv, skb, state);
}

static inline void nf_hook_state_init(struct nf_hook_state *p,
				      unsigned int hook,
				      u_int8_t pf,
				      struct net_device *indev,
				      struct net_device *outdev,
				      struct sock *sk,
				      struct net *net,
				      int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	p->hook = hook;
	p->pf = pf;
	p->in = indev;
	p->out = outdev;
	p->sk = sk;
	p->net = net;
	p->okfn = okfn;
}



struct nf_sockopt_ops {
	struct list_head list;

	u_int8_t pf;

	
	int set_optmin;
	int set_optmax;
	int (*set)(struct sock *sk, int optval, sockptr_t arg,
		   unsigned int len);
	int get_optmin;
	int get_optmax;
	int (*get)(struct sock *sk, int optval, void __user *user, int *len);
	
	struct module *owner;
};


int nf_register_net_hook(struct net *net, const struct nf_hook_ops *ops);
void nf_unregister_net_hook(struct net *net, const struct nf_hook_ops *ops);
int nf_register_net_hooks(struct net *net, const struct nf_hook_ops *reg,
			  unsigned int n);
void nf_unregister_net_hooks(struct net *net, const struct nf_hook_ops *reg,
			     unsigned int n);


int nf_register_sockopt(struct nf_sockopt_ops *reg);
void nf_unregister_sockopt(struct nf_sockopt_ops *reg);

#ifdef CONFIG_JUMP_LABEL
extern struct static_key nf_hooks_needed[NFPROTO_NUMPROTO][NF_MAX_HOOKS];
#endif

int nf_hook_slow(struct sk_buff *skb, struct nf_hook_state *state,
		 const struct nf_hook_entries *e, unsigned int i);

void nf_hook_slow_list(struct list_head *head, struct nf_hook_state *state,
		       const struct nf_hook_entries *e);

static inline int nf_hook(u_int8_t pf, unsigned int hook, struct net *net,
			  struct sock *sk, struct sk_buff *skb,
			  struct net_device *indev, struct net_device *outdev,
			  int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	struct nf_hook_entries *hook_head = NULL;
	int ret = 1;

#ifdef CONFIG_JUMP_LABEL
	if (__builtin_constant_p(pf) &&
	    __builtin_constant_p(hook) &&
	    !static_key_false(&nf_hooks_needed[pf][hook]))
		return 1;
#endif

	rcu_read_lock();
	switch (pf) {
	case NFPROTO_IPV4:
		hook_head = rcu_dereference(net->nf.hooks_ipv4[hook]);
		break;
	case NFPROTO_IPV6:
		hook_head = rcu_dereference(net->nf.hooks_ipv6[hook]);
		break;
	case NFPROTO_ARP:
#ifdef CONFIG_NETFILTER_FAMILY_ARP
		if (WARN_ON_ONCE(hook >= ARRAY_SIZE(net->nf.hooks_arp)))
			break;
		hook_head = rcu_dereference(net->nf.hooks_arp[hook]);
#endif
		break;
	case NFPROTO_BRIDGE:
#ifdef CONFIG_NETFILTER_FAMILY_BRIDGE
		hook_head = rcu_dereference(net->nf.hooks_bridge[hook]);
#endif
		break;
	default:
		WARN_ON_ONCE(1);
		break;
	}

	if (hook_head) {
		struct nf_hook_state state;

		nf_hook_state_init(&state, hook, pf, indev, outdev,
				   sk, net, okfn);

		ret = nf_hook_slow(skb, &state, hook_head, 0);
	}
	rcu_read_unlock();

	return ret;
}





static inline int
NF_HOOK_COND(uint8_t pf, unsigned int hook, struct net *net, struct sock *sk,
	     struct sk_buff *skb, struct net_device *in, struct net_device *out,
	     int (*okfn)(struct net *, struct sock *, struct sk_buff *),
	     bool cond)
{
	int ret;

	if (!cond ||
	    ((ret = nf_hook(pf, hook, net, sk, skb, in, out, okfn)) == 1))
		ret = okfn(net, sk, skb);
	return ret;
}

static inline int
NF_HOOK(uint8_t pf, unsigned int hook, struct net *net, struct sock *sk, struct sk_buff *skb,
	struct net_device *in, struct net_device *out,
	int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	int ret = nf_hook(pf, hook, net, sk, skb, in, out, okfn);
	if (ret == 1)
		ret = okfn(net, sk, skb);
	return ret;
}

static inline void
NF_HOOK_LIST(uint8_t pf, unsigned int hook, struct net *net, struct sock *sk,
	     struct list_head *head, struct net_device *in, struct net_device *out,
	     int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	struct nf_hook_entries *hook_head = NULL;

#ifdef CONFIG_JUMP_LABEL
	if (__builtin_constant_p(pf) &&
	    __builtin_constant_p(hook) &&
	    !static_key_false(&nf_hooks_needed[pf][hook]))
		return;
#endif

	rcu_read_lock();
	switch (pf) {
	case NFPROTO_IPV4:
		hook_head = rcu_dereference(net->nf.hooks_ipv4[hook]);
		break;
	case NFPROTO_IPV6:
		hook_head = rcu_dereference(net->nf.hooks_ipv6[hook]);
		break;
	default:
		WARN_ON_ONCE(1);
		break;
	}

	if (hook_head) {
		struct nf_hook_state state;

		nf_hook_state_init(&state, hook, pf, in, out, sk, net, okfn);

		nf_hook_slow_list(head, &state, hook_head);
	}
	rcu_read_unlock();
}


int nf_setsockopt(struct sock *sk, u_int8_t pf, int optval, sockptr_t opt,
		  unsigned int len);
int nf_getsockopt(struct sock *sk, u_int8_t pf, int optval, char __user *opt,
		  int *len);

struct flowi;
struct nf_queue_entry;

__sum16 nf_checksum(struct sk_buff *skb, unsigned int hook,
		    unsigned int dataoff, u_int8_t protocol,
		    unsigned short family);

__sum16 nf_checksum_partial(struct sk_buff *skb, unsigned int hook,
			    unsigned int dataoff, unsigned int len,
			    u_int8_t protocol, unsigned short family);
int nf_route(struct net *net, struct dst_entry **dst, struct flowi *fl,
	     bool strict, unsigned short family);
int nf_reroute(struct sk_buff *skb, struct nf_queue_entry *entry);

#include <net/flow.h>

struct nf_conn;
enum nf_nat_manip_type;
struct nlattr;
enum ip_conntrack_dir;

struct nf_nat_hook {
	int (*parse_nat_setup)(struct nf_conn *ct, enum nf_nat_manip_type manip,
			       const struct nlattr *attr);
	void (*decode_session)(struct sk_buff *skb, struct flowi *fl);
	unsigned int (*manip_pkt)(struct sk_buff *skb, struct nf_conn *ct,
				  enum nf_nat_manip_type mtype,
				  enum ip_conntrack_dir dir);
	void (*remove_nat_bysrc)(struct nf_conn *ct);
};

extern const struct nf_nat_hook __rcu *nf_nat_hook;

static inline void
nf_nat_decode_session(struct sk_buff *skb, struct flowi *fl, u_int8_t family)
{
#if IS_ENABLED(CONFIG_NF_NAT)
	const struct nf_nat_hook *nat_hook;

	rcu_read_lock();
	nat_hook = rcu_dereference(nf_nat_hook);
	if (nat_hook && nat_hook->decode_session)
		nat_hook->decode_session(skb, fl);
	rcu_read_unlock();
#endif
}

#else 
static inline int
NF_HOOK_COND(uint8_t pf, unsigned int hook, struct net *net, struct sock *sk,
	     struct sk_buff *skb, struct net_device *in, struct net_device *out,
	     int (*okfn)(struct net *, struct sock *, struct sk_buff *),
	     bool cond)
{
	return okfn(net, sk, skb);
}

static inline int
NF_HOOK(uint8_t pf, unsigned int hook, struct net *net, struct sock *sk,
	struct sk_buff *skb, struct net_device *in, struct net_device *out,
	int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	return okfn(net, sk, skb);
}

static inline void
NF_HOOK_LIST(uint8_t pf, unsigned int hook, struct net *net, struct sock *sk,
	     struct list_head *head, struct net_device *in, struct net_device *out,
	     int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	
}

static inline int nf_hook(u_int8_t pf, unsigned int hook, struct net *net,
			  struct sock *sk, struct sk_buff *skb,
			  struct net_device *indev, struct net_device *outdev,
			  int (*okfn)(struct net *, struct sock *, struct sk_buff *))
{
	return 1;
}
struct flowi;
static inline void
nf_nat_decode_session(struct sk_buff *skb, struct flowi *fl, u_int8_t family)
{
}
#endif 

#if IS_ENABLED(CONFIG_NF_CONNTRACK)
#include <linux/netfilter/nf_conntrack_zones_common.h>

void nf_ct_attach(struct sk_buff *, const struct sk_buff *);
void nf_ct_set_closing(struct nf_conntrack *nfct);
struct nf_conntrack_tuple;
bool nf_ct_get_tuple_skb(struct nf_conntrack_tuple *dst_tuple,
			 const struct sk_buff *skb);
#else
static inline void nf_ct_attach(struct sk_buff *new, struct sk_buff *skb) {}
static inline void nf_ct_set_closing(struct nf_conntrack *nfct) {}
struct nf_conntrack_tuple;
static inline bool nf_ct_get_tuple_skb(struct nf_conntrack_tuple *dst_tuple,
				       const struct sk_buff *skb)
{
	return false;
}
#endif

struct nf_conn;
enum ip_conntrack_info;

struct nf_ct_hook {
	int (*update)(struct net *net, struct sk_buff *skb);
	void (*destroy)(struct nf_conntrack *);
	bool (*get_tuple_skb)(struct nf_conntrack_tuple *,
			      const struct sk_buff *);
	void (*attach)(struct sk_buff *nskb, const struct sk_buff *skb);
	void (*set_closing)(struct nf_conntrack *nfct);
	int (*confirm)(struct sk_buff *skb);
};
extern const struct nf_ct_hook __rcu *nf_ct_hook;

struct nlattr;

struct nfnl_ct_hook {
	size_t (*build_size)(const struct nf_conn *ct);
	int (*build)(struct sk_buff *skb, struct nf_conn *ct,
		     enum ip_conntrack_info ctinfo,
		     u_int16_t ct_attr, u_int16_t ct_info_attr);
	int (*parse)(const struct nlattr *attr, struct nf_conn *ct);
	int (*attach_expect)(const struct nlattr *attr, struct nf_conn *ct,
			     u32 portid, u32 report);
	void (*seq_adjust)(struct sk_buff *skb, struct nf_conn *ct,
			   enum ip_conntrack_info ctinfo, s32 off);
};
extern const struct nfnl_ct_hook __rcu *nfnl_ct_hook;

struct nf_defrag_hook {
	struct module *owner;
	int (*enable)(struct net *net);
	void (*disable)(struct net *net);
};

extern const struct nf_defrag_hook __rcu *nf_defrag_v4_hook;
extern const struct nf_defrag_hook __rcu *nf_defrag_v6_hook;


DECLARE_PER_CPU(bool, nf_skb_duplicated);


extern u8 nf_ctnetlink_has_listener;
#endif 
