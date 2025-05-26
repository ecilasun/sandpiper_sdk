

#ifndef __DRM_BRIDGE_H__
#define __DRM_BRIDGE_H__

#include <linux/ctype.h>
#include <linux/list.h>
#include <linux/mutex.h>

#include <drm/drm_atomic.h>
#include <drm/drm_encoder.h>
#include <drm/drm_mode_object.h>
#include <drm/drm_modes.h>

struct drm_bridge;
struct drm_bridge_timings;
struct drm_connector;
struct drm_display_info;
struct drm_minor;
struct drm_panel;
struct edid;
struct i2c_adapter;


enum drm_bridge_attach_flags {
	
	DRM_BRIDGE_ATTACH_NO_CONNECTOR = BIT(0),
};


struct drm_bridge_funcs {
	
	int (*attach)(struct drm_bridge *bridge,
		      enum drm_bridge_attach_flags flags);

	
	void (*detach)(struct drm_bridge *bridge);

	
	enum drm_mode_status (*mode_valid)(struct drm_bridge *bridge,
					   const struct drm_display_info *info,
					   const struct drm_display_mode *mode);

	
	bool (*mode_fixup)(struct drm_bridge *bridge,
			   const struct drm_display_mode *mode,
			   struct drm_display_mode *adjusted_mode);
	
	void (*disable)(struct drm_bridge *bridge);

	
	void (*post_disable)(struct drm_bridge *bridge);

	
	void (*mode_set)(struct drm_bridge *bridge,
			 const struct drm_display_mode *mode,
			 const struct drm_display_mode *adjusted_mode);
	
	void (*pre_enable)(struct drm_bridge *bridge);

	
	void (*enable)(struct drm_bridge *bridge);

	
	void (*atomic_pre_enable)(struct drm_bridge *bridge,
				  struct drm_bridge_state *old_bridge_state);

	
	void (*atomic_enable)(struct drm_bridge *bridge,
			      struct drm_bridge_state *old_bridge_state);
	
	void (*atomic_disable)(struct drm_bridge *bridge,
			       struct drm_bridge_state *old_bridge_state);

	
	void (*atomic_post_disable)(struct drm_bridge *bridge,
				    struct drm_bridge_state *old_bridge_state);

	
	struct drm_bridge_state *(*atomic_duplicate_state)(struct drm_bridge *bridge);

	
	void (*atomic_destroy_state)(struct drm_bridge *bridge,
				     struct drm_bridge_state *state);

	
	u32 *(*atomic_get_output_bus_fmts)(struct drm_bridge *bridge,
					   struct drm_bridge_state *bridge_state,
					   struct drm_crtc_state *crtc_state,
					   struct drm_connector_state *conn_state,
					   unsigned int *num_output_fmts);

	
	u32 *(*atomic_get_input_bus_fmts)(struct drm_bridge *bridge,
					  struct drm_bridge_state *bridge_state,
					  struct drm_crtc_state *crtc_state,
					  struct drm_connector_state *conn_state,
					  u32 output_fmt,
					  unsigned int *num_input_fmts);

	
	int (*atomic_check)(struct drm_bridge *bridge,
			    struct drm_bridge_state *bridge_state,
			    struct drm_crtc_state *crtc_state,
			    struct drm_connector_state *conn_state);

	
	struct drm_bridge_state *(*atomic_reset)(struct drm_bridge *bridge);

	
	enum drm_connector_status (*detect)(struct drm_bridge *bridge);

	
	int (*get_modes)(struct drm_bridge *bridge,
			 struct drm_connector *connector);

	
	const struct drm_edid *(*edid_read)(struct drm_bridge *bridge,
					    struct drm_connector *connector);

	
	struct edid *(*get_edid)(struct drm_bridge *bridge,
				 struct drm_connector *connector);

	
	void (*hpd_notify)(struct drm_bridge *bridge,
			   enum drm_connector_status status);

	
	void (*hpd_enable)(struct drm_bridge *bridge);

	
	void (*hpd_disable)(struct drm_bridge *bridge);

	
	void (*debugfs_init)(struct drm_bridge *bridge, struct dentry *root);
};


struct drm_bridge_timings {
	
	u32 input_bus_flags;
	
	u32 setup_time_ps;
	
	u32 hold_time_ps;
	
	bool dual_link;
};


enum drm_bridge_ops {
	
	DRM_BRIDGE_OP_DETECT = BIT(0),
	
	DRM_BRIDGE_OP_EDID = BIT(1),
	
	DRM_BRIDGE_OP_HPD = BIT(2),
	
	DRM_BRIDGE_OP_MODES = BIT(3),
};


struct drm_bridge {
	
	struct drm_private_obj base;
	
	struct drm_device *dev;
	
	struct drm_encoder *encoder;
	
	struct list_head chain_node;
#ifdef CONFIG_OF
	
	struct device_node *of_node;
#endif
	
	struct list_head list;
	
	const struct drm_bridge_timings *timings;
	
	const struct drm_bridge_funcs *funcs;
	
	void *driver_private;
	
	enum drm_bridge_ops ops;
	
	int type;
	
	bool interlace_allowed;
	
	bool pre_enable_prev_first;
	
	struct i2c_adapter *ddc;
	
	
	struct mutex hpd_mutex;
	
	void (*hpd_cb)(void *data, enum drm_connector_status status);
	
	void *hpd_data;
};

static inline struct drm_bridge *
drm_priv_to_bridge(struct drm_private_obj *priv)
{
	return container_of(priv, struct drm_bridge, base);
}

void drm_bridge_add(struct drm_bridge *bridge);
int devm_drm_bridge_add(struct device *dev, struct drm_bridge *bridge);
void drm_bridge_remove(struct drm_bridge *bridge);
int drm_bridge_attach(struct drm_encoder *encoder, struct drm_bridge *bridge,
		      struct drm_bridge *previous,
		      enum drm_bridge_attach_flags flags);

