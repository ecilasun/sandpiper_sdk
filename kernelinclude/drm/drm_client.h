/* SPDX-License-Identifier: GPL-2.0 or MIT */

#ifndef _DRM_CLIENT_H_
#define _DRM_CLIENT_H_

#include <linux/iosys-map.h>
#include <linux/lockdep.h>
#include <linux/mutex.h>
#include <linux/types.h>

#include <drm/drm_connector.h>
#include <drm/drm_crtc.h>

struct drm_client_dev;
struct drm_device;
struct drm_file;
struct drm_framebuffer;
struct drm_gem_object;
struct drm_minor;
struct module;


struct drm_client_funcs {
	
	struct module *owner;

	
	void (*unregister)(struct drm_client_dev *client);

	
	int (*restore)(struct drm_client_dev *client);

	
	int (*hotplug)(struct drm_client_dev *client);
};


struct drm_client_dev {
	
	struct drm_device *dev;

	
	const char *name;

	
	struct list_head list;

	
	const struct drm_client_funcs *funcs;

	
	struct drm_file *file;

	
	struct mutex modeset_mutex;

	
	struct drm_mode_set *modesets;

	
	bool hotplug_failed;
};

int drm_client_init(struct drm_device *dev, struct drm_client_dev *client,
		    const char *name, const struct drm_client_funcs *funcs);
void drm_client_release(struct drm_client_dev *client);
void drm_client_register(struct drm_client_dev *client);

void drm_client_dev_unregister(struct drm_device *dev);
void drm_client_dev_hotplug(struct drm_device *dev);
void drm_client_dev_restore(struct drm_device *dev);


struct drm_client_buffer {
	
	struct drm_client_dev *client;

	
	u32 pitch;

	
	struct drm_gem_object *gem;

	
	struct iosys_map map;

	
	struct drm_framebuffer *fb;
};

struct drm_client_buffer *
drm_client_framebuffer_create(struct drm_client_dev *client, u32 width, u32 height, u32 format);
void drm_client_framebuffer_delete(struct drm_client_buffer *buffer);
int drm_client_framebuffer_flush(struct drm_client_buffer *buffer, struct drm_rect *rect);
int drm_client_buffer_vmap(struct drm_client_buffer *buffer,
			   struct iosys_map *map);
void drm_client_buffer_vunmap(struct drm_client_buffer *buffer);

int drm_client_modeset_create(struct drm_client_dev *client);
void drm_client_modeset_free(struct drm_client_dev *client);
int drm_client_modeset_probe(struct drm_client_dev *client, unsigned int width, unsigned int height);
bool drm_client_rotation(struct drm_mode_set *modeset, unsigned int *rotation);
int drm_client_modeset_check(struct drm_client_dev *client);
int drm_client_modeset_commit_locked(struct drm_client_dev *client);
int drm_client_modeset_commit(struct drm_client_dev *client);
int drm_client_modeset_dpms(struct drm_client_dev *client, int mode);


#define drm_client_for_each_modeset(modeset, client) \
	for (({ lockdep_assert_held(&(client)->modeset_mutex); }), \
	     modeset = (client)->modesets; modeset->crtc; modeset++)


#define drm_client_for_each_connector_iter(connector, iter) \
	drm_for_each_connector_iter(connector, iter) \
		if (connector->connector_type != DRM_MODE_CONNECTOR_WRITEBACK)

void drm_client_debugfs_init(struct drm_minor *minor);

#endif
