/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRM_GPUVA_MGR_H__
#define __DRM_GPUVA_MGR_H__



#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/types.h>

#include <drm/drm_gem.h>

struct drm_gpuva_manager;
struct drm_gpuva_fn_ops;


enum drm_gpuva_flags {
	
	DRM_GPUVA_INVALIDATED = (1 << 0),

	
	DRM_GPUVA_SPARSE = (1 << 1),

	
	DRM_GPUVA_USERBITS = (1 << 2),
};


struct drm_gpuva {
	
	struct drm_gpuva_manager *mgr;

	
	enum drm_gpuva_flags flags;

	
	struct {
		
		u64 addr;

		
		u64 range;
	} va;

	
	struct {
		
		u64 offset;

		
		struct drm_gem_object *obj;

		
		struct list_head entry;
	} gem;

	
	struct {
		
		struct rb_node node;

		
		struct list_head entry;

		
		u64 __subtree_last;
	} rb;
};

int drm_gpuva_insert(struct drm_gpuva_manager *mgr, struct drm_gpuva *va);
void drm_gpuva_remove(struct drm_gpuva *va);

void drm_gpuva_link(struct drm_gpuva *va);
void drm_gpuva_unlink(struct drm_gpuva *va);

struct drm_gpuva *drm_gpuva_find(struct drm_gpuva_manager *mgr,
				 u64 addr, u64 range);
struct drm_gpuva *drm_gpuva_find_first(struct drm_gpuva_manager *mgr,
				       u64 addr, u64 range);
struct drm_gpuva *drm_gpuva_find_prev(struct drm_gpuva_manager *mgr, u64 start);
struct drm_gpuva *drm_gpuva_find_next(struct drm_gpuva_manager *mgr, u64 end);

bool drm_gpuva_interval_empty(struct drm_gpuva_manager *mgr, u64 addr, u64 range);

static inline void drm_gpuva_init(struct drm_gpuva *va, u64 addr, u64 range,
				  struct drm_gem_object *obj, u64 offset)
{
	va->va.addr = addr;
	va->va.range = range;
	va->gem.obj = obj;
	va->gem.offset = offset;
}


static inline void drm_gpuva_invalidate(struct drm_gpuva *va, bool invalidate)
{
	if (invalidate)
		va->flags |= DRM_GPUVA_INVALIDATED;
	else
		va->flags &= ~DRM_GPUVA_INVALIDATED;
}


static inline bool drm_gpuva_invalidated(struct drm_gpuva *va)
{
	return va->flags & DRM_GPUVA_INVALIDATED;
}


struct drm_gpuva_manager {
	
	const char *name;

	
	u64 mm_start;

	
	u64 mm_range;

	
	struct {
		
		struct rb_root_cached tree;

		
		struct list_head list;
	} rb;

	
	struct drm_gpuva kernel_alloc_node;

	
	const struct drm_gpuva_fn_ops *ops;
};

void drm_gpuva_manager_init(struct drm_gpuva_manager *mgr,
			    const char *name,
			    u64 start_offset, u64 range,
			    u64 reserve_offset, u64 reserve_range,
			    const struct drm_gpuva_fn_ops *ops);
void drm_gpuva_manager_destroy(struct drm_gpuva_manager *mgr);

static inline struct drm_gpuva *
__drm_gpuva_next(struct drm_gpuva *va)
{
	if (va && !list_is_last(&va->rb.entry, &va->mgr->rb.list))
		return list_next_entry(va, rb.entry);

	return NULL;
}


#define drm_gpuva_for_each_va_range(va__, mgr__, start__, end__) \
	for (va__ = drm_gpuva_find_first((mgr__), (start__), (end__) - (start__)); \
	     va__ && (va__->va.addr < (end__)); \
	     va__ = __drm_gpuva_next(va__))


#define drm_gpuva_for_each_va_range_safe(va__, next__, mgr__, start__, end__) \
	for (va__ = drm_gpuva_find_first((mgr__), (start__), (end__) - (start__)), \
	     next__ = __drm_gpuva_next(va__); \
	     va__ && (va__->va.addr < (end__)); \
	     va__ = next__, next__ = __drm_gpuva_next(va__))


#define drm_gpuva_for_each_va(va__, mgr__) \
	list_for_each_entry(va__, &(mgr__)->rb.list, rb.entry)


#define drm_gpuva_for_each_va_safe(va__, next__, mgr__) \
	list_for_each_entry_safe(va__, next__, &(mgr__)->rb.list, rb.entry)


