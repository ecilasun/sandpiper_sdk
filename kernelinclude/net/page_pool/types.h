/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _NET_PAGE_POOL_TYPES_H
#define _NET_PAGE_POOL_TYPES_H

#include <linux/dma-direction.h>
#include <linux/ptr_ring.h>

#define PP_FLAG_DMA_MAP		BIT(0) 
#define PP_FLAG_DMA_SYNC_DEV	BIT(1) 
#define PP_FLAG_PAGE_FRAG	BIT(2) 
#define PP_FLAG_ALL		(PP_FLAG_DMA_MAP |\
				 PP_FLAG_DMA_SYNC_DEV |\
				 PP_FLAG_PAGE_FRAG)


#define PP_ALLOC_CACHE_SIZE	128
#define PP_ALLOC_CACHE_REFILL	64
struct pp_alloc_cache {
	u32 count;
	struct page *cache[PP_ALLOC_CACHE_SIZE];
};


struct page_pool_params {
	unsigned int	flags;
	unsigned int	order;
	unsigned int	pool_size;
	int		nid;
	struct device	*dev;
	struct napi_struct *napi;
	enum dma_data_direction dma_dir;
	unsigned int	max_len;
	unsigned int	offset;

	void (*init_callback)(struct page *page, void *arg);
	void *init_arg;
};

#ifdef CONFIG_PAGE_POOL_STATS

struct page_pool_alloc_stats {
	u64 fast;
	u64 slow;
	u64 slow_high_order;
	u64 empty;
	u64 refill;
	u64 waive;
};


struct page_pool_recycle_stats {
	u64 cached;
	u64 cache_full;
	u64 ring;
	u64 ring_full;
	u64 released_refcnt;
};


struct page_pool_stats {
	struct page_pool_alloc_stats alloc_stats;
	struct page_pool_recycle_stats recycle_stats;
};
#endif

struct page_pool {
	struct page_pool_params p;

	long frag_users;
	struct page *frag_page;
	unsigned int frag_offset;
	u32 pages_state_hold_cnt;

	struct delayed_work release_dw;
	void (*disconnect)(void *pool);
	unsigned long defer_start;
	unsigned long defer_warn;

#ifdef CONFIG_PAGE_POOL_STATS
	
	struct page_pool_alloc_stats alloc_stats;
#endif
	u32 xdp_mem_id;

	
	struct pp_alloc_cache alloc ____cacheline_aligned_in_smp;

	
	struct ptr_ring ring;

#ifdef CONFIG_PAGE_POOL_STATS
	
	struct page_pool_recycle_stats __percpu *recycle_stats;
#endif
	atomic_t pages_state_release_cnt;

	
	refcount_t user_cnt;

	u64 destroy_cnt;
};

struct page *page_pool_alloc_pages(struct page_pool *pool, gfp_t gfp);
struct page *page_pool_alloc_frag(struct page_pool *pool, unsigned int *offset,
				  unsigned int size, gfp_t gfp);
struct page_pool *page_pool_create(const struct page_pool_params *params);

struct xdp_mem_info;

#ifdef CONFIG_PAGE_POOL
void page_pool_unlink_napi(struct page_pool *pool);
void page_pool_destroy(struct page_pool *pool);
void page_pool_use_xdp_mem(struct page_pool *pool, void (*disconnect)(void *),
			   struct xdp_mem_info *mem);
void page_pool_put_page_bulk(struct page_pool *pool, void **data,
			     int count);
#else
static inline void page_pool_unlink_napi(struct page_pool *pool)
{
}

static inline void page_pool_destroy(struct page_pool *pool)
{
}

static inline void page_pool_use_xdp_mem(struct page_pool *pool,
					 void (*disconnect)(void *),
					 struct xdp_mem_info *mem)
{
}

static inline void page_pool_put_page_bulk(struct page_pool *pool, void **data,
					   int count)
{
}
#endif

void page_pool_put_defragged_page(struct page_pool *pool, struct page *page,
				  unsigned int dma_sync_size,
				  bool allow_direct);

static inline bool is_page_pool_compiled_in(void)
{
#ifdef CONFIG_PAGE_POOL
	return true;
#else
	return false;
#endif
}


void page_pool_update_nid(struct page_pool *pool, int new_nid);

#endif 
