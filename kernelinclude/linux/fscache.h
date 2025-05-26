/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_FSCACHE_H
#define _LINUX_FSCACHE_H

#include <linux/fs.h>
#include <linux/netfs.h>
#include <linux/writeback.h>

#if defined(CONFIG_FSCACHE) || defined(CONFIG_FSCACHE_MODULE)
#define __fscache_available (1)
#define fscache_available() (1)
#define fscache_volume_valid(volume) (volume)
#define fscache_cookie_valid(cookie) (cookie)
#define fscache_resources_valid(cres) ((cres)->cache_priv)
#define fscache_cookie_enabled(cookie) (cookie && !test_bit(FSCACHE_COOKIE_DISABLED, &cookie->flags))
#else
#define __fscache_available (0)
#define fscache_available() (0)
#define fscache_volume_valid(volume) (0)
#define fscache_cookie_valid(cookie) (0)
#define fscache_resources_valid(cres) (false)
#define fscache_cookie_enabled(cookie) (0)
#endif

struct fscache_cookie;

#define FSCACHE_ADV_SINGLE_CHUNK	0x01 
#define FSCACHE_ADV_WRITE_CACHE		0x00 
#define FSCACHE_ADV_WRITE_NOCACHE	0x02 
#define FSCACHE_ADV_WANT_CACHE_SIZE	0x04 

#define FSCACHE_INVAL_DIO_WRITE		0x01 

enum fscache_want_state {
	FSCACHE_WANT_PARAMS,
	FSCACHE_WANT_WRITE,
	FSCACHE_WANT_READ,
};


enum fscache_cookie_state {
	FSCACHE_COOKIE_STATE_QUIESCENT,		
	FSCACHE_COOKIE_STATE_LOOKING_UP,	
	FSCACHE_COOKIE_STATE_CREATING,		
	FSCACHE_COOKIE_STATE_ACTIVE,		
	FSCACHE_COOKIE_STATE_INVALIDATING,	
	FSCACHE_COOKIE_STATE_FAILED,		
	FSCACHE_COOKIE_STATE_LRU_DISCARDING,	
	FSCACHE_COOKIE_STATE_WITHDRAWING,	
	FSCACHE_COOKIE_STATE_RELINQUISHING,	
	FSCACHE_COOKIE_STATE_DROPPED,		
#define FSCACHE_COOKIE_STATE__NR (FSCACHE_COOKIE_STATE_DROPPED + 1)
} __attribute__((mode(byte)));


struct fscache_volume {
	refcount_t			ref;
	atomic_t			n_cookies;	
	atomic_t			n_accesses;	
	unsigned int			debug_id;
	unsigned int			key_hash;	
	u8				*key;		
	struct list_head		proc_link;	
	struct hlist_bl_node		hash_link;	
	struct work_struct		work;
	struct fscache_cache		*cache;		
	void				*cache_priv;	
	spinlock_t			lock;
	unsigned long			flags;
#define FSCACHE_VOLUME_RELINQUISHED	0	
#define FSCACHE_VOLUME_INVALIDATE	1	
#define FSCACHE_VOLUME_COLLIDED_WITH	2	
#define FSCACHE_VOLUME_ACQUIRE_PENDING	3	
#define FSCACHE_VOLUME_CREATING		4	
	u8				coherency_len;	
	u8				coherency[];	
};


struct fscache_cookie {
	refcount_t			ref;
	atomic_t			n_active;	
	atomic_t			n_accesses;	
	unsigned int			debug_id;
	unsigned int			inval_counter;	
	spinlock_t			lock;
	struct fscache_volume		*volume;	
	void				*cache_priv;	
	struct hlist_bl_node		hash_link;	
	struct list_head		proc_link;	
	struct list_head		commit_link;	
	struct work_struct		work;		
	loff_t				object_size;	
	unsigned long			unused_at;	
	unsigned long			flags;
#define FSCACHE_COOKIE_RELINQUISHED	0		
#define FSCACHE_COOKIE_RETIRED		1		
#define FSCACHE_COOKIE_IS_CACHING	2		
#define FSCACHE_COOKIE_NO_DATA_TO_READ	3		
#define FSCACHE_COOKIE_NEEDS_UPDATE	4		
#define FSCACHE_COOKIE_HAS_BEEN_CACHED	5		
#define FSCACHE_COOKIE_DISABLED		6		
#define FSCACHE_COOKIE_LOCAL_WRITE	7		
#define FSCACHE_COOKIE_NO_ACCESS_WAKE	8		
#define FSCACHE_COOKIE_DO_RELINQUISH	9		
#define FSCACHE_COOKIE_DO_WITHDRAW	10		
#define FSCACHE_COOKIE_DO_LRU_DISCARD	11		
#define FSCACHE_COOKIE_DO_PREP_TO_WRITE	12		
#define FSCACHE_COOKIE_HAVE_DATA	13		
#define FSCACHE_COOKIE_IS_HASHED	14		
#define FSCACHE_COOKIE_DO_INVALIDATE	15		

