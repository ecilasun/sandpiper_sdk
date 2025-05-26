

#ifndef _DRM_VBLANK_H_
#define _DRM_VBLANK_H_

#include <linux/seqlock.h>
#include <linux/idr.h>
#include <linux/poll.h>
#include <linux/kthread.h>

#include <drm/drm_file.h>
#include <drm/drm_modes.h>

struct drm_device;
struct drm_crtc;
struct drm_vblank_work;


struct drm_pending_vblank_event {
	
	struct drm_pending_event base;
	
	unsigned int pipe;
	
	u64 sequence;
	
	union {
		
		struct drm_event base;

		
		struct drm_event_vblank vbl;

		
		struct drm_event_crtc_sequence seq;
	} event;
};


struct drm_vblank_crtc {
	
	struct drm_device *dev;
	
	wait_queue_head_t queue;
	
	struct timer_list disable_timer;

	
	seqlock_t seqlock;

	
	atomic64_t count;
	
	ktime_t time;

	
	atomic_t refcount;
	
	u32 last;
	
	u32 max_vblank_count;
	
	unsigned int inmodeset;
	
	unsigned int pipe;
	
	int framedur_ns;
	
	int linedur_ns;

	
	struct drm_display_mode hwmode;

	
	bool enabled;

	
	struct kthread_worker *worker;

	
	struct list_head pending_work;

	
	wait_queue_head_t work_wait_queue;
};

int drm_vblank_init(struct drm_device *dev, unsigned int num_crtcs);
bool drm_dev_has_vblank(const struct drm_device *dev);
u64 drm_crtc_vblank_count(struct drm_crtc *crtc);
u64 drm_crtc_vblank_count_and_time(struct drm_crtc *crtc,
				   ktime_t *vblanktime);
int drm_crtc_next_vblank_start(struct drm_crtc *crtc, ktime_t *vblanktime);
void drm_crtc_send_vblank_event(struct drm_crtc *crtc,
			       struct drm_pending_vblank_event *e);
void drm_crtc_arm_vblank_event(struct drm_crtc *crtc,
			      struct drm_pending_vblank_event *e);
void drm_vblank_set_event(struct drm_pending_vblank_event *e,
			  u64 *seq,
			  ktime_t *now);
bool drm_handle_vblank(struct drm_device *dev, unsigned int pipe);
bool drm_crtc_handle_vblank(struct drm_crtc *crtc);
int drm_crtc_vblank_get(struct drm_crtc *crtc);
void drm_crtc_vblank_put(struct drm_crtc *crtc);
void drm_wait_one_vblank(struct drm_device *dev, unsigned int pipe);
void drm_crtc_wait_one_vblank(struct drm_crtc *crtc);
void drm_crtc_vblank_off(struct drm_crtc *crtc);
void drm_crtc_vblank_reset(struct drm_crtc *crtc);
void drm_crtc_vblank_on(struct drm_crtc *crtc);
u64 drm_crtc_accurate_vblank_count(struct drm_crtc *crtc);
void drm_crtc_vblank_restore(struct drm_crtc *crtc);

void drm_calc_timestamping_constants(struct drm_crtc *crtc,
				     const struct drm_display_mode *mode);
wait_queue_head_t *drm_crtc_vblank_waitqueue(struct drm_crtc *crtc);
void drm_crtc_set_max_vblank_count(struct drm_crtc *crtc,
				   u32 max_vblank_count);



typedef bool (*drm_vblank_get_scanout_position_func)(struct drm_crtc *crtc,
						     bool in_vblank_irq,
						     int *vpos, int *hpos,
						     ktime_t *stime,
						     ktime_t *etime,
						     const struct drm_display_mode *mode);

bool
drm_crtc_vblank_helper_get_vblank_timestamp_internal(struct drm_crtc *crtc,
						     int *max_error,
						     ktime_t *vblank_time,
						     bool in_vblank_irq,
						     drm_vblank_get_scanout_position_func get_scanout_position);
bool drm_crtc_vblank_helper_get_vblank_timestamp(struct drm_crtc *crtc,
						 int *max_error,
						 ktime_t *vblank_time,
						 bool in_vblank_irq);

#endif
