

#ifndef _TTM_DEVICE_H_
#define _TTM_DEVICE_H_

#include <linux/types.h>
#include <linux/workqueue.h>
#include <drm/ttm/ttm_resource.h>
#include <drm/ttm/ttm_pool.h>

struct ttm_device;
struct ttm_placement;
struct ttm_buffer_object;
struct ttm_operation_ctx;


extern struct ttm_global {

	
	struct page *dummy_read_page;

	
	struct list_head device_list;

	
	atomic_t bo_count;
} ttm_glob;

struct ttm_device_funcs {
	
	struct ttm_tt *(*ttm_tt_create)(struct ttm_buffer_object *bo,
					uint32_t page_flags);

	
	int (*ttm_tt_populate)(struct ttm_device *bdev,
			       struct ttm_tt *ttm,
			       struct ttm_operation_ctx *ctx);

	
	void (*ttm_tt_unpopulate)(struct ttm_device *bdev,
				  struct ttm_tt *ttm);

	
	void (*ttm_tt_destroy)(struct ttm_device *bdev, struct ttm_tt *ttm);

	
	bool (*eviction_valuable)(struct ttm_buffer_object *bo,
				  const struct ttm_place *place);
	

	void (*evict_flags)(struct ttm_buffer_object *bo,
			    struct ttm_placement *placement);

	
	int (*move)(struct ttm_buffer_object *bo, bool evict,
		    struct ttm_operation_ctx *ctx,
		    struct ttm_resource *new_mem,
		    struct ttm_place *hop);

	
	void (*delete_mem_notify)(struct ttm_buffer_object *bo);

	
	void (*swap_notify)(struct ttm_buffer_object *bo);

	
	int (*io_mem_reserve)(struct ttm_device *bdev,
			      struct ttm_resource *mem);
	void (*io_mem_free)(struct ttm_device *bdev,
			    struct ttm_resource *mem);

	
	unsigned long (*io_mem_pfn)(struct ttm_buffer_object *bo,
				    unsigned long page_offset);

	
	int (*access_memory)(struct ttm_buffer_object *bo, unsigned long offset,
			     void *buf, int len, int write);

	
	void (*release_notify)(struct ttm_buffer_object *bo);
};


struct ttm_device {
	
	struct list_head device_list;

	
	const struct ttm_device_funcs *funcs;

	
	struct ttm_resource_manager sysman;

	
	struct ttm_resource_manager *man_drv[TTM_NUM_MEM_TYPES];

	
	struct drm_vma_offset_manager *vma_manager;

	
	struct ttm_pool pool;

	
	spinlock_t lru_lock;

	
	struct list_head pinned;

	
	struct address_space *dev_mapping;

	
	struct workqueue_struct *wq;
};

int ttm_global_swapout(struct ttm_operation_ctx *ctx, gfp_t gfp_flags);
int ttm_device_swapout(struct ttm_device *bdev, struct ttm_operation_ctx *ctx,
		       gfp_t gfp_flags);

static inline struct ttm_resource_manager *
ttm_manager_type(struct ttm_device *bdev, int mem_type)
{
	BUILD_BUG_ON(__builtin_constant_p(mem_type)
		     && mem_type >= TTM_NUM_MEM_TYPES);
	return bdev->man_drv[mem_type];
}

static inline void ttm_set_driver_manager(struct ttm_device *bdev, int type,
					  struct ttm_resource_manager *manager)
{
	BUILD_BUG_ON(__builtin_constant_p(type) && type >= TTM_NUM_MEM_TYPES);
	bdev->man_drv[type] = manager;
}

int ttm_device_init(struct ttm_device *bdev, const struct ttm_device_funcs *funcs,
		    struct device *dev, struct address_space *mapping,
		    struct drm_vma_offset_manager *vma_manager,
		    bool use_dma_alloc, bool use_dma32);
void ttm_device_fini(struct ttm_device *bdev);
void ttm_device_clear_dma_mappings(struct ttm_device *bdev);

#endif
