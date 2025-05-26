

#ifndef __DRM_PANEL_H__
#define __DRM_PANEL_H__

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/mutex.h>

struct backlight_device;
struct dentry;
struct device_node;
struct drm_connector;
struct drm_device;
struct drm_panel_follower;
struct drm_panel;
struct display_timing;

enum drm_panel_orientation;


struct drm_panel_funcs {
	
	int (*prepare)(struct drm_panel *panel);

	
	int (*enable)(struct drm_panel *panel);

	
	int (*disable)(struct drm_panel *panel);

	
	int (*unprepare)(struct drm_panel *panel);

	
	int (*get_modes)(struct drm_panel *panel,
			 struct drm_connector *connector);

	
	enum drm_panel_orientation (*get_orientation)(struct drm_panel *panel);

	
	int (*get_timings)(struct drm_panel *panel, unsigned int num_timings,
			   struct display_timing *timings);

	
	void (*debugfs_init)(struct drm_panel *panel, struct dentry *root);
};

struct drm_panel_follower_funcs {
	
	int (*panel_prepared)(struct drm_panel_follower *follower);

	
	int (*panel_unpreparing)(struct drm_panel_follower *follower);
};

struct drm_panel_follower {
	
	const struct drm_panel_follower_funcs *funcs;

	
	struct list_head list;

	
	struct drm_panel *panel;
};


struct drm_panel {
	
	struct device *dev;

	
	struct backlight_device *backlight;

	
	const struct drm_panel_funcs *funcs;

	
	int connector_type;

	
	struct list_head list;

	
	struct list_head followers;

	
	struct mutex follower_lock;

	
	bool prepare_prev_first;

	
	bool prepared;

	
	bool enabled;
};

void drm_panel_init(struct drm_panel *panel, struct device *dev,
		    const struct drm_panel_funcs *funcs,
		    int connector_type);

void drm_panel_add(struct drm_panel *panel);
void drm_panel_remove(struct drm_panel *panel);

int drm_panel_prepare(struct drm_panel *panel);
int drm_panel_unprepare(struct drm_panel *panel);

int drm_panel_enable(struct drm_panel *panel);
int drm_panel_disable(struct drm_panel *panel);

int drm_panel_get_modes(struct drm_panel *panel, struct drm_connector *connector);

#if defined(CONFIG_OF) && defined(CONFIG_DRM_PANEL)
struct drm_panel *of_drm_find_panel(const struct device_node *np);
int of_drm_get_panel_orientation(const struct device_node *np,
				 enum drm_panel_orientation *orientation);
#else
static inline struct drm_panel *of_drm_find_panel(const struct device_node *np)
{
	return ERR_PTR(-ENODEV);
}

static inline int of_drm_get_panel_orientation(const struct device_node *np,
					       enum drm_panel_orientation *orientation)
{
	return -ENODEV;
}
#endif

#if defined(CONFIG_DRM_PANEL)
bool drm_is_panel_follower(struct device *dev);
int drm_panel_add_follower(struct device *follower_dev,
			   struct drm_panel_follower *follower);
void drm_panel_remove_follower(struct drm_panel_follower *follower);
int devm_drm_panel_add_follower(struct device *follower_dev,
				struct drm_panel_follower *follower);
#else
static inline bool drm_is_panel_follower(struct device *dev)
{
	return false;
}

static inline int drm_panel_add_follower(struct device *follower_dev,
					 struct drm_panel_follower *follower)
{
	return -ENODEV;
}

static inline void drm_panel_remove_follower(struct drm_panel_follower *follower) { }
static inline int devm_drm_panel_add_follower(struct device *follower_dev,
					      struct drm_panel_follower *follower)
{
	return -ENODEV;
}
#endif

#if IS_ENABLED(CONFIG_DRM_PANEL) && (IS_BUILTIN(CONFIG_BACKLIGHT_CLASS_DEVICE) || \
	(IS_MODULE(CONFIG_DRM) && IS_MODULE(CONFIG_BACKLIGHT_CLASS_DEVICE)))
int drm_panel_of_backlight(struct drm_panel *panel);
#else
static inline int drm_panel_of_backlight(struct drm_panel *panel)
{
	return 0;
}
#endif

#endif
