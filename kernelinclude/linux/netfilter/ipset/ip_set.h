/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _IP_SET_H
#define _IP_SET_H

#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/x_tables.h>
#include <linux/stringify.h>
#include <linux/vmalloc.h>
#include <net/netlink.h>
#include <uapi/linux/netfilter/ipset/ip_set.h>

#define _IP_SET_MODULE_DESC(a, b, c)		\
	MODULE_DESCRIPTION(a " type of IP sets, revisions " b "-" c)
#define IP_SET_MODULE_DESC(a, b, c)		\
	_IP_SET_MODULE_DESC(a, __stringify(b), __stringify(c))


enum ip_set_feature {
	IPSET_TYPE_IP_FLAG = 0,
	IPSET_TYPE_IP = (1 << IPSET_TYPE_IP_FLAG),
	IPSET_TYPE_PORT_FLAG = 1,
	IPSET_TYPE_PORT = (1 << IPSET_TYPE_PORT_FLAG),
	IPSET_TYPE_MAC_FLAG = 2,
	IPSET_TYPE_MAC = (1 << IPSET_TYPE_MAC_FLAG),
	IPSET_TYPE_IP2_FLAG = 3,
	IPSET_TYPE_IP2 = (1 << IPSET_TYPE_IP2_FLAG),
	IPSET_TYPE_NAME_FLAG = 4,
	IPSET_TYPE_NAME = (1 << IPSET_TYPE_NAME_FLAG),
	IPSET_TYPE_IFACE_FLAG = 5,
	IPSET_TYPE_IFACE = (1 << IPSET_TYPE_IFACE_FLAG),
	IPSET_TYPE_MARK_FLAG = 6,
	IPSET_TYPE_MARK = (1 << IPSET_TYPE_MARK_FLAG),
	IPSET_TYPE_NOMATCH_FLAG = 7,
	IPSET_TYPE_NOMATCH = (1 << IPSET_TYPE_NOMATCH_FLAG),
	
	IPSET_DUMP_LAST_FLAG = 8,
	IPSET_DUMP_LAST = (1 << IPSET_DUMP_LAST_FLAG),
};


enum ip_set_extension {
	IPSET_EXT_BIT_TIMEOUT = 0,
	IPSET_EXT_TIMEOUT = (1 << IPSET_EXT_BIT_TIMEOUT),
	IPSET_EXT_BIT_COUNTER = 1,
	IPSET_EXT_COUNTER = (1 << IPSET_EXT_BIT_COUNTER),
	IPSET_EXT_BIT_COMMENT = 2,
	IPSET_EXT_COMMENT = (1 << IPSET_EXT_BIT_COMMENT),
	IPSET_EXT_BIT_SKBINFO = 3,
	IPSET_EXT_SKBINFO = (1 << IPSET_EXT_BIT_SKBINFO),
	
	IPSET_EXT_BIT_DESTROY = 7,
	IPSET_EXT_DESTROY = (1 << IPSET_EXT_BIT_DESTROY),
};

#define SET_WITH_TIMEOUT(s)	((s)->extensions & IPSET_EXT_TIMEOUT)
#define SET_WITH_COUNTER(s)	((s)->extensions & IPSET_EXT_COUNTER)
#define SET_WITH_COMMENT(s)	((s)->extensions & IPSET_EXT_COMMENT)
#define SET_WITH_SKBINFO(s)	((s)->extensions & IPSET_EXT_SKBINFO)
#define SET_WITH_FORCEADD(s)	((s)->flags & IPSET_CREATE_FLAG_FORCEADD)


enum ip_set_ext_id {
	IPSET_EXT_ID_COUNTER = 0,
	IPSET_EXT_ID_TIMEOUT,
	IPSET_EXT_ID_SKBINFO,
	IPSET_EXT_ID_COMMENT,
	IPSET_EXT_ID_MAX,
};

struct ip_set;


struct ip_set_ext_type {
	
	void (*destroy)(struct ip_set *set, void *ext);
	enum ip_set_extension type;
	enum ipset_cadt_flags flag;
	
	u8 len;
	u8 align;
};

extern const struct ip_set_ext_type ip_set_extensions[];

struct ip_set_counter {
	atomic64_t bytes;
	atomic64_t packets;
};

struct ip_set_comment_rcu {
	struct rcu_head rcu;
	char str[];
};

