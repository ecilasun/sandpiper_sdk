/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_SKBUFF_H
#define _LINUX_SKBUFF_H

#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/bvec.h>
#include <linux/cache.h>
#include <linux/rbtree.h>
#include <linux/socket.h>
#include <linux/refcount.h>

#include <linux/atomic.h>
#include <asm/types.h>
#include <linux/spinlock.h>
#include <net/checksum.h>
#include <linux/rcupdate.h>
#include <linux/dma-mapping.h>
#include <linux/netdev_features.h>
#include <net/flow_dissector.h>
#include <linux/in6.h>
#include <linux/if_packet.h>
#include <linux/llist.h>
#include <net/flow.h>
#if IS_ENABLED(CONFIG_NF_CONNTRACK)
#include <linux/netfilter/nf_conntrack_common.h>
#endif
#include <net/net_debug.h>
#include <net/dropreason-core.h>




#define CHECKSUM_NONE		0
#define CHECKSUM_UNNECESSARY	1
#define CHECKSUM_COMPLETE	2
#define CHECKSUM_PARTIAL	3


#define SKB_MAX_CSUM_LEVEL	3

#define SKB_DATA_ALIGN(X)	ALIGN(X, SMP_CACHE_BYTES)
#define SKB_WITH_OVERHEAD(X)	\
	((X) - SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))


#define SKB_HEAD_ALIGN(X) (SKB_DATA_ALIGN(X) + \
	SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

#define SKB_MAX_ORDER(X, ORDER) \
	SKB_WITH_OVERHEAD((PAGE_SIZE << (ORDER)) - (X))
#define SKB_MAX_HEAD(X)		(SKB_MAX_ORDER((X), 0))
#define SKB_MAX_ALLOC		(SKB_MAX_ORDER(0, 2))


#define SKB_TRUESIZE(X) ((X) +						\
			 SKB_DATA_ALIGN(sizeof(struct sk_buff)) +	\
			 SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

struct ahash_request;
struct net_device;
struct scatterlist;
struct pipe_inode_info;
struct iov_iter;
struct napi_struct;
struct bpf_prog;
union bpf_attr;
struct skb_ext;
struct ts_config;

#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
struct nf_bridge_info {
	enum {
		BRNF_PROTO_UNCHANGED,
		BRNF_PROTO_8021Q,
		BRNF_PROTO_PPPOE
	} orig_proto:8;
	u8			pkt_otherhost:1;
	u8			in_prerouting:1;
	u8			bridged_dnat:1;
	u8			sabotage_in_done:1;
	__u16			frag_max_size;
	int			physinif;

	
	struct net_device	*physoutdev;
	union {
		
		__be32          ipv4_daddr;
		struct in6_addr ipv6_daddr;

		
		char neigh_header[8];
	};
};
#endif

#if IS_ENABLED(CONFIG_NET_TC_SKB_EXT)

struct tc_skb_ext {
	union {
		u64 act_miss_cookie;
		__u32 chain;
	};
	__u16 mru;
	__u16 zone;
	u8 post_ct:1;
	u8 post_ct_snat:1;
	u8 post_ct_dnat:1;
	u8 act_miss:1; 
	u8 l2_miss:1; 
};
#endif

struct sk_buff_head {
	
	struct_group_tagged(sk_buff_list, list,
		struct sk_buff	*next;
		struct sk_buff	*prev;
	);

	__u32		qlen;
	spinlock_t	lock;
};

struct sk_buff;

#ifndef CONFIG_MAX_SKB_FRAGS
# define CONFIG_MAX_SKB_FRAGS 17
#endif

#define MAX_SKB_FRAGS CONFIG_MAX_SKB_FRAGS

extern int sysctl_max_skb_frags;


#define GSO_BY_FRAGS	0xFFFF

typedef struct bio_vec skb_frag_t;


static inline unsigned int skb_frag_size(const skb_frag_t *frag)
{
	return frag->bv_len;
}


static inline void skb_frag_size_set(skb_frag_t *frag, unsigned int size)
{
	frag->bv_len = size;
}


static inline void skb_frag_size_add(skb_frag_t *frag, int delta)
{
	frag->bv_len += delta;
}


static inline void skb_frag_size_sub(skb_frag_t *frag, int delta)
{
	frag->bv_len -= delta;
}


static inline bool skb_frag_must_loop(struct page *p)
{
#if defined(CONFIG_HIGHMEM)
	if (IS_ENABLED(CONFIG_DEBUG_KMAP_LOCAL_FORCE_MAP) || PageHighMem(p))
		return true;
#endif
	return false;
}


#define skb_frag_foreach_page(f, f_off, f_len, p, p_off, p_len, copied)	\
	for (p = skb_frag_page(f) + ((f_off) >> PAGE_SHIFT),		\
	     p_off = (f_off) & (PAGE_SIZE - 1),				\
	     p_len = skb_frag_must_loop(p) ?				\
	     min_t(u32, f_len, PAGE_SIZE - p_off) : f_len,		\
	     copied = 0;						\
	     copied < f_len;						\
	     copied += p_len, p++, p_off = 0,				\
	     p_len = min_t(u32, f_len - copied, PAGE_SIZE))		\


struct skb_shared_hwtstamps {
	union {
		ktime_t	hwtstamp;
		void *netdev_data;
	};
};


enum {
	
	SKBTX_HW_TSTAMP = 1 << 0,

	
	SKBTX_SW_TSTAMP = 1 << 1,

	
	SKBTX_IN_PROGRESS = 1 << 2,

	
	SKBTX_HW_TSTAMP_USE_CYCLES = 1 << 3,

	
	SKBTX_WIFI_STATUS = 1 << 4,

	
	SKBTX_HW_TSTAMP_NETDEV = 1 << 5,

	
	SKBTX_SCHED_TSTAMP = 1 << 6,
};

#define SKBTX_ANY_SW_TSTAMP	(SKBTX_SW_TSTAMP    | \
				 SKBTX_SCHED_TSTAMP)
#define SKBTX_ANY_TSTAMP	(SKBTX_HW_TSTAMP | \
				 SKBTX_HW_TSTAMP_USE_CYCLES | \
				 SKBTX_ANY_SW_TSTAMP)


enum {
	
	SKBFL_ZEROCOPY_ENABLE = BIT(0),

	
	SKBFL_SHARED_FRAG = BIT(1),

	
	SKBFL_PURE_ZEROCOPY = BIT(2),

	SKBFL_DONT_ORPHAN = BIT(3),

	
	SKBFL_MANAGED_FRAG_REFS = BIT(4),
};

#define SKBFL_ZEROCOPY_FRAG	(SKBFL_ZEROCOPY_ENABLE | SKBFL_SHARED_FRAG)
#define SKBFL_ALL_ZEROCOPY	(SKBFL_ZEROCOPY_FRAG | SKBFL_PURE_ZEROCOPY | \
				 SKBFL_DONT_ORPHAN | SKBFL_MANAGED_FRAG_REFS)


struct ubuf_info {
	void (*callback)(struct sk_buff *, struct ubuf_info *,
			 bool zerocopy_success);
	refcount_t refcnt;
	u8 flags;
};

struct ubuf_info_msgzc {
	struct ubuf_info ubuf;

	union {
		struct {
			unsigned long desc;
			void *ctx;
		};
		struct {
			u32 id;
			u16 len;
			u16 zerocopy:1;
			u32 bytelen;
		};
	};

	struct mmpin {
		struct user_struct *user;
		unsigned int num_pg;
	} mmp;
};

#define skb_uarg(SKB)	((struct ubuf_info *)(skb_shinfo(SKB)->destructor_arg))
#define uarg_to_msgzc(ubuf_ptr)	container_of((ubuf_ptr), struct ubuf_info_msgzc, \
					     ubuf)

int mm_account_pinned_pages(struct mmpin *mmp, size_t size);
void mm_unaccount_pinned_pages(struct mmpin *mmp);


struct skb_shared_info {
	__u8		flags;
	__u8		meta_len;
	__u8		nr_frags;
	__u8		tx_flags;
	unsigned short	gso_size;
	
	unsigned short	gso_segs;
	struct sk_buff	*frag_list;
	struct skb_shared_hwtstamps hwtstamps;
	unsigned int	gso_type;
	u32		tskey;

	
	atomic_t	dataref;
	unsigned int	xdp_frags_size;

	
	void *		destructor_arg;

	
	skb_frag_t	frags[MAX_SKB_FRAGS];
};


#define SKB_DATAREF_SHIFT 16
#define SKB_DATAREF_MASK ((1 << SKB_DATAREF_SHIFT) - 1)


enum {
	SKB_FCLONE_UNAVAILABLE,	
	SKB_FCLONE_ORIG,	
	SKB_FCLONE_CLONE,	
};

enum {
	SKB_GSO_TCPV4 = 1 << 0,

	
	SKB_GSO_DODGY = 1 << 1,

	
	SKB_GSO_TCP_ECN = 1 << 2,

	SKB_GSO_TCP_FIXEDID = 1 << 3,

	SKB_GSO_TCPV6 = 1 << 4,

	SKB_GSO_FCOE = 1 << 5,

	SKB_GSO_GRE = 1 << 6,

	SKB_GSO_GRE_CSUM = 1 << 7,

	SKB_GSO_IPXIP4 = 1 << 8,

	SKB_GSO_IPXIP6 = 1 << 9,

	SKB_GSO_UDP_TUNNEL = 1 << 10,

	SKB_GSO_UDP_TUNNEL_CSUM = 1 << 11,

	SKB_GSO_PARTIAL = 1 << 12,

	SKB_GSO_TUNNEL_REMCSUM = 1 << 13,

	SKB_GSO_SCTP = 1 << 14,

	SKB_GSO_ESP = 1 << 15,

	SKB_GSO_UDP = 1 << 16,

	SKB_GSO_UDP_L4 = 1 << 17,

	SKB_GSO_FRAGLIST = 1 << 18,
};

#if BITS_PER_LONG > 32
#define NET_SKBUFF_DATA_USES_OFFSET 1
#endif

#ifdef NET_SKBUFF_DATA_USES_OFFSET
typedef unsigned int sk_buff_data_t;
#else
typedef unsigned char *sk_buff_data_t;
#endif





struct sk_buff {
	union {
		struct {
			
			struct sk_buff		*next;
			struct sk_buff		*prev;

			union {
				struct net_device	*dev;
				
				unsigned long		dev_scratch;
			};
		};
		struct rb_node		rbnode; 
		struct list_head	list;
		struct llist_node	ll_node;
	};

	struct sock		*sk;

	union {
		ktime_t		tstamp;
		u64		skb_mstamp_ns; 
	};
	
	char			cb[48] __aligned(8);

	union {
		struct {
			unsigned long	_skb_refdst;
			void		(*destructor)(struct sk_buff *skb);
		};
		struct list_head	tcp_tsorted_anchor;
#ifdef CONFIG_NET_SOCK_MSG
		unsigned long		_sk_redir;
#endif
	};

#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	unsigned long		 _nfct;
#endif
	unsigned int		len,
				data_len;
	__u16			mac_len,
				hdr_len;

	
	__u16			queue_mapping;


#ifdef __BIG_ENDIAN_BITFIELD
#define CLONED_MASK	(1 << 7)
#else
#define CLONED_MASK	1
#endif
#define CLONED_OFFSET		offsetof(struct sk_buff, __cloned_offset)

	
	__u8			__cloned_offset[0];
	
	__u8			cloned:1,
				nohdr:1,
				fclone:2,
				peeked:1,
				head_frag:1,
				pfmemalloc:1,
				pp_recycle:1; 
#ifdef CONFIG_SKB_EXTENSIONS
	__u8			active_extensions;
#endif

	
	struct_group(headers,

	
	__u8			__pkt_type_offset[0];
	
	__u8			pkt_type:3; 
	__u8			ignore_df:1;
	__u8			dst_pending_confirm:1;
	__u8			ip_summed:2;
	__u8			ooo_okay:1;

	
	__u8			__mono_tc_offset[0];
	
	__u8			mono_delivery_time:1;	
#ifdef CONFIG_NET_XGRESS
	__u8			tc_at_ingress:1;	
	__u8			tc_skip_classify:1;
#endif
	__u8			remcsum_offload:1;
	__u8			csum_complete_sw:1;
	__u8			csum_level:2;
	__u8			inner_protocol_type:1;

	__u8			l4_hash:1;
	__u8			sw_hash:1;
#ifdef CONFIG_WIRELESS
	__u8			wifi_acked_valid:1;
	__u8			wifi_acked:1;
#endif
	__u8			no_fcs:1;
	
	__u8			encapsulation:1;
	__u8			encap_hdr_csum:1;
	__u8			csum_valid:1;
#ifdef CONFIG_IPV6_NDISC_NODETYPE
	__u8			ndisc_nodetype:2;
#endif

#if IS_ENABLED(CONFIG_IP_VS)
	__u8			ipvs_property:1;
#endif
#if IS_ENABLED(CONFIG_NETFILTER_XT_TARGET_TRACE) || IS_ENABLED(CONFIG_NF_TABLES)
	__u8			nf_trace:1;
#endif
#ifdef CONFIG_NET_SWITCHDEV
	__u8			offload_fwd_mark:1;
	__u8			offload_l3_fwd_mark:1;
#endif
	__u8			redirected:1;
#ifdef CONFIG_NET_REDIRECT
	__u8			from_ingress:1;
#endif
#ifdef CONFIG_NETFILTER_SKIP_EGRESS
	__u8			nf_skip_egress:1;
#endif
#ifdef CONFIG_TLS_DEVICE
	__u8			decrypted:1;
#endif
	__u8			slow_gro:1;
#if IS_ENABLED(CONFIG_IP_SCTP)
	__u8			csum_not_inet:1;
#endif

#if defined(CONFIG_NET_SCHED) || defined(CONFIG_NET_XGRESS)
	__u16			tc_index;	
#endif

	u16			alloc_cpu;

	union {
		__wsum		csum;
		struct {
			__u16	csum_start;
			__u16	csum_offset;
		};
	};
	__u32			priority;
	int			skb_iif;
	__u32			hash;
	union {
		u32		vlan_all;
		struct {
			__be16	vlan_proto;
			__u16	vlan_tci;
		};
	};
#if defined(CONFIG_NET_RX_BUSY_POLL) || defined(CONFIG_XPS)
	union {
		unsigned int	napi_id;
		unsigned int	sender_cpu;
	};
#endif
#ifdef CONFIG_NETWORK_SECMARK
	__u32		secmark;
#endif

	union {
		__u32		mark;
		__u32		reserved_tailroom;
	};

	union {
		__be16		inner_protocol;
		__u8		inner_ipproto;
	};

	__u16			inner_transport_header;
	__u16			inner_network_header;
	__u16			inner_mac_header;

	__be16			protocol;
	__u16			transport_header;
	__u16			network_header;
	__u16			mac_header;

#ifdef CONFIG_KCOV
	u64			kcov_handle;
#endif

	); 

	
	sk_buff_data_t		tail;
	sk_buff_data_t		end;
	unsigned char		*head,
				*data;
	unsigned int		truesize;
	refcount_t		users;

#ifdef CONFIG_SKB_EXTENSIONS
	
