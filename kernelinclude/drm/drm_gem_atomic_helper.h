/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRM_GEM_ATOMIC_HELPER_H__
#define __DRM_GEM_ATOMIC_HELPER_H__

#include <linux/iosys-map.h>

#include <drm/drm_fourcc.h>
#include <drm/drm_plane.h>

struct drm_simple_display_pipe;



int drm_gem_plane_helper_prepare_fb(struct drm_plane *plane, struct drm_plane_state *state);




#define DRM_SHADOW_PLANE_MAX_WIDTH	(4096u)


#define DRM_SHADOW_PLANE_MAX_HEIGHT	(4096u)


struct drm_shadow_plane_state {
	
	struct drm_plane_state base;

	

	
	struct iosys_map map[DRM_FORMAT_MAX_PLANES];

	
	struct iosys_map data[DRM_FORMAT_MAX_PLANES];
};


static inline struct drm_shadow_plane_state *
to_drm_shadow_plane_state(struct drm_plane_state *state)
{
	return container_of(state, struct drm_shadow_plane_state, base);
}

void __drm_gem_duplicate_shadow_plane_state(struct drm_plane *plane,
					    struct drm_shadow_plane_state *new_shadow_plane_state);
void __drm_gem_destroy_shadow_plane_state(struct drm_shadow_plane_state *shadow_plane_state);
void __drm_gem_reset_shadow_plane(struct drm_plane *plane,
				  struct drm_shadow_plane_state *shadow_plane_state);

void drm_gem_reset_shadow_plane(struct drm_plane *plane);
struct drm_plane_state *drm_gem_duplicate_shadow_plane_state(struct drm_plane *plane);
void drm_gem_destroy_shadow_plane_state(struct drm_plane *plane,
					struct drm_plane_state *plane_state);


#define DRM_GEM_SHADOW_PLANE_FUNCS \
	.reset = drm_gem_reset_shadow_plane, \
	.atomic_duplicate_state = drm_gem_duplicate_shadow_plane_state, \
	.atomic_destroy_state = drm_gem_destroy_shadow_plane_state

int drm_gem_begin_shadow_fb_access(struct drm_plane *plane, struct drm_plane_state *plane_state);
void drm_gem_end_shadow_fb_access(struct drm_plane *plane, struct drm_plane_state *plane_state);


#define DRM_GEM_SHADOW_PLANE_HELPER_FUNCS \
	.begin_fb_access = drm_gem_begin_shadow_fb_access, \
	.end_fb_access = drm_gem_end_shadow_fb_access

int drm_gem_simple_kms_begin_shadow_fb_access(struct drm_simple_display_pipe *pipe,
					      struct drm_plane_state *plane_state);
void drm_gem_simple_kms_end_shadow_fb_access(struct drm_simple_display_pipe *pipe,
					     struct drm_plane_state *plane_state);
void drm_gem_simple_kms_reset_shadow_plane(struct drm_simple_display_pipe *pipe);
struct drm_plane_state *
drm_gem_simple_kms_duplicate_shadow_plane_state(struct drm_simple_display_pipe *pipe);
void drm_gem_simple_kms_destroy_shadow_plane_state(struct drm_simple_display_pipe *pipe,
						   struct drm_plane_state *plane_state);


#define DRM_GEM_SIMPLE_DISPLAY_PIPE_SHADOW_PLANE_FUNCS \
	.begin_fb_access = drm_gem_simple_kms_begin_shadow_fb_access, \
	.end_fb_access = drm_gem_simple_kms_end_shadow_fb_access, \
	.reset_plane = drm_gem_simple_kms_reset_shadow_plane, \
	.duplicate_plane_state = drm_gem_simple_kms_duplicate_shadow_plane_state, \
	.destroy_plane_state = drm_gem_simple_kms_destroy_shadow_plane_state

#endif 
