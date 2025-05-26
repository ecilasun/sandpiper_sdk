/* SPDX-License-Identifier: MIT */

#ifndef _DRM_VBLANK_WORK_H_
#define _DRM_VBLANK_WORK_H_

#include <linux/kthread.h>

struct drm_crtc;


struct drm_vblank_work {
	
	struct kthread_work base;

	
	struct drm_vblank_crtc *vblank;

	
	u64 count;

	
	int cancelling;

	
	struct list_head node;
};


#define to_drm_vblank_work(_work) \
	container_of((_work), struct drm_vblank_work, base)

int drm_vblank_work_schedule(struct drm_vblank_work *work,
			     u64 count, bool nextonmiss);
void drm_vblank_work_init(struct drm_vblank_work *work, struct drm_crtc *crtc,
			  void (*func)(struct kthread_work *work));
bool drm_vblank_work_cancel_sync(struct drm_vblank_work *work);
void drm_vblank_work_flush(struct drm_vblank_work *work);

#endif 