	struct skb_ext		*extensions;
#endif
};


#ifdef __BIG_ENDIAN_BITFIELD
#define PKT_TYPE_MAX	(7 << 5)
#else
#define PKT_TYPE_MAX	7
#endif
#define PKT_TYPE_OFFSET		offsetof(struct sk_buff, __pkt_type_offset)


#ifdef __BIG_ENDIAN_BITFIELD
#define SKB_MONO_DELIVERY_TIME_MASK	(1 << 7)
#define TC_AT_INGRESS_MASK		(1 << 6)
#else
#define SKB_MONO_DELIVERY_TIME_MASK	(1 << 0)
#define TC_AT_INGRESS_MASK		(1 << 1)
#endif
#define SKB_BF_MONO_TC_OFFSET		offsetof(struct sk_buff, __mono_tc_offset)

#ifdef __KERNEL__


#define SKB_ALLOC_FCLONE	0x01
#define SKB_ALLOC_RX		0x02
#define SKB_ALLOC_NAPI		0x04


static inline bool skb_pfmemalloc(const struct sk_buff *skb)
{
	return unlikely(skb->pfmemalloc);
}


#define SKB_DST_NOREF	1UL
#define SKB_DST_PTRMASK	~(SKB_DST_NOREF)


static inline struct dst_entry *skb_dst(const struct sk_buff *skb)
{
	
	WARN_ON((skb->_skb_refdst & SKB_DST_NOREF) &&
		!rcu_read_lock_held() &&
		!rcu_read_lock_bh_held());
	return (struct dst_entry *)(skb->_skb_refdst & SKB_DST_PTRMASK);
}


static inline void skb_dst_set(struct sk_buff *skb, struct dst_entry *dst)
{
	skb->slow_gro |= !!dst;
	skb->_skb_refdst = (unsigned long)dst;
}


static inline void skb_dst_set_noref(struct sk_buff *skb, struct dst_entry *dst)
{
	WARN_ON(!rcu_read_lock_held() && !rcu_read_lock_bh_held());
	skb->slow_gro |= !!dst;
	skb->_skb_refdst = (unsigned long)dst | SKB_DST_NOREF;
}


static inline bool skb_dst_is_noref(const struct sk_buff *skb)
{
	return (skb->_skb_refdst & SKB_DST_NOREF) && skb_dst(skb);
}


static inline struct rtable *skb_rtable(const struct sk_buff *skb)
{
	return (struct rtable *)skb_dst(skb);
}


static inline bool skb_pkt_type_ok(u32 ptype)
{
	return ptype <= PACKET_OTHERHOST;
}


static inline unsigned int skb_napi_id(const struct sk_buff *skb)
{
#ifdef CONFIG_NET_RX_BUSY_POLL
	return skb->napi_id;
#else
	return 0;
#endif
}

static inline bool skb_wifi_acked_valid(const struct sk_buff *skb)
{
#ifdef CONFIG_WIRELESS
	return skb->wifi_acked_valid;
#else
	return 0;
#endif
}


static inline bool skb_unref(struct sk_buff *skb)
{
	if (unlikely(!skb))
		return false;
	if (likely(refcount_read(&skb->users) == 1))
		smp_rmb();
	else if (likely(!refcount_dec_and_test(&skb->users)))
		return false;

	return true;
}

void __fix_address
kfree_skb_reason(struct sk_buff *skb, enum skb_drop_reason reason);


static inline void kfree_skb(struct sk_buff *skb)
{
	kfree_skb_reason(skb, SKB_DROP_REASON_NOT_SPECIFIED);
}

void skb_release_head_state(struct sk_buff *skb);
void kfree_skb_list_reason(struct sk_buff *segs,
			   enum skb_drop_reason reason);
void skb_dump(const char *level, const struct sk_buff *skb, bool full_pkt);
void skb_tx_error(struct sk_buff *skb);

static inline void kfree_skb_list(struct sk_buff *segs)
{
	kfree_skb_list_reason(segs, SKB_DROP_REASON_NOT_SPECIFIED);
}

#ifdef CONFIG_TRACEPOINTS
void consume_skb(struct sk_buff *skb);
#else
static inline void consume_skb(struct sk_buff *skb)
{
	return kfree_skb(skb);
}
#endif

void __consume_stateless_skb(struct sk_buff *skb);
void  __kfree_skb(struct sk_buff *skb);
extern struct kmem_cache *skbuff_cache;

void kfree_skb_partial(struct sk_buff *skb, bool head_stolen);
bool skb_try_coalesce(struct sk_buff *to, struct sk_buff *from,
		      bool *fragstolen, int *delta_truesize);

struct sk_buff *__alloc_skb(unsigned int size, gfp_t priority, int flags,
			    int node);
struct sk_buff *__build_skb(void *data, unsigned int frag_size);
struct sk_buff *build_skb(void *data, unsigned int frag_size);
struct sk_buff *build_skb_around(struct sk_buff *skb,
				 void *data, unsigned int frag_size);
void skb_attempt_defer_free(struct sk_buff *skb);

struct sk_buff *napi_build_skb(void *data, unsigned int frag_size);
struct sk_buff *slab_build_skb(void *data);


static inline struct sk_buff *alloc_skb(unsigned int size,
					gfp_t priority)
{
	return __alloc_skb(size, priority, 0, NUMA_NO_NODE);
}

struct sk_buff *alloc_skb_with_frags(unsigned long header_len,
				     unsigned long data_len,
				     int max_page_order,
				     int *errcode,
				     gfp_t gfp_mask);
struct sk_buff *alloc_skb_for_msg(struct sk_buff *first);


struct sk_buff_fclones {
	struct sk_buff	skb1;

	struct sk_buff	skb2;

	refcount_t	fclone_ref;
};


static inline bool skb_fclone_busy(const struct sock *sk,
				   const struct sk_buff *skb)
{
	const struct sk_buff_fclones *fclones;

	fclones = container_of(skb, struct sk_buff_fclones, skb1);

	return skb->fclone == SKB_FCLONE_ORIG &&
	       refcount_read(&fclones->fclone_ref) > 1 &&
	       READ_ONCE(fclones->skb2.sk) == sk;
}


static inline struct sk_buff *alloc_skb_fclone(unsigned int size,
					       gfp_t priority)
{
	return __alloc_skb(size, priority, SKB_ALLOC_FCLONE, NUMA_NO_NODE);
}

struct sk_buff *skb_morph(struct sk_buff *dst, struct sk_buff *src);
void skb_headers_offset_update(struct sk_buff *skb, int off);
int skb_copy_ubufs(struct sk_buff *skb, gfp_t gfp_mask);
struct sk_buff *skb_clone(struct sk_buff *skb, gfp_t priority);
void skb_copy_header(struct sk_buff *new, const struct sk_buff *old);
struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t priority);
struct sk_buff *__pskb_copy_fclone(struct sk_buff *skb, int headroom,
				   gfp_t gfp_mask, bool fclone);
static inline struct sk_buff *__pskb_copy(struct sk_buff *skb, int headroom,
					  gfp_t gfp_mask)
{
	return __pskb_copy_fclone(skb, headroom, gfp_mask, false);
}

int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail, gfp_t gfp_mask);
struct sk_buff *skb_realloc_headroom(struct sk_buff *skb,
				     unsigned int headroom);
struct sk_buff *skb_expand_head(struct sk_buff *skb, unsigned int headroom);
struct sk_buff *skb_copy_expand(const struct sk_buff *skb, int newheadroom,
				int newtailroom, gfp_t priority);
int __must_check skb_to_sgvec_nomark(struct sk_buff *skb, struct scatterlist *sg,
				     int offset, int len);
int __must_check skb_to_sgvec(struct sk_buff *skb, struct scatterlist *sg,
			      int offset, int len);
int skb_cow_data(struct sk_buff *skb, int tailbits, struct sk_buff **trailer);
int __skb_pad(struct sk_buff *skb, int pad, bool free_on_error);


static inline int skb_pad(struct sk_buff *skb, int pad)
{
	return __skb_pad(skb, pad, true);
}
#define dev_kfree_skb(a)	consume_skb(a)

int skb_append_pagefrags(struct sk_buff *skb, struct page *page,
			 int offset, size_t size, size_t max_frags);

struct skb_seq_state {
	__u32		lower_offset;
	__u32		upper_offset;
	__u32		frag_idx;
	__u32		stepped_offset;
	struct sk_buff	*root_skb;
	struct sk_buff	*cur_skb;
	__u8		*frag_data;
	__u32		frag_off;
};

void skb_prepare_seq_read(struct sk_buff *skb, unsigned int from,
			  unsigned int to, struct skb_seq_state *st);
unsigned int skb_seq_read(unsigned int consumed, const u8 **data,
			  struct skb_seq_state *st);
void skb_abort_seq_read(struct skb_seq_state *st);

unsigned int skb_find_text(struct sk_buff *skb, unsigned int from,
			   unsigned int to, struct ts_config *config);


enum pkt_hash_types {
	PKT_HASH_TYPE_NONE,	
	PKT_HASH_TYPE_L2,	
	PKT_HASH_TYPE_L3,	
	PKT_HASH_TYPE_L4,	
};

static inline void skb_clear_hash(struct sk_buff *skb)
{
	skb->hash = 0;
	skb->sw_hash = 0;
	skb->l4_hash = 0;
}

static inline void skb_clear_hash_if_not_l4(struct sk_buff *skb)
{
	if (!skb->l4_hash)
		skb_clear_hash(skb);
}

static inline void
__skb_set_hash(struct sk_buff *skb, __u32 hash, bool is_sw, bool is_l4)
{
	skb->l4_hash = is_l4;
	skb->sw_hash = is_sw;
	skb->hash = hash;
}

static inline void
skb_set_hash(struct sk_buff *skb, __u32 hash, enum pkt_hash_types type)
{
	
	__skb_set_hash(skb, hash, false, type == PKT_HASH_TYPE_L4);
}

static inline void
__skb_set_sw_hash(struct sk_buff *skb, __u32 hash, bool is_l4)
{
	__skb_set_hash(skb, hash, true, is_l4);
}

void __skb_get_hash(struct sk_buff *skb);
u32 __skb_get_hash_symmetric(const struct sk_buff *skb);
u32 skb_get_poff(const struct sk_buff *skb);
u32 __skb_get_poff(const struct sk_buff *skb, const void *data,
		   const struct flow_keys_basic *keys, int hlen);
__be32 __skb_flow_get_ports(const struct sk_buff *skb, int thoff, u8 ip_proto,
			    const void *data, int hlen_proto);

static inline __be32 skb_flow_get_ports(const struct sk_buff *skb,
					int thoff, u8 ip_proto)
{
	return __skb_flow_get_ports(skb, thoff, ip_proto, NULL, 0);
}

void skb_flow_dissector_init(struct flow_dissector *flow_dissector,
			     const struct flow_dissector_key *key,
			     unsigned int key_count);

struct bpf_flow_dissector;
u32 bpf_flow_dissect(struct bpf_prog *prog, struct bpf_flow_dissector *ctx,
		     __be16 proto, int nhoff, int hlen, unsigned int flags);

bool __skb_flow_dissect(const struct net *net,
			const struct sk_buff *skb,
			struct flow_dissector *flow_dissector,
			void *target_container, const void *data,
			__be16 proto, int nhoff, int hlen, unsigned int flags);

