

#ifndef __DRM_MODESET_H__
#define __DRM_MODESET_H__

#include <linux/kref.h>
#include <drm/drm_lease.h>
struct drm_object_properties;
struct drm_property;
struct drm_device;
struct drm_file;


struct drm_mode_object {
	uint32_t id;
	uint32_t type;
	struct drm_object_properties *properties;
	struct kref refcount;
	void (*free_cb)(struct kref *kref);
};

#define DRM_OBJECT_MAX_PROPERTY 24

struct drm_object_properties {
	

	int count;
	
	struct drm_property *properties[DRM_OBJECT_MAX_PROPERTY];

	
	uint64_t values[DRM_OBJECT_MAX_PROPERTY];
};


#define DRM_ENUM_NAME_FN(fnname, list)				\
	const char *fnname(int val)				\
	{							\
		int i;						\
		for (i = 0; i < ARRAY_SIZE(list); i++) {	\
			if (list[i].type == val)		\
				return list[i].name;		\
		}						\
		return "(unknown)";				\
	}

struct drm_mode_object *drm_mode_object_find(struct drm_device *dev,
					     struct drm_file *file_priv,
					     uint32_t id, uint32_t type);
void drm_mode_object_get(struct drm_mode_object *obj);
void drm_mode_object_put(struct drm_mode_object *obj);

int drm_object_property_set_value(struct drm_mode_object *obj,
				  struct drm_property *property,
				  uint64_t val);
int drm_object_property_get_value(struct drm_mode_object *obj,
				  struct drm_property *property,
				  uint64_t *value);
int drm_object_property_get_default_value(struct drm_mode_object *obj,
					  struct drm_property *property,
					  uint64_t *val);

void drm_object_attach_property(struct drm_mode_object *obj,
				struct drm_property *property,
				uint64_t init_val);

bool drm_mode_object_lease_required(uint32_t type);
#endif
