



#ifndef __DRM_CRTC_HELPER_H__
#define __DRM_CRTC_HELPER_H__

#include <linux/types.h>

struct drm_atomic_state;
struct drm_connector;
struct drm_crtc;
struct drm_device;
struct drm_display_mode;
struct drm_encoder;
struct drm_framebuffer;
struct drm_mode_set;
struct drm_modeset_acquire_ctx;

void drm_helper_disable_unused_functions(struct drm_device *dev);
int drm_crtc_helper_set_config(struct drm_mode_set *set,
			       struct drm_modeset_acquire_ctx *ctx);
bool drm_crtc_helper_set_mode(struct drm_crtc *crtc,
			      struct drm_display_mode *mode,
			      int x, int y,
			      struct drm_framebuffer *old_fb);
int drm_crtc_helper_atomic_check(struct drm_crtc *crtc,
				 struct drm_atomic_state *state);
bool drm_helper_crtc_in_use(struct drm_crtc *crtc);
bool drm_helper_encoder_in_use(struct drm_encoder *encoder);

int drm_helper_connector_dpms(struct drm_connector *connector, int mode);

void drm_helper_resume_force_mode(struct drm_device *dev);
int drm_helper_force_disable_all(struct drm_device *dev);

#endif