static inline bool skb_flow_dissect(const struct sk_buff *skb,
				    struct flow_dissector *flow_dissector,
				    void *target_container, unsigned int flags)
{
	return __skb_flow_dissect(NULL, skb, flow_dissector,
				  target_container, NULL, 0, 0, 0, flags);
}

static inline bool skb_flow_dissect_flow_keys(const struct sk_buff *skb,
					      struct flow_keys *flow,
					      unsigned int flags)
{
	memset(flow, 0, sizeof(*flow));
	return __skb_flow_dissect(NULL, skb, &flow_keys_dissector,
				  flow, NULL, 0, 0, 0, flags);
}

static inline bool
skb_flow_dissect_flow_keys_basic(const struct net *net,
				 const struct sk_buff *skb,
				 struct flow_keys_basic *flow,
				 const void *data, __be16 proto,
				 int nhoff, int hlen, unsigned int flags)
{
	memset(flow, 0, sizeof(*flow));
	return __skb_flow_dissect(net, skb, &flow_keys_basic_dissector, flow,
				  data, proto, nhoff, hlen, flags);
}

void skb_flow_dissect_meta(const struct sk_buff *skb,
			   struct flow_dissector *flow_dissector,
			   void *target_container);


void
skb_flow_dissect_ct(const struct sk_buff *skb,
		    struct flow_dissector *flow_dissector,
		    void *target_container,
		    u16 *ctinfo_map, size_t mapsize,
		    bool post_ct, u16 zone);
void
skb_flow_dissect_tunnel_info(const struct sk_buff *skb,
			     struct flow_dissector *flow_dissector,
			     void *target_container);

void skb_flow_dissect_hash(const struct sk_buff *skb,
			   struct flow_dissector *flow_dissector,
			   void *target_container);

static inline __u32 skb_get_hash(struct sk_buff *skb)
{
	if (!skb->l4_hash && !skb->sw_hash)
		__skb_get_hash(skb);

	return skb->hash;
}

static inline __u32 skb_get_hash_flowi6(struct sk_buff *skb, const struct flowi6 *fl6)
{
	if (!skb->l4_hash && !skb->sw_hash) {
		struct flow_keys keys;
		__u32 hash = __get_hash_from_flowi6(fl6, &keys);

		__skb_set_sw_hash(skb, hash, flow_keys_have_l4(&keys));
	}

	return skb->hash;
}

__u32 skb_get_hash_perturb(const struct sk_buff *skb,
			   const siphash_key_t *perturb);

static inline __u32 skb_get_hash_raw(const struct sk_buff *skb)
{
	return skb->hash;
}

static inline void skb_copy_hash(struct sk_buff *to, const struct sk_buff *from)
{
	to->hash = from->hash;
	to->sw_hash = from->sw_hash;
	to->l4_hash = from->l4_hash;
};

static inline int skb_cmp_decrypted(const struct sk_buff *skb1,
				    const struct sk_buff *skb2)
{
#ifdef CONFIG_TLS_DEVICE
	return skb2->decrypted - skb1->decrypted;
#else
	return 0;
#endif
}

static inline void skb_copy_decrypted(struct sk_buff *to,
				      const struct sk_buff *from)
{
#ifdef CONFIG_TLS_DEVICE
	to->decrypted = from->decrypted;
#endif
}

#ifdef NET_SKBUFF_DATA_USES_OFFSET
static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->head + skb->end;
}

static inline unsigned int skb_end_offset(const struct sk_buff *skb)
{
	return skb->end;
}

static inline void skb_set_end_offset(struct sk_buff *skb, unsigned int offset)
{
	skb->end = offset;
}
#else
static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}

static inline unsigned int skb_end_offset(const struct sk_buff *skb)
{
	return skb->end - skb->head;
}

static inline void skb_set_end_offset(struct sk_buff *skb, unsigned int offset)
{
	skb->end = skb->head + offset;
}
#endif

struct ubuf_info *msg_zerocopy_realloc(struct sock *sk, size_t size,
				       struct ubuf_info *uarg);

void msg_zerocopy_put_abort(struct ubuf_info *uarg, bool have_uref);

void msg_zerocopy_callback(struct sk_buff *skb, struct ubuf_info *uarg,
			   bool success);

int __zerocopy_sg_from_iter(struct msghdr *msg, struct sock *sk,
			    struct sk_buff *skb, struct iov_iter *from,
			    size_t length);

static inline int skb_zerocopy_iter_dgram(struct sk_buff *skb,
					  struct msghdr *msg, int len)
{
	return __zerocopy_sg_from_iter(msg, skb->sk, skb, &msg->msg_iter, len);
}

int skb_zerocopy_iter_stream(struct sock *sk, struct sk_buff *skb,
			     struct msghdr *msg, int len,
			     struct ubuf_info *uarg);


#define skb_shinfo(SKB)	((struct skb_shared_info *)(skb_end_pointer(SKB)))

static inline struct skb_shared_hwtstamps *skb_hwtstamps(struct sk_buff *skb)
{
	return &skb_shinfo(skb)->hwtstamps;
}

static inline struct ubuf_info *skb_zcopy(struct sk_buff *skb)
{
	bool is_zcopy = skb && skb_shinfo(skb)->flags & SKBFL_ZEROCOPY_ENABLE;

	return is_zcopy ? skb_uarg(skb) : NULL;
}

static inline bool skb_zcopy_pure(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->flags & SKBFL_PURE_ZEROCOPY;
}

static inline bool skb_zcopy_managed(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->flags & SKBFL_MANAGED_FRAG_REFS;
}

static inline bool skb_pure_zcopy_same(const struct sk_buff *skb1,
				       const struct sk_buff *skb2)
{
	return skb_zcopy_pure(skb1) == skb_zcopy_pure(skb2);
}

static inline void net_zcopy_get(struct ubuf_info *uarg)
{
	refcount_inc(&uarg->refcnt);
}

static inline void skb_zcopy_init(struct sk_buff *skb, struct ubuf_info *uarg)
{
	skb_shinfo(skb)->destructor_arg = uarg;
	skb_shinfo(skb)->flags |= uarg->flags;
}

static inline void skb_zcopy_set(struct sk_buff *skb, struct ubuf_info *uarg,
				 bool *have_ref)
{
	if (skb && uarg && !skb_zcopy(skb)) {
		if (unlikely(have_ref && *have_ref))
			*have_ref = false;
		else
			net_zcopy_get(uarg);
		skb_zcopy_init(skb, uarg);
	}
}

static inline void skb_zcopy_set_nouarg(struct sk_buff *skb, void *val)
{
	skb_shinfo(skb)->destructor_arg = (void *)((uintptr_t) val | 0x1UL);
	skb_shinfo(skb)->flags |= SKBFL_ZEROCOPY_FRAG;
}

static inline bool skb_zcopy_is_nouarg(struct sk_buff *skb)
{
	return (uintptr_t) skb_shinfo(skb)->destructor_arg & 0x1UL;
}

static inline void *skb_zcopy_get_nouarg(struct sk_buff *skb)
{
	return (void *)((uintptr_t) skb_shinfo(skb)->destructor_arg & ~0x1UL);
}

static inline void net_zcopy_put(struct ubuf_info *uarg)
{
	if (uarg)
		uarg->callback(NULL, uarg, true);
}

static inline void net_zcopy_put_abort(struct ubuf_info *uarg, bool have_uref)
{
	if (uarg) {
		if (uarg->callback == msg_zerocopy_callback)
			msg_zerocopy_put_abort(uarg, have_uref);
		else if (have_uref)
			net_zcopy_put(uarg);
	}
}


static inline void skb_zcopy_clear(struct sk_buff *skb, bool zerocopy_success)
{
	struct ubuf_info *uarg = skb_zcopy(skb);

	if (uarg) {
		if (!skb_zcopy_is_nouarg(skb))
			uarg->callback(skb, uarg, zerocopy_success);

		skb_shinfo(skb)->flags &= ~SKBFL_ALL_ZEROCOPY;
	}
}

void __skb_zcopy_downgrade_managed(struct sk_buff *skb);

static inline void skb_zcopy_downgrade_managed(struct sk_buff *skb)
{
	if (unlikely(skb_zcopy_managed(skb)))
		__skb_zcopy_downgrade_managed(skb);
}

static inline void skb_mark_not_on_list(struct sk_buff *skb)
{
	skb->next = NULL;
}

static inline void skb_poison_list(struct sk_buff *skb)
{
#ifdef CONFIG_DEBUG_NET
	skb->next = SKB_LIST_POISON_NEXT;
#endif
}


#define skb_list_walk_safe(first, skb, next_skb)                               \
	for ((skb) = (first), (next_skb) = (skb) ? (skb)->next : NULL; (skb);  \
	     (skb) = (next_skb), (next_skb) = (skb) ? (skb)->next : NULL)

static inline void skb_list_del_init(struct sk_buff *skb)
{
	__list_del_entry(&skb->list);
	skb_mark_not_on_list(skb);
}


static inline int skb_queue_empty(const struct sk_buff_head *list)
{
	return list->next == (const struct sk_buff *) list;
}


static inline bool skb_queue_empty_lockless(const struct sk_buff_head *list)
{
	return READ_ONCE(list->next) == (const struct sk_buff *) list;
}



static inline bool skb_queue_is_last(const struct sk_buff_head *list,
				     const struct sk_buff *skb)
{
	return skb->next == (const struct sk_buff *) list;
}


static inline bool skb_queue_is_first(const struct sk_buff_head *list,
				      const struct sk_buff *skb)
{
	return skb->prev == (const struct sk_buff *) list;
}


static inline struct sk_buff *skb_queue_next(const struct sk_buff_head *list,
					     const struct sk_buff *skb)
{
	
	BUG_ON(skb_queue_is_last(list, skb));
	return skb->next;
}


static inline struct sk_buff *skb_queue_prev(const struct sk_buff_head *list,
					     const struct sk_buff *skb)
{
	
	BUG_ON(skb_queue_is_first(list, skb));
	return skb->prev;
}


static inline struct sk_buff *skb_get(struct sk_buff *skb)
{
	refcount_inc(&skb->users);
	return skb;
}




static inline int skb_cloned(const struct sk_buff *skb)
{
	return skb->cloned &&
	       (atomic_read(&skb_shinfo(skb)->dataref) & SKB_DATAREF_MASK) != 1;
}

static inline int skb_unclone(struct sk_buff *skb, gfp_t pri)
{
	might_sleep_if(gfpflags_allow_blocking(pri));

	if (skb_cloned(skb))
		return pskb_expand_head(skb, 0, 0, pri);

	return 0;
}


int __skb_unclone_keeptruesize(struct sk_buff *skb, gfp_t pri);
static inline int skb_unclone_keeptruesize(struct sk_buff *skb, gfp_t pri)
{
	might_sleep_if(gfpflags_allow_blocking(pri));

	if (skb_cloned(skb))
		return __skb_unclone_keeptruesize(skb, pri);
	return 0;
}


static inline int skb_header_cloned(const struct sk_buff *skb)
{
	int dataref;

	if (!skb->cloned)
		return 0;

	dataref = atomic_read(&skb_shinfo(skb)->dataref);
	dataref = (dataref & SKB_DATAREF_MASK) - (dataref >> SKB_DATAREF_SHIFT);
	return dataref != 1;
}

static inline int skb_header_unclone(struct sk_buff *skb, gfp_t pri)
{
	might_sleep_if(gfpflags_allow_blocking(pri));

	if (skb_header_cloned(skb))
		return pskb_expand_head(skb, 0, 0, pri);

	return 0;
}


static inline void __skb_header_release(struct sk_buff *skb)
{
	skb->nohdr = 1;
	atomic_set(&skb_shinfo(skb)->dataref, 1 + (1 << SKB_DATAREF_SHIFT));
}



static inline int skb_shared(const struct sk_buff *skb)
{
	return refcount_read(&skb->users) != 1;
}


static inline struct sk_buff *skb_share_check(struct sk_buff *skb, gfp_t pri)
{
	might_sleep_if(gfpflags_allow_blocking(pri));
	if (skb_shared(skb)) {
		struct sk_buff *nskb = skb_clone(skb, pri);

		if (likely(nskb))
			consume_skb(skb);
		else
			kfree_skb(skb);
		skb = nskb;
	}
	return skb;
}




static inline struct sk_buff *skb_unshare(struct sk_buff *skb,
					  gfp_t pri)
{
	might_sleep_if(gfpflags_allow_blocking(pri));
	if (skb_cloned(skb)) {
		struct sk_buff *nskb = skb_copy(skb, pri);

		
		if (likely(nskb))
			consume_skb(skb);
		else
			kfree_skb(skb);
		skb = nskb;
	}
	return skb;
}


static inline struct sk_buff *skb_peek(const struct sk_buff_head *list_)
{
	struct sk_buff *skb = list_->next;

	if (skb == (struct sk_buff *)list_)
		skb = NULL;
	return skb;
}


static inline struct sk_buff *__skb_peek(const struct sk_buff_head *list_)
{
	return list_->next;
}


static inline struct sk_buff *skb_peek_next(struct sk_buff *skb,
		const struct sk_buff_head *list_)
{
	struct sk_buff *next = skb->next;

	if (next == (struct sk_buff *)list_)
		next = NULL;
	return next;
}


static inline struct sk_buff *skb_peek_tail(const struct sk_buff_head *list_)
{
	struct sk_buff *skb = READ_ONCE(list_->prev);

	if (skb == (struct sk_buff *)list_)
		skb = NULL;
	return skb;

}