	enum fscache_cookie_state	state;
	u8				advice;		
	u8				key_len;	
	u8				aux_len;	
	u32				key_hash;	
	union {
		void			*key;		
		u8			inline_key[16];	
	};
	union {
		void			*aux;		
		u8			inline_aux[8];	
	};
};


extern struct fscache_volume *__fscache_acquire_volume(const char *, const char *,
						       const void *, size_t);
extern void __fscache_relinquish_volume(struct fscache_volume *, const void *, bool);

extern struct fscache_cookie *__fscache_acquire_cookie(
	struct fscache_volume *,
	u8,
	const void *, size_t,
	const void *, size_t,
	loff_t);
extern void __fscache_use_cookie(struct fscache_cookie *, bool);
extern void __fscache_unuse_cookie(struct fscache_cookie *, const void *, const loff_t *);
extern void __fscache_relinquish_cookie(struct fscache_cookie *, bool);
extern void __fscache_resize_cookie(struct fscache_cookie *, loff_t);
extern void __fscache_invalidate(struct fscache_cookie *, const void *, loff_t, unsigned int);
extern int __fscache_begin_read_operation(struct netfs_cache_resources *, struct fscache_cookie *);
extern int __fscache_begin_write_operation(struct netfs_cache_resources *, struct fscache_cookie *);

extern void __fscache_write_to_cache(struct fscache_cookie *, struct address_space *,
				     loff_t, size_t, loff_t, netfs_io_terminated_t, void *,
				     bool);
extern void __fscache_clear_page_bits(struct address_space *, loff_t, size_t);


static inline
struct fscache_volume *fscache_acquire_volume(const char *volume_key,
					      const char *cache_name,
					      const void *coherency_data,
					      size_t coherency_len)
{
	if (!fscache_available())
		return NULL;
	return __fscache_acquire_volume(volume_key, cache_name,
					coherency_data, coherency_len);
}


static inline
void fscache_relinquish_volume(struct fscache_volume *volume,
			       const void *coherency_data,
			       bool invalidate)
{
	if (fscache_volume_valid(volume))
		__fscache_relinquish_volume(volume, coherency_data, invalidate);
}


static inline
struct fscache_cookie *fscache_acquire_cookie(struct fscache_volume *volume,
					      u8 advice,
					      const void *index_key,
					      size_t index_key_len,
					      const void *aux_data,
					      size_t aux_data_len,
					      loff_t object_size)
{
	if (!fscache_volume_valid(volume))
		return NULL;
	return __fscache_acquire_cookie(volume, advice,
					index_key, index_key_len,
					aux_data, aux_data_len,
					object_size);
}


static inline void fscache_use_cookie(struct fscache_cookie *cookie,
				      bool will_modify)
{
	if (fscache_cookie_valid(cookie))
		__fscache_use_cookie(cookie, will_modify);
}


static inline void fscache_unuse_cookie(struct fscache_cookie *cookie,
					const void *aux_data,
					const loff_t *object_size)
{
	if (fscache_cookie_valid(cookie))
		__fscache_unuse_cookie(cookie, aux_data, object_size);
}


static inline
void fscache_relinquish_cookie(struct fscache_cookie *cookie, bool retire)
{
	if (fscache_cookie_valid(cookie))
		__fscache_relinquish_cookie(cookie, retire);
}


static inline void *fscache_get_aux(struct fscache_cookie *cookie)
{
	if (cookie->aux_len <= sizeof(cookie->inline_aux))
		return cookie->inline_aux;
	else
		return cookie->aux;
}


static inline
void fscache_update_aux(struct fscache_cookie *cookie,
			const void *aux_data, const loff_t *object_size)
{
	void *p = fscache_get_aux(cookie);

	if (aux_data && p)
		memcpy(p, aux_data, cookie->aux_len);
	if (object_size)
		cookie->object_size = *object_size;
}

#ifdef CONFIG_FSCACHE_STATS
extern atomic_t fscache_n_updates;
#endif

static inline
void __fscache_update_cookie(struct fscache_cookie *cookie, const void *aux_data,
			     const loff_t *object_size)
{
#ifdef CONFIG_FSCACHE_STATS
	atomic_inc(&fscache_n_updates);
#endif
	fscache_update_aux(cookie, aux_data, object_size);
	smp_wmb();
	set_bit(FSCACHE_COOKIE_NEEDS_UPDATE, &cookie->flags);
}


