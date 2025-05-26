/* SPDX-License-Identifier: MIT */


#ifndef __DRM_PRIVACY_SCREEN_DRIVER_H__
#define __DRM_PRIVACY_SCREEN_DRIVER_H__

#include <linux/device.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <drm/drm_connector.h>

struct drm_privacy_screen;


struct drm_privacy_screen_ops {
	
	int (*set_sw_state)(struct drm_privacy_screen *priv,
			    enum drm_privacy_screen_status sw_state);
	
	void (*get_hw_state)(struct drm_privacy_screen *priv);
};


struct drm_privacy_screen {
	
	struct device dev;
	
	struct mutex lock;
	
	struct list_head list;
	
	struct blocking_notifier_head notifier_head;
	
	const struct drm_privacy_screen_ops *ops;
	
	enum drm_privacy_screen_status sw_state;
	
	enum drm_privacy_screen_status hw_state;
	
	void *drvdata;
};

static inline
void *drm_privacy_screen_get_drvdata(struct drm_privacy_screen *priv)
{
	return priv->drvdata;
}

struct drm_privacy_screen *drm_privacy_screen_register(
	struct device *parent, const struct drm_privacy_screen_ops *ops,
	void *data);
void drm_privacy_screen_unregister(struct drm_privacy_screen *priv);

void drm_privacy_screen_call_notifier_chain(struct drm_privacy_screen *priv);

#endif