struct ip_set_comment {
	struct ip_set_comment_rcu __rcu *c;
};

struct ip_set_skbinfo {
	u32 skbmark;
	u32 skbmarkmask;
	u32 skbprio;
	u16 skbqueue;
	u16 __pad;
};

struct ip_set_ext {
	struct ip_set_skbinfo skbinfo;
	u64 packets;
	u64 bytes;
	char *comment;
	u32 timeout;
	u8 packets_op;
	u8 bytes_op;
	bool target;
};

#define ext_timeout(e, s)	\
((unsigned long *)(((void *)(e)) + (s)->offset[IPSET_EXT_ID_TIMEOUT]))
#define ext_counter(e, s)	\
((struct ip_set_counter *)(((void *)(e)) + (s)->offset[IPSET_EXT_ID_COUNTER]))
#define ext_comment(e, s)	\
((struct ip_set_comment *)(((void *)(e)) + (s)->offset[IPSET_EXT_ID_COMMENT]))
#define ext_skbinfo(e, s)	\
((struct ip_set_skbinfo *)(((void *)(e)) + (s)->offset[IPSET_EXT_ID_SKBINFO]))

typedef int (*ipset_adtfn)(struct ip_set *set, void *value,
			   const struct ip_set_ext *ext,
			   struct ip_set_ext *mext, u32 cmdflags);


struct ip_set_adt_opt {
	u8 family;		
	u8 dim;			
	u8 flags;		
	u32 cmdflags;		
	struct ip_set_ext ext;	
};


struct ip_set_type_variant {
	
	int (*kadt)(struct ip_set *set, const struct sk_buff *skb,
		    const struct xt_action_param *par,
		    enum ipset_adt adt, struct ip_set_adt_opt *opt);

	
	int (*uadt)(struct ip_set *set, struct nlattr *tb[],
		    enum ipset_adt adt, u32 *lineno, u32 flags, bool retried);

	
	ipset_adtfn adt[IPSET_ADT_MAX];

	
	int (*resize)(struct ip_set *set, bool retried);
	
	void (*destroy)(struct ip_set *set);
	
	void (*flush)(struct ip_set *set);
	
	void (*expire)(struct ip_set *set);
	
	int (*head)(struct ip_set *set, struct sk_buff *skb);
	
	int (*list)(const struct ip_set *set, struct sk_buff *skb,
		    struct netlink_callback *cb);
	
	void (*uref)(struct ip_set *set, struct netlink_callback *cb,
		     bool start);

	
	bool (*same_set)(const struct ip_set *a, const struct ip_set *b);
	
	void (*cancel_gc)(struct ip_set *set);
	
	bool region_lock;
};

struct ip_set_region {
	spinlock_t lock;	
	size_t ext_size;	
	u32 elements;		
};


#define IPSET_MAX_RANGE		(1<<14)


#define IPSET_REVISION_MAX	9


struct ip_set_type {
	struct list_head list;

	
	char name[IPSET_MAXNAMELEN];
	
	u8 protocol;
	
	u8 dimension;
	
	u8 family;
	
	u8 revision_min, revision_max;
	
	u8 create_flags[IPSET_REVISION_MAX+1];
	
	u16 features;

	
	int (*create)(struct net *net, struct ip_set *set,
		      struct nlattr *tb[], u32 flags);

	
	const struct nla_policy create_policy[IPSET_ATTR_CREATE_MAX + 1];
	const struct nla_policy adt_policy[IPSET_ATTR_ADT_MAX + 1];

	
	struct module *me;
};


extern int ip_set_type_register(struct ip_set_type *set_type);
extern void ip_set_type_unregister(struct ip_set_type *set_type);


struct ip_set {
	
	struct rcu_head rcu;
	
	char name[IPSET_MAXNAMELEN];
	
	spinlock_t lock;
	
	u32 ref;
	
	u32 ref_netlink;
	
	struct ip_set_type *type;
	
	const struct ip_set_type_variant *variant;
	
	u8 family;
	
	u8 revision;
	
	u8 extensions;
	
	u8 flags;
	
	u32 timeout;
	
	u32 elements;
	
	size_t ext_size;
	
	size_t dsize;
	
	size_t offset[IPSET_EXT_ID_MAX];
	
	void *data;
};

