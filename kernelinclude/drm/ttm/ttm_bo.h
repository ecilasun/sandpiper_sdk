


#ifndef _TTM_BO_API_H_
#define _TTM_BO_API_H_

#include <drm/drm_gem.h>

#include <linux/kref.h>
#include <linux/list.h>

#include "ttm_device.h"


#define TTM_BO_VM_NUM_PREFAULT 16

struct iosys_map;

struct ttm_global;
struct ttm_device;
struct ttm_placement;
struct ttm_place;
struct ttm_resource;
struct ttm_resource_manager;
struct ttm_tt;


enum ttm_bo_type {
	ttm_bo_type_device,
	ttm_bo_type_kernel,
	ttm_bo_type_sg
};


struct ttm_buffer_object {
	struct drm_gem_object base;

	
	struct ttm_device *bdev;
	enum ttm_bo_type type;
	uint32_t page_alignment;
	void (*destroy) (struct ttm_buffer_object *);

	
	struct kref kref;

	
	struct ttm_resource *resource;
	struct ttm_tt *ttm;
	bool deleted;
	struct ttm_lru_bulk_move *bulk_move;
	unsigned priority;
	unsigned pin_count;

	
	struct work_struct delayed_delete;

	
	struct sg_table *sg;
};


#define TTM_BO_MAP_IOMEM_MASK 0x80
struct ttm_bo_kmap_obj {
	void *virtual;
	struct page *page;
	enum {
		ttm_bo_map_iomap        = 1 | TTM_BO_MAP_IOMEM_MASK,
		ttm_bo_map_vmap         = 2,
		ttm_bo_map_kmap         = 3,
		ttm_bo_map_premapped    = 4 | TTM_BO_MAP_IOMEM_MASK,
	} bo_kmap_type;
	struct ttm_buffer_object *bo;
};


struct ttm_operation_ctx {
	bool interruptible;
	bool no_wait_gpu;
	bool gfp_retry_mayfail;
	bool allow_res_evict;
	bool force_alloc;
	struct dma_resv *resv;
	uint64_t bytes_moved;
};


static inline void ttm_bo_get(struct ttm_buffer_object *bo)
{
	kref_get(&bo->kref);
}


static inline __must_check struct ttm_buffer_object *
ttm_bo_get_unless_zero(struct ttm_buffer_object *bo)
{
	if (!kref_get_unless_zero(&bo->kref))
		return NULL;
	return bo;
}


static inline int ttm_bo_reserve(struct ttm_buffer_object *bo,
				 bool interruptible, bool no_wait,
				 struct ww_acquire_ctx *ticket)
{
	int ret = 0;

	if (no_wait) {
		bool success;

		if (WARN_ON(ticket))
			return -EBUSY;

		success = dma_resv_trylock(bo->base.resv);
		return success ? 0 : -EBUSY;
	}

	if (interruptible)
		ret = dma_resv_lock_interruptible(bo->base.resv, ticket);
	else
		ret = dma_resv_lock(bo->base.resv, ticket);
	if (ret == -EINTR)
		return -ERESTARTSYS;
	return ret;
}


static inline int ttm_bo_reserve_slowpath(struct ttm_buffer_object *bo,
					  bool interruptible,
					  struct ww_acquire_ctx *ticket)
{
	if (interruptible) {
		int ret = dma_resv_lock_slow_interruptible(bo->base.resv,
							   ticket);
		if (ret == -EINTR)
			ret = -ERESTARTSYS;
		return ret;
	}
	dma_resv_lock_slow(bo->base.resv, ticket);
	return 0;
}

void ttm_bo_move_to_lru_tail(struct ttm_buffer_object *bo);

static inline void
ttm_bo_move_to_lru_tail_unlocked(struct ttm_buffer_object *bo)
{
	spin_lock(&bo->bdev->lru_lock);
	ttm_bo_move_to_lru_tail(bo);
	spin_unlock(&bo->bdev->lru_lock);
}

static inline void ttm_bo_assign_mem(struct ttm_buffer_object *bo,
				     struct ttm_resource *new_mem)
{
	WARN_ON(bo->resource);
	bo->resource = new_mem;
}


static inline void ttm_bo_move_null(struct ttm_buffer_object *bo,
				    struct ttm_resource *new_mem)
{
	ttm_resource_free(bo, &bo->resource);
	ttm_bo_assign_mem(bo, new_mem);
}


static inline void ttm_bo_unreserve(struct ttm_buffer_object *bo)
{
	ttm_bo_move_to_lru_tail_unlocked(bo);
	dma_resv_unlock(bo->base.resv);
}


static inline void *ttm_kmap_obj_virtual(struct ttm_bo_kmap_obj *map,
					 bool *is_iomem)
{
	*is_iomem = !!(map->bo_kmap_type & TTM_BO_MAP_IOMEM_MASK);
	return map->virtual;
}