static inline
void fscache_update_cookie(struct fscache_cookie *cookie, const void *aux_data,
			   const loff_t *object_size)
{
	if (fscache_cookie_enabled(cookie))
		__fscache_update_cookie(cookie, aux_data, object_size);
}


static inline
void fscache_resize_cookie(struct fscache_cookie *cookie, loff_t new_size)
{
	if (fscache_cookie_enabled(cookie))
		__fscache_resize_cookie(cookie, new_size);
}


static inline
void fscache_invalidate(struct fscache_cookie *cookie,
			const void *aux_data, loff_t size, unsigned int flags)
{
	if (fscache_cookie_enabled(cookie))
		__fscache_invalidate(cookie, aux_data, size, flags);
}


static inline
const struct netfs_cache_ops *fscache_operation_valid(const struct netfs_cache_resources *cres)
{
	return fscache_resources_valid(cres) ? cres->ops : NULL;
}


static inline
int fscache_begin_read_operation(struct netfs_cache_resources *cres,
				 struct fscache_cookie *cookie)
{
	if (fscache_cookie_enabled(cookie))
		return __fscache_begin_read_operation(cres, cookie);
	return -ENOBUFS;
}


static inline void fscache_end_operation(struct netfs_cache_resources *cres)
{
	const struct netfs_cache_ops *ops = fscache_operation_valid(cres);

	if (ops)
		ops->end_operation(cres);
}


static inline
int fscache_read(struct netfs_cache_resources *cres,
		 loff_t start_pos,
		 struct iov_iter *iter,
		 enum netfs_read_from_hole read_hole,
		 netfs_io_terminated_t term_func,
		 void *term_func_priv)
{
	const struct netfs_cache_ops *ops = fscache_operation_valid(cres);
	return ops->read(cres, start_pos, iter, read_hole,
			 term_func, term_func_priv);
}


static inline
int fscache_begin_write_operation(struct netfs_cache_resources *cres,
				  struct fscache_cookie *cookie)
{
	if (fscache_cookie_enabled(cookie))
		return __fscache_begin_write_operation(cres, cookie);
	return -ENOBUFS;
}


static inline
int fscache_write(struct netfs_cache_resources *cres,
		  loff_t start_pos,
		  struct iov_iter *iter,
		  netfs_io_terminated_t term_func,
		  void *term_func_priv)
{
	const struct netfs_cache_ops *ops = fscache_operation_valid(cres);
	return ops->write(cres, start_pos, iter, term_func, term_func_priv);
}


static inline void fscache_clear_page_bits(struct address_space *mapping,
					   loff_t start, size_t len,
					   bool caching)
{
	if (caching)
		__fscache_clear_page_bits(mapping, start, len);
}


static inline void fscache_write_to_cache(struct fscache_cookie *cookie,
					  struct address_space *mapping,
					  loff_t start, size_t len, loff_t i_size,
					  netfs_io_terminated_t term_func,
					  void *term_func_priv,
					  bool caching)
{
	if (caching)
		__fscache_write_to_cache(cookie, mapping, start, len, i_size,
					 term_func, term_func_priv, caching);
	else if (term_func)
		term_func(term_func_priv, -ENOBUFS, false);

}

#if __fscache_available
bool fscache_dirty_folio(struct address_space *mapping, struct folio *folio,
		struct fscache_cookie *cookie);
#else
#define fscache_dirty_folio(MAPPING, FOLIO, COOKIE) \
		filemap_dirty_folio(MAPPING, FOLIO)
#endif


static inline void fscache_unpin_writeback(struct writeback_control *wbc,
					   struct fscache_cookie *cookie)
{
	if (wbc->unpinned_fscache_wb)
		fscache_unuse_cookie(cookie, NULL, NULL);
}


static inline void fscache_clear_inode_writeback(struct fscache_cookie *cookie,
						 struct inode *inode,
						 const void *aux)
{
	if (inode->i_state & I_PINNING_FSCACHE_WB) {
		loff_t i_size = i_size_read(inode);
		fscache_unuse_cookie(cookie, aux, &i_size);
	}
}


static inline
void fscache_note_page_release(struct fscache_cookie *cookie)
{
	
	if (cookie &&
	    test_bit(FSCACHE_COOKIE_HAVE_DATA, &cookie->flags) &&
	    test_bit(FSCACHE_COOKIE_NO_DATA_TO_READ, &cookie->flags))
		clear_bit(FSCACHE_COOKIE_NO_DATA_TO_READ, &cookie->flags);
}

#endif 
