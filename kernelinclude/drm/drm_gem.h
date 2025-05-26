#ifndef __DRM_GEM_H__
#define __DRM_GEM_H__



#include <linux/kref.h>
#include <linux/dma-resv.h>
#include <linux/list.h>
#include <linux/mutex.h>

#include <drm/drm_vma_manager.h>

struct iosys_map;
struct drm_gem_object;


enum drm_gem_object_status {
	DRM_GEM_OBJECT_RESIDENT  = BIT(0),
	DRM_GEM_OBJECT_PURGEABLE = BIT(1),
};


struct drm_gem_object_funcs {
	
	void (*free)(struct drm_gem_object *obj);

	
	int (*open)(struct drm_gem_object *obj, struct drm_file *file);

	
	void (*close)(struct drm_gem_object *obj, struct drm_file *file);

	
	void (*print_info)(struct drm_printer *p, unsigned int indent,
			   const struct drm_gem_object *obj);

	
	struct dma_buf *(*export)(struct drm_gem_object *obj, int flags);

	
	int (*pin)(struct drm_gem_object *obj);

	
	void (*unpin)(struct drm_gem_object *obj);

	
	struct sg_table *(*get_sg_table)(struct drm_gem_object *obj);

	
	int (*vmap)(struct drm_gem_object *obj, struct iosys_map *map);

	
	void (*vunmap)(struct drm_gem_object *obj, struct iosys_map *map);

	
	int (*mmap)(struct drm_gem_object *obj, struct vm_area_struct *vma);

	
	int (*evict)(struct drm_gem_object *obj);

	
	enum drm_gem_object_status (*status)(struct drm_gem_object *obj);

	
	const struct vm_operations_struct *vm_ops;
};


struct drm_gem_lru {
	
	struct mutex *lock;

	
	long count;

	
	struct list_head list;
};


struct drm_gem_object {
	
	struct kref refcount;

	
	unsigned handle_count;

	
	struct drm_device *dev;

	
	struct file *filp;

	
	struct drm_vma_offset_node vma_node;

	
	size_t size;

	
	int name;

	
	struct dma_buf *dma_buf;

	
	struct dma_buf_attachment *import_attach;

	
	struct dma_resv *resv;

	
	struct dma_resv _resv;

	
	struct {
		struct list_head list;

#ifdef CONFIG_LOCKDEP
		struct lockdep_map *lock_dep_map;
#endif
	} gpuva;

	
	const struct drm_gem_object_funcs *funcs;

	
	struct list_head lru_node;

	
	struct drm_gem_lru *lru;
};


#define DRM_GEM_FOPS \
	.open		= drm_open,\
	.release	= drm_release,\
	.unlocked_ioctl	= drm_ioctl,\
	.compat_ioctl	= drm_compat_ioctl,\
	.poll		= drm_poll,\
	.read		= drm_read,\
	.llseek		= noop_llseek,\
	.mmap		= drm_gem_mmap


#define DEFINE_DRM_GEM_FOPS(name) \
	static const struct file_operations name = {\
		.owner		= THIS_MODULE,\
		DRM_GEM_FOPS,\
	}

void drm_gem_object_release(struct drm_gem_object *obj);
void drm_gem_object_free(struct kref *kref);
int drm_gem_object_init(struct drm_device *dev,
			struct drm_gem_object *obj, size_t size);
void drm_gem_private_object_init(struct drm_device *dev,
				 struct drm_gem_object *obj, size_t size);
void drm_gem_private_object_fini(struct drm_gem_object *obj);
void drm_gem_vm_open(struct vm_area_struct *vma);
void drm_gem_vm_close(struct vm_area_struct *vma);
int drm_gem_mmap_obj(struct drm_gem_object *obj, unsigned long obj_size,
		     struct vm_area_struct *vma);
int drm_gem_mmap(struct file *filp, struct vm_area_struct *vma);


static inline void drm_gem_object_get(struct drm_gem_object *obj)
{
	kref_get(&obj->refcount);
}

