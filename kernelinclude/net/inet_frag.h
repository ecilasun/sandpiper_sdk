/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_FRAG_H__
#define __NET_FRAG_H__

#include <linux/rhashtable-types.h>
#include <linux/completion.h>
#include <linux/in6.h>
#include <linux/rbtree_types.h>
#include <linux/refcount.h>
#include <net/dropreason-core.h>


struct fqdir {
	
	long			high_thresh;
	long			low_thresh;
	int			timeout;
	int			max_dist;
	struct inet_frags	*f;
	struct net		*net;
	bool			dead;

	struct rhashtable       rhashtable ____cacheline_aligned_in_smp;

	
	atomic_long_t		mem ____cacheline_aligned_in_smp;
	struct work_struct	destroy_work;
	struct llist_node	free_list;
};


enum {
	INET_FRAG_FIRST_IN	= BIT(0),
	INET_FRAG_LAST_IN	= BIT(1),
	INET_FRAG_COMPLETE	= BIT(2),
	INET_FRAG_HASH_DEAD	= BIT(3),
	INET_FRAG_DROP		= BIT(4),
};

struct frag_v4_compare_key {
	__be32		saddr;
	__be32		daddr;
	u32		user;
	u32		vif;
	__be16		id;
	u16		protocol;
};

struct frag_v6_compare_key {
	struct in6_addr	saddr;
	struct in6_addr	daddr;
	u32		user;
	__be32		id;
	u32		iif;
};


struct inet_frag_queue {
	struct rhash_head	node;
	union {
		struct frag_v4_compare_key v4;
		struct frag_v6_compare_key v6;
	} key;
	struct timer_list	timer;
	spinlock_t		lock;
	refcount_t		refcnt;
	struct rb_root		rb_fragments;
	struct sk_buff		*fragments_tail;
	struct sk_buff		*last_run_head;
	ktime_t			stamp;
	int			len;
	int			meat;
	u8			mono_delivery_time;
	__u8			flags;
	u16			max_size;
	struct fqdir		*fqdir;
	struct rcu_head		rcu;
};

struct inet_frags {
	unsigned int		qsize;

	void			(*constructor)(struct inet_frag_queue *q,
					       const void *arg);
	void			(*destructor)(struct inet_frag_queue *);
	void			(*frag_expire)(struct timer_list *t);
	struct kmem_cache	*frags_cachep;
	const char		*frags_cache_name;
	struct rhashtable_params rhash_params;
	refcount_t		refcnt;
	struct completion	completion;
};

int inet_frags_init(struct inet_frags *);
void inet_frags_fini(struct inet_frags *);

int fqdir_init(struct fqdir **fqdirp, struct inet_frags *f, struct net *net);

static inline void fqdir_pre_exit(struct fqdir *fqdir)
{
	
	WRITE_ONCE(fqdir->high_thresh, 0);

	
	WRITE_ONCE(fqdir->dead, true);
}
void fqdir_exit(struct fqdir *fqdir);

void inet_frag_kill(struct inet_frag_queue *q);
void inet_frag_destroy(struct inet_frag_queue *q);
struct inet_frag_queue *inet_frag_find(struct fqdir *fqdir, void *key);


unsigned int inet_frag_rbtree_purge(struct rb_root *root,
				    enum skb_drop_reason reason);

static inline void inet_frag_put(struct inet_frag_queue *q)
{
	if (refcount_dec_and_test(&q->refcnt))
		inet_frag_destroy(q);
}



static inline long frag_mem_limit(const struct fqdir *fqdir)
{
	return atomic_long_read(&fqdir->mem);
}

static inline void sub_frag_mem_limit(struct fqdir *fqdir, long val)
{
	atomic_long_sub(val, &fqdir->mem);
}

static inline void add_frag_mem_limit(struct fqdir *fqdir, long val)
{
	atomic_long_add(val, &fqdir->mem);
}


#define	IPFRAG_ECN_NOT_ECT	0x01 
#define	IPFRAG_ECN_ECT_1	0x02 
#define	IPFRAG_ECN_ECT_0	0x04 
#define	IPFRAG_ECN_CE		0x08 

extern const u8 ip_frag_ecn_table[16];


#define IPFRAG_OK	0
#define IPFRAG_DUP	1
#define IPFRAG_OVERLAP	2
int inet_frag_queue_insert(struct inet_frag_queue *q, struct sk_buff *skb,
			   int offset, int end);
void *inet_frag_reasm_prepare(struct inet_frag_queue *q, struct sk_buff *skb,
			      struct sk_buff *parent);
void inet_frag_reasm_finish(struct inet_frag_queue *q, struct sk_buff *head,
			    void *reasm_data, bool try_coalesce);
struct sk_buff *inet_frag_pull_head(struct inet_frag_queue *q);

#endif
