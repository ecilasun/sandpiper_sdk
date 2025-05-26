/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_FSCACHE_CACHE_H
#define _LINUX_FSCACHE_CACHE_H

#include <linux/fscache.h>

enum fscache_cache_trace;
enum fscache_cookie_trace;
enum fscache_access_trace;

enum fscache_cache_state {
	FSCACHE_CACHE_IS_NOT_PRESENT,	
	FSCACHE_CACHE_IS_PREPARING,	
	FSCACHE_CACHE_IS_ACTIVE,	
	FSCACHE_CACHE_GOT_IOERROR,	
	FSCACHE_CACHE_IS_WITHDRAWN,	
#define NR__FSCACHE_CACHE_STATE (FSCACHE_CACHE_IS_WITHDRAWN + 1)
};


struct fscache_cache {
	const struct fscache_cache_ops *ops;
	struct list_head	cache_link;	
	void			*cache_priv;	
	refcount_t		ref;
	atomic_t		n_volumes;	
	atomic_t		n_accesses;	
	atomic_t		object_count;	
	unsigned int		debug_id;
	enum fscache_cache_state state;
	char			*name;
};


struct fscache_cache_ops {
	
	const char *name;

	
	void (*acquire_volume)(struct fscache_volume *volume);

	
	void (*free_volume)(struct fscache_volume *volume);

	
	bool (*lookup_cookie)(struct fscache_cookie *cookie);

	
	void (*withdraw_cookie)(struct fscache_cookie *cookie);

	
	void (*resize_cookie)(struct netfs_cache_resources *cres,
			      loff_t new_size);

	
	bool (*invalidate_cookie)(struct fscache_cookie *cookie);

	
	bool (*begin_operation)(struct netfs_cache_resources *cres,
				enum fscache_want_state want_state);

	
	void (*prepare_to_write)(struct fscache_cookie *cookie);
};

extern struct workqueue_struct *fscache_wq;
extern wait_queue_head_t fscache_clearance_waiters;


extern struct rw_semaphore fscache_addremove_sem;
extern struct fscache_cache *fscache_acquire_cache(const char *name);
extern void fscache_relinquish_cache(struct fscache_cache *cache);
extern int fscache_add_cache(struct fscache_cache *cache,
			     const struct fscache_cache_ops *ops,
			     void *cache_priv);
extern void fscache_withdraw_cache(struct fscache_cache *cache);
extern void fscache_withdraw_volume(struct fscache_volume *volume);
extern void fscache_withdraw_cookie(struct fscache_cookie *cookie);

extern void fscache_io_error(struct fscache_cache *cache);

extern void fscache_end_volume_access(struct fscache_volume *volume,
				      struct fscache_cookie *cookie,
				      enum fscache_access_trace why);

extern struct fscache_cookie *fscache_get_cookie(struct fscache_cookie *cookie,
						 enum fscache_cookie_trace where);
extern void fscache_put_cookie(struct fscache_cookie *cookie,
			       enum fscache_cookie_trace where);
extern void fscache_end_cookie_access(struct fscache_cookie *cookie,
				      enum fscache_access_trace why);
extern void fscache_cookie_lookup_negative(struct fscache_cookie *cookie);
extern void fscache_resume_after_invalidation(struct fscache_cookie *cookie);
extern void fscache_caching_failed(struct fscache_cookie *cookie);
extern bool fscache_wait_for_operation(struct netfs_cache_resources *cred,
				       enum fscache_want_state state);


static inline
enum fscache_cookie_state fscache_cookie_state(struct fscache_cookie *cookie)
{
	return smp_load_acquire(&cookie->state);
}


static inline void *fscache_get_key(struct fscache_cookie *cookie)
{
	if (cookie->key_len <= sizeof(cookie->inline_key))
		return cookie->inline_key;
	else
		return cookie->key;
}

static inline struct fscache_cookie *fscache_cres_cookie(struct netfs_cache_resources *cres)
{
	return cres->cache_priv;
}


static inline void fscache_count_object(struct fscache_cache *cache)
{
	atomic_inc(&cache->object_count);
}


static inline void fscache_uncount_object(struct fscache_cache *cache)
{
	if (atomic_dec_and_test(&cache->object_count))
		wake_up_all(&fscache_clearance_waiters);
}


static inline void fscache_wait_for_objects(struct fscache_cache *cache)
{
	wait_event(fscache_clearance_waiters,
		   atomic_read(&cache->object_count) == 0);
}

#ifdef CONFIG_FSCACHE_STATS
extern atomic_t fscache_n_read;
extern atomic_t fscache_n_write;
extern atomic_t fscache_n_no_write_space;
extern atomic_t fscache_n_no_create_space;
extern atomic_t fscache_n_culled;
#define fscache_count_read() atomic_inc(&fscache_n_read)
#define fscache_count_write() atomic_inc(&fscache_n_write)
#define fscache_count_no_write_space() atomic_inc(&fscache_n_no_write_space)
#define fscache_count_no_create_space() atomic_inc(&fscache_n_no_create_space)
#define fscache_count_culled() atomic_inc(&fscache_n_culled)
#else
#define fscache_count_read() do {} while(0)
#define fscache_count_write() do {} while(0)
#define fscache_count_no_write_space() do {} while(0)
#define fscache_count_no_create_space() do {} while(0)
#define fscache_count_culled() do {} while(0)
#endif

#endif 
