/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DRM_GEM_VRAM_HELPER_H
#define DRM_GEM_VRAM_HELPER_H

#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_gem_ttm_helper.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_modes.h>
#include <drm/ttm/ttm_bo.h>
#include <drm/ttm/ttm_placement.h>

#include <linux/container_of.h>
#include <linux/iosys-map.h>

struct drm_mode_create_dumb;
struct drm_plane;
struct drm_plane_state;
struct drm_simple_display_pipe;
struct filp;
struct vm_area_struct;

#define DRM_GEM_VRAM_PL_FLAG_SYSTEM	(1 << 0)
#define DRM_GEM_VRAM_PL_FLAG_VRAM	(1 << 1)
#define DRM_GEM_VRAM_PL_FLAG_TOPDOWN	(1 << 2)




struct drm_gem_vram_object {
	struct ttm_buffer_object bo;
	struct iosys_map map;

	
	unsigned int vmap_use_count;

	
	struct ttm_placement placement;
	struct ttm_place placements[2];
};


static inline struct drm_gem_vram_object *drm_gem_vram_of_bo(
	struct ttm_buffer_object *bo)
{
	return container_of(bo, struct drm_gem_vram_object, bo);
}


static inline struct drm_gem_vram_object *drm_gem_vram_of_gem(
	struct drm_gem_object *gem)
{
	return container_of(gem, struct drm_gem_vram_object, bo.base);
}

struct drm_gem_vram_object *drm_gem_vram_create(struct drm_device *dev,
						size_t size,
						unsigned long pg_align);
void drm_gem_vram_put(struct drm_gem_vram_object *gbo);
s64 drm_gem_vram_offset(struct drm_gem_vram_object *gbo);
int drm_gem_vram_pin(struct drm_gem_vram_object *gbo, unsigned long pl_flag);
int drm_gem_vram_unpin(struct drm_gem_vram_object *gbo);
int drm_gem_vram_vmap(struct drm_gem_vram_object *gbo, struct iosys_map *map);
void drm_gem_vram_vunmap(struct drm_gem_vram_object *gbo,
			 struct iosys_map *map);

int drm_gem_vram_fill_create_dumb(struct drm_file *file,
				  struct drm_device *dev,
				  unsigned long pg_align,
				  unsigned long pitch_align,
				  struct drm_mode_create_dumb *args);



int drm_gem_vram_driver_dumb_create(struct drm_file *file,
				    struct drm_device *dev,
				    struct drm_mode_create_dumb *args);


int
drm_gem_vram_plane_helper_prepare_fb(struct drm_plane *plane,
				     struct drm_plane_state *new_state);
void
drm_gem_vram_plane_helper_cleanup_fb(struct drm_plane *plane,
				     struct drm_plane_state *old_state);


#define DRM_GEM_VRAM_PLANE_HELPER_FUNCS \
	.prepare_fb = drm_gem_vram_plane_helper_prepare_fb, \
	.cleanup_fb = drm_gem_vram_plane_helper_cleanup_fb



int drm_gem_vram_simple_display_pipe_prepare_fb(
	struct drm_simple_display_pipe *pipe,
	struct drm_plane_state *new_state);

void drm_gem_vram_simple_display_pipe_cleanup_fb(
	struct drm_simple_display_pipe *pipe,
	struct drm_plane_state *old_state);


#define DRM_GEM_VRAM_DRIVER \
	.debugfs_init	 = drm_vram_mm_debugfs_init, \
	.dumb_create	 = drm_gem_vram_driver_dumb_create, \
	.dumb_map_offset = drm_gem_ttm_dumb_map_offset




struct drm_vram_mm {
	uint64_t vram_base;
	size_t vram_size;

	struct ttm_device bdev;
};


static inline struct drm_vram_mm *drm_vram_mm_of_bdev(
	struct ttm_device *bdev)
{
	return container_of(bdev, struct drm_vram_mm, bdev);
}

void drm_vram_mm_debugfs_init(struct drm_minor *minor);



int drmm_vram_helper_init(struct drm_device *dev, uint64_t vram_base,
			  size_t vram_size);



enum drm_mode_status
drm_vram_helper_mode_valid(struct drm_device *dev,
			   const struct drm_display_mode *mode);

#endif
