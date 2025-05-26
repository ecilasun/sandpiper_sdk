/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_NET_XDP_H__
#define __LINUX_NET_XDP_H__

#include <linux/bitfield.h>
#include <linux/filter.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h> 



enum xdp_mem_type {
	MEM_TYPE_PAGE_SHARED = 0, 
	MEM_TYPE_PAGE_ORDER0,     
	MEM_TYPE_PAGE_POOL,
	MEM_TYPE_XSK_BUFF_POOL,
	MEM_TYPE_MAX,
};


#define XDP_XMIT_FLUSH		(1U << 0)	
#define XDP_XMIT_FLAGS_MASK	XDP_XMIT_FLUSH

struct xdp_mem_info {
	u32 type; 
	u32 id;
};

struct page_pool;

struct xdp_rxq_info {
	struct net_device *dev;
	u32 queue_index;
	u32 reg_state;
	struct xdp_mem_info mem;
	unsigned int napi_id;
	u32 frag_size;
} ____cacheline_aligned; 

struct xdp_txq_info {
	struct net_device *dev;
};

enum xdp_buff_flags {
	XDP_FLAGS_HAS_FRAGS		= BIT(0), 
	XDP_FLAGS_FRAGS_PF_MEMALLOC	= BIT(1), 
};

struct xdp_buff {
	void *data;
	void *data_end;
	void *data_meta;
	void *data_hard_start;
	struct xdp_rxq_info *rxq;
	struct xdp_txq_info *txq;
	u32 frame_sz; 
	u32 flags; 
};

static __always_inline bool xdp_buff_has_frags(struct xdp_buff *xdp)
{
	return !!(xdp->flags & XDP_FLAGS_HAS_FRAGS);
}

static __always_inline void xdp_buff_set_frags_flag(struct xdp_buff *xdp)
{
	xdp->flags |= XDP_FLAGS_HAS_FRAGS;
}

static __always_inline void xdp_buff_clear_frags_flag(struct xdp_buff *xdp)
{
	xdp->flags &= ~XDP_FLAGS_HAS_FRAGS;
}

static __always_inline bool xdp_buff_is_frag_pfmemalloc(struct xdp_buff *xdp)
{
	return !!(xdp->flags & XDP_FLAGS_FRAGS_PF_MEMALLOC);
}

static __always_inline void xdp_buff_set_frag_pfmemalloc(struct xdp_buff *xdp)
{
	xdp->flags |= XDP_FLAGS_FRAGS_PF_MEMALLOC;
}

static __always_inline void
xdp_init_buff(struct xdp_buff *xdp, u32 frame_sz, struct xdp_rxq_info *rxq)
{
	xdp->frame_sz = frame_sz;
	xdp->rxq = rxq;
	xdp->flags = 0;
}

static __always_inline void
xdp_prepare_buff(struct xdp_buff *xdp, unsigned char *hard_start,
		 int headroom, int data_len, const bool meta_valid)
{
	unsigned char *data = hard_start + headroom;

	xdp->data_hard_start = hard_start;
	xdp->data = data;
	xdp->data_end = data + data_len;
	xdp->data_meta = meta_valid ? data : data + 1;
}


