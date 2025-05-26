

#ifndef __DRM_MODESET_HELPER_VTABLES_H__
#define __DRM_MODESET_HELPER_VTABLES_H__

#include <drm/drm_crtc.h>
#include <drm/drm_encoder.h>



struct drm_writeback_connector;
struct drm_writeback_job;

enum mode_set_atomic {
	LEAVE_ATOMIC_MODE_SET,
	ENTER_ATOMIC_MODE_SET,
};


struct drm_crtc_helper_funcs {
	
	void (*dpms)(struct drm_crtc *crtc, int mode);

	
	void (*prepare)(struct drm_crtc *crtc);

	
	void (*commit)(struct drm_crtc *crtc);

	
	enum drm_mode_status (*mode_valid)(struct drm_crtc *crtc,
					   const struct drm_display_mode *mode);

	
	bool (*mode_fixup)(struct drm_crtc *crtc,
			   const struct drm_display_mode *mode,
			   struct drm_display_mode *adjusted_mode);

	
	int (*mode_set)(struct drm_crtc *crtc, struct drm_display_mode *mode,
			struct drm_display_mode *adjusted_mode, int x, int y,
			struct drm_framebuffer *old_fb);

	
	void (*mode_set_nofb)(struct drm_crtc *crtc);

	
	int (*mode_set_base)(struct drm_crtc *crtc, int x, int y,
			     struct drm_framebuffer *old_fb);

	
	int (*mode_set_base_atomic)(struct drm_crtc *crtc,
				    struct drm_framebuffer *fb, int x, int y,
				    enum mode_set_atomic);

	
	void (*disable)(struct drm_crtc *crtc);

	
	int (*atomic_check)(struct drm_crtc *crtc,
			    struct drm_atomic_state *state);

	
	void (*atomic_begin)(struct drm_crtc *crtc,
			     struct drm_atomic_state *state);
	
	void (*atomic_flush)(struct drm_crtc *crtc,
			     struct drm_atomic_state *state);

	
	void (*atomic_enable)(struct drm_crtc *crtc,
			      struct drm_atomic_state *state);

	
	void (*atomic_disable)(struct drm_crtc *crtc,
			       struct drm_atomic_state *state);

	
	bool (*get_scanout_position)(struct drm_crtc *crtc,
				     bool in_vblank_irq, int *vpos, int *hpos,
				     ktime_t *stime, ktime_t *etime,
				     const struct drm_display_mode *mode);
};


static inline void drm_crtc_helper_add(struct drm_crtc *crtc,
				       const struct drm_crtc_helper_funcs *funcs)
{
	crtc->helper_private = funcs;
}


struct drm_encoder_helper_funcs {
	
	void (*dpms)(struct drm_encoder *encoder, int mode);

	
	enum drm_mode_status (*mode_valid)(struct drm_encoder *crtc,
					   const struct drm_display_mode *mode);

	
	bool (*mode_fixup)(struct drm_encoder *encoder,
			   const struct drm_display_mode *mode,
			   struct drm_display_mode *adjusted_mode);

	
	void (*prepare)(struct drm_encoder *encoder);

	
	void (*commit)(struct drm_encoder *encoder);

	
	void (*mode_set)(struct drm_encoder *encoder,
			 struct drm_display_mode *mode,
			 struct drm_display_mode *adjusted_mode);

	
	void (*atomic_mode_set)(struct drm_encoder *encoder,
				struct drm_crtc_state *crtc_state,
				struct drm_connector_state *conn_state);

	
	enum drm_connector_status (*detect)(struct drm_encoder *encoder,
					    struct drm_connector *connector);

	
	void (*atomic_disable)(struct drm_encoder *encoder,
			       struct drm_atomic_state *state);

	
	void (*atomic_enable)(struct drm_encoder *encoder,
			      struct drm_atomic_state *state);

	
	void (*disable)(struct drm_encoder *encoder);

	
	void (*enable)(struct drm_encoder *encoder);

	
	int (*atomic_check)(struct drm_encoder *encoder,
			    struct drm_crtc_state *crtc_state,
			    struct drm_connector_state *conn_state);
};


static inline void drm_encoder_helper_add(struct drm_encoder *encoder,
					  const struct drm_encoder_helper_funcs *funcs)
{
	encoder->helper_private = funcs;
}


struct drm_connector_helper_funcs {
	
	int (*get_modes)(struct drm_connector *connector);

	
	int (*detect_ctx)(struct drm_connector *connector,
			  struct drm_modeset_acquire_ctx *ctx,
			  bool force);

	
	enum drm_mode_status (*mode_valid)(struct drm_connector *connector,
					   struct drm_display_mode *mode);

	
	int (*mode_valid_ctx)(struct drm_connector *connector,
			      struct drm_display_mode *mode,
			      struct drm_modeset_acquire_ctx *ctx,
			      enum drm_mode_status *status);

	
	struct drm_encoder *(*best_encoder)(struct drm_connector *connector);

	
	struct drm_encoder *(*atomic_best_encoder)(struct drm_connector *connector,
						   struct drm_atomic_state *state);

	
	int (*atomic_check)(struct drm_connector *connector,
			    struct drm_atomic_state *state);

	
	void (*atomic_commit)(struct drm_connector *connector,
			      struct drm_atomic_state *state);

	
	int (*prepare_writeback_job)(struct drm_writeback_connector *connector,
				     struct drm_writeback_job *job);
	
	void (*cleanup_writeback_job)(struct drm_writeback_connector *connector,
				      struct drm_writeback_job *job);

	
	void (*enable_hpd)(struct drm_connector *connector);

	
	void (*disable_hpd)(struct drm_connector *connector);
};


static inline void drm_connector_helper_add(struct drm_connector *connector,
					    const struct drm_connector_helper_funcs *funcs)
{
	connector->helper_private = funcs;
}


struct drm_plane_helper_funcs {
	
	int (*prepare_fb)(struct drm_plane *plane,
			  struct drm_plane_state *new_state);
	
	void (*cleanup_fb)(struct drm_plane *plane,
			   struct drm_plane_state *old_state);

	
	int (*begin_fb_access)(struct drm_plane *plane, struct drm_plane_state *new_plane_state);

	
	void (*end_fb_access)(struct drm_plane *plane, struct drm_plane_state *new_plane_state);

	
	int (*atomic_check)(struct drm_plane *plane,
			    struct drm_atomic_state *state);

	
	void (*atomic_update)(struct drm_plane *plane,
			      struct drm_atomic_state *state);

	
	void (*atomic_enable)(struct drm_plane *plane,
			      struct drm_atomic_state *state);

	
	void (*atomic_disable)(struct drm_plane *plane,
			       struct drm_atomic_state *state);

	
	int (*atomic_async_check)(struct drm_plane *plane,
				  struct drm_atomic_state *state);

	
	void (*atomic_async_update)(struct drm_plane *plane,
				    struct drm_atomic_state *state);
};


static inline void drm_plane_helper_add(struct drm_plane *plane,
					const struct drm_plane_helper_funcs *funcs)
{
	plane->helper_private = funcs;
}


struct drm_mode_config_helper_funcs {
	
	void (*atomic_commit_tail)(struct drm_atomic_state *state);

	
	int (*atomic_commit_setup)(struct drm_atomic_state *state);
};

#endif
