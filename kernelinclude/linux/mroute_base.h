#ifndef __LINUX_MROUTE_BASE_H
#define __LINUX_MROUTE_BASE_H

#include <linux/netdevice.h>
#include <linux/rhashtable-types.h>
#include <linux/spinlock.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <net/fib_notifier.h>
#include <net/ip_fib.h>


struct vif_device {
	struct net_device __rcu *dev;
	netdevice_tracker dev_tracker;
	unsigned long bytes_in, bytes_out;
	unsigned long pkt_in, pkt_out;
	unsigned long rate_limit;
	unsigned char threshold;
	unsigned short flags;
	int link;

	
	struct netdev_phys_item_id dev_parent_id;
	__be32 local, remote;
};

struct vif_entry_notifier_info {
	struct fib_notifier_info info;
	struct net_device *dev;
	unsigned short vif_index;
	unsigned short vif_flags;
	u32 tb_id;
};

static inline int mr_call_vif_notifier(struct notifier_block *nb,
				       unsigned short family,
				       enum fib_event_type event_type,
				       struct vif_device *vif,
				       struct net_device *vif_dev,
				       unsigned short vif_index, u32 tb_id,
				       struct netlink_ext_ack *extack)
{
	struct vif_entry_notifier_info info = {
		.info = {
			.family = family,
			.extack = extack,
		},
		.dev = vif_dev,
		.vif_index = vif_index,
		.vif_flags = vif->flags,
		.tb_id = tb_id,
	};

	return call_fib_notifier(nb, event_type, &info.info);
}

static inline int mr_call_vif_notifiers(struct net *net,
					unsigned short family,
					enum fib_event_type event_type,
					struct vif_device *vif,
					struct net_device *vif_dev,
					unsigned short vif_index, u32 tb_id,
					unsigned int *ipmr_seq)
{
	struct vif_entry_notifier_info info = {
		.info = {
			.family = family,
		},
		.dev = vif_dev,
		.vif_index = vif_index,
		.vif_flags = vif->flags,
		.tb_id = tb_id,
	};

	ASSERT_RTNL();
	(*ipmr_seq)++;
	return call_fib_notifiers(net, event_type, &info.info);
}

#ifndef MAXVIFS

#define MAXVIFS	32
#endif


#define VIF_EXISTS(_mrt, _idx) (!!rcu_access_pointer((_mrt)->vif_table[_idx].dev))


enum {
	MFC_STATIC = BIT(0),
	MFC_OFFLOAD = BIT(1),
};


struct mr_mfc {
	struct rhlist_head mnode;
	unsigned short mfc_parent;
	int mfc_flags;

	union {
		struct {
			unsigned long expires;
			struct sk_buff_head unresolved;
		} unres;
		struct {
			unsigned long last_assert;
			int minvif;
			int maxvif;
			unsigned long bytes;
			unsigned long pkt;
			unsigned long wrong_if;
			unsigned long lastuse;
			unsigned char ttls[MAXVIFS];
			refcount_t refcount;
		} res;
	} mfc_un;
	struct list_head list;
	struct rcu_head	rcu;
	void (*free)(struct rcu_head *head);
};

static inline void mr_cache_put(struct mr_mfc *c)
{
	if (refcount_dec_and_test(&c->mfc_un.res.refcount))
		call_rcu(&c->rcu, c->free);
}

static inline void mr_cache_hold(struct mr_mfc *c)
{
	refcount_inc(&c->mfc_un.res.refcount);
}

struct mfc_entry_notifier_info {
	struct fib_notifier_info info;
	struct mr_mfc *mfc;
	u32 tb_id;
};

static inline int mr_call_mfc_notifier(struct notifier_block *nb,
				       unsigned short family,
				       enum fib_event_type event_type,
				       struct mr_mfc *mfc, u32 tb_id,
				       struct netlink_ext_ack *extack)
{
	struct mfc_entry_notifier_info info = {
		.info = {
			.family = family,
			.extack = extack,
		},
		.mfc = mfc,
		.tb_id = tb_id
	};

	return call_fib_notifier(nb, event_type, &info.info);
}

