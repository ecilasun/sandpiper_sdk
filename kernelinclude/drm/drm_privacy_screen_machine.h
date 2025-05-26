/* SPDX-License-Identifier: MIT */


#ifndef __DRM_PRIVACY_SCREEN_MACHINE_H__
#define __DRM_PRIVACY_SCREEN_MACHINE_H__

#include <linux/list.h>


struct drm_privacy_screen_lookup {
	
	struct list_head list;
	
	const char *dev_id;
	
	const char *con_id;
	
	const char *provider;
};

void drm_privacy_screen_lookup_add(struct drm_privacy_screen_lookup *lookup);
void drm_privacy_screen_lookup_remove(struct drm_privacy_screen_lookup *lookup);

#if IS_ENABLED(CONFIG_DRM_PRIVACY_SCREEN) && IS_ENABLED(CONFIG_X86)
void drm_privacy_screen_lookup_init(void);
void drm_privacy_screen_lookup_exit(void);
#else
static inline void drm_privacy_screen_lookup_init(void)
{
}
static inline void drm_privacy_screen_lookup_exit(void)
{
}
#endif

#endif