static inline __u32 skb_queue_len(const struct sk_buff_head *list_)
{
	return list_->qlen;
}


static inline __u32 skb_queue_len_lockless(const struct sk_buff_head *list_)
{
	return READ_ONCE(list_->qlen);
}


static inline void __skb_queue_head_init(struct sk_buff_head *list)
{
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}


static inline void skb_queue_head_init(struct sk_buff_head *list)
{
	spin_lock_init(&list->lock);
	__skb_queue_head_init(list);
}

static inline void skb_queue_head_init_class(struct sk_buff_head *list,
		struct lock_class_key *class)
{
	skb_queue_head_init(list);
	lockdep_set_class(&list->lock, class);
}


static inline void __skb_insert(struct sk_buff *newsk,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list)
{
	
	WRITE_ONCE(newsk->next, next);
	WRITE_ONCE(newsk->prev, prev);
	WRITE_ONCE(((struct sk_buff_list *)next)->prev, newsk);
	WRITE_ONCE(((struct sk_buff_list *)prev)->next, newsk);
	WRITE_ONCE(list->qlen, list->qlen + 1);
}

static inline void __skb_queue_splice(const struct sk_buff_head *list,
				      struct sk_buff *prev,
				      struct sk_buff *next)
{
	struct sk_buff *first = list->next;
	struct sk_buff *last = list->prev;

	WRITE_ONCE(first->prev, prev);
	WRITE_ONCE(prev->next, first);

	WRITE_ONCE(last->next, next);
	WRITE_ONCE(next->prev, last);
}


static inline void skb_queue_splice(const struct sk_buff_head *list,
				    struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, (struct sk_buff *) head, head->next);
		head->qlen += list->qlen;
	}
}


static inline void skb_queue_splice_init(struct sk_buff_head *list,
					 struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, (struct sk_buff *) head, head->next);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}


static inline void skb_queue_splice_tail(const struct sk_buff_head *list,
					 struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
	}
}


static inline void skb_queue_splice_tail_init(struct sk_buff_head *list,
					      struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}


static inline void __skb_queue_after(struct sk_buff_head *list,
				     struct sk_buff *prev,
				     struct sk_buff *newsk)
{
	__skb_insert(newsk, prev, ((struct sk_buff_list *)prev)->next, list);
}

void skb_append(struct sk_buff *old, struct sk_buff *newsk,
		struct sk_buff_head *list);

static inline void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk)
{
	__skb_insert(newsk, ((struct sk_buff_list *)next)->prev, next, list);
}


static inline void __skb_queue_head(struct sk_buff_head *list,
				    struct sk_buff *newsk)
{
	__skb_queue_after(list, (struct sk_buff *)list, newsk);
}
void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk);


static inline void __skb_queue_tail(struct sk_buff_head *list,
				   struct sk_buff *newsk)
{
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
}
void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);


void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list);
static inline void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
	struct sk_buff *next, *prev;

	WRITE_ONCE(list->qlen, list->qlen - 1);
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = NULL;
	WRITE_ONCE(next->prev, prev);
	WRITE_ONCE(prev->next, next);
}


static inline struct sk_buff *__skb_dequeue(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}
struct sk_buff *skb_dequeue(struct sk_buff_head *list);


static inline struct sk_buff *__skb_dequeue_tail(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek_tail(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}
struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list);


static inline bool skb_is_nonlinear(const struct sk_buff *skb)
{
	return skb->data_len;
}

static inline unsigned int skb_headlen(const struct sk_buff *skb)
{
	return skb->len - skb->data_len;
}

static inline unsigned int __skb_pagelen(const struct sk_buff *skb)
{
	unsigned int i, len = 0;

	for (i = skb_shinfo(skb)->nr_frags - 1; (int)i >= 0; i--)
		len += skb_frag_size(&skb_shinfo(skb)->frags[i]);
	return len;
}

static inline unsigned int skb_pagelen(const struct sk_buff *skb)
{
	return skb_headlen(skb) + __skb_pagelen(skb);
}

static inline void skb_frag_fill_page_desc(skb_frag_t *frag,
					   struct page *page,
					   int off, int size)
{
	frag->bv_page = page;
	frag->bv_offset = off;
	skb_frag_size_set(frag, size);
}

static inline void __skb_fill_page_desc_noacc(struct skb_shared_info *shinfo,
					      int i, struct page *page,
					      int off, int size)
{
	skb_frag_t *frag = &shinfo->frags[i];

	skb_frag_fill_page_desc(frag, page, off, size);
}


static inline void skb_len_add(struct sk_buff *skb, int delta)
{
	skb->len += delta;
	skb->data_len += delta;
	skb->truesize += delta;
}


static inline void __skb_fill_page_desc(struct sk_buff *skb, int i,
					struct page *page, int off, int size)
{
	__skb_fill_page_desc_noacc(skb_shinfo(skb), i, page, off, size);

	
	page = compound_head(page);
	if (page_is_pfmemalloc(page))
		skb->pfmemalloc	= true;
}


static inline void skb_fill_page_desc(struct sk_buff *skb, int i,
				      struct page *page, int off, int size)
{
	__skb_fill_page_desc(skb, i, page, off, size);
	skb_shinfo(skb)->nr_frags = i + 1;
}


static inline void skb_fill_page_desc_noacc(struct sk_buff *skb, int i,
					    struct page *page, int off,
					    int size)
{
	struct skb_shared_info *shinfo = skb_shinfo(skb);

	__skb_fill_page_desc_noacc(shinfo, i, page, off, size);
	shinfo->nr_frags = i + 1;
}

void skb_add_rx_frag(struct sk_buff *skb, int i, struct page *page, int off,
		     int size, unsigned int truesize);

void skb_coalesce_rx_frag(struct sk_buff *skb, int i, int size,
			  unsigned int truesize);

#define SKB_LINEAR_ASSERT(skb)  BUG_ON(skb_is_nonlinear(skb))

#ifdef NET_SKBUFF_DATA_USES_OFFSET
static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->head + skb->tail;
}

static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data - skb->head;
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb_reset_tail_pointer(skb);
	skb->tail += offset;
}

#else 
static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb->tail = skb->data + offset;
}

#endif 

static inline void skb_assert_len(struct sk_buff *skb)
{
#ifdef CONFIG_DEBUG_NET
	if (WARN_ONCE(!skb->len, "%s\n", __func__))
		DO_ONCE_LITE(skb_dump, KERN_ERR, skb, false);
#endif 
}


void *pskb_put(struct sk_buff *skb, struct sk_buff *tail, int len);
void *skb_put(struct sk_buff *skb, unsigned int len);
static inline void *__skb_put(struct sk_buff *skb, unsigned int len)
{
	void *tmp = skb_tail_pointer(skb);
	SKB_LINEAR_ASSERT(skb);
	skb->tail += len;
	skb->len  += len;
	return tmp;
}

static inline void *__skb_put_zero(struct sk_buff *skb, unsigned int len)
{
	void *tmp = __skb_put(skb, len);

	memset(tmp, 0, len);
	return tmp;
}

static inline void *__skb_put_data(struct sk_buff *skb, const void *data,
				   unsigned int len)
{
	void *tmp = __skb_put(skb, len);

	memcpy(tmp, data, len);
	return tmp;
}

static inline void __skb_put_u8(struct sk_buff *skb, u8 val)
{
	*(u8 *)__skb_put(skb, 1) = val;
}

static inline void *skb_put_zero(struct sk_buff *skb, unsigned int len)
{
	void *tmp = skb_put(skb, len);

	memset(tmp, 0, len);

	return tmp;
}

static inline void *skb_put_data(struct sk_buff *skb, const void *data,
				 unsigned int len)
{
	void *tmp = skb_put(skb, len);

	memcpy(tmp, data, len);

	return tmp;
}

static inline void skb_put_u8(struct sk_buff *skb, u8 val)
{
	*(u8 *)skb_put(skb, 1) = val;
}

void *skb_push(struct sk_buff *skb, unsigned int len);
static inline void *__skb_push(struct sk_buff *skb, unsigned int len)
{
	DEBUG_NET_WARN_ON_ONCE(len > INT_MAX);

	skb->data -= len;
	skb->len  += len;
	return skb->data;
}

void *skb_pull(struct sk_buff *skb, unsigned int len);
static inline void *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	DEBUG_NET_WARN_ON_ONCE(len > INT_MAX);

	skb->len -= len;
	if (unlikely(skb->len < skb->data_len)) {
#if defined(CONFIG_DEBUG_NET)
		skb->len += len;
		pr_err("__skb_pull(len=%u)\n", len);
		skb_dump(KERN_ERR, skb, false);
#endif
		BUG();
	}
	return skb->data += len;
}

static inline void *skb_pull_inline(struct sk_buff *skb, unsigned int len)
{
	return unlikely(len > skb->len) ? NULL : __skb_pull(skb, len);
}

void *skb_pull_data(struct sk_buff *skb, size_t len);

void *__pskb_pull_tail(struct sk_buff *skb, int delta);

static inline enum skb_drop_reason
pskb_may_pull_reason(struct sk_buff *skb, unsigned int len)
{
	DEBUG_NET_WARN_ON_ONCE(len > INT_MAX);

	if (likely(len <= skb_headlen(skb)))
		return SKB_NOT_DROPPED_YET;

	if (unlikely(len > skb->len))
		return SKB_DROP_REASON_PKT_TOO_SMALL;

	if (unlikely(!__pskb_pull_tail(skb, len - skb_headlen(skb))))
		return SKB_DROP_REASON_NOMEM;

	return SKB_NOT_DROPPED_YET;
}

static inline bool pskb_may_pull(struct sk_buff *skb, unsigned int len)
{
	return pskb_may_pull_reason(skb, len) == SKB_NOT_DROPPED_YET;
}

static inline void *pskb_pull(struct sk_buff *skb, unsigned int len)
{
	if (!pskb_may_pull(skb, len))
		return NULL;

	skb->len -= len;
	return skb->data += len;
}

void skb_condense(struct sk_buff *skb);


static inline unsigned int skb_headroom(const struct sk_buff *skb)
{
	return skb->data - skb->head;
}


static inline int skb_tailroom(const struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}


static inline int skb_availroom(const struct sk_buff *skb)
{
	if (skb_is_nonlinear(skb))
		return 0;

	return skb->end - skb->tail - skb->reserved_tailroom;
}


static inline void skb_reserve(struct sk_buff *skb, int len)
{
	skb->data += len;
	skb->tail += len;
}


static inline void skb_tailroom_reserve(struct sk_buff *skb, unsigned int mtu,
					unsigned int needed_tailroom)
{
	SKB_LINEAR_ASSERT(skb);
	if (mtu < skb_tailroom(skb) - needed_tailroom)
		
		skb->reserved_tailroom = skb_tailroom(skb) - mtu;
	else
		
		skb->reserved_tailroom = needed_tailroom;
}

#define ENCAP_TYPE_ETHER	0
#define ENCAP_TYPE_IPPROTO	1

static inline void skb_set_inner_protocol(struct sk_buff *skb,
					  __be16 protocol)
{
	skb->inner_protocol = protocol;
	skb->inner_protocol_type = ENCAP_TYPE_ETHER;
}

static inline void skb_set_inner_ipproto(struct sk_buff *skb,
					 __u8 ipproto)
{
	skb->inner_ipproto = ipproto;
	skb->inner_protocol_type = ENCAP_TYPE_IPPROTO;
}

static inline void skb_reset_inner_headers(struct sk_buff *skb)
{
	skb->inner_mac_header = skb->mac_header;
	skb->inner_network_header = skb->network_header;
	skb->inner_transport_header = skb->transport_header;
}

static inline void skb_reset_mac_len(struct sk_buff *skb)
{
	skb->mac_len = skb->network_header - skb->mac_header;
}

static inline unsigned char *skb_inner_transport_header(const struct sk_buff
							*skb)
{
	return skb->head + skb->inner_transport_header;
}

static inline int skb_inner_transport_offset(const struct sk_buff *skb)
{
	return skb_inner_transport_header(skb) - skb->data;
}

static inline void skb_reset_inner_transport_header(struct sk_buff *skb)
{
	skb->inner_transport_header = skb->data - skb->head;
}

static inline void skb_set_inner_transport_header(struct sk_buff *skb,
						   const int offset)
{
	skb_reset_inner_transport_header(skb);
	skb->inner_transport_header += offset;
}

static inline unsigned char *skb_inner_network_header(const struct sk_buff *skb)
{
	return skb->head + skb->inner_network_header;
}

static inline void skb_reset_inner_network_header(struct sk_buff *skb)
{
	skb->inner_network_header = skb->data - skb->head;
}

static inline void skb_set_inner_network_header(struct sk_buff *skb,
						const int offset)
{
	skb_reset_inner_network_header(skb);
	skb->inner_network_header += offset;
}

static inline bool skb_inner_network_header_was_set(const struct sk_buff *skb)
{
	return skb->inner_network_header > 0;
}

static inline unsigned char *skb_inner_mac_header(const struct sk_buff *skb)
{
	return skb->head + skb->inner_mac_header;
}

static inline void skb_reset_inner_mac_header(struct sk_buff *skb)
{
	skb->inner_mac_header = skb->data - skb->head;
}