static inline int mr_call_mfc_notifiers(struct net *net,
					unsigned short family,
					enum fib_event_type event_type,
					struct mr_mfc *mfc, u32 tb_id,
					unsigned int *ipmr_seq)
{
	struct mfc_entry_notifier_info info = {
		.info = {
			.family = family,
		},
		.mfc = mfc,
		.tb_id = tb_id
	};

	ASSERT_RTNL();
	(*ipmr_seq)++;
	return call_fib_notifiers(net, event_type, &info.info);
}

struct mr_table;


struct mr_table_ops {
	const struct rhashtable_params *rht_params;
	void *cmparg_any;
};


struct mr_table {
	struct list_head	list;
	possible_net_t		net;
	struct mr_table_ops	ops;
	u32			id;
	struct sock __rcu	*mroute_sk;
	struct timer_list	ipmr_expire_timer;
	struct list_head	mfc_unres_queue;
	struct vif_device	vif_table[MAXVIFS];
	struct rhltable		mfc_hash;
	struct list_head	mfc_cache_list;
	int			maxvif;
	atomic_t		cache_resolve_queue_len;
	bool			mroute_do_assert;
	bool			mroute_do_pim;
	bool			mroute_do_wrvifwhole;
	int			mroute_reg_vif_num;
};

#ifdef CONFIG_IP_MROUTE_COMMON
void vif_device_init(struct vif_device *v,
		     struct net_device *dev,
		     unsigned long rate_limit,
		     unsigned char threshold,
		     unsigned short flags,
		     unsigned short get_iflink_mask);

struct mr_table *
mr_table_alloc(struct net *net, u32 id,
	       struct mr_table_ops *ops,
	       void (*expire_func)(struct timer_list *t),
	       void (*table_set)(struct mr_table *mrt,
				 struct net *net));


void *mr_mfc_find_parent(struct mr_table *mrt,
			 void *hasharg, int parent);
void *mr_mfc_find_any_parent(struct mr_table *mrt, int vifi);
void *mr_mfc_find_any(struct mr_table *mrt, int vifi, void *hasharg);

int mr_fill_mroute(struct mr_table *mrt, struct sk_buff *skb,
		   struct mr_mfc *c, struct rtmsg *rtm);
int mr_table_dump(struct mr_table *mrt, struct sk_buff *skb,
		  struct netlink_callback *cb,
		  int (*fill)(struct mr_table *mrt, struct sk_buff *skb,
			      u32 portid, u32 seq, struct mr_mfc *c,
			      int cmd, int flags),
		  spinlock_t *lock, struct fib_dump_filter *filter);
int mr_rtm_dumproute(struct sk_buff *skb, struct netlink_callback *cb,
		     struct mr_table *(*iter)(struct net *net,
					      struct mr_table *mrt),
		     int (*fill)(struct mr_table *mrt,
				 struct sk_buff *skb,
				 u32 portid, u32 seq, struct mr_mfc *c,
				 int cmd, int flags),
		     spinlock_t *lock, struct fib_dump_filter *filter);

int mr_dump(struct net *net, struct notifier_block *nb, unsigned short family,
	    int (*rules_dump)(struct net *net,
			      struct notifier_block *nb,
			      struct netlink_ext_ack *extack),
	    struct mr_table *(*mr_iter)(struct net *net,
					struct mr_table *mrt),
	    struct netlink_ext_ack *extack);
#else
static inline void vif_device_init(struct vif_device *v,
				   struct net_device *dev,
				   unsigned long rate_limit,
				   unsigned char threshold,
				   unsigned short flags,
				   unsigned short get_iflink_mask)
{
}

static inline void *mr_mfc_find_parent(struct mr_table *mrt,
				       void *hasharg, int parent)
{
	return NULL;
}

static inline void *mr_mfc_find_any_parent(struct mr_table *mrt,
					   int vifi)
{
	return NULL;
}

static inline struct mr_mfc *mr_mfc_find_any(struct mr_table *mrt,
					     int vifi, void *hasharg)
{
	return NULL;
}

