/* SPDX-License-Identifier: GPL-2.0 OR MIT */


#ifndef _TTM_PAGE_POOL_H_
#define _TTM_PAGE_POOL_H_

#include <linux/mmzone.h>
#include <linux/llist.h>
#include <linux/spinlock.h>
#include <drm/ttm/ttm_caching.h>

struct device;
struct ttm_tt;
struct ttm_pool;
struct ttm_operation_ctx;


struct ttm_pool_type {
	struct ttm_pool *pool;
	unsigned int order;
	enum ttm_caching caching;

	struct list_head shrinker_list;

	spinlock_t lock;
	struct list_head pages;
};


struct ttm_pool {
	struct device *dev;
	int nid;

	bool use_dma_alloc;
	bool use_dma32;

	struct {
		struct ttm_pool_type orders[NR_PAGE_ORDERS];
	} caching[TTM_NUM_CACHING_TYPES];
};

int ttm_pool_alloc(struct ttm_pool *pool, struct ttm_tt *tt,
		   struct ttm_operation_ctx *ctx);
void ttm_pool_free(struct ttm_pool *pool, struct ttm_tt *tt);

void ttm_pool_init(struct ttm_pool *pool, struct device *dev,
		   int nid, bool use_dma_alloc, bool use_dma32);
void ttm_pool_fini(struct ttm_pool *pool);

int ttm_pool_debugfs(struct ttm_pool *pool, struct seq_file *m);

int ttm_pool_mgr_init(unsigned long num_pages);
void ttm_pool_mgr_fini(void);

#endif
