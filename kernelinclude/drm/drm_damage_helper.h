/* SPDX-License-Identifier: GPL-2.0 OR MIT */


#ifndef DRM_DAMAGE_HELPER_H_
#define DRM_DAMAGE_HELPER_H_

#include <drm/drm_atomic_helper.h>


#define drm_atomic_for_each_plane_damage(iter, rect) \
	while (drm_atomic_helper_damage_iter_next(iter, rect))


struct drm_atomic_helper_damage_iter {
	
	struct drm_rect plane_src;
	
	const struct drm_rect *clips;
	
	uint32_t num_clips;
	
	uint32_t curr_clip;
	
	bool full_update;
};

void drm_atomic_helper_check_plane_damage(struct drm_atomic_state *state,
					  struct drm_plane_state *plane_state);
int drm_atomic_helper_dirtyfb(struct drm_framebuffer *fb,
			      struct drm_file *file_priv, unsigned int flags,
			      unsigned int color, struct drm_clip_rect *clips,
			      unsigned int num_clips);
void
drm_atomic_helper_damage_iter_init(struct drm_atomic_helper_damage_iter *iter,
				   const struct drm_plane_state *old_state,
				   const struct drm_plane_state *new_state);
bool
drm_atomic_helper_damage_iter_next(struct drm_atomic_helper_damage_iter *iter,
				   struct drm_rect *rect);
bool drm_atomic_helper_damage_merged(const struct drm_plane_state *old_state,
				     struct drm_plane_state *state,
				     struct drm_rect *rect);

#endif
