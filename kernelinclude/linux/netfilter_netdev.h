/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NETFILTER_NETDEV_H_
#define _NETFILTER_NETDEV_H_

#include <linux/netfilter.h>
#include <linux/netdevice.h>

#ifdef CONFIG_NETFILTER_INGRESS
static inline bool nf_hook_ingress_active(const struct sk_buff *skb)
{
#ifdef CONFIG_JUMP_LABEL
	if (!static_key_false(&nf_hooks_needed[NFPROTO_NETDEV][NF_NETDEV_INGRESS]))
		return false;
#endif
	return rcu_access_pointer(skb->dev->nf_hooks_ingress);
}


static inline int nf_hook_ingress(struct sk_buff *skb)
{
	struct nf_hook_entries *e = rcu_dereference(skb->dev->nf_hooks_ingress);
	struct nf_hook_state state;
	int ret;

	
	if (unlikely(!e))
		return 0;

	nf_hook_state_init(&state, NF_NETDEV_INGRESS,
			   NFPROTO_NETDEV, skb->dev, NULL, NULL,
			   dev_net(skb->dev), NULL);
	ret = nf_hook_slow(skb, &state, e, 0);
	if (ret == 0)
		return -1;

	return ret;
}

#else 
static inline int nf_hook_ingress_active(struct sk_buff *skb)
{
	return 0;
}

static inline int nf_hook_ingress(struct sk_buff *skb)
{
	return 0;
}
#endif 

#ifdef CONFIG_NETFILTER_EGRESS
static inline bool nf_hook_egress_active(void)
{
#ifdef CONFIG_JUMP_LABEL
	if (!static_key_false(&nf_hooks_needed[NFPROTO_NETDEV][NF_NETDEV_EGRESS]))
		return false;
#endif
	return true;
}


static inline struct sk_buff *nf_hook_egress(struct sk_buff *skb, int *rc,
					     struct net_device *dev)
{
	struct nf_hook_entries *e;
	struct nf_hook_state state;
	int ret;

#ifdef CONFIG_NETFILTER_SKIP_EGRESS
	if (skb->nf_skip_egress)
		return skb;
#endif

	e = rcu_dereference_check(dev->nf_hooks_egress, rcu_read_lock_bh_held());
	if (!e)
		return skb;

	nf_hook_state_init(&state, NF_NETDEV_EGRESS,
			   NFPROTO_NETDEV, NULL, dev, NULL,
			   dev_net(dev), NULL);

	
	rcu_read_lock();
	ret = nf_hook_slow(skb, &state, e, 0);
	rcu_read_unlock();

	if (ret == 1) {
		return skb;
	} else if (ret < 0) {
		*rc = NET_XMIT_DROP;
		return NULL;
	} else { 
		*rc = NET_XMIT_SUCCESS;
		return NULL;
	}
}
#else 
static inline bool nf_hook_egress_active(void)
{
	return false;
}

static inline struct sk_buff *nf_hook_egress(struct sk_buff *skb, int *rc,
					     struct net_device *dev)
{
	return skb;
}
#endif 

static inline void nf_skip_egress(struct sk_buff *skb, bool skip)
{
#ifdef CONFIG_NETFILTER_SKIP_EGRESS
	skb->nf_skip_egress = skip;
#endif
}

static inline void nf_hook_netdev_init(struct net_device *dev)
{
#ifdef CONFIG_NETFILTER_INGRESS
	RCU_INIT_POINTER(dev->nf_hooks_ingress, NULL);
#endif
#ifdef CONFIG_NETFILTER_EGRESS
	RCU_INIT_POINTER(dev->nf_hooks_egress, NULL);
#endif
}

#endif 