static inline int mr_fill_mroute(struct mr_table *mrt, struct sk_buff *skb,
				 struct mr_mfc *c, struct rtmsg *rtm)
{
	return -EINVAL;
}

static inline int
mr_rtm_dumproute(struct sk_buff *skb, struct netlink_callback *cb,
		 struct mr_table *(*iter)(struct net *net,
					  struct mr_table *mrt),
		 int (*fill)(struct mr_table *mrt,
			     struct sk_buff *skb,
			     u32 portid, u32 seq, struct mr_mfc *c,
			     int cmd, int flags),
		 spinlock_t *lock, struct fib_dump_filter *filter)
{
	return -EINVAL;
}

static inline int mr_dump(struct net *net, struct notifier_block *nb,
			  unsigned short family,
			  int (*rules_dump)(struct net *net,
					    struct notifier_block *nb,
					    struct netlink_ext_ack *extack),
			  struct mr_table *(*mr_iter)(struct net *net,
						      struct mr_table *mrt),
			  struct netlink_ext_ack *extack)
{
	return -EINVAL;
}
#endif

static inline void *mr_mfc_find(struct mr_table *mrt, void *hasharg)
{
	return mr_mfc_find_parent(mrt, hasharg, -1);
}

#ifdef CONFIG_PROC_FS
struct mr_vif_iter {
	struct seq_net_private p;
	struct mr_table *mrt;
	int ct;
};

struct mr_mfc_iter {
	struct seq_net_private p;
	struct mr_table *mrt;
	struct list_head *cache;

	
	spinlock_t *lock;
};

#ifdef CONFIG_IP_MROUTE_COMMON
void *mr_vif_seq_idx(struct net *net, struct mr_vif_iter *iter, loff_t pos);
void *mr_vif_seq_next(struct seq_file *seq, void *v, loff_t *pos);

static inline void *mr_vif_seq_start(struct seq_file *seq, loff_t *pos)
{
	return *pos ? mr_vif_seq_idx(seq_file_net(seq),
				     seq->private, *pos - 1)
		    : SEQ_START_TOKEN;
}


void *mr_mfc_seq_idx(struct net *net,
		     struct mr_mfc_iter *it, loff_t pos);
void *mr_mfc_seq_next(struct seq_file *seq, void *v,
		      loff_t *pos);

static inline void *mr_mfc_seq_start(struct seq_file *seq, loff_t *pos,
				     struct mr_table *mrt, spinlock_t *lock)
{
	struct mr_mfc_iter *it = seq->private;

	it->mrt = mrt;
	it->cache = NULL;
	it->lock = lock;

	return *pos ? mr_mfc_seq_idx(seq_file_net(seq),
				     seq->private, *pos - 1)
		    : SEQ_START_TOKEN;
}

static inline void mr_mfc_seq_stop(struct seq_file *seq, void *v)
{
	struct mr_mfc_iter *it = seq->private;
	struct mr_table *mrt = it->mrt;

	if (it->cache == &mrt->mfc_unres_queue)
		spin_unlock_bh(it->lock);
	else if (it->cache == &mrt->mfc_cache_list)
		rcu_read_unlock();
}
#else
static inline void *mr_vif_seq_idx(struct net *net, struct mr_vif_iter *iter,
				   loff_t pos)
{
	return NULL;
}

static inline void *mr_vif_seq_next(struct seq_file *seq,
				    void *v, loff_t *pos)
{
	return NULL;
}

static inline void *mr_vif_seq_start(struct seq_file *seq, loff_t *pos)
{
	return NULL;
}

static inline void *mr_mfc_seq_idx(struct net *net,
				   struct mr_mfc_iter *it, loff_t pos)
{
	return NULL;
}

static inline void *mr_mfc_seq_next(struct seq_file *seq, void *v,
				    loff_t *pos)
{
	return NULL;
}

static inline void *mr_mfc_seq_start(struct seq_file *seq, loff_t *pos,
				     struct mr_table *mrt, spinlock_t *lock)
{
	return NULL;
}

static inline void mr_mfc_seq_stop(struct seq_file *seq, void *v)
{
}
#endif
#endif
#endif
