

#ifndef DRM_SCDC_HELPER_H
#define DRM_SCDC_HELPER_H

#include <linux/types.h>

#include <drm/display/drm_scdc.h>

struct drm_connector;
struct i2c_adapter;

ssize_t drm_scdc_read(struct i2c_adapter *adapter, u8 offset, void *buffer,
		      size_t size);
ssize_t drm_scdc_write(struct i2c_adapter *adapter, u8 offset,
		       const void *buffer, size_t size);


static inline int drm_scdc_readb(struct i2c_adapter *adapter, u8 offset,
				 u8 *value)
{
	return drm_scdc_read(adapter, offset, value, sizeof(*value));
}


static inline int drm_scdc_writeb(struct i2c_adapter *adapter, u8 offset,
				  u8 value)
{
	return drm_scdc_write(adapter, offset, &value, sizeof(value));
}

bool drm_scdc_get_scrambling_status(struct drm_connector *connector);

bool drm_scdc_set_scrambling(struct drm_connector *connector, bool enable);
bool drm_scdc_set_high_tmds_clock_ratio(struct drm_connector *connector, bool set);

#endif