static inline void skb_set_inner_mac_header(struct sk_buff *skb,
					    const int offset)
{
	skb_reset_inner_mac_header(skb);
	skb->inner_mac_header += offset;
}
static inline bool skb_transport_header_was_set(const struct sk_buff *skb)
{
	return skb->transport_header != (typeof(skb->transport_header))~0U;
}

static inline unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	DEBUG_NET_WARN_ON_ONCE(!skb_transport_header_was_set(skb));
	return skb->head + skb->transport_header;
}

static inline void skb_reset_transport_header(struct sk_buff *skb)
{
	skb->transport_header = skb->data - skb->head;
}

static inline void skb_set_transport_header(struct sk_buff *skb,
					    const int offset)
{
	skb_reset_transport_header(skb);
	skb->transport_header += offset;
}

static inline unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return skb->head + skb->network_header;
}

static inline void skb_reset_network_header(struct sk_buff *skb)
{
	skb->network_header = skb->data - skb->head;
}

static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb_reset_network_header(skb);
	skb->network_header += offset;
}

static inline int skb_mac_header_was_set(const struct sk_buff *skb)
{
	return skb->mac_header != (typeof(skb->mac_header))~0U;
}

static inline unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	DEBUG_NET_WARN_ON_ONCE(!skb_mac_header_was_set(skb));
	return skb->head + skb->mac_header;
}

static inline int skb_mac_offset(const struct sk_buff *skb)
{
	return skb_mac_header(skb) - skb->data;
}

static inline u32 skb_mac_header_len(const struct sk_buff *skb)
{
	DEBUG_NET_WARN_ON_ONCE(!skb_mac_header_was_set(skb));
	return skb->network_header - skb->mac_header;
}

static inline void skb_unset_mac_header(struct sk_buff *skb)
{
	skb->mac_header = (typeof(skb->mac_header))~0U;
}

static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac_header = skb->data - skb->head;
}

static inline void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb_reset_mac_header(skb);
	skb->mac_header += offset;
}

static inline void skb_pop_mac_header(struct sk_buff *skb)
{
	skb->mac_header = skb->network_header;
}

static inline void skb_probe_transport_header(struct sk_buff *skb)
{
	struct flow_keys_basic keys;

	if (skb_transport_header_was_set(skb))
		return;

	if (skb_flow_dissect_flow_keys_basic(NULL, skb, &keys,
					     NULL, 0, 0, 0, 0))
		skb_set_transport_header(skb, keys.control.thoff);
}

static inline void skb_mac_header_rebuild(struct sk_buff *skb)
{
	if (skb_mac_header_was_set(skb)) {
		const unsigned char *old_mac = skb_mac_header(skb);

		skb_set_mac_header(skb, -skb->mac_len);
		memmove(skb_mac_header(skb), old_mac, skb->mac_len);
	}
}


static inline void skb_mac_header_rebuild_full(struct sk_buff *skb, u32 full_mac_len)
{
	if (skb_mac_header_was_set(skb)) {
		const unsigned char *old_mac = skb_mac_header(skb);

		skb_set_mac_header(skb, -full_mac_len);
		memmove(skb_mac_header(skb), old_mac, full_mac_len);
		__skb_push(skb, full_mac_len - skb->mac_len);
	}
}

static inline int skb_checksum_start_offset(const struct sk_buff *skb)
{
	return skb->csum_start - skb_headroom(skb);
}

static inline unsigned char *skb_checksum_start(const struct sk_buff *skb)
{
	return skb->head + skb->csum_start;
}

static inline int skb_transport_offset(const struct sk_buff *skb)
{
	return skb_transport_header(skb) - skb->data;
}

static inline u32 skb_network_header_len(const struct sk_buff *skb)
{
	return skb->transport_header - skb->network_header;
}

static inline u32 skb_inner_network_header_len(const struct sk_buff *skb)
{
	return skb->inner_transport_header - skb->inner_network_header;
}

static inline int skb_network_offset(const struct sk_buff *skb)
{
	return skb_network_header(skb) - skb->data;
}

static inline int skb_inner_network_offset(const struct sk_buff *skb)
{
	return skb_inner_network_header(skb) - skb->data;
}

static inline int pskb_network_may_pull(struct sk_buff *skb, unsigned int len)
{
	return pskb_may_pull(skb, skb_network_offset(skb) + len);
}


#ifndef NET_IP_ALIGN
#define NET_IP_ALIGN	2
#endif


#ifndef NET_SKB_PAD
#define NET_SKB_PAD	max(32, L1_CACHE_BYTES)
#endif

int ___pskb_trim(struct sk_buff *skb, unsigned int len);

static inline void __skb_set_length(struct sk_buff *skb, unsigned int len)
{
	if (WARN_ON(skb_is_nonlinear(skb)))
		return;
	skb->len = len;
	skb_set_tail_pointer(skb, len);
}

static inline void __skb_trim(struct sk_buff *skb, unsigned int len)
{
	__skb_set_length(skb, len);
}

void skb_trim(struct sk_buff *skb, unsigned int len);

static inline int __pskb_trim(struct sk_buff *skb, unsigned int len)
{
	if (skb->data_len)
		return ___pskb_trim(skb, len);
	__skb_trim(skb, len);
	return 0;
}

static inline int pskb_trim(struct sk_buff *skb, unsigned int len)
{
	return (len < skb->len) ? __pskb_trim(skb, len) : 0;
}


static inline void pskb_trim_unique(struct sk_buff *skb, unsigned int len)
{
	int err = pskb_trim(skb, len);
	BUG_ON(err);
}

static inline int __skb_grow(struct sk_buff *skb, unsigned int len)
{
	unsigned int diff = len - skb->len;

	if (skb_tailroom(skb) < diff) {
		int ret = pskb_expand_head(skb, 0, diff - skb_tailroom(skb),
					   GFP_ATOMIC);
		if (ret)
			return ret;
	}
	__skb_set_length(skb, len);
	return 0;
}


static inline void skb_orphan(struct sk_buff *skb)
{
	if (skb->destructor) {
		skb->destructor(skb);
		skb->destructor = NULL;
		skb->sk		= NULL;
	} else {
		BUG_ON(skb->sk);
	}
}


static inline int skb_orphan_frags(struct sk_buff *skb, gfp_t gfp_mask)
{
	if (likely(!skb_zcopy(skb)))
		return 0;
	if (skb_shinfo(skb)->flags & SKBFL_DONT_ORPHAN)
		return 0;
	return skb_copy_ubufs(skb, gfp_mask);
}


static inline int skb_orphan_frags_rx(struct sk_buff *skb, gfp_t gfp_mask)
{
	if (likely(!skb_zcopy(skb)))
		return 0;
	return skb_copy_ubufs(skb, gfp_mask);
}


static inline void __skb_queue_purge_reason(struct sk_buff_head *list,
					    enum skb_drop_reason reason)
{
	struct sk_buff *skb;

	while ((skb = __skb_dequeue(list)) != NULL)
		kfree_skb_reason(skb, reason);
}

static inline void __skb_queue_purge(struct sk_buff_head *list)
{
	__skb_queue_purge_reason(list, SKB_DROP_REASON_QUEUE_PURGE);
}

void skb_queue_purge_reason(struct sk_buff_head *list,
			    enum skb_drop_reason reason);

static inline void skb_queue_purge(struct sk_buff_head *list)
{
	skb_queue_purge_reason(list, SKB_DROP_REASON_QUEUE_PURGE);
}

unsigned int skb_rbtree_purge(struct rb_root *root);
void skb_errqueue_purge(struct sk_buff_head *list);

void *__netdev_alloc_frag_align(unsigned int fragsz, unsigned int align_mask);


static inline void *netdev_alloc_frag(unsigned int fragsz)
{
	return __netdev_alloc_frag_align(fragsz, ~0u);
}

static inline void *netdev_alloc_frag_align(unsigned int fragsz,
					    unsigned int align)
{
	WARN_ON_ONCE(!is_power_of_2(align));
	return __netdev_alloc_frag_align(fragsz, -align);
}

struct sk_buff *__netdev_alloc_skb(struct net_device *dev, unsigned int length,
				   gfp_t gfp_mask);


static inline struct sk_buff *netdev_alloc_skb(struct net_device *dev,
					       unsigned int length)
{
	return __netdev_alloc_skb(dev, length, GFP_ATOMIC);
}


static inline struct sk_buff *__dev_alloc_skb(unsigned int length,
					      gfp_t gfp_mask)
{
	return __netdev_alloc_skb(NULL, length, gfp_mask);
}


static inline struct sk_buff *dev_alloc_skb(unsigned int length)
{
	return netdev_alloc_skb(NULL, length);
}


static inline struct sk_buff *__netdev_alloc_skb_ip_align(struct net_device *dev,
		unsigned int length, gfp_t gfp)
{
	struct sk_buff *skb = __netdev_alloc_skb(dev, length + NET_IP_ALIGN, gfp);

	if (NET_IP_ALIGN && skb)
		skb_reserve(skb, NET_IP_ALIGN);
	return skb;
}

static inline struct sk_buff *netdev_alloc_skb_ip_align(struct net_device *dev,
		unsigned int length)
{
	return __netdev_alloc_skb_ip_align(dev, length, GFP_ATOMIC);
}

static inline void skb_free_frag(void *addr)
{
	page_frag_free(addr);
}

void *__napi_alloc_frag_align(unsigned int fragsz, unsigned int align_mask);

static inline void *napi_alloc_frag(unsigned int fragsz)
{
	return __napi_alloc_frag_align(fragsz, ~0u);
}

static inline void *napi_alloc_frag_align(unsigned int fragsz,
					  unsigned int align)
{
	WARN_ON_ONCE(!is_power_of_2(align));
	return __napi_alloc_frag_align(fragsz, -align);
}

struct sk_buff *__napi_alloc_skb(struct napi_struct *napi,
				 unsigned int length, gfp_t gfp_mask);
static inline struct sk_buff *napi_alloc_skb(struct napi_struct *napi,
					     unsigned int length)
{
	return __napi_alloc_skb(napi, length, GFP_ATOMIC);
}
void napi_consume_skb(struct sk_buff *skb, int budget);

void napi_skb_free_stolen_head(struct sk_buff *skb);
void __napi_kfree_skb(struct sk_buff *skb, enum skb_drop_reason reason);


static inline struct page *__dev_alloc_pages(gfp_t gfp_mask,
					     unsigned int order)
{
	
	gfp_mask |= __GFP_COMP | __GFP_MEMALLOC;

	return alloc_pages_node(NUMA_NO_NODE, gfp_mask, order);
}

static inline struct page *dev_alloc_pages(unsigned int order)
{
	return __dev_alloc_pages(GFP_ATOMIC | __GFP_NOWARN, order);
}


static inline struct page *__dev_alloc_page(gfp_t gfp_mask)
{
	return __dev_alloc_pages(gfp_mask, 0);
}

static inline struct page *dev_alloc_page(void)
{
	return dev_alloc_pages(0);
}


static inline bool dev_page_is_reusable(const struct page *page)
{
	return likely(page_to_nid(page) == numa_mem_id() &&
		      !page_is_pfmemalloc(page));
}


static inline void skb_propagate_pfmemalloc(const struct page *page,
					    struct sk_buff *skb)
{
	if (page_is_pfmemalloc(page))
		skb->pfmemalloc = true;
}


static inline unsigned int skb_frag_off(const skb_frag_t *frag)
{
	return frag->bv_offset;
}


static inline void skb_frag_off_add(skb_frag_t *frag, int delta)
{
	frag->bv_offset += delta;
}


static inline void skb_frag_off_set(skb_frag_t *frag, unsigned int offset)
{
	frag->bv_offset = offset;
}


static inline void skb_frag_off_copy(skb_frag_t *fragto,
				     const skb_frag_t *fragfrom)
{
	fragto->bv_offset = fragfrom->bv_offset;
}


static inline struct page *skb_frag_page(const skb_frag_t *frag)
{
	return frag->bv_page;
}


static inline void __skb_frag_ref(skb_frag_t *frag)
{
	get_page(skb_frag_page(frag));
}


static inline void skb_frag_ref(struct sk_buff *skb, int f)
{
	__skb_frag_ref(&skb_shinfo(skb)->frags[f]);
}

bool napi_pp_put_page(struct page *page, bool napi_safe);

static inline void
skb_page_unref(const struct sk_buff *skb, struct page *page, bool napi_safe)
{
#ifdef CONFIG_PAGE_POOL
	if (skb->pp_recycle && napi_pp_put_page(page, napi_safe))
		return;
#endif
	put_page(page);
}

static inline void
napi_frag_unref(skb_frag_t *frag, bool recycle, bool napi_safe)
{
	struct page *page = skb_frag_page(frag);

#ifdef CONFIG_PAGE_POOL
	if (recycle && napi_pp_put_page(page, napi_safe))
		return;
#endif
	put_page(page);
}


static inline void __skb_frag_unref(skb_frag_t *frag, bool recycle)
{
	napi_frag_unref(frag, recycle, false);
}


static inline void skb_frag_unref(struct sk_buff *skb, int f)
{
	struct skb_shared_info *shinfo = skb_shinfo(skb);

	if (!skb_zcopy_managed(skb))
		__skb_frag_unref(&shinfo->frags[f], skb->pp_recycle);
}