static inline void
ip_set_ext_destroy(struct ip_set *set, void *data)
{
	
	if (SET_WITH_COMMENT(set)) {
		struct ip_set_comment *c = ext_comment(data, set);

		ip_set_extensions[IPSET_EXT_ID_COMMENT].destroy(set, c);
	}
}

int ip_set_put_flags(struct sk_buff *skb, struct ip_set *set);


enum {
	IPSET_CB_NET = 0,	
	IPSET_CB_PROTO,		
	IPSET_CB_DUMP,		
	IPSET_CB_INDEX,		
	IPSET_CB_PRIVATE,	
	IPSET_CB_ARG0,		
};


extern ip_set_id_t ip_set_get_byname(struct net *net,
				     const char *name, struct ip_set **set);
extern void ip_set_put_byindex(struct net *net, ip_set_id_t index);
extern void ip_set_name_byindex(struct net *net, ip_set_id_t index, char *name);
extern ip_set_id_t ip_set_nfnl_get_byindex(struct net *net, ip_set_id_t index);
extern void ip_set_nfnl_put(struct net *net, ip_set_id_t index);



extern int ip_set_add(ip_set_id_t id, const struct sk_buff *skb,
		      const struct xt_action_param *par,
		      struct ip_set_adt_opt *opt);
extern int ip_set_del(ip_set_id_t id, const struct sk_buff *skb,
		      const struct xt_action_param *par,
		      struct ip_set_adt_opt *opt);
extern int ip_set_test(ip_set_id_t id, const struct sk_buff *skb,
		       const struct xt_action_param *par,
		       struct ip_set_adt_opt *opt);


extern void *ip_set_alloc(size_t size);
extern void ip_set_free(void *members);
extern int ip_set_get_ipaddr4(struct nlattr *nla,  __be32 *ipaddr);
extern int ip_set_get_ipaddr6(struct nlattr *nla, union nf_inet_addr *ipaddr);
extern size_t ip_set_elem_len(struct ip_set *set, struct nlattr *tb[],
			      size_t len, size_t align);
extern int ip_set_get_extensions(struct ip_set *set, struct nlattr *tb[],
				 struct ip_set_ext *ext);
extern int ip_set_put_extensions(struct sk_buff *skb, const struct ip_set *set,
				 const void *e, bool active);
extern bool ip_set_match_extensions(struct ip_set *set,
				    const struct ip_set_ext *ext,
				    struct ip_set_ext *mext,
				    u32 flags, void *data);

static inline int
ip_set_get_hostipaddr4(struct nlattr *nla, u32 *ipaddr)
{
	__be32 ip;
	int ret = ip_set_get_ipaddr4(nla, &ip);

	if (ret)
		return ret;
	*ipaddr = ntohl(ip);
	return 0;
}


static inline bool
ip_set_eexist(int ret, u32 flags)
{
	return ret == -IPSET_ERR_EXIST && (flags & IPSET_FLAG_EXIST);
}


static inline bool
ip_set_enomatch(int ret, u32 flags, enum ipset_adt adt, struct ip_set *set)
{
	return adt == IPSET_TEST &&
	       (set->type->features & IPSET_TYPE_NOMATCH) &&
	       ((flags >> 16) & IPSET_FLAG_NOMATCH) &&
	       (ret > 0 || ret == -ENOTEMPTY);
}


static inline bool
ip_set_attr_netorder(struct nlattr *tb[], int type)
{
	return tb[type] && (tb[type]->nla_type & NLA_F_NET_BYTEORDER);
}

static inline bool
ip_set_optattr_netorder(struct nlattr *tb[], int type)
{
	return !tb[type] || (tb[type]->nla_type & NLA_F_NET_BYTEORDER);
}


static inline u32
ip_set_get_h32(const struct nlattr *attr)
{
	return ntohl(nla_get_be32(attr));
}

static inline u16
ip_set_get_h16(const struct nlattr *attr)
{
	return ntohs(nla_get_be16(attr));
}

static inline int nla_put_ipaddr4(struct sk_buff *skb, int type, __be32 ipaddr)
{
	struct nlattr *__nested = nla_nest_start(skb, type);
	int ret;

	if (!__nested)
		return -EMSGSIZE;
	ret = nla_put_in_addr(skb, IPSET_ATTR_IPADDR_IPV4, ipaddr);
	if (!ret)
		nla_nest_end(skb, __nested);
	return ret;
}

