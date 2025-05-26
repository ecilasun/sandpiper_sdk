/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DRM_GEM_DMA_HELPER_H__
#define __DRM_GEM_DMA_HELPER_H__

#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_gem.h>

struct drm_mode_create_dumb;


struct drm_gem_dma_object {
	struct drm_gem_object base;
	dma_addr_t dma_addr;
	struct sg_table *sgt;

	
	void *vaddr;

	bool map_noncoherent;
};

#define to_drm_gem_dma_obj(gem_obj) \
	container_of(gem_obj, struct drm_gem_dma_object, base)

struct drm_gem_dma_object *drm_gem_dma_create(struct drm_device *drm,
					      size_t size);
void drm_gem_dma_free(struct drm_gem_dma_object *dma_obj);
void drm_gem_dma_print_info(const struct drm_gem_dma_object *dma_obj,
			    struct drm_printer *p, unsigned int indent);
struct sg_table *drm_gem_dma_get_sg_table(struct drm_gem_dma_object *dma_obj);
int drm_gem_dma_vmap(struct drm_gem_dma_object *dma_obj,
		     struct iosys_map *map);
int drm_gem_dma_mmap(struct drm_gem_dma_object *dma_obj, struct vm_area_struct *vma);

extern const struct vm_operations_struct drm_gem_dma_vm_ops;




static inline void drm_gem_dma_object_free(struct drm_gem_object *obj)
{
	struct drm_gem_dma_object *dma_obj = to_drm_gem_dma_obj(obj);

	drm_gem_dma_free(dma_obj);
}


static inline void drm_gem_dma_object_print_info(struct drm_printer *p, unsigned int indent,
						 const struct drm_gem_object *obj)
{
	const struct drm_gem_dma_object *dma_obj = to_drm_gem_dma_obj(obj);

	drm_gem_dma_print_info(dma_obj, p, indent);
}


static inline struct sg_table *drm_gem_dma_object_get_sg_table(struct drm_gem_object *obj)
{
	struct drm_gem_dma_object *dma_obj = to_drm_gem_dma_obj(obj);

	return drm_gem_dma_get_sg_table(dma_obj);
}


static inline int drm_gem_dma_object_vmap(struct drm_gem_object *obj,
					  struct iosys_map *map)
{
	struct drm_gem_dma_object *dma_obj = to_drm_gem_dma_obj(obj);

	return drm_gem_dma_vmap(dma_obj, map);
}


static inline int drm_gem_dma_object_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma)
{
	struct drm_gem_dma_object *dma_obj = to_drm_gem_dma_obj(obj);

	return drm_gem_dma_mmap(dma_obj, vma);
}




int drm_gem_dma_dumb_create_internal(struct drm_file *file_priv,
				     struct drm_device *drm,
				     struct drm_mode_create_dumb *args);


int drm_gem_dma_dumb_create(struct drm_file *file_priv,
			    struct drm_device *drm,
			    struct drm_mode_create_dumb *args);

struct drm_gem_object *
drm_gem_dma_prime_import_sg_table(struct drm_device *dev,
				  struct dma_buf_attachment *attach,
				  struct sg_table *sgt);


#define DRM_GEM_DMA_DRIVER_OPS_WITH_DUMB_CREATE(dumb_create_func) \
	.dumb_create		   = (dumb_create_func), \
	.gem_prime_import_sg_table = drm_gem_dma_prime_import_sg_table


#define DRM_GEM_DMA_DRIVER_OPS \
	DRM_GEM_DMA_DRIVER_OPS_WITH_DUMB_CREATE(drm_gem_dma_dumb_create)


#define DRM_GEM_DMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE(dumb_create_func) \
	.dumb_create		   = (dumb_create_func), \
	.gem_prime_import_sg_table = drm_gem_dma_prime_import_sg_table_vmap


#define DRM_GEM_DMA_DRIVER_OPS_VMAP \
	DRM_GEM_DMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE(drm_gem_dma_dumb_create)

struct drm_gem_object *
drm_gem_dma_prime_import_sg_table_vmap(struct drm_device *drm,
				       struct dma_buf_attachment *attach,
				       struct sg_table *sgt);



#ifndef CONFIG_MMU
unsigned long drm_gem_dma_get_unmapped_area(struct file *filp,
					    unsigned long addr,
					    unsigned long len,
					    unsigned long pgoff,
					    unsigned long flags);
#define DRM_GEM_DMA_UNMAPPED_AREA_FOPS \
	.get_unmapped_area	= drm_gem_dma_get_unmapped_area,
#else
#define DRM_GEM_DMA_UNMAPPED_AREA_FOPS
#endif


#define DEFINE_DRM_GEM_DMA_FOPS(name) \
	static const struct file_operations name = {\
		.owner		= THIS_MODULE,\
		.open		= drm_open,\
		.release	= drm_release,\
		.unlocked_ioctl	= drm_ioctl,\
		.compat_ioctl	= drm_compat_ioctl,\
		.poll		= drm_poll,\
		.read		= drm_read,\
		.llseek		= noop_llseek,\
		.mmap		= drm_gem_mmap,\
		DRM_GEM_DMA_UNMAPPED_AREA_FOPS \
	}

#endif 