static inline void *skb_frag_address(const skb_frag_t *frag)
{
	return page_address(skb_frag_page(frag)) + skb_frag_off(frag);
}


static inline void *skb_frag_address_safe(const skb_frag_t *frag)
{
	void *ptr = page_address(skb_frag_page(frag));
	if (unlikely(!ptr))
		return NULL;

	return ptr + skb_frag_off(frag);
}


static inline void skb_frag_page_copy(skb_frag_t *fragto,
				      const skb_frag_t *fragfrom)
{
	fragto->bv_page = fragfrom->bv_page;
}

bool skb_page_frag_refill(unsigned int sz, struct page_frag *pfrag, gfp_t prio);


static inline dma_addr_t skb_frag_dma_map(struct device *dev,
					  const skb_frag_t *frag,
					  size_t offset, size_t size,
					  enum dma_data_direction dir)
{
	return dma_map_page(dev, skb_frag_page(frag),
			    skb_frag_off(frag) + offset, size, dir);
}

static inline struct sk_buff *pskb_copy(struct sk_buff *skb,
					gfp_t gfp_mask)
{
	return __pskb_copy(skb, skb_headroom(skb), gfp_mask);
}


static inline struct sk_buff *pskb_copy_for_clone(struct sk_buff *skb,
						  gfp_t gfp_mask)
{
	return __pskb_copy_fclone(skb, skb_headroom(skb), gfp_mask, true);
}



static inline int skb_clone_writable(const struct sk_buff *skb, unsigned int len)
{
	return !skb_header_cloned(skb) &&
	       skb_headroom(skb) + len <= skb->hdr_len;
}

static inline int skb_try_make_writable(struct sk_buff *skb,
					unsigned int write_len)
{
	return skb_cloned(skb) && !skb_clone_writable(skb, write_len) &&
	       pskb_expand_head(skb, 0, 0, GFP_ATOMIC);
}

static inline int __skb_cow(struct sk_buff *skb, unsigned int headroom,
			    int cloned)
{
	int delta = 0;

	if (headroom > skb_headroom(skb))
		delta = headroom - skb_headroom(skb);

	if (delta || cloned)
		return pskb_expand_head(skb, ALIGN(delta, NET_SKB_PAD), 0,
					GFP_ATOMIC);
	return 0;
}


static inline int skb_cow(struct sk_buff *skb, unsigned int headroom)
{
	return __skb_cow(skb, headroom, skb_cloned(skb));
}


static inline int skb_cow_head(struct sk_buff *skb, unsigned int headroom)
{
	return __skb_cow(skb, headroom, skb_header_cloned(skb));
}


static inline int skb_padto(struct sk_buff *skb, unsigned int len)
{
	unsigned int size = skb->len;
	if (likely(size >= len))
		return 0;
	return skb_pad(skb, len - size);
}


static inline int __must_check __skb_put_padto(struct sk_buff *skb,
					       unsigned int len,
					       bool free_on_error)
{
	unsigned int size = skb->len;

	if (unlikely(size < len)) {
		len -= size;
		if (__skb_pad(skb, len, free_on_error))
			return -ENOMEM;
		__skb_put(skb, len);
	}
	return 0;
}


static inline int __must_check skb_put_padto(struct sk_buff *skb, unsigned int len)
{
	return __skb_put_padto(skb, len, true);
}

static inline int skb_add_data(struct sk_buff *skb,
			       struct iov_iter *from, int copy)
{
	const int off = skb->len;

	if (skb->ip_summed == CHECKSUM_NONE) {
		__wsum csum = 0;
		if (csum_and_copy_from_iter_full(skb_put(skb, copy), copy,
					         &csum, from)) {
			skb->csum = csum_block_add(skb->csum, csum, off);
			return 0;
		}
	} else if (copy_from_iter_full(skb_put(skb, copy), copy, from))
		return 0;

	__skb_trim(skb, off);
	return -EFAULT;
}

static inline bool skb_can_coalesce(struct sk_buff *skb, int i,
				    const struct page *page, int off)
{
	if (skb_zcopy(skb))
		return false;
	if (i) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i - 1];

		return page == skb_frag_page(frag) &&
		       off == skb_frag_off(frag) + skb_frag_size(frag);
	}
	return false;
}

static inline int __skb_linearize(struct sk_buff *skb)
{
	return __pskb_pull_tail(skb, skb->data_len) ? 0 : -ENOMEM;
}


static inline int skb_linearize(struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? __skb_linearize(skb) : 0;
}


static inline bool skb_has_shared_frag(const struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) &&
	       skb_shinfo(skb)->flags & SKBFL_SHARED_FRAG;
}


static inline int skb_linearize_cow(struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) || skb_cloned(skb) ?
	       __skb_linearize(skb) : 0;
}

static __always_inline void
__skb_postpull_rcsum(struct sk_buff *skb, const void *start, unsigned int len,
		     unsigned int off)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->csum = csum_block_sub(skb->csum,
					   csum_partial(start, len, 0), off);
	else if (skb->ip_summed == CHECKSUM_PARTIAL &&
		 skb_checksum_start_offset(skb) < 0)
		skb->ip_summed = CHECKSUM_NONE;
}


static inline void skb_postpull_rcsum(struct sk_buff *skb,
				      const void *start, unsigned int len)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->csum = wsum_negate(csum_partial(start, len,
						     wsum_negate(skb->csum)));
	else if (skb->ip_summed == CHECKSUM_PARTIAL &&
		 skb_checksum_start_offset(skb) < 0)
		skb->ip_summed = CHECKSUM_NONE;
}

static __always_inline void
__skb_postpush_rcsum(struct sk_buff *skb, const void *start, unsigned int len,
		     unsigned int off)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->csum = csum_block_add(skb->csum,
					   csum_partial(start, len, 0), off);
}


static inline void skb_postpush_rcsum(struct sk_buff *skb,
				      const void *start, unsigned int len)
{
	__skb_postpush_rcsum(skb, start, len, 0);
}

void *skb_pull_rcsum(struct sk_buff *skb, unsigned int len);


static inline void *skb_push_rcsum(struct sk_buff *skb, unsigned int len)
{
	skb_push(skb, len);
	skb_postpush_rcsum(skb, skb->data, len);
	return skb->data;
}

int pskb_trim_rcsum_slow(struct sk_buff *skb, unsigned int len);


static inline int pskb_trim_rcsum(struct sk_buff *skb, unsigned int len)
{
	if (likely(len >= skb->len))
		return 0;
	return pskb_trim_rcsum_slow(skb, len);
}

static inline int __skb_trim_rcsum(struct sk_buff *skb, unsigned int len)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->ip_summed = CHECKSUM_NONE;
	__skb_trim(skb, len);
	return 0;
}

static inline int __skb_grow_rcsum(struct sk_buff *skb, unsigned int len)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->ip_summed = CHECKSUM_NONE;
	return __skb_grow(skb, len);
}

#define rb_to_skb(rb) rb_entry_safe(rb, struct sk_buff, rbnode)
#define skb_rb_first(root) rb_to_skb(rb_first(root))
#define skb_rb_last(root)  rb_to_skb(rb_last(root))
#define skb_rb_next(skb)   rb_to_skb(rb_next(&(skb)->rbnode))
#define skb_rb_prev(skb)   rb_to_skb(rb_prev(&(skb)->rbnode))

#define skb_queue_walk(queue, skb) \
		for (skb = (queue)->next;					\
		     skb != (struct sk_buff *)(queue);				\
		     skb = skb->next)

#define skb_queue_walk_safe(queue, skb, tmp)					\
		for (skb = (queue)->next, tmp = skb->next;			\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->next)

#define skb_queue_walk_from(queue, skb)						\
		for (; skb != (struct sk_buff *)(queue);			\
		     skb = skb->next)

#define skb_rbtree_walk(skb, root)						\
		for (skb = skb_rb_first(root); skb != NULL;			\
		     skb = skb_rb_next(skb))

#define skb_rbtree_walk_from(skb)						\
		for (; skb != NULL;						\
		     skb = skb_rb_next(skb))

#define skb_rbtree_walk_from_safe(skb, tmp)					\
		for (; tmp = skb ? skb_rb_next(skb) : NULL, (skb != NULL);	\
		     skb = tmp)

#define skb_queue_walk_from_safe(queue, skb, tmp)				\
		for (tmp = skb->next;						\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->next)

#define skb_queue_reverse_walk(queue, skb) \
		for (skb = (queue)->prev;					\
		     skb != (struct sk_buff *)(queue);				\
		     skb = skb->prev)

#define skb_queue_reverse_walk_safe(queue, skb, tmp)				\
		for (skb = (queue)->prev, tmp = skb->prev;			\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->prev)

#define skb_queue_reverse_walk_from_safe(queue, skb, tmp)			\
		for (tmp = skb->prev;						\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->prev)

static inline bool skb_has_frag_list(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->frag_list != NULL;
}

static inline void skb_frag_list_init(struct sk_buff *skb)
{
	skb_shinfo(skb)->frag_list = NULL;
}

#define skb_walk_frags(skb, iter)	\
	for (iter = skb_shinfo(skb)->frag_list; iter; iter = iter->next)


int __skb_wait_for_more_packets(struct sock *sk, struct sk_buff_head *queue,
				int *err, long *timeo_p,
				const struct sk_buff *skb);
struct sk_buff *__skb_try_recv_from_queue(struct sock *sk,
					  struct sk_buff_head *queue,
					  unsigned int flags,
					  int *off, int *err,
					  struct sk_buff **last);
struct sk_buff *__skb_try_recv_datagram(struct sock *sk,
					struct sk_buff_head *queue,
					unsigned int flags, int *off, int *err,
					struct sk_buff **last);
struct sk_buff *__skb_recv_datagram(struct sock *sk,
				    struct sk_buff_head *sk_queue,
				    unsigned int flags, int *off, int *err);
struct sk_buff *skb_recv_datagram(struct sock *sk, unsigned int flags, int *err);
__poll_t datagram_poll(struct file *file, struct socket *sock,
			   struct poll_table_struct *wait);
int skb_copy_datagram_iter(const struct sk_buff *from, int offset,
			   struct iov_iter *to, int size);
static inline int skb_copy_datagram_msg(const struct sk_buff *from, int offset,
					struct msghdr *msg, int size)
{
	return skb_copy_datagram_iter(from, offset, &msg->msg_iter, size);
}
int skb_copy_and_csum_datagram_msg(struct sk_buff *skb, int hlen,
				   struct msghdr *msg);
int skb_copy_and_hash_datagram_iter(const struct sk_buff *skb, int offset,
			   struct iov_iter *to, int len,
			   struct ahash_request *hash);
int skb_copy_datagram_from_iter(struct sk_buff *skb, int offset,
				 struct iov_iter *from, int len);
int zerocopy_sg_from_iter(struct sk_buff *skb, struct iov_iter *frm);
void skb_free_datagram(struct sock *sk, struct sk_buff *skb);
void __skb_free_datagram_locked(struct sock *sk, struct sk_buff *skb, int len);
static inline void skb_free_datagram_locked(struct sock *sk,
					    struct sk_buff *skb)
{
	__skb_free_datagram_locked(sk, skb, 0);
}
int skb_kill_datagram(struct sock *sk, struct sk_buff *skb, unsigned int flags);
int skb_copy_bits(const struct sk_buff *skb, int offset, void *to, int len);
int skb_store_bits(struct sk_buff *skb, int offset, const void *from, int len);
__wsum skb_copy_and_csum_bits(const struct sk_buff *skb, int offset, u8 *to,
			      int len);
int skb_splice_bits(struct sk_buff *skb, struct sock *sk, unsigned int offset,
		    struct pipe_inode_info *pipe, unsigned int len,
		    unsigned int flags);
int skb_send_sock_locked(struct sock *sk, struct sk_buff *skb, int offset,
			 int len);
int skb_send_sock(struct sock *sk, struct sk_buff *skb, int offset, int len);
void skb_copy_and_csum_dev(const struct sk_buff *skb, u8 *to);
unsigned int skb_zerocopy_headlen(const struct sk_buff *from);
int skb_zerocopy(struct sk_buff *to, struct sk_buff *from,
		 int len, int hlen);
void skb_split(struct sk_buff *skb, struct sk_buff *skb1, const u32 len);
int skb_shift(struct sk_buff *tgt, struct sk_buff *skb, int shiftlen);
void skb_scrub_packet(struct sk_buff *skb, bool xnet);
struct sk_buff *skb_segment(struct sk_buff *skb, netdev_features_t features);
struct sk_buff *skb_segment_list(struct sk_buff *skb, netdev_features_t features,
				 unsigned int offset);
struct sk_buff *skb_vlan_untag(struct sk_buff *skb);
int skb_ensure_writable(struct sk_buff *skb, unsigned int write_len);
int __skb_vlan_pop(struct sk_buff *skb, u16 *vlan_tci);
int skb_vlan_pop(struct sk_buff *skb);
int skb_vlan_push(struct sk_buff *skb, __be16 vlan_proto, u16 vlan_tci);
int skb_eth_pop(struct sk_buff *skb);
int skb_eth_push(struct sk_buff *skb, const unsigned char *dst,
		 const unsigned char *src);
int skb_mpls_push(struct sk_buff *skb, __be32 mpls_lse, __be16 mpls_proto,
		  int mac_len, bool ethernet);