int ttm_bo_wait_ctx(struct ttm_buffer_object *bo,
		    struct ttm_operation_ctx *ctx);
int ttm_bo_validate(struct ttm_buffer_object *bo,
		    struct ttm_placement *placement,
		    struct ttm_operation_ctx *ctx);
void ttm_bo_put(struct ttm_buffer_object *bo);
void ttm_bo_set_bulk_move(struct ttm_buffer_object *bo,
			  struct ttm_lru_bulk_move *bulk);
bool ttm_bo_eviction_valuable(struct ttm_buffer_object *bo,
			      const struct ttm_place *place);
int ttm_bo_init_reserved(struct ttm_device *bdev, struct ttm_buffer_object *bo,
			 enum ttm_bo_type type, struct ttm_placement *placement,
			 uint32_t alignment, struct ttm_operation_ctx *ctx,
			 struct sg_table *sg, struct dma_resv *resv,
			 void (*destroy)(struct ttm_buffer_object *));
int ttm_bo_init_validate(struct ttm_device *bdev, struct ttm_buffer_object *bo,
			 enum ttm_bo_type type, struct ttm_placement *placement,
			 uint32_t alignment, bool interruptible,
			 struct sg_table *sg, struct dma_resv *resv,
			 void (*destroy)(struct ttm_buffer_object *));
int ttm_bo_kmap(struct ttm_buffer_object *bo, unsigned long start_page,
		unsigned long num_pages, struct ttm_bo_kmap_obj *map);
void ttm_bo_kunmap(struct ttm_bo_kmap_obj *map);
int ttm_bo_vmap(struct ttm_buffer_object *bo, struct iosys_map *map);
void ttm_bo_vunmap(struct ttm_buffer_object *bo, struct iosys_map *map);
int ttm_bo_mmap_obj(struct vm_area_struct *vma, struct ttm_buffer_object *bo);
int ttm_bo_swapout(struct ttm_buffer_object *bo, struct ttm_operation_ctx *ctx,
		   gfp_t gfp_flags);
void ttm_bo_pin(struct ttm_buffer_object *bo);
void ttm_bo_unpin(struct ttm_buffer_object *bo);
int ttm_mem_evict_first(struct ttm_device *bdev,
			struct ttm_resource_manager *man,
			const struct ttm_place *place,
			struct ttm_operation_ctx *ctx,
			struct ww_acquire_ctx *ticket);
vm_fault_t ttm_bo_vm_reserve(struct ttm_buffer_object *bo,
			     struct vm_fault *vmf);
vm_fault_t ttm_bo_vm_fault_reserved(struct vm_fault *vmf,
				    pgprot_t prot,
				    pgoff_t num_prefault);
vm_fault_t ttm_bo_vm_fault(struct vm_fault *vmf);
void ttm_bo_vm_open(struct vm_area_struct *vma);
void ttm_bo_vm_close(struct vm_area_struct *vma);
int ttm_bo_vm_access(struct vm_area_struct *vma, unsigned long addr,
		     void *buf, int len, int write);
vm_fault_t ttm_bo_vm_dummy_page(struct vm_fault *vmf, pgprot_t prot);

int ttm_bo_mem_space(struct ttm_buffer_object *bo,
		     struct ttm_placement *placement,
		     struct ttm_resource **mem,
		     struct ttm_operation_ctx *ctx);

void ttm_bo_unmap_virtual(struct ttm_buffer_object *bo);

int ttm_mem_io_reserve(struct ttm_device *bdev,
		       struct ttm_resource *mem);
void ttm_mem_io_free(struct ttm_device *bdev,
		     struct ttm_resource *mem);
void ttm_move_memcpy(bool clear, u32 num_pages,
		     struct ttm_kmap_iter *dst_iter,
		     struct ttm_kmap_iter *src_iter);
int ttm_bo_move_memcpy(struct ttm_buffer_object *bo,
		       struct ttm_operation_ctx *ctx,
		       struct ttm_resource *new_mem);
int ttm_bo_move_accel_cleanup(struct ttm_buffer_object *bo,
			      struct dma_fence *fence, bool evict,
			      bool pipeline,
			      struct ttm_resource *new_mem);
void ttm_bo_move_sync_cleanup(struct ttm_buffer_object *bo,
			      struct ttm_resource *new_mem);
int ttm_bo_pipeline_gutting(struct ttm_buffer_object *bo);
pgprot_t ttm_io_prot(struct ttm_buffer_object *bo, struct ttm_resource *res,
		     pgprot_t tmp);
void ttm_bo_tt_destroy(struct ttm_buffer_object *bo);

#endif
