

#ifndef DRM_ATOMIC_UAPI_H_
#define DRM_ATOMIC_UAPI_H_

struct drm_crtc_state;
struct drm_display_mode;
struct drm_property_blob;
struct drm_plane_state;
struct drm_crtc;
struct drm_connector_state;
struct dma_fence;
struct drm_framebuffer;

int __must_check
drm_atomic_set_mode_for_crtc(struct drm_crtc_state *state,
			     const struct drm_display_mode *mode);
int __must_check
drm_atomic_set_mode_prop_for_crtc(struct drm_crtc_state *state,
				  struct drm_property_blob *blob);
int __must_check
drm_atomic_set_crtc_for_plane(struct drm_plane_state *plane_state,
			      struct drm_crtc *crtc);
void drm_atomic_set_fb_for_plane(struct drm_plane_state *plane_state,
				 struct drm_framebuffer *fb);
int __must_check
drm_atomic_set_crtc_for_connector(struct drm_connector_state *conn_state,
				  struct drm_crtc *crtc);

#endif
