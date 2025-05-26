/* SPDX-License-Identifier: GPL-2.0 OR MIT */

#ifndef __DRM_EXEC_H__
#define __DRM_EXEC_H__

#include <linux/compiler.h>
#include <linux/ww_mutex.h>

#define DRM_EXEC_INTERRUPTIBLE_WAIT	BIT(0)
#define DRM_EXEC_IGNORE_DUPLICATES	BIT(1)

struct drm_gem_object;


struct drm_exec {
	
	uint32_t		flags;

	
	struct ww_acquire_ctx	ticket;

	
	unsigned int		num_objects;

	
	unsigned int		max_objects;

	
	struct drm_gem_object	**objects;

	
	struct drm_gem_object	*contended;

	
	struct drm_gem_object *prelocked;
};


static inline struct drm_gem_object *
drm_exec_obj(struct drm_exec *exec, unsigned long index)
{
	return index < exec->num_objects ? exec->objects[index] : NULL;
}


#define drm_exec_for_each_locked_object(exec, index, obj)		\
	for ((index) = 0; ((obj) = drm_exec_obj(exec, index)); ++(index))


#define drm_exec_for_each_locked_object_reverse(exec, index, obj)	\
	for ((index) = (exec)->num_objects - 1;				\
	     ((obj) = drm_exec_obj(exec, index)); --(index))


#define drm_exec_until_all_locked(exec)					\
__PASTE(__drm_exec_, __LINE__):						\
	for (void *__drm_exec_retry_ptr; ({				\
		__drm_exec_retry_ptr = &&__PASTE(__drm_exec_, __LINE__);\
		(void)__drm_exec_retry_ptr;				\
		drm_exec_cleanup(exec);					\
	});)


#define drm_exec_retry_on_contention(exec)			\
	do {							\
		if (unlikely(drm_exec_is_contended(exec)))	\
			goto *__drm_exec_retry_ptr;		\
	} while (0)


static inline bool drm_exec_is_contended(struct drm_exec *exec)
{
	return !!exec->contended;
}

void drm_exec_init(struct drm_exec *exec, uint32_t flags);
void drm_exec_fini(struct drm_exec *exec);
bool drm_exec_cleanup(struct drm_exec *exec);
int drm_exec_lock_obj(struct drm_exec *exec, struct drm_gem_object *obj);
void drm_exec_unlock_obj(struct drm_exec *exec, struct drm_gem_object *obj);
int drm_exec_prepare_obj(struct drm_exec *exec, struct drm_gem_object *obj,
			 unsigned int num_fences);
int drm_exec_prepare_array(struct drm_exec *exec,
			   struct drm_gem_object **objects,
			   unsigned int num_objects,
			   unsigned int num_fences);

#endif