enum drm_gpuva_op_type {
	
	DRM_GPUVA_OP_MAP,

	
	DRM_GPUVA_OP_REMAP,

	
	DRM_GPUVA_OP_UNMAP,

	
	DRM_GPUVA_OP_PREFETCH,
};


struct drm_gpuva_op_map {
	
	struct {
		
		u64 addr;

		
		u64 range;
	} va;

	
	struct {
		
		u64 offset;

		
		struct drm_gem_object *obj;
	} gem;
};


struct drm_gpuva_op_unmap {
	
	struct drm_gpuva *va;

	
	bool keep;
};


struct drm_gpuva_op_remap {
	
	struct drm_gpuva_op_map *prev;

	
	struct drm_gpuva_op_map *next;

	
	struct drm_gpuva_op_unmap *unmap;
};


struct drm_gpuva_op_prefetch {
	
	struct drm_gpuva *va;
};


struct drm_gpuva_op {
	
	struct list_head entry;

	
	enum drm_gpuva_op_type op;

	union {
		
		struct drm_gpuva_op_map map;

		
		struct drm_gpuva_op_remap remap;

		
		struct drm_gpuva_op_unmap unmap;

		
		struct drm_gpuva_op_prefetch prefetch;
	};
};


struct drm_gpuva_ops {
	
	struct list_head list;
};


#define drm_gpuva_for_each_op(op, ops) list_for_each_entry(op, &(ops)->list, entry)


#define drm_gpuva_for_each_op_safe(op, next, ops) \
	list_for_each_entry_safe(op, next, &(ops)->list, entry)


#define drm_gpuva_for_each_op_from_reverse(op, ops) \
	list_for_each_entry_from_reverse(op, &(ops)->list, entry)


#define drm_gpuva_first_op(ops) \
	list_first_entry(&(ops)->list, struct drm_gpuva_op, entry)


#define drm_gpuva_last_op(ops) \
	list_last_entry(&(ops)->list, struct drm_gpuva_op, entry)


#define drm_gpuva_prev_op(op) list_prev_entry(op, entry)


#define drm_gpuva_next_op(op) list_next_entry(op, entry)

struct drm_gpuva_ops *
drm_gpuva_sm_map_ops_create(struct drm_gpuva_manager *mgr,
			    u64 addr, u64 range,
			    struct drm_gem_object *obj, u64 offset);
struct drm_gpuva_ops *
drm_gpuva_sm_unmap_ops_create(struct drm_gpuva_manager *mgr,
			      u64 addr, u64 range);

struct drm_gpuva_ops *
drm_gpuva_prefetch_ops_create(struct drm_gpuva_manager *mgr,
				 u64 addr, u64 range);

struct drm_gpuva_ops *
drm_gpuva_gem_unmap_ops_create(struct drm_gpuva_manager *mgr,
			       struct drm_gem_object *obj);

void drm_gpuva_ops_free(struct drm_gpuva_manager *mgr,
			struct drm_gpuva_ops *ops);

static inline void drm_gpuva_init_from_op(struct drm_gpuva *va,
					  struct drm_gpuva_op_map *op)
{
	drm_gpuva_init(va, op->va.addr, op->va.range,
		       op->gem.obj, op->gem.offset);
}


struct drm_gpuva_fn_ops {
	
	struct drm_gpuva_op *(*op_alloc)(void);

	
	void (*op_free)(struct drm_gpuva_op *op);

	
	int (*sm_step_map)(struct drm_gpuva_op *op, void *priv);

	
	int (*sm_step_remap)(struct drm_gpuva_op *op, void *priv);

	
	int (*sm_step_unmap)(struct drm_gpuva_op *op, void *priv);
};

int drm_gpuva_sm_map(struct drm_gpuva_manager *mgr, void *priv,
		     u64 addr, u64 range,
		     struct drm_gem_object *obj, u64 offset);

int drm_gpuva_sm_unmap(struct drm_gpuva_manager *mgr, void *priv,
		       u64 addr, u64 range);

void drm_gpuva_map(struct drm_gpuva_manager *mgr,
		   struct drm_gpuva *va,
		   struct drm_gpuva_op_map *op);

void drm_gpuva_remap(struct drm_gpuva *prev,
		     struct drm_gpuva *next,
		     struct drm_gpuva_op_remap *op);

void drm_gpuva_unmap(struct drm_gpuva_op_unmap *op);

#endif 
