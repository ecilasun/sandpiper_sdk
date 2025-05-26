
#ifndef __DRM_CRTC_H__
#define __DRM_CRTC_H__

#include <linux/spinlock.h>
#include <linux/types.h>
#include <drm/drm_modeset_lock.h>
#include <drm/drm_mode_object.h>
#include <drm/drm_modes.h>
#include <drm/drm_device.h>
#include <drm/drm_plane.h>
#include <drm/drm_debugfs_crc.h>
#include <drm/drm_mode_config.h>

struct drm_connector;
struct drm_device;
struct drm_framebuffer;
struct drm_mode_set;
struct drm_file;
struct drm_printer;
struct drm_self_refresh_data;
struct device_node;
struct edid;

static inline int64_t U642I64(uint64_t val)
{
	return (int64_t)*((int64_t *)&val);
}
static inline uint64_t I642U64(int64_t val)
{
	return (uint64_t)*((uint64_t *)&val);
}

struct drm_crtc;
struct drm_pending_vblank_event;
struct drm_plane;
struct drm_bridge;
struct drm_atomic_state;

struct drm_crtc_helper_funcs;
struct drm_plane_helper_funcs;


struct drm_crtc_state {
	
	struct drm_crtc *crtc;

	
	bool enable;

	
	bool active;

	
	bool planes_changed : 1;

	
	bool mode_changed : 1;

	
	bool active_changed : 1;

	
	bool connectors_changed : 1;
	
	bool zpos_changed : 1;
	
	bool color_mgmt_changed : 1;

	
	bool no_vblank : 1;

	
	u32 plane_mask;

	
	u32 connector_mask;

	
	u32 encoder_mask;

	
	struct drm_display_mode adjusted_mode;

	
	struct drm_display_mode mode;

	
	struct drm_property_blob *mode_blob;

	
	struct drm_property_blob *degamma_lut;

	
	struct drm_property_blob *ctm;

	
	struct drm_property_blob *gamma_lut;

	
	u32 target_vblank;

	
	bool async_flip;

	
	bool vrr_enabled;

	
	bool self_refresh_active;

	
	enum drm_scaling_filter scaling_filter;

	
	struct drm_pending_vblank_event *event;

	
	struct drm_crtc_commit *commit;

	
	struct drm_atomic_state *state;
};


struct drm_crtc_funcs {
	
	void (*reset)(struct drm_crtc *crtc);

	
	int (*cursor_set)(struct drm_crtc *crtc, struct drm_file *file_priv,
			  uint32_t handle, uint32_t width, uint32_t height);

	
	int (*cursor_set2)(struct drm_crtc *crtc, struct drm_file *file_priv,
			   uint32_t handle, uint32_t width, uint32_t height,
			   int32_t hot_x, int32_t hot_y);

	
	int (*cursor_move)(struct drm_crtc *crtc, int x, int y);

	
	int (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
			 uint32_t size,
			 struct drm_modeset_acquire_ctx *ctx);

	
	void (*destroy)(struct drm_crtc *crtc);

	
	int (*set_config)(struct drm_mode_set *set,
			  struct drm_modeset_acquire_ctx *ctx);

	
	int (*page_flip)(struct drm_crtc *crtc,
			 struct drm_framebuffer *fb,
			 struct drm_pending_vblank_event *event,
			 uint32_t flags,
			 struct drm_modeset_acquire_ctx *ctx);

	
	int (*page_flip_target)(struct drm_crtc *crtc,
				struct drm_framebuffer *fb,
				struct drm_pending_vblank_event *event,
				uint32_t flags, uint32_t target,
				struct drm_modeset_acquire_ctx *ctx);

	
	int (*set_property)(struct drm_crtc *crtc,
			    struct drm_property *property, uint64_t val);

	
	struct drm_crtc_state *(*atomic_duplicate_state)(struct drm_crtc *crtc);

	
	void (*atomic_destroy_state)(struct drm_crtc *crtc,
				     struct drm_crtc_state *state);

	
	int (*atomic_set_property)(struct drm_crtc *crtc,
				   struct drm_crtc_state *state,
				   struct drm_property *property,
				   uint64_t val);
	
	int (*atomic_get_property)(struct drm_crtc *crtc,
				   const struct drm_crtc_state *state,
				   struct drm_property *property,
				   uint64_t *val);

	
	int (*late_register)(struct drm_crtc *crtc);

	
	void (*early_unregister)(struct drm_crtc *crtc);

	
	int (*set_crc_source)(struct drm_crtc *crtc, const char *source);

	
	int (*verify_crc_source)(struct drm_crtc *crtc, const char *source,
				 size_t *values_cnt);
	