static inline int nla_put_ipaddr6(struct sk_buff *skb, int type,
				  const struct in6_addr *ipaddrptr)
{
	struct nlattr *__nested = nla_nest_start(skb, type);
	int ret;

	if (!__nested)
		return -EMSGSIZE;
	ret = nla_put_in6_addr(skb, IPSET_ATTR_IPADDR_IPV6, ipaddrptr);
	if (!ret)
		nla_nest_end(skb, __nested);
	return ret;
}


static inline __be32
ip4addr(const struct sk_buff *skb, bool src)
{
	return src ? ip_hdr(skb)->saddr : ip_hdr(skb)->daddr;
}

static inline void
ip4addrptr(const struct sk_buff *skb, bool src, __be32 *addr)
{
	*addr = src ? ip_hdr(skb)->saddr : ip_hdr(skb)->daddr;
}

static inline void
ip6addrptr(const struct sk_buff *skb, bool src, struct in6_addr *addr)
{
	memcpy(addr, src ? &ipv6_hdr(skb)->saddr : &ipv6_hdr(skb)->daddr,
	       sizeof(*addr));
}


#define IPSET_GC_TIME			(3 * 60)


#define IPSET_GC_PERIOD(timeout) \
	((timeout/3) ? min_t(u32, (timeout)/3, IPSET_GC_TIME) : 1)


#define IPSET_ELEM_PERMANENT	0


#define IPSET_NO_TIMEOUT	UINT_MAX


#define IPSET_MAX_TIMEOUT	(UINT_MAX >> 1)/MSEC_PER_SEC

#define ip_set_adt_opt_timeout(opt, set)	\
((opt)->ext.timeout != IPSET_NO_TIMEOUT ? (opt)->ext.timeout : (set)->timeout)

static inline unsigned int
ip_set_timeout_uget(struct nlattr *tb)
{
	unsigned int timeout = ip_set_get_h32(tb);

	
	if (timeout > IPSET_MAX_TIMEOUT)
		timeout = IPSET_MAX_TIMEOUT;

	return timeout;
}

static inline bool
ip_set_timeout_expired(const unsigned long *t)
{
	return *t != IPSET_ELEM_PERMANENT && time_is_before_jiffies(*t);
}

static inline void
ip_set_timeout_set(unsigned long *timeout, u32 value)
{
	unsigned long t;

	if (!value) {
		*timeout = IPSET_ELEM_PERMANENT;
		return;
	}

	t = msecs_to_jiffies(value * MSEC_PER_SEC) + jiffies;
	if (t == IPSET_ELEM_PERMANENT)
		
		t--;
	*timeout = t;
}

void ip_set_init_comment(struct ip_set *set, struct ip_set_comment *comment,
			 const struct ip_set_ext *ext);

static inline void
ip_set_init_counter(struct ip_set_counter *counter,
		    const struct ip_set_ext *ext)
{
	if (ext->bytes != ULLONG_MAX)
		atomic64_set(&(counter)->bytes, (long long)(ext->bytes));
	if (ext->packets != ULLONG_MAX)
		atomic64_set(&(counter)->packets, (long long)(ext->packets));
}

static inline void
ip_set_init_skbinfo(struct ip_set_skbinfo *skbinfo,
		    const struct ip_set_ext *ext)
{
	*skbinfo = ext->skbinfo;
}

static inline void
nf_inet_addr_mask_inplace(union nf_inet_addr *a1,
			  const union nf_inet_addr *mask)
{
	a1->all[0] &= mask->all[0];
	a1->all[1] &= mask->all[1];
	a1->all[2] &= mask->all[2];
	a1->all[3] &= mask->all[3];
}

#define IP_SET_INIT_KEXT(skb, opt, set)			\
	{ .bytes = (skb)->len, .packets = 1, .target = true,\
	  .timeout = ip_set_adt_opt_timeout(opt, set) }

#define IP_SET_INIT_UEXT(set)				\
	{ .bytes = ULLONG_MAX, .packets = ULLONG_MAX,	\
	  .timeout = (set)->timeout }

#define IPSET_CONCAT(a, b)		a##b
#define IPSET_TOKEN(a, b)		IPSET_CONCAT(a, b)

#endif 
