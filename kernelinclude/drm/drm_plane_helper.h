

#ifndef DRM_PLANE_HELPER_H
#define DRM_PLANE_HELPER_H

#include <linux/types.h>

struct drm_atomic_state;
struct drm_crtc;
struct drm_framebuffer;
struct drm_modeset_acquire_ctx;
struct drm_plane;

int drm_plane_helper_update_primary(struct drm_plane *plane, struct drm_crtc *crtc,
				    struct drm_framebuffer *fb,
				    int crtc_x, int crtc_y,
				    unsigned int crtc_w, unsigned int crtc_h,
				    uint32_t src_x, uint32_t src_y,
				    uint32_t src_w, uint32_t src_h,
				    struct drm_modeset_acquire_ctx *ctx);
int drm_plane_helper_disable_primary(struct drm_plane *plane,
				     struct drm_modeset_acquire_ctx *ctx);
void drm_plane_helper_destroy(struct drm_plane *plane);
int drm_plane_helper_atomic_check(struct drm_plane *plane, struct drm_atomic_state *state);


#define DRM_PLANE_NON_ATOMIC_FUNCS \
	.update_plane = drm_plane_helper_update_primary, \
	.disable_plane = drm_plane_helper_disable_primary, \
	.destroy = drm_plane_helper_destroy

#endif
