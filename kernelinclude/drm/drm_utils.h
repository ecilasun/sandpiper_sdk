/* SPDX-License-Identifier: MIT */


#ifndef __DRM_UTILS_H__
#define __DRM_UTILS_H__

#include <linux/types.h>

int drm_get_panel_orientation_quirk(int width, int height);

signed long drm_timeout_abs_to_jiffies(int64_t timeout_nsec);

#endif