int skb_mpls_pop(struct sk_buff *skb, __be16 next_proto, int mac_len,
		 bool ethernet);
int skb_mpls_update_lse(struct sk_buff *skb, __be32 mpls_lse);
int skb_mpls_dec_ttl(struct sk_buff *skb);
struct sk_buff *pskb_extract(struct sk_buff *skb, int off, int to_copy,
			     gfp_t gfp);

static inline int memcpy_from_msg(void *data, struct msghdr *msg, int len)
{
	return copy_from_iter_full(data, len, &msg->msg_iter) ? 0 : -EFAULT;
}

static inline int memcpy_to_msg(struct msghdr *msg, void *data, int len)
{
	return copy_to_iter(data, len, &msg->msg_iter) == len ? 0 : -EFAULT;
}

struct skb_checksum_ops {
	__wsum (*update)(const void *mem, int len, __wsum wsum);
	__wsum (*combine)(__wsum csum, __wsum csum2, int offset, int len);
};

extern const struct skb_checksum_ops *crc32c_csum_stub __read_mostly;

__wsum __skb_checksum(const struct sk_buff *skb, int offset, int len,
		      __wsum csum, const struct skb_checksum_ops *ops);
__wsum skb_checksum(const struct sk_buff *skb, int offset, int len,
		    __wsum csum);

static inline void * __must_check
__skb_header_pointer(const struct sk_buff *skb, int offset, int len,
		     const void *data, int hlen, void *buffer)
{
	if (likely(hlen - offset >= len))
		return (void *)data + offset;

	if (!skb || unlikely(skb_copy_bits(skb, offset, buffer, len) < 0))
		return NULL;

	return buffer;
}

static inline void * __must_check
skb_header_pointer(const struct sk_buff *skb, int offset, int len, void *buffer)
{
	return __skb_header_pointer(skb, offset, len, skb->data,
				    skb_headlen(skb), buffer);
}

static inline void * __must_check
skb_pointer_if_linear(const struct sk_buff *skb, int offset, int len)
{
	if (likely(skb_headlen(skb) - offset >= len))
		return skb->data + offset;
	return NULL;
}


static inline bool skb_needs_linearize(struct sk_buff *skb,
				       netdev_features_t features)
{
	return skb_is_nonlinear(skb) &&
	       ((skb_has_frag_list(skb) && !(features & NETIF_F_FRAGLIST)) ||
		(skb_shinfo(skb)->nr_frags && !(features & NETIF_F_SG)));
}

static inline void skb_copy_from_linear_data(const struct sk_buff *skb,
					     void *to,
					     const unsigned int len)
{
	memcpy(to, skb->data, len);
}

static inline void skb_copy_from_linear_data_offset(const struct sk_buff *skb,
						    const int offset, void *to,
						    const unsigned int len)
{
	memcpy(to, skb->data + offset, len);
}

static inline void skb_copy_to_linear_data(struct sk_buff *skb,
					   const void *from,
					   const unsigned int len)
{
	memcpy(skb->data, from, len);
}

static inline void skb_copy_to_linear_data_offset(struct sk_buff *skb,
						  const int offset,
						  const void *from,
						  const unsigned int len)
{
	memcpy(skb->data + offset, from, len);
}

void skb_init(void);

static inline ktime_t skb_get_ktime(const struct sk_buff *skb)
{
	return skb->tstamp;
}


static inline void skb_get_timestamp(const struct sk_buff *skb,
				     struct __kernel_old_timeval *stamp)
{
	*stamp = ns_to_kernel_old_timeval(skb->tstamp);
}

static inline void skb_get_new_timestamp(const struct sk_buff *skb,
					 struct __kernel_sock_timeval *stamp)
{
	struct timespec64 ts = ktime_to_timespec64(skb->tstamp);

	stamp->tv_sec = ts.tv_sec;
	stamp->tv_usec = ts.tv_nsec / 1000;
}

static inline void skb_get_timestampns(const struct sk_buff *skb,
				       struct __kernel_old_timespec *stamp)
{
	struct timespec64 ts = ktime_to_timespec64(skb->tstamp);

	stamp->tv_sec = ts.tv_sec;
	stamp->tv_nsec = ts.tv_nsec;
}

static inline void skb_get_new_timestampns(const struct sk_buff *skb,
					   struct __kernel_timespec *stamp)
{
	struct timespec64 ts = ktime_to_timespec64(skb->tstamp);

	stamp->tv_sec = ts.tv_sec;
	stamp->tv_nsec = ts.tv_nsec;
}

static inline void __net_timestamp(struct sk_buff *skb)
{
	skb->tstamp = ktime_get_real();
	skb->mono_delivery_time = 0;
}

static inline ktime_t net_timedelta(ktime_t t)
{
	return ktime_sub(ktime_get_real(), t);
}

static inline void skb_set_delivery_time(struct sk_buff *skb, ktime_t kt,
					 bool mono)
{
	skb->tstamp = kt;
	skb->mono_delivery_time = kt && mono;
}

DECLARE_STATIC_KEY_FALSE(netstamp_needed_key);


static inline void skb_clear_delivery_time(struct sk_buff *skb)
{
	if (skb->mono_delivery_time) {
		skb->mono_delivery_time = 0;
		if (static_branch_unlikely(&netstamp_needed_key))
			skb->tstamp = ktime_get_real();
		else
			skb->tstamp = 0;
	}
}

static inline void skb_clear_tstamp(struct sk_buff *skb)
{
	if (skb->mono_delivery_time)
		return;

	skb->tstamp = 0;
}

static inline ktime_t skb_tstamp(const struct sk_buff *skb)
{
	if (skb->mono_delivery_time)
		return 0;

	return skb->tstamp;
}

static inline ktime_t skb_tstamp_cond(const struct sk_buff *skb, bool cond)
{
	if (!skb->mono_delivery_time && skb->tstamp)
		return skb->tstamp;

	if (static_branch_unlikely(&netstamp_needed_key) || cond)
		return ktime_get_real();

	return 0;
}

static inline u8 skb_metadata_len(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->meta_len;
}

static inline void *skb_metadata_end(const struct sk_buff *skb)
{
	return skb_mac_header(skb);
}

static inline bool __skb_metadata_differs(const struct sk_buff *skb_a,
					  const struct sk_buff *skb_b,
					  u8 meta_len)
{
	const void *a = skb_metadata_end(skb_a);
	const void *b = skb_metadata_end(skb_b);
	
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS) && BITS_PER_LONG == 64
	u64 diffs = 0;

	switch (meta_len) {
#define __it(x, op) (x -= sizeof(u##op))
#define __it_diff(a, b, op) (*(u##op *)__it(a, op)) ^ (*(u##op *)__it(b, op))
	case 32: diffs |= __it_diff(a, b, 64);
		fallthrough;
	case 24: diffs |= __it_diff(a, b, 64);
		fallthrough;
	case 16: diffs |= __it_diff(a, b, 64);
		fallthrough;
	case  8: diffs |= __it_diff(a, b, 64);
		break;
	case 28: diffs |= __it_diff(a, b, 64);
		fallthrough;
	case 20: diffs |= __it_diff(a, b, 64);
		fallthrough;
	case 12: diffs |= __it_diff(a, b, 64);
		fallthrough;
	case  4: diffs |= __it_diff(a, b, 32);
		break;
	}
	return diffs;
#else
	return memcmp(a - meta_len, b - meta_len, meta_len);
#endif
}

static inline bool skb_metadata_differs(const struct sk_buff *skb_a,
					const struct sk_buff *skb_b)
{
	u8 len_a = skb_metadata_len(skb_a);
	u8 len_b = skb_metadata_len(skb_b);

	if (!(len_a | len_b))
		return false;

	return len_a != len_b ?
	       true : __skb_metadata_differs(skb_a, skb_b, len_a);
}

static inline void skb_metadata_set(struct sk_buff *skb, u8 meta_len)
{
	skb_shinfo(skb)->meta_len = meta_len;
}

static inline void skb_metadata_clear(struct sk_buff *skb)
{
	skb_metadata_set(skb, 0);
}

struct sk_buff *skb_clone_sk(struct sk_buff *skb);

#ifdef CONFIG_NETWORK_PHY_TIMESTAMPING

void skb_clone_tx_timestamp(struct sk_buff *skb);
bool skb_defer_rx_timestamp(struct sk_buff *skb);

#else 

static inline void skb_clone_tx_timestamp(struct sk_buff *skb)
{
}

static inline bool skb_defer_rx_timestamp(struct sk_buff *skb)
{
	return false;
}

#endif 


void skb_complete_tx_timestamp(struct sk_buff *skb,
			       struct skb_shared_hwtstamps *hwtstamps);

void __skb_tstamp_tx(struct sk_buff *orig_skb, const struct sk_buff *ack_skb,
		     struct skb_shared_hwtstamps *hwtstamps,
		     struct sock *sk, int tstype);


void skb_tstamp_tx(struct sk_buff *orig_skb,
		   struct skb_shared_hwtstamps *hwtstamps);


static inline void skb_tx_timestamp(struct sk_buff *skb)
{
	skb_clone_tx_timestamp(skb);
	if (skb_shinfo(skb)->tx_flags & SKBTX_SW_TSTAMP)
		skb_tstamp_tx(skb, NULL);
}


void skb_complete_wifi_ack(struct sk_buff *skb, bool acked);

__sum16 __skb_checksum_complete_head(struct sk_buff *skb, int len);
__sum16 __skb_checksum_complete(struct sk_buff *skb);

static inline int skb_csum_unnecessary(const struct sk_buff *skb)
{
	return ((skb->ip_summed == CHECKSUM_UNNECESSARY) ||
		skb->csum_valid ||
		(skb->ip_summed == CHECKSUM_PARTIAL &&
		 skb_checksum_start_offset(skb) >= 0));
}


static inline __sum16 skb_checksum_complete(struct sk_buff *skb)
{
	return skb_csum_unnecessary(skb) ?
	       0 : __skb_checksum_complete(skb);
}

static inline void __skb_decr_checksum_unnecessary(struct sk_buff *skb)
{
	if (skb->ip_summed == CHECKSUM_UNNECESSARY) {
		if (skb->csum_level == 0)
			skb->ip_summed = CHECKSUM_NONE;
		else
			skb->csum_level--;
	}
}

static inline void __skb_incr_checksum_unnecessary(struct sk_buff *skb)
{
	if (skb->ip_summed == CHECKSUM_UNNECESSARY) {
		if (skb->csum_level < SKB_MAX_CSUM_LEVEL)
			skb->csum_level++;
	} else if (skb->ip_summed == CHECKSUM_NONE) {
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		skb->csum_level = 0;
	}
}

static inline void __skb_reset_checksum_unnecessary(struct sk_buff *skb)
{
	if (skb->ip_summed == CHECKSUM_UNNECESSARY) {
		skb->ip_summed = CHECKSUM_NONE;
		skb->csum_level = 0;
	}
}


static inline bool __skb_checksum_validate_needed(struct sk_buff *skb,
						  bool zero_okay,
						  __sum16 check)
{
	if (skb_csum_unnecessary(skb) || (zero_okay && !check)) {
		skb->csum_valid = 1;
		__skb_decr_checksum_unnecessary(skb);
		return false;
	}

	return true;
}


#define CHECKSUM_BREAK 76


static inline void skb_checksum_complete_unset(struct sk_buff *skb)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->ip_summed = CHECKSUM_NONE;
}


static inline __sum16 __skb_checksum_validate_complete(struct sk_buff *skb,
						       bool complete,
						       __wsum psum)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE) {
		if (!csum_fold(csum_add(psum, skb->csum))) {
			skb->csum_valid = 1;
			return 0;
		}
	}

	skb->csum = psum;

	if (complete || skb->len <= CHECKSUM_BREAK) {
		__sum16 csum;

		csum = __skb_checksum_complete(skb);
		skb->csum_valid = !csum;
		return csum;
	}

	return 0;
}

static inline __wsum null_compute_pseudo(struct sk_buff *skb, int proto)
{
	return 0;
}


#define __skb_checksum_validate(skb, proto, complete,			\
				zero_okay, check, compute_pseudo)	\
({									\
	__sum16 __ret = 0;						\
	skb->csum_valid = 0;						\
	if (__skb_checksum_validate_needed(skb, zero_okay, check))	\
		__ret = __skb_checksum_validate_complete(skb,		\
				complete, compute_pseudo(skb, proto));	\
	__ret;								\
})

#define skb_checksum_init(skb, proto, compute_pseudo)			\
	__skb_checksum_validate(skb, proto, false, false, 0, compute_pseudo)

#define skb_checksum_init_zero_check(skb, proto, check, compute_pseudo)	\
	__skb_checksum_validate(skb, proto, false, true, check, compute_pseudo)

#define skb_checksum_validate(skb, proto, compute_pseudo)		\
	__skb_checksum_validate(skb, proto, true, false, 0, compute_pseudo)

#define skb_checksum_validate_zero_check(skb, proto, check,		\
					 compute_pseudo)		\
	__skb_checksum_validate(skb, proto, true, true, check, compute_pseudo)

#define skb_checksum_simple_validate(skb)				\
	__skb_checksum_validate(skb, 0, true, false, 0, null_compute_pseudo)

static inline bool __skb_checksum_convert_check(struct sk_buff *skb)
{
	return (skb->ip_summed == CHECKSUM_NONE && skb->csum_valid);
}