#ifdef CONFIG_OF
struct drm_bridge *of_drm_find_bridge(struct device_node *np);
#else
static inline struct drm_bridge *of_drm_find_bridge(struct device_node *np)
{
	return NULL;
}
#endif


static inline struct drm_bridge *
drm_bridge_get_next_bridge(struct drm_bridge *bridge)
{
	if (list_is_last(&bridge->chain_node, &bridge->encoder->bridge_chain))
		return NULL;

	return list_next_entry(bridge, chain_node);
}


static inline struct drm_bridge *
drm_bridge_get_prev_bridge(struct drm_bridge *bridge)
{
	if (list_is_first(&bridge->chain_node, &bridge->encoder->bridge_chain))
		return NULL;

	return list_prev_entry(bridge, chain_node);
}


static inline struct drm_bridge *
drm_bridge_chain_get_first_bridge(struct drm_encoder *encoder)
{
	return list_first_entry_or_null(&encoder->bridge_chain,
					struct drm_bridge, chain_node);
}


#define drm_for_each_bridge_in_chain(encoder, bridge)			\
	list_for_each_entry(bridge, &(encoder)->bridge_chain, chain_node)

bool drm_bridge_chain_mode_fixup(struct drm_bridge *bridge,
				 const struct drm_display_mode *mode,
				 struct drm_display_mode *adjusted_mode);
enum drm_mode_status
drm_bridge_chain_mode_valid(struct drm_bridge *bridge,
			    const struct drm_display_info *info,
			    const struct drm_display_mode *mode);
void drm_bridge_chain_mode_set(struct drm_bridge *bridge,
			       const struct drm_display_mode *mode,
			       const struct drm_display_mode *adjusted_mode);

int drm_atomic_bridge_chain_check(struct drm_bridge *bridge,
				  struct drm_crtc_state *crtc_state,
				  struct drm_connector_state *conn_state);
void drm_atomic_bridge_chain_disable(struct drm_bridge *bridge,
				     struct drm_atomic_state *state);
void drm_atomic_bridge_chain_post_disable(struct drm_bridge *bridge,
					  struct drm_atomic_state *state);
void drm_atomic_bridge_chain_pre_enable(struct drm_bridge *bridge,
					struct drm_atomic_state *state);
void drm_atomic_bridge_chain_enable(struct drm_bridge *bridge,
				    struct drm_atomic_state *state);

u32 *
drm_atomic_helper_bridge_propagate_bus_fmt(struct drm_bridge *bridge,
					struct drm_bridge_state *bridge_state,
					struct drm_crtc_state *crtc_state,
					struct drm_connector_state *conn_state,
					u32 output_fmt,
					unsigned int *num_input_fmts);

enum drm_connector_status drm_bridge_detect(struct drm_bridge *bridge);
int drm_bridge_get_modes(struct drm_bridge *bridge,
			 struct drm_connector *connector);
const struct drm_edid *drm_bridge_edid_read(struct drm_bridge *bridge,
					    struct drm_connector *connector);
struct edid *drm_bridge_get_edid(struct drm_bridge *bridge,
				 struct drm_connector *connector);
void drm_bridge_hpd_enable(struct drm_bridge *bridge,
			   void (*cb)(void *data,
				      enum drm_connector_status status),
			   void *data);
void drm_bridge_hpd_disable(struct drm_bridge *bridge);
void drm_bridge_hpd_notify(struct drm_bridge *bridge,
			   enum drm_connector_status status);

#ifdef CONFIG_DRM_PANEL_BRIDGE
bool drm_bridge_is_panel(const struct drm_bridge *bridge);
struct drm_bridge *drm_panel_bridge_add(struct drm_panel *panel);
struct drm_bridge *drm_panel_bridge_add_typed(struct drm_panel *panel,
					      u32 connector_type);
void drm_panel_bridge_remove(struct drm_bridge *bridge);
int drm_panel_bridge_set_orientation(struct drm_connector *connector,
				     struct drm_bridge *bridge);
struct drm_bridge *devm_drm_panel_bridge_add(struct device *dev,
					     struct drm_panel *panel);
struct drm_bridge *devm_drm_panel_bridge_add_typed(struct device *dev,
						   struct drm_panel *panel,
						   u32 connector_type);
struct drm_bridge *drmm_panel_bridge_add(struct drm_device *drm,
					     struct drm_panel *panel);
struct drm_connector *drm_panel_bridge_connector(struct drm_bridge *bridge);
#else
static inline bool drm_bridge_is_panel(const struct drm_bridge *bridge)
{
	return false;
}

static inline int drm_panel_bridge_set_orientation(struct drm_connector *connector,
						   struct drm_bridge *bridge)
{
	return -EINVAL;
}
#endif

#if defined(CONFIG_OF) && defined(CONFIG_DRM_PANEL_BRIDGE)
struct drm_bridge *devm_drm_of_get_bridge(struct device *dev, struct device_node *node,
					  u32 port, u32 endpoint);
struct drm_bridge *drmm_of_get_bridge(struct drm_device *drm, struct device_node *node,
					  u32 port, u32 endpoint);
#else
static inline struct drm_bridge *devm_drm_of_get_bridge(struct device *dev,
							struct device_node *node,
							u32 port,
							u32 endpoint)
{
	return ERR_PTR(-ENODEV);
}

static inline struct drm_bridge *drmm_of_get_bridge(struct drm_device *drm,
						     struct device_node *node,
						     u32 port,
						     u32 endpoint)
{
	return ERR_PTR(-ENODEV);
}
#endif

void drm_bridge_debugfs_init(struct drm_minor *minor);

#endif
