
#ifndef _TTM_TT_H_
#define _TTM_TT_H_

#include <linux/pagemap.h>
#include <linux/types.h>
#include <drm/ttm/ttm_caching.h>
#include <drm/ttm/ttm_kmap_iter.h>

struct ttm_device;
struct ttm_tt;
struct ttm_resource;
struct ttm_buffer_object;
struct ttm_operation_ctx;


struct ttm_tt {
	
	struct page **pages;
	
#define TTM_TT_FLAG_SWAPPED		BIT(0)
#define TTM_TT_FLAG_ZERO_ALLOC		BIT(1)
#define TTM_TT_FLAG_EXTERNAL		BIT(2)
#define TTM_TT_FLAG_EXTERNAL_MAPPABLE	BIT(3)
#define TTM_TT_FLAG_DECRYPTED		BIT(4)

#define TTM_TT_FLAG_PRIV_POPULATED	BIT(5)
	uint32_t page_flags;
	
	uint32_t num_pages;
	
	struct sg_table *sg;
	
	dma_addr_t *dma_address;
	
	struct file *swap_storage;
	
	enum ttm_caching caching;
};


struct ttm_kmap_iter_tt {
	struct ttm_kmap_iter base;
	struct ttm_tt *tt;
	pgprot_t prot;
};

static inline bool ttm_tt_is_populated(struct ttm_tt *tt)
{
	return tt->page_flags & TTM_TT_FLAG_PRIV_POPULATED;
}


int ttm_tt_create(struct ttm_buffer_object *bo, bool zero_alloc);


int ttm_tt_init(struct ttm_tt *ttm, struct ttm_buffer_object *bo,
		uint32_t page_flags, enum ttm_caching caching,
		unsigned long extra_pages);
int ttm_sg_tt_init(struct ttm_tt *ttm_dma, struct ttm_buffer_object *bo,
		   uint32_t page_flags, enum ttm_caching caching);


void ttm_tt_fini(struct ttm_tt *ttm);


void ttm_tt_destroy(struct ttm_device *bdev, struct ttm_tt *ttm);


int ttm_tt_swapin(struct ttm_tt *ttm);
int ttm_tt_swapout(struct ttm_device *bdev, struct ttm_tt *ttm,
		   gfp_t gfp_flags);


int ttm_tt_populate(struct ttm_device *bdev, struct ttm_tt *ttm,
		    struct ttm_operation_ctx *ctx);


void ttm_tt_unpopulate(struct ttm_device *bdev, struct ttm_tt *ttm);


static inline void ttm_tt_mark_for_clear(struct ttm_tt *ttm)
{
	ttm->page_flags |= TTM_TT_FLAG_ZERO_ALLOC;
}

void ttm_tt_mgr_init(unsigned long num_pages, unsigned long num_dma32_pages);

struct ttm_kmap_iter *ttm_kmap_iter_tt_init(struct ttm_kmap_iter_tt *iter_tt,
					    struct ttm_tt *tt);
unsigned long ttm_tt_pages_limit(void);
#if IS_ENABLED(CONFIG_AGP)
#include <linux/agp_backend.h>


struct ttm_tt *ttm_agp_tt_create(struct ttm_buffer_object *bo,
				 struct agp_bridge_data *bridge,
				 uint32_t page_flags);
int ttm_agp_bind(struct ttm_tt *ttm, struct ttm_resource *bo_mem);
void ttm_agp_unbind(struct ttm_tt *ttm);
void ttm_agp_destroy(struct ttm_tt *ttm);
bool ttm_agp_is_bound(struct ttm_tt *ttm);
#endif

#endif
