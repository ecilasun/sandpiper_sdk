

#ifndef DRM_MODESET_LOCK_H_
#define DRM_MODESET_LOCK_H_

#include <linux/types.h> 
#include <linux/stackdepot.h>
#include <linux/ww_mutex.h>

struct drm_modeset_lock;


struct drm_modeset_acquire_ctx {

	struct ww_acquire_ctx ww_ctx;

	
	struct drm_modeset_lock *contended;

	
	depot_stack_handle_t stack_depot;

	
	struct list_head locked;

	
	bool trylock_only;

	
	bool interruptible;
};


struct drm_modeset_lock {
	
	struct ww_mutex mutex;

	
	struct list_head head;
};

#define DRM_MODESET_ACQUIRE_INTERRUPTIBLE BIT(0)

void drm_modeset_acquire_init(struct drm_modeset_acquire_ctx *ctx,
		uint32_t flags);
void drm_modeset_acquire_fini(struct drm_modeset_acquire_ctx *ctx);
void drm_modeset_drop_locks(struct drm_modeset_acquire_ctx *ctx);
int drm_modeset_backoff(struct drm_modeset_acquire_ctx *ctx);

void drm_modeset_lock_init(struct drm_modeset_lock *lock);


static inline void drm_modeset_lock_fini(struct drm_modeset_lock *lock)
{
	WARN_ON(!list_empty(&lock->head));
}


static inline bool drm_modeset_is_locked(struct drm_modeset_lock *lock)
{
	return ww_mutex_is_locked(&lock->mutex);
}


static inline void drm_modeset_lock_assert_held(struct drm_modeset_lock *lock)
{
	lockdep_assert_held(&lock->mutex.base);
}

int drm_modeset_lock(struct drm_modeset_lock *lock,
		struct drm_modeset_acquire_ctx *ctx);
int __must_check drm_modeset_lock_single_interruptible(struct drm_modeset_lock *lock);
void drm_modeset_unlock(struct drm_modeset_lock *lock);

struct drm_device;
struct drm_crtc;
struct drm_plane;

void drm_modeset_lock_all(struct drm_device *dev);
void drm_modeset_unlock_all(struct drm_device *dev);
void drm_warn_on_modeset_not_all_locked(struct drm_device *dev);

int drm_modeset_lock_all_ctx(struct drm_device *dev,
			     struct drm_modeset_acquire_ctx *ctx);


#define DRM_MODESET_LOCK_ALL_BEGIN(dev, ctx, flags, ret)		\
	if (!drm_drv_uses_atomic_modeset(dev))				\
		mutex_lock(&dev->mode_config.mutex);			\
	drm_modeset_acquire_init(&ctx, flags);				\
modeset_lock_retry:							\
	ret = drm_modeset_lock_all_ctx(dev, &ctx);			\
	if (ret)							\
		goto modeset_lock_fail;


#define DRM_MODESET_LOCK_ALL_END(dev, ctx, ret)				\
modeset_lock_fail:							\
	if (ret == -EDEADLK) {						\
		ret = drm_modeset_backoff(&ctx);			\
		if (!ret)						\
			goto modeset_lock_retry;			\
	}								\
	drm_modeset_drop_locks(&ctx);					\
	drm_modeset_acquire_fini(&ctx);					\
	if (!drm_drv_uses_atomic_modeset(dev))				\
		mutex_unlock(&dev->mode_config.mutex);

#endif 
