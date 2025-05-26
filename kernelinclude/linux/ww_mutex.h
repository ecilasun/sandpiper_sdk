/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_WW_MUTEX_H
#define __LINUX_WW_MUTEX_H

#include <linux/mutex.h>
#include <linux/rtmutex.h>

#if defined(CONFIG_DEBUG_MUTEXES) || \
   (defined(CONFIG_PREEMPT_RT) && defined(CONFIG_DEBUG_RT_MUTEXES))
#define DEBUG_WW_MUTEXES
#endif

#ifndef CONFIG_PREEMPT_RT
#define WW_MUTEX_BASE			mutex
#define ww_mutex_base_init(l,n,k)	__mutex_init(l,n,k)
#define ww_mutex_base_is_locked(b)	mutex_is_locked((b))
#else
#define WW_MUTEX_BASE			rt_mutex
#define ww_mutex_base_init(l,n,k)	__rt_mutex_init(l,n,k)
#define ww_mutex_base_is_locked(b)	rt_mutex_base_is_locked(&(b)->rtmutex)
#endif

struct ww_class {
	atomic_long_t stamp;
	struct lock_class_key acquire_key;
	struct lock_class_key mutex_key;
	const char *acquire_name;
	const char *mutex_name;
	unsigned int is_wait_die;
};

struct ww_mutex {
	struct WW_MUTEX_BASE base;
	struct ww_acquire_ctx *ctx;
#ifdef DEBUG_WW_MUTEXES
	struct ww_class *ww_class;
#endif
};

struct ww_acquire_ctx {
	struct task_struct *task;
	unsigned long stamp;
	unsigned int acquired;
	unsigned short wounded;
	unsigned short is_wait_die;
#ifdef DEBUG_WW_MUTEXES
	unsigned int done_acquire;
	struct ww_class *ww_class;
	void *contending_lock;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
#ifdef CONFIG_DEBUG_WW_MUTEX_SLOWPATH
	unsigned int deadlock_inject_interval;
	unsigned int deadlock_inject_countdown;
#endif
};

#define __WW_CLASS_INITIALIZER(ww_class, _is_wait_die)	    \
		{ .stamp = ATOMIC_LONG_INIT(0) \
		, .acquire_name = #ww_class "_acquire" \
		, .mutex_name = #ww_class "_mutex" \
		, .is_wait_die = _is_wait_die }

#define DEFINE_WD_CLASS(classname) \
	struct ww_class classname = __WW_CLASS_INITIALIZER(classname, 1)

#define DEFINE_WW_CLASS(classname) \
	struct ww_class classname = __WW_CLASS_INITIALIZER(classname, 0)


static inline void ww_mutex_init(struct ww_mutex *lock,
				 struct ww_class *ww_class)
{
	ww_mutex_base_init(&lock->base, ww_class->mutex_name, &ww_class->mutex_key);
	lock->ctx = NULL;
#ifdef DEBUG_WW_MUTEXES
	lock->ww_class = ww_class;
#endif
}


static inline void ww_acquire_init(struct ww_acquire_ctx *ctx,
				   struct ww_class *ww_class)
{
	ctx->task = current;
	ctx->stamp = atomic_long_inc_return_relaxed(&ww_class->stamp);
	ctx->acquired = 0;
	ctx->wounded = false;
	ctx->is_wait_die = ww_class->is_wait_die;
#ifdef DEBUG_WW_MUTEXES
	ctx->ww_class = ww_class;
	ctx->done_acquire = 0;
	ctx->contending_lock = NULL;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	debug_check_no_locks_freed((void *)ctx, sizeof(*ctx));
	lockdep_init_map(&ctx->dep_map, ww_class->acquire_name,
			 &ww_class->acquire_key, 0);
	mutex_acquire(&ctx->dep_map, 0, 0, _RET_IP_);
#endif
#ifdef CONFIG_DEBUG_WW_MUTEX_SLOWPATH
	ctx->deadlock_inject_interval = 1;
	ctx->deadlock_inject_countdown = ctx->stamp & 0xf;
#endif
}


static inline void ww_acquire_done(struct ww_acquire_ctx *ctx)
{
#ifdef DEBUG_WW_MUTEXES
	lockdep_assert_held(ctx);

	DEBUG_LOCKS_WARN_ON(ctx->done_acquire);
	ctx->done_acquire = 1;
#endif
}


static inline void ww_acquire_fini(struct ww_acquire_ctx *ctx)
{
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	mutex_release(&ctx->dep_map, _THIS_IP_);
#endif
#ifdef DEBUG_WW_MUTEXES
	DEBUG_LOCKS_WARN_ON(ctx->acquired);
	if (!IS_ENABLED(CONFIG_PROVE_LOCKING))
		
		ctx->done_acquire = 1;

	if (!IS_ENABLED(CONFIG_DEBUG_LOCK_ALLOC))
		
		ctx->acquired = ~0U;
#endif
}


extern int  ww_mutex_lock(struct ww_mutex *lock, struct ww_acquire_ctx *ctx);


extern int __must_check ww_mutex_lock_interruptible(struct ww_mutex *lock,
						    struct ww_acquire_ctx *ctx);


static inline void
ww_mutex_lock_slow(struct ww_mutex *lock, struct ww_acquire_ctx *ctx)
{
	int ret;
#ifdef DEBUG_WW_MUTEXES
	DEBUG_LOCKS_WARN_ON(!ctx->contending_lock);
#endif
	ret = ww_mutex_lock(lock, ctx);
	(void)ret;
}


static inline int __must_check
ww_mutex_lock_slow_interruptible(struct ww_mutex *lock,
				 struct ww_acquire_ctx *ctx)
{
#ifdef DEBUG_WW_MUTEXES
	DEBUG_LOCKS_WARN_ON(!ctx->contending_lock);
#endif
	return ww_mutex_lock_interruptible(lock, ctx);
}

extern void ww_mutex_unlock(struct ww_mutex *lock);

extern int __must_check ww_mutex_trylock(struct ww_mutex *lock,
					 struct ww_acquire_ctx *ctx);


static inline void ww_mutex_destroy(struct ww_mutex *lock)
{
#ifndef CONFIG_PREEMPT_RT
	mutex_destroy(&lock->base);
#endif
}


static inline bool ww_mutex_is_locked(struct ww_mutex *lock)
{
	return ww_mutex_base_is_locked(&lock->base);
}

#endif