static inline void __skb_checksum_convert(struct sk_buff *skb, __wsum pseudo)
{
	skb->csum = ~pseudo;
	skb->ip_summed = CHECKSUM_COMPLETE;
}

#define skb_checksum_try_convert(skb, proto, compute_pseudo)	\
do {									\
	if (__skb_checksum_convert_check(skb))				\
		__skb_checksum_convert(skb, compute_pseudo(skb, proto)); \
} while (0)

static inline void skb_remcsum_adjust_partial(struct sk_buff *skb, void *ptr,
					      u16 start, u16 offset)
{
	skb->ip_summed = CHECKSUM_PARTIAL;
	skb->csum_start = ((unsigned char *)ptr + start) - skb->head;
	skb->csum_offset = offset - start;
}


static inline void skb_remcsum_process(struct sk_buff *skb, void *ptr,
				       int start, int offset, bool nopartial)
{
	__wsum delta;

	if (!nopartial) {
		skb_remcsum_adjust_partial(skb, ptr, start, offset);
		return;
	}

	if (unlikely(skb->ip_summed != CHECKSUM_COMPLETE)) {
		__skb_checksum_complete(skb);
		skb_postpull_rcsum(skb, skb->data, ptr - (void *)skb->data);
	}

	delta = remcsum_adjust(ptr, skb->csum, start, offset);

	
	skb->csum = csum_add(skb->csum, delta);
}

static inline struct nf_conntrack *skb_nfct(const struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	return (void *)(skb->_nfct & NFCT_PTRMASK);
#else
	return NULL;
#endif
}

static inline unsigned long skb_get_nfct(const struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	return skb->_nfct;
#else
	return 0UL;
#endif
}

static inline void skb_set_nfct(struct sk_buff *skb, unsigned long nfct)
{
#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	skb->slow_gro |= !!nfct;
	skb->_nfct = nfct;
#endif
}

#ifdef CONFIG_SKB_EXTENSIONS
enum skb_ext_id {
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	SKB_EXT_BRIDGE_NF,
#endif
#ifdef CONFIG_XFRM
	SKB_EXT_SEC_PATH,
#endif
#if IS_ENABLED(CONFIG_NET_TC_SKB_EXT)
	TC_SKB_EXT,
#endif
#if IS_ENABLED(CONFIG_MPTCP)
	SKB_EXT_MPTCP,
#endif
#if IS_ENABLED(CONFIG_MCTP_FLOWS)
	SKB_EXT_MCTP,
#endif
	SKB_EXT_NUM, 
};


struct skb_ext {
	refcount_t refcnt;
	u8 offset[SKB_EXT_NUM]; 
	u8 chunks;		
	char data[] __aligned(8);
};

struct skb_ext *__skb_ext_alloc(gfp_t flags);
void *__skb_ext_set(struct sk_buff *skb, enum skb_ext_id id,
		    struct skb_ext *ext);
void *skb_ext_add(struct sk_buff *skb, enum skb_ext_id id);
void __skb_ext_del(struct sk_buff *skb, enum skb_ext_id id);
void __skb_ext_put(struct skb_ext *ext);

static inline void skb_ext_put(struct sk_buff *skb)
{
	if (skb->active_extensions)
		__skb_ext_put(skb->extensions);
}

static inline void __skb_ext_copy(struct sk_buff *dst,
				  const struct sk_buff *src)
{
	dst->active_extensions = src->active_extensions;

	if (src->active_extensions) {
		struct skb_ext *ext = src->extensions;

		refcount_inc(&ext->refcnt);
		dst->extensions = ext;
	}
}

static inline void skb_ext_copy(struct sk_buff *dst, const struct sk_buff *src)
{
	skb_ext_put(dst);
	__skb_ext_copy(dst, src);
}

static inline bool __skb_ext_exist(const struct skb_ext *ext, enum skb_ext_id i)
{
	return !!ext->offset[i];
}

static inline bool skb_ext_exist(const struct sk_buff *skb, enum skb_ext_id id)
{
	return skb->active_extensions & (1 << id);
}

static inline void skb_ext_del(struct sk_buff *skb, enum skb_ext_id id)
{
	if (skb_ext_exist(skb, id))
		__skb_ext_del(skb, id);
}

static inline void *skb_ext_find(const struct sk_buff *skb, enum skb_ext_id id)
{
	if (skb_ext_exist(skb, id)) {
		struct skb_ext *ext = skb->extensions;

		return (void *)ext + (ext->offset[id] << 3);
	}

	return NULL;
}

static inline void skb_ext_reset(struct sk_buff *skb)
{
	if (unlikely(skb->active_extensions)) {
		__skb_ext_put(skb->extensions);
		skb->active_extensions = 0;
	}
}

static inline bool skb_has_extensions(struct sk_buff *skb)
{
	return unlikely(skb->active_extensions);
}
#else
static inline void skb_ext_put(struct sk_buff *skb) {}
static inline void skb_ext_reset(struct sk_buff *skb) {}
static inline void skb_ext_del(struct sk_buff *skb, int unused) {}
static inline void __skb_ext_copy(struct sk_buff *d, const struct sk_buff *s) {}
static inline void skb_ext_copy(struct sk_buff *dst, const struct sk_buff *s) {}
static inline bool skb_has_extensions(struct sk_buff *skb) { return false; }
#endif 

static inline void nf_reset_ct(struct sk_buff *skb)
{
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_put(skb_nfct(skb));
	skb->_nfct = 0;
#endif
}

static inline void nf_reset_trace(struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_NETFILTER_XT_TARGET_TRACE) || IS_ENABLED(CONFIG_NF_TABLES)
	skb->nf_trace = 0;
#endif
}

static inline void ipvs_reset(struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_IP_VS)
	skb->ipvs_property = 0;
#endif
}


static inline void __nf_copy(struct sk_buff *dst, const struct sk_buff *src,
			     bool copy)
{
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	dst->_nfct = src->_nfct;
	nf_conntrack_get(skb_nfct(src));
#endif
#if IS_ENABLED(CONFIG_NETFILTER_XT_TARGET_TRACE) || IS_ENABLED(CONFIG_NF_TABLES)
	if (copy)
		dst->nf_trace = src->nf_trace;
#endif
}

static inline void nf_copy(struct sk_buff *dst, const struct sk_buff *src)
{
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_put(skb_nfct(dst));
#endif
	dst->slow_gro = src->slow_gro;
	__nf_copy(dst, src, true);
}

#ifdef CONFIG_NETWORK_SECMARK
static inline void skb_copy_secmark(struct sk_buff *to, const struct sk_buff *from)
{
	to->secmark = from->secmark;
}

static inline void skb_init_secmark(struct sk_buff *skb)
{
	skb->secmark = 0;
}
#else
static inline void skb_copy_secmark(struct sk_buff *to, const struct sk_buff *from)
{ }

static inline void skb_init_secmark(struct sk_buff *skb)
{ }
#endif

static inline int secpath_exists(const struct sk_buff *skb)
{
#ifdef CONFIG_XFRM
	return skb_ext_exist(skb, SKB_EXT_SEC_PATH);
#else
	return 0;
#endif
}

static inline bool skb_irq_freeable(const struct sk_buff *skb)
{
	return !skb->destructor &&
		!secpath_exists(skb) &&
		!skb_nfct(skb) &&
		!skb->_skb_refdst &&
		!skb_has_frag_list(skb);
}

static inline void skb_set_queue_mapping(struct sk_buff *skb, u16 queue_mapping)
{
	skb->queue_mapping = queue_mapping;
}

static inline u16 skb_get_queue_mapping(const struct sk_buff *skb)
{
	return skb->queue_mapping;
}

static inline void skb_copy_queue_mapping(struct sk_buff *to, const struct sk_buff *from)
{
	to->queue_mapping = from->queue_mapping;
}

static inline void skb_record_rx_queue(struct sk_buff *skb, u16 rx_queue)
{
	skb->queue_mapping = rx_queue + 1;
}

static inline u16 skb_get_rx_queue(const struct sk_buff *skb)
{
	return skb->queue_mapping - 1;
}

static inline bool skb_rx_queue_recorded(const struct sk_buff *skb)
{
	return skb->queue_mapping != 0;
}

static inline void skb_set_dst_pending_confirm(struct sk_buff *skb, u32 val)
{
	skb->dst_pending_confirm = val;
}

static inline bool skb_get_dst_pending_confirm(const struct sk_buff *skb)
{
	return skb->dst_pending_confirm != 0;
}

static inline struct sec_path *skb_sec_path(const struct sk_buff *skb)
{
#ifdef CONFIG_XFRM
	return skb_ext_find(skb, SKB_EXT_SEC_PATH);
#else
	return NULL;
#endif
}

static inline bool skb_is_gso(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_size;
}


static inline bool skb_is_gso_v6(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6;
}


static inline bool skb_is_gso_sctp(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_type & SKB_GSO_SCTP;
}


static inline bool skb_is_gso_tcp(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_type & (SKB_GSO_TCPV4 | SKB_GSO_TCPV6);
}

static inline void skb_gso_reset(struct sk_buff *skb)
{
	skb_shinfo(skb)->gso_size = 0;
	skb_shinfo(skb)->gso_segs = 0;
	skb_shinfo(skb)->gso_type = 0;
}

static inline void skb_increase_gso_size(struct skb_shared_info *shinfo,
					 u16 increment)
{
	if (WARN_ON_ONCE(shinfo->gso_size == GSO_BY_FRAGS))
		return;
	shinfo->gso_size += increment;
}

static inline void skb_decrease_gso_size(struct skb_shared_info *shinfo,
					 u16 decrement)
{
	if (WARN_ON_ONCE(shinfo->gso_size == GSO_BY_FRAGS))
		return;
	shinfo->gso_size -= decrement;
}

void __skb_warn_lro_forwarding(const struct sk_buff *skb);

static inline bool skb_warn_if_lro(const struct sk_buff *skb)
{
	
	const struct skb_shared_info *shinfo = skb_shinfo(skb);

	if (skb_is_nonlinear(skb) && shinfo->gso_size != 0 &&
	    unlikely(shinfo->gso_type == 0)) {
		__skb_warn_lro_forwarding(skb);
		return true;
	}
	return false;
}

static inline void skb_forward_csum(struct sk_buff *skb)
{
	
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->ip_summed = CHECKSUM_NONE;
}


static inline void skb_checksum_none_assert(const struct sk_buff *skb)
{
	DEBUG_NET_WARN_ON_ONCE(skb->ip_summed != CHECKSUM_NONE);
}

bool skb_partial_csum_set(struct sk_buff *skb, u16 start, u16 off);

int skb_checksum_setup(struct sk_buff *skb, bool recalculate);
struct sk_buff *skb_checksum_trimmed(struct sk_buff *skb,
				     unsigned int transport_len,
				     __sum16(*skb_chkf)(struct sk_buff *skb));


static inline bool skb_head_is_locked(const struct sk_buff *skb)
{
	return !skb->head_frag || skb_cloned(skb);
}


static inline __wsum lco_csum(struct sk_buff *skb)
{
	unsigned char *csum_start = skb_checksum_start(skb);
	unsigned char *l4_hdr = skb_transport_header(skb);
	__wsum partial;

	
	partial = ~csum_unfold(*(__force __sum16 *)(csum_start +
						    skb->csum_offset));

	
	return csum_partial(l4_hdr, csum_start - l4_hdr, partial);
}

static inline bool skb_is_redirected(const struct sk_buff *skb)
{
	return skb->redirected;
}

static inline void skb_set_redirected(struct sk_buff *skb, bool from_ingress)
{
	skb->redirected = 1;
#ifdef CONFIG_NET_REDIRECT
	skb->from_ingress = from_ingress;
	if (skb->from_ingress)
		skb_clear_tstamp(skb);
#endif
}

static inline void skb_reset_redirect(struct sk_buff *skb)
{
	skb->redirected = 0;
}

static inline void skb_set_redirected_noclear(struct sk_buff *skb,
					      bool from_ingress)
{
	skb->redirected = 1;
#ifdef CONFIG_NET_REDIRECT
	skb->from_ingress = from_ingress;
#endif
}

static inline bool skb_csum_is_sctp(struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_IP_SCTP)
	return skb->csum_not_inet;
#else
	return 0;
#endif
}

static inline void skb_reset_csum_not_inet(struct sk_buff *skb)
{
	skb->ip_summed = CHECKSUM_NONE;
#if IS_ENABLED(CONFIG_IP_SCTP)
	skb->csum_not_inet = 0;
#endif
}

static inline void skb_set_kcov_handle(struct sk_buff *skb,
				       const u64 kcov_handle)
{
#ifdef CONFIG_KCOV
	skb->kcov_handle = kcov_handle;
#endif
}

static inline u64 skb_get_kcov_handle(struct sk_buff *skb)
{
#ifdef CONFIG_KCOV
	return skb->kcov_handle;
#else
	return 0;
#endif
}

static inline void skb_mark_for_recycle(struct sk_buff *skb)
{
#ifdef CONFIG_PAGE_POOL
	skb->pp_recycle = 1;
#endif
}

ssize_t skb_splice_from_iter(struct sk_buff *skb, struct iov_iter *iter,
			     ssize_t maxsize, gfp_t gfp);

#endif	
#endif	