	const char *const *(*get_crc_sources)(struct drm_crtc *crtc,
					      size_t *count);

	
	void (*atomic_print_state)(struct drm_printer *p,
				   const struct drm_crtc_state *state);

	
	u32 (*get_vblank_counter)(struct drm_crtc *crtc);

	
	int (*enable_vblank)(struct drm_crtc *crtc);

	
	void (*disable_vblank)(struct drm_crtc *crtc);

	
	bool (*get_vblank_timestamp)(struct drm_crtc *crtc,
				     int *max_error,
				     ktime_t *vblank_time,
				     bool in_vblank_irq);
};


struct drm_crtc {
	
	struct drm_device *dev;
	
	struct device_node *port;
	
	struct list_head head;

	
	char *name;

	
	struct drm_modeset_lock mutex;

	
	struct drm_mode_object base;

	
	struct drm_plane *primary;

	
	struct drm_plane *cursor;

	
	unsigned index;

	
	int cursor_x;
	
	int cursor_y;

	
	bool enabled;

	
	struct drm_display_mode mode;

	
	struct drm_display_mode hwmode;

	
	int x;
	
	int y;

	
	const struct drm_crtc_funcs *funcs;

	
	uint32_t gamma_size;

	
	uint16_t *gamma_store;

	
	const struct drm_crtc_helper_funcs *helper_private;

	
	struct drm_object_properties properties;

	
	struct drm_property *scaling_filter_property;

	
	struct drm_crtc_state *state;

	
	struct list_head commit_list;

	
	spinlock_t commit_lock;

	
	struct dentry *debugfs_entry;

	
	struct drm_crtc_crc crc;

	
	unsigned int fence_context;

	
	spinlock_t fence_lock;
	
	unsigned long fence_seqno;

	
	char timeline_name[32];

	
	struct drm_self_refresh_data *self_refresh_data;
};


struct drm_mode_set {
	struct drm_framebuffer *fb;
	struct drm_crtc *crtc;
	struct drm_display_mode *mode;

	uint32_t x;
	uint32_t y;

	struct drm_connector **connectors;
	size_t num_connectors;
};

#define obj_to_crtc(x) container_of(x, struct drm_crtc, base)

__printf(6, 7)
int drm_crtc_init_with_planes(struct drm_device *dev,
			      struct drm_crtc *crtc,
			      struct drm_plane *primary,
			      struct drm_plane *cursor,
			      const struct drm_crtc_funcs *funcs,
			      const char *name, ...);

__printf(6, 7)
int drmm_crtc_init_with_planes(struct drm_device *dev,
			       struct drm_crtc *crtc,
			       struct drm_plane *primary,
			       struct drm_plane *cursor,
			       const struct drm_crtc_funcs *funcs,
			       const char *name, ...);

void drm_crtc_cleanup(struct drm_crtc *crtc);

__printf(7, 8)
void *__drmm_crtc_alloc_with_planes(struct drm_device *dev,
				    size_t size, size_t offset,
				    struct drm_plane *primary,
				    struct drm_plane *cursor,
				    const struct drm_crtc_funcs *funcs,
				    const char *name, ...);


#define drmm_crtc_alloc_with_planes(dev, type, member, primary, cursor, funcs, name, ...) \
	((type *)__drmm_crtc_alloc_with_planes(dev, sizeof(type), \
					       offsetof(type, member), \
					       primary, cursor, funcs, \
					       name, ##__VA_ARGS__))


static inline unsigned int drm_crtc_index(const struct drm_crtc *crtc)
{
	return crtc->index;
}


static inline uint32_t drm_crtc_mask(const struct drm_crtc *crtc)
{
	return 1 << drm_crtc_index(crtc);
}

int drm_mode_set_config_internal(struct drm_mode_set *set);
struct drm_crtc *drm_crtc_from_index(struct drm_device *dev, int idx);


static inline struct drm_crtc *drm_crtc_find(struct drm_device *dev,
		struct drm_file *file_priv,
		uint32_t id)
{
	struct drm_mode_object *mo;
	mo = drm_mode_object_find(dev, file_priv, id, DRM_MODE_OBJECT_CRTC);
	return mo ? obj_to_crtc(mo) : NULL;
}


#define drm_for_each_crtc(crtc, dev) \
	list_for_each_entry(crtc, &(dev)->mode_config.crtc_list, head)


#define drm_for_each_crtc_reverse(crtc, dev) \
	list_for_each_entry_reverse(crtc, &(dev)->mode_config.crtc_list, head)

int drm_crtc_create_scaling_filter_property(struct drm_crtc *crtc,
					    unsigned int supported_filters);

#endif 
