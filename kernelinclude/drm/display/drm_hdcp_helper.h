/* SPDX-License-Identifier: MIT */


#ifndef _DRM_HDCP_HELPER_H_INCLUDED_
#define _DRM_HDCP_HELPER_H_INCLUDED_

#include <drm/display/drm_hdcp.h>

struct drm_device;
struct drm_connector;

int drm_hdcp_check_ksvs_revoked(struct drm_device *dev, u8 *ksvs, u32 ksv_count);
int drm_connector_attach_content_protection_property(struct drm_connector *connector,
						     bool hdcp_content_type);
void drm_hdcp_update_content_protection(struct drm_connector *connector, u64 val);

#endif
