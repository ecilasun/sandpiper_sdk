

#ifndef __DRM_KMS_HELPER_H__
#define __DRM_KMS_HELPER_H__

struct drm_crtc;
struct drm_crtc_funcs;
struct drm_device;
struct drm_framebuffer;
struct drm_mode_fb_cmd2;

void drm_helper_move_panel_connectors_to_head(struct drm_device *);

void drm_helper_mode_fill_fb_struct(struct drm_device *dev,
				    struct drm_framebuffer *fb,
				    const struct drm_mode_fb_cmd2 *mode_cmd);

int drm_crtc_init(struct drm_device *dev, struct drm_crtc *crtc,
		  const struct drm_crtc_funcs *funcs);

int drm_mode_config_helper_suspend(struct drm_device *dev);
int drm_mode_config_helper_resume(struct drm_device *dev);

#endif
