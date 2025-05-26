

#ifndef _TTM_RESOURCE_H_
#define _TTM_RESOURCE_H_

#include <linux/types.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/iosys-map.h>
#include <linux/dma-fence.h>

#include <drm/drm_print.h>
#include <drm/ttm/ttm_caching.h>
#include <drm/ttm/ttm_kmap_iter.h>

#define TTM_MAX_BO_PRIORITY	4U
#define TTM_NUM_MEM_TYPES 8

struct ttm_device;
struct ttm_resource_manager;
struct ttm_resource;
struct ttm_place;
struct ttm_buffer_object;
struct ttm_placement;
struct iosys_map;
struct io_mapping;
struct sg_table;
struct scatterlist;

struct ttm_resource_manager_func {
	
	int  (*alloc)(struct ttm_resource_manager *man,
		      struct ttm_buffer_object *bo,
		      const struct ttm_place *place,
		      struct ttm_resource **res);

	
	void (*free)(struct ttm_resource_manager *man,
		     struct ttm_resource *res);

	
	bool (*intersects)(struct ttm_resource_manager *man,
			   struct ttm_resource *res,
			   const struct ttm_place *place,
			   size_t size);

	
	bool (*compatible)(struct ttm_resource_manager *man,
			   struct ttm_resource *res,
			   const struct ttm_place *place,
			   size_t size);

	
	void (*debug)(struct ttm_resource_manager *man,
		      struct drm_printer *printer);
};


struct ttm_resource_manager {
	
	bool use_type;
	bool use_tt;
	struct ttm_device *bdev;
	uint64_t size;
	const struct ttm_resource_manager_func *func;
	spinlock_t move_lock;

	
	struct dma_fence *move;

	
	struct list_head lru[TTM_MAX_BO_PRIORITY];

	
	uint64_t usage;
};


struct ttm_bus_placement {
	void			*addr;
	phys_addr_t		offset;
	bool			is_iomem;
	enum ttm_caching	caching;
};


struct ttm_resource {
	unsigned long start;
	size_t size;
	uint32_t mem_type;
	uint32_t placement;
	struct ttm_bus_placement bus;
	struct ttm_buffer_object *bo;

	
	struct list_head lru;
};


struct ttm_resource_cursor {
	unsigned int priority;
};


struct ttm_lru_bulk_move_pos {
	struct ttm_resource *first;
	struct ttm_resource *last;
};


struct ttm_lru_bulk_move {
	struct ttm_lru_bulk_move_pos pos[TTM_NUM_MEM_TYPES][TTM_MAX_BO_PRIORITY];
};


struct ttm_kmap_iter_iomap {
	struct ttm_kmap_iter base;
	struct io_mapping *iomap;
	struct sg_table *st;
	resource_size_t start;
	struct {
		struct scatterlist *sg;
		pgoff_t i;
		pgoff_t end;
		pgoff_t offs;
	} cache;
};


struct ttm_kmap_iter_linear_io {
	struct ttm_kmap_iter base;
	struct iosys_map dmap;
	bool needs_unmap;
};


static inline void
ttm_resource_manager_set_used(struct ttm_resource_manager *man, bool used)
{
	int i;

	for (i = 0; i < TTM_MAX_BO_PRIORITY; i++)
		WARN_ON(!list_empty(&man->lru[i]));
	man->use_type = used;
}


static inline bool ttm_resource_manager_used(struct ttm_resource_manager *man)
{
	return man->use_type;
}


static inline void
ttm_resource_manager_cleanup(struct ttm_resource_manager *man)
{
	dma_fence_put(man->move);
	man->move = NULL;
}

void ttm_lru_bulk_move_init(struct ttm_lru_bulk_move *bulk);
void ttm_lru_bulk_move_tail(struct ttm_lru_bulk_move *bulk);

void ttm_resource_add_bulk_move(struct ttm_resource *res,
				struct ttm_buffer_object *bo);
void ttm_resource_del_bulk_move(struct ttm_resource *res,
				struct ttm_buffer_object *bo);
void ttm_resource_move_to_lru_tail(struct ttm_resource *res);

void ttm_resource_init(struct ttm_buffer_object *bo,
                       const struct ttm_place *place,
                       struct ttm_resource *res);
void ttm_resource_fini(struct ttm_resource_manager *man,
		       struct ttm_resource *res);

int ttm_resource_alloc(struct ttm_buffer_object *bo,
		       const struct ttm_place *place,
		       struct ttm_resource **res);
void ttm_resource_free(struct ttm_buffer_object *bo, struct ttm_resource **res);
bool ttm_resource_intersects(struct ttm_device *bdev,
			     struct ttm_resource *res,
			     const struct ttm_place *place,
			     size_t size);
bool ttm_resource_compatible(struct ttm_device *bdev,
			     struct ttm_resource *res,
			     const struct ttm_place *place,
			     size_t size);
bool ttm_resource_compat(struct ttm_resource *res,
			 struct ttm_placement *placement);
void ttm_resource_set_bo(struct ttm_resource *res,
			 struct ttm_buffer_object *bo);

void ttm_resource_manager_init(struct ttm_resource_manager *man,
			       struct ttm_device *bdev,
			       uint64_t size);

int ttm_resource_manager_evict_all(struct ttm_device *bdev,
				   struct ttm_resource_manager *man);

uint64_t ttm_resource_manager_usage(struct ttm_resource_manager *man);
void ttm_resource_manager_debug(struct ttm_resource_manager *man,
				struct drm_printer *p);

struct ttm_resource *
ttm_resource_manager_first(struct ttm_resource_manager *man,
			   struct ttm_resource_cursor *cursor);
struct ttm_resource *
ttm_resource_manager_next(struct ttm_resource_manager *man,
			  struct ttm_resource_cursor *cursor,
			  struct ttm_resource *res);


#define ttm_resource_manager_for_each_res(man, cursor, res)		\
	for (res = ttm_resource_manager_first(man, cursor); res;	\
	     res = ttm_resource_manager_next(man, cursor, res))

struct ttm_kmap_iter *
ttm_kmap_iter_iomap_init(struct ttm_kmap_iter_iomap *iter_io,
			 struct io_mapping *iomap,
			 struct sg_table *st,
			 resource_size_t start);

struct ttm_kmap_iter_linear_io;

struct ttm_kmap_iter *
ttm_kmap_iter_linear_io_init(struct ttm_kmap_iter_linear_io *iter_io,
			     struct ttm_device *bdev,
			     struct ttm_resource *mem);

void ttm_kmap_iter_linear_io_fini(struct ttm_kmap_iter_linear_io *iter_io,
				  struct ttm_device *bdev,
				  struct ttm_resource *mem);

void ttm_resource_manager_create_debugfs(struct ttm_resource_manager *man,
					 struct dentry * parent,
					 const char *name);
#endif