__attribute__((nonnull))
static inline void
__drm_gem_object_put(struct drm_gem_object *obj)
{
	kref_put(&obj->refcount, drm_gem_object_free);
}


static inline void
drm_gem_object_put(struct drm_gem_object *obj)
{
	if (obj)
		__drm_gem_object_put(obj);
}

int drm_gem_handle_create(struct drm_file *file_priv,
			  struct drm_gem_object *obj,
			  u32 *handlep);
int drm_gem_handle_delete(struct drm_file *filp, u32 handle);


void drm_gem_free_mmap_offset(struct drm_gem_object *obj);
int drm_gem_create_mmap_offset(struct drm_gem_object *obj);
int drm_gem_create_mmap_offset_size(struct drm_gem_object *obj, size_t size);

struct page **drm_gem_get_pages(struct drm_gem_object *obj);
void drm_gem_put_pages(struct drm_gem_object *obj, struct page **pages,
		bool dirty, bool accessed);

int drm_gem_vmap_unlocked(struct drm_gem_object *obj, struct iosys_map *map);
void drm_gem_vunmap_unlocked(struct drm_gem_object *obj, struct iosys_map *map);

int drm_gem_objects_lookup(struct drm_file *filp, void __user *bo_handles,
			   int count, struct drm_gem_object ***objs_out);
struct drm_gem_object *drm_gem_object_lookup(struct drm_file *filp, u32 handle);
long drm_gem_dma_resv_wait(struct drm_file *filep, u32 handle,
				    bool wait_all, unsigned long timeout);
int drm_gem_lock_reservations(struct drm_gem_object **objs, int count,
			      struct ww_acquire_ctx *acquire_ctx);
void drm_gem_unlock_reservations(struct drm_gem_object **objs, int count,
				 struct ww_acquire_ctx *acquire_ctx);
int drm_gem_dumb_map_offset(struct drm_file *file, struct drm_device *dev,
			    u32 handle, u64 *offset);

void drm_gem_lru_init(struct drm_gem_lru *lru, struct mutex *lock);
void drm_gem_lru_remove(struct drm_gem_object *obj);
void drm_gem_lru_move_tail_locked(struct drm_gem_lru *lru, struct drm_gem_object *obj);
void drm_gem_lru_move_tail(struct drm_gem_lru *lru, struct drm_gem_object *obj);
unsigned long drm_gem_lru_scan(struct drm_gem_lru *lru,
			       unsigned int nr_to_scan,
			       unsigned long *remaining,
			       bool (*shrink)(struct drm_gem_object *obj));

int drm_gem_evict(struct drm_gem_object *obj);


static inline bool drm_gem_object_is_shared_for_memory_stats(struct drm_gem_object *obj)
{
	return (obj->handle_count > 1) || obj->dma_buf;
}

#ifdef CONFIG_LOCKDEP

#define drm_gem_gpuva_set_lock(obj, lock) \
	if (!WARN((obj)->gpuva.lock_dep_map, \
		  "GEM GPUVA lock should be set only once.")) \
		(obj)->gpuva.lock_dep_map = &(lock)->dep_map
#define drm_gem_gpuva_assert_lock_held(obj) \
	lockdep_assert((obj)->gpuva.lock_dep_map ? \
		       lock_is_held((obj)->gpuva.lock_dep_map) : \
		       dma_resv_held((obj)->resv))
#else
#define drm_gem_gpuva_set_lock(obj, lock) do {} while (0)
#define drm_gem_gpuva_assert_lock_held(obj) do {} while (0)
#endif


static inline void drm_gem_gpuva_init(struct drm_gem_object *obj)
{
	INIT_LIST_HEAD(&obj->gpuva.list);
}


#define drm_gem_for_each_gpuva(entry__, obj__) \
	list_for_each_entry(entry__, &(obj__)->gpuva.list, gem.entry)


#define drm_gem_for_each_gpuva_safe(entry__, next__, obj__) \
	list_for_each_entry_safe(entry__, next__, &(obj__)->gpuva.list, gem.entry)

#endif 