#define xdp_data_hard_end(xdp)				\
	((xdp)->data_hard_start + (xdp)->frame_sz -	\
	 SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

static inline struct skb_shared_info *
xdp_get_shared_info_from_buff(struct xdp_buff *xdp)
{
	return (struct skb_shared_info *)xdp_data_hard_end(xdp);
}

static __always_inline unsigned int xdp_get_buff_len(struct xdp_buff *xdp)
{
	unsigned int len = xdp->data_end - xdp->data;
	struct skb_shared_info *sinfo;

	if (likely(!xdp_buff_has_frags(xdp)))
		goto out;

	sinfo = xdp_get_shared_info_from_buff(xdp);
	len += sinfo->xdp_frags_size;
out:
	return len;
}

struct xdp_frame {
	void *data;
	u16 len;
	u16 headroom;
	u32 metasize; 
	
	struct xdp_mem_info mem;
	struct net_device *dev_rx; 
	u32 frame_sz;
	u32 flags; 
};

static __always_inline bool xdp_frame_has_frags(struct xdp_frame *frame)
{
	return !!(frame->flags & XDP_FLAGS_HAS_FRAGS);
}

static __always_inline bool xdp_frame_is_frag_pfmemalloc(struct xdp_frame *frame)
{
	return !!(frame->flags & XDP_FLAGS_FRAGS_PF_MEMALLOC);
}

#define XDP_BULK_QUEUE_SIZE	16
struct xdp_frame_bulk {
	int count;
	void *xa;
	void *q[XDP_BULK_QUEUE_SIZE];
};

static __always_inline void xdp_frame_bulk_init(struct xdp_frame_bulk *bq)
{
	
	bq->xa = NULL;
}

static inline struct skb_shared_info *
xdp_get_shared_info_from_frame(struct xdp_frame *frame)
{
	void *data_hard_start = frame->data - frame->headroom - sizeof(*frame);

	return (struct skb_shared_info *)(data_hard_start + frame->frame_sz -
				SKB_DATA_ALIGN(sizeof(struct skb_shared_info)));
}

struct xdp_cpumap_stats {
	unsigned int redirect;
	unsigned int pass;
	unsigned int drop;
};


static inline void xdp_scrub_frame(struct xdp_frame *frame)
{
	frame->data = NULL;
	frame->dev_rx = NULL;
}

static inline void
xdp_update_skb_shared_info(struct sk_buff *skb, u8 nr_frags,
			   unsigned int size, unsigned int truesize,
			   bool pfmemalloc)
{
	skb_shinfo(skb)->nr_frags = nr_frags;

	skb->len += size;
	skb->data_len += size;
	skb->truesize += truesize;
	skb->pfmemalloc |= pfmemalloc;
}


void xdp_warn(const char *msg, const char *func, const int line);
#define XDP_WARN(msg) xdp_warn(msg, __func__, __LINE__)

struct xdp_frame *xdp_convert_zc_to_xdp_frame(struct xdp_buff *xdp);
struct sk_buff *__xdp_build_skb_from_frame(struct xdp_frame *xdpf,
					   struct sk_buff *skb,
					   struct net_device *dev);
struct sk_buff *xdp_build_skb_from_frame(struct xdp_frame *xdpf,
					 struct net_device *dev);
int xdp_alloc_skb_bulk(void **skbs, int n_skb, gfp_t gfp);
struct xdp_frame *xdpf_clone(struct xdp_frame *xdpf);

static inline
void xdp_convert_frame_to_buff(struct xdp_frame *frame, struct xdp_buff *xdp)
{
	xdp->data_hard_start = frame->data - frame->headroom - sizeof(*frame);
	xdp->data = frame->data;
	xdp->data_end = frame->data + frame->len;
	xdp->data_meta = frame->data - frame->metasize;
	xdp->frame_sz = frame->frame_sz;
	xdp->flags = frame->flags;
}

static inline
int xdp_update_frame_from_buff(struct xdp_buff *xdp,
			       struct xdp_frame *xdp_frame)
{
	int metasize, headroom;

	
	headroom = xdp->data - xdp->data_hard_start;
	metasize = xdp->data - xdp->data_meta;
	metasize = metasize > 0 ? metasize : 0;
	if (unlikely((headroom - metasize) < sizeof(*xdp_frame)))
		return -ENOSPC;

	
	if (unlikely(xdp->data_end > xdp_data_hard_end(xdp))) {
		XDP_WARN("Driver BUG: missing reserved tailroom");
		return -ENOSPC;
	}

	xdp_frame->data = xdp->data;
	xdp_frame->len  = xdp->data_end - xdp->data;
	xdp_frame->headroom = headroom - sizeof(*xdp_frame);
	xdp_frame->metasize = metasize;
	xdp_frame->frame_sz = xdp->frame_sz;
	xdp_frame->flags = xdp->flags;

	return 0;
}


static inline
struct xdp_frame *xdp_convert_buff_to_frame(struct xdp_buff *xdp)
{
	struct xdp_frame *xdp_frame;

	if (xdp->rxq->mem.type == MEM_TYPE_XSK_BUFF_POOL)
		return xdp_convert_zc_to_xdp_frame(xdp);

	
	xdp_frame = xdp->data_hard_start;
	if (unlikely(xdp_update_frame_from_buff(xdp, xdp_frame) < 0))
		return NULL;

	
	xdp_frame->mem = xdp->rxq->mem;

	return xdp_frame;
}

void __xdp_return(void *data, struct xdp_mem_info *mem, bool napi_direct,
		  struct xdp_buff *xdp);
void xdp_return_frame(struct xdp_frame *xdpf);
void xdp_return_frame_rx_napi(struct xdp_frame *xdpf);
void xdp_return_buff(struct xdp_buff *xdp);
void xdp_flush_frame_bulk(struct xdp_frame_bulk *bq);
void xdp_return_frame_bulk(struct xdp_frame *xdpf,
			   struct xdp_frame_bulk *bq);

static __always_inline unsigned int xdp_get_frame_len(struct xdp_frame *xdpf)
{
	struct skb_shared_info *sinfo;
	unsigned int len = xdpf->len;

	if (likely(!xdp_frame_has_frags(xdpf)))
		goto out;

	sinfo = xdp_get_shared_info_from_frame(xdpf);
	len += sinfo->xdp_frags_size;
out:
	return len;
}

int __xdp_rxq_info_reg(struct xdp_rxq_info *xdp_rxq,
		       struct net_device *dev, u32 queue_index,
		       unsigned int napi_id, u32 frag_size);
static inline int
xdp_rxq_info_reg(struct xdp_rxq_info *xdp_rxq,
		 struct net_device *dev, u32 queue_index,
		 unsigned int napi_id)
{
	return __xdp_rxq_info_reg(xdp_rxq, dev, queue_index, napi_id, 0);
}

void xdp_rxq_info_unreg(struct xdp_rxq_info *xdp_rxq);
void xdp_rxq_info_unused(struct xdp_rxq_info *xdp_rxq);
bool xdp_rxq_info_is_reg(struct xdp_rxq_info *xdp_rxq);
int xdp_rxq_info_reg_mem_model(struct xdp_rxq_info *xdp_rxq,
			       enum xdp_mem_type type, void *allocator);
void xdp_rxq_info_unreg_mem_model(struct xdp_rxq_info *xdp_rxq);
int xdp_reg_mem_model(struct xdp_mem_info *mem,
		      enum xdp_mem_type type, void *allocator);
void xdp_unreg_mem_model(struct xdp_mem_info *mem);


static __always_inline void
xdp_set_data_meta_invalid(struct xdp_buff *xdp)
{
	xdp->data_meta = xdp->data + 1;
}

static __always_inline bool
xdp_data_meta_unsupported(const struct xdp_buff *xdp)
{
	return unlikely(xdp->data_meta > xdp->data);
}

static inline bool xdp_metalen_invalid(unsigned long metalen)
{
	return (metalen & (sizeof(__u32) - 1)) || (metalen > 32);
}

struct xdp_attachment_info {
	struct bpf_prog *prog;
	u32 flags;
};

struct netdev_bpf;
void xdp_attachment_setup(struct xdp_attachment_info *info,
			  struct netdev_bpf *bpf);

#define DEV_MAP_BULK_SIZE XDP_BULK_QUEUE_SIZE

#define XDP_METADATA_KFUNC_xxx	\
	XDP_METADATA_KFUNC(XDP_METADATA_KFUNC_RX_TIMESTAMP, \
			   bpf_xdp_metadata_rx_timestamp) \
	XDP_METADATA_KFUNC(XDP_METADATA_KFUNC_RX_HASH, \
			   bpf_xdp_metadata_rx_hash) \

enum {
#define XDP_METADATA_KFUNC(name, _) name,
XDP_METADATA_KFUNC_xxx
#undef XDP_METADATA_KFUNC
MAX_XDP_METADATA_KFUNC,
};

enum xdp_rss_hash_type {
	
	XDP_RSS_L3_IPV4		= BIT(0),
	XDP_RSS_L3_IPV6		= BIT(1),

	
	XDP_RSS_L3_DYNHDR	= BIT(2),

	
	XDP_RSS_L4		= BIT(3), 
	XDP_RSS_L4_TCP		= BIT(4),
	XDP_RSS_L4_UDP		= BIT(5),
	XDP_RSS_L4_SCTP		= BIT(6),
	XDP_RSS_L4_IPSEC	= BIT(7), 

	
	XDP_RSS_TYPE_NONE            = 0,
	XDP_RSS_TYPE_L2              = XDP_RSS_TYPE_NONE,

	XDP_RSS_TYPE_L3_IPV4         = XDP_RSS_L3_IPV4,
	XDP_RSS_TYPE_L3_IPV6         = XDP_RSS_L3_IPV6,
	XDP_RSS_TYPE_L3_IPV4_OPT     = XDP_RSS_L3_IPV4 | XDP_RSS_L3_DYNHDR,
	XDP_RSS_TYPE_L3_IPV6_EX      = XDP_RSS_L3_IPV6 | XDP_RSS_L3_DYNHDR,

	XDP_RSS_TYPE_L4_ANY          = XDP_RSS_L4,
	XDP_RSS_TYPE_L4_IPV4_TCP     = XDP_RSS_L3_IPV4 | XDP_RSS_L4 | XDP_RSS_L4_TCP,
	XDP_RSS_TYPE_L4_IPV4_UDP     = XDP_RSS_L3_IPV4 | XDP_RSS_L4 | XDP_RSS_L4_UDP,
	XDP_RSS_TYPE_L4_IPV4_SCTP    = XDP_RSS_L3_IPV4 | XDP_RSS_L4 | XDP_RSS_L4_SCTP,
	XDP_RSS_TYPE_L4_IPV4_IPSEC   = XDP_RSS_L3_IPV4 | XDP_RSS_L4 | XDP_RSS_L4_IPSEC,

	XDP_RSS_TYPE_L4_IPV6_TCP     = XDP_RSS_L3_IPV6 | XDP_RSS_L4 | XDP_RSS_L4_TCP,
	XDP_RSS_TYPE_L4_IPV6_UDP     = XDP_RSS_L3_IPV6 | XDP_RSS_L4 | XDP_RSS_L4_UDP,
	XDP_RSS_TYPE_L4_IPV6_SCTP    = XDP_RSS_L3_IPV6 | XDP_RSS_L4 | XDP_RSS_L4_SCTP,
	XDP_RSS_TYPE_L4_IPV6_IPSEC   = XDP_RSS_L3_IPV6 | XDP_RSS_L4 | XDP_RSS_L4_IPSEC,

	XDP_RSS_TYPE_L4_IPV6_TCP_EX  = XDP_RSS_TYPE_L4_IPV6_TCP  | XDP_RSS_L3_DYNHDR,
	XDP_RSS_TYPE_L4_IPV6_UDP_EX  = XDP_RSS_TYPE_L4_IPV6_UDP  | XDP_RSS_L3_DYNHDR,
	XDP_RSS_TYPE_L4_IPV6_SCTP_EX = XDP_RSS_TYPE_L4_IPV6_SCTP | XDP_RSS_L3_DYNHDR,
};

struct xdp_metadata_ops {
	int	(*xmo_rx_timestamp)(const struct xdp_md *ctx, u64 *timestamp);
	int	(*xmo_rx_hash)(const struct xdp_md *ctx, u32 *hash,
			       enum xdp_rss_hash_type *rss_type);
};

#ifdef CONFIG_NET
u32 bpf_xdp_metadata_kfunc_id(int id);
bool bpf_dev_bound_kfunc_id(u32 btf_id);
void xdp_set_features_flag(struct net_device *dev, xdp_features_t val);
void xdp_features_set_redirect_target(struct net_device *dev, bool support_sg);
void xdp_features_clear_redirect_target(struct net_device *dev);
#else
static inline u32 bpf_xdp_metadata_kfunc_id(int id) { return 0; }
static inline bool bpf_dev_bound_kfunc_id(u32 btf_id) { return false; }

static inline void
xdp_set_features_flag(struct net_device *dev, xdp_features_t val)
{
}

static inline void
xdp_features_set_redirect_target(struct net_device *dev, bool support_sg)
{
}

static inline void
xdp_features_clear_redirect_target(struct net_device *dev)
{
}
#endif

static inline void xdp_clear_features_flag(struct net_device *dev)
{
	xdp_set_features_flag(dev, 0);
}

static __always_inline u32 bpf_prog_run_xdp(const struct bpf_prog *prog,
					    struct xdp_buff *xdp)
{
	
	u32 act = __bpf_prog_run(prog, xdp, BPF_DISPATCHER_FUNC(xdp));

	if (static_branch_unlikely(&bpf_master_redirect_enabled_key)) {
		if (act == XDP_TX && netif_is_bond_slave(xdp->rxq->dev))
			act = xdp_master_redirect(xdp);
	}

	return act;
}
#endif 
