/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __DRM_GEM_SHMEM_HELPER_H__
#define __DRM_GEM_SHMEM_HELPER_H__

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/mutex.h>

#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_prime.h>

struct dma_buf_attachment;
struct drm_mode_create_dumb;
struct drm_printer;
struct sg_table;


struct drm_gem_shmem_object {
	
	struct drm_gem_object base;

	
	struct page **pages;

	
	unsigned int pages_use_count;

	
	int madv;

	
	struct list_head madv_list;

	
	struct sg_table *sgt;

	
	void *vaddr;

	
	unsigned int vmap_use_count;

	
	bool pages_mark_dirty_on_put : 1;

	
	bool pages_mark_accessed_on_put : 1;

	
	bool map_wc : 1;
};

#define to_drm_gem_shmem_obj(obj) \
	container_of(obj, struct drm_gem_shmem_object, base)

struct drm_gem_shmem_object *drm_gem_shmem_create(struct drm_device *dev, size_t size);
void drm_gem_shmem_free(struct drm_gem_shmem_object *shmem);

void drm_gem_shmem_put_pages(struct drm_gem_shmem_object *shmem);
int drm_gem_shmem_pin(struct drm_gem_shmem_object *shmem);
void drm_gem_shmem_unpin(struct drm_gem_shmem_object *shmem);
int drm_gem_shmem_vmap(struct drm_gem_shmem_object *shmem,
		       struct iosys_map *map);
void drm_gem_shmem_vunmap(struct drm_gem_shmem_object *shmem,
			  struct iosys_map *map);
int drm_gem_shmem_mmap(struct drm_gem_shmem_object *shmem, struct vm_area_struct *vma);

int drm_gem_shmem_madvise(struct drm_gem_shmem_object *shmem, int madv);

static inline bool drm_gem_shmem_is_purgeable(struct drm_gem_shmem_object *shmem)
{
	return (shmem->madv > 0) &&
		!shmem->vmap_use_count && shmem->sgt &&
		!shmem->base.dma_buf && !shmem->base.import_attach;
}

void drm_gem_shmem_purge(struct drm_gem_shmem_object *shmem);

struct sg_table *drm_gem_shmem_get_sg_table(struct drm_gem_shmem_object *shmem);
struct sg_table *drm_gem_shmem_get_pages_sgt(struct drm_gem_shmem_object *shmem);

void drm_gem_shmem_print_info(const struct drm_gem_shmem_object *shmem,
			      struct drm_printer *p, unsigned int indent);

extern const struct vm_operations_struct drm_gem_shmem_vm_ops;




static inline void drm_gem_shmem_object_free(struct drm_gem_object *obj)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	drm_gem_shmem_free(shmem);
}


static inline void drm_gem_shmem_object_print_info(struct drm_printer *p, unsigned int indent,
						   const struct drm_gem_object *obj)
{
	const struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	drm_gem_shmem_print_info(shmem, p, indent);
}


static inline int drm_gem_shmem_object_pin(struct drm_gem_object *obj)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	return drm_gem_shmem_pin(shmem);
}


static inline void drm_gem_shmem_object_unpin(struct drm_gem_object *obj)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	drm_gem_shmem_unpin(shmem);
}


static inline struct sg_table *drm_gem_shmem_object_get_sg_table(struct drm_gem_object *obj)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	return drm_gem_shmem_get_sg_table(shmem);
}


static inline int drm_gem_shmem_object_vmap(struct drm_gem_object *obj,
					    struct iosys_map *map)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	return drm_gem_shmem_vmap(shmem, map);
}


static inline void drm_gem_shmem_object_vunmap(struct drm_gem_object *obj,
					       struct iosys_map *map)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	drm_gem_shmem_vunmap(shmem, map);
}


static inline int drm_gem_shmem_object_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma)
{
	struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);

	return drm_gem_shmem_mmap(shmem, vma);
}



struct drm_gem_object *
drm_gem_shmem_prime_import_sg_table(struct drm_device *dev,
				    struct dma_buf_attachment *attach,
				    struct sg_table *sgt);
int drm_gem_shmem_dumb_create(struct drm_file *file, struct drm_device *dev,
			      struct drm_mode_create_dumb *args);


#define DRM_GEM_SHMEM_DRIVER_OPS \
	.gem_prime_import_sg_table = drm_gem_shmem_prime_import_sg_table, \
	.dumb_create		   = drm_gem_shmem_dumb_create

#endif 
