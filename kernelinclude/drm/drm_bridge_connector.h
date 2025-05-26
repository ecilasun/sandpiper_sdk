/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __DRM_BRIDGE_CONNECTOR_H__
#define __DRM_BRIDGE_CONNECTOR_H__

struct drm_connector;
struct drm_device;
struct drm_encoder;

struct drm_connector *drm_bridge_connector_init(struct drm_device *drm,
						struct drm_encoder *encoder);

#endif 
