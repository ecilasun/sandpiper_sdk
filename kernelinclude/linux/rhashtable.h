/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_RHASHTABLE_H
#define _LINUX_RHASHTABLE_H

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/jhash.h>
#include <linux/list_nulls.h>
#include <linux/workqueue.h>
#include <linux/rculist.h>
#include <linux/bit_spinlock.h>

#include <linux/rhashtable-types.h>

struct rhash_lock_head {};


#define RHT_ELASTICITY	16u


struct bucket_table {
	unsigned int		size;
	unsigned int		nest;
	u32			hash_rnd;
	struct list_head	walkers;
	struct rcu_head		rcu;

	struct bucket_table __rcu *future_tbl;

	struct lockdep_map	dep_map;

	struct rhash_lock_head __rcu *buckets[] ____cacheline_aligned_in_smp;
};


#define	RHT_NULLS_MARKER(ptr)	\
	((void *)NULLS_MARKER(((unsigned long) (ptr)) >> 1))
#define INIT_RHT_NULLS_HEAD(ptr)	\
	((ptr) = NULL)

static inline bool rht_is_a_nulls(const struct rhash_head *ptr)
{
	return ((unsigned long) ptr & 1);
}

static inline void *rht_obj(const struct rhashtable *ht,
			    const struct rhash_head *he)
{
	return (char *)he - ht->p.head_offset;
}

static inline unsigned int rht_bucket_index(const struct bucket_table *tbl,
					    unsigned int hash)
{
	return hash & (tbl->size - 1);
}

static inline unsigned int rht_key_get_hash(struct rhashtable *ht,
	const void *key, const struct rhashtable_params params,
	unsigned int hash_rnd)
{
	unsigned int hash;

	
	if (!__builtin_constant_p(params.key_len))
		hash = ht->p.hashfn(key, ht->key_len, hash_rnd);
	else if (params.key_len) {
		unsigned int key_len = params.key_len;

		if (params.hashfn)
			hash = params.hashfn(key, key_len, hash_rnd);
		else if (key_len & (sizeof(u32) - 1))
			hash = jhash(key, key_len, hash_rnd);
		else
			hash = jhash2(key, key_len / sizeof(u32), hash_rnd);
	} else {
		unsigned int key_len = ht->p.key_len;

		if (params.hashfn)
			hash = params.hashfn(key, key_len, hash_rnd);
		else
			hash = jhash(key, key_len, hash_rnd);
	}

	return hash;
}

static inline unsigned int rht_key_hashfn(
	struct rhashtable *ht, const struct bucket_table *tbl,
	const void *key, const struct rhashtable_params params)
{
	unsigned int hash = rht_key_get_hash(ht, key, params, tbl->hash_rnd);

	return rht_bucket_index(tbl, hash);
}

static inline unsigned int rht_head_hashfn(
	struct rhashtable *ht, const struct bucket_table *tbl,
	const struct rhash_head *he, const struct rhashtable_params params)
{
	const char *ptr = rht_obj(ht, he);

	return likely(params.obj_hashfn) ?
	       rht_bucket_index(tbl, params.obj_hashfn(ptr, params.key_len ?:
							    ht->p.key_len,
						       tbl->hash_rnd)) :
	       rht_key_hashfn(ht, tbl, ptr + params.key_offset, params);
}


static inline bool rht_grow_above_75(const struct rhashtable *ht,
				     const struct bucket_table *tbl)
{
	
	return atomic_read(&ht->nelems) > (tbl->size / 4 * 3) &&
	       (!ht->p.max_size || tbl->size < ht->p.max_size);
}


static inline bool rht_shrink_below_30(const struct rhashtable *ht,
				       const struct bucket_table *tbl)
{
	
	return atomic_read(&ht->nelems) < (tbl->size * 3 / 10) &&
	       tbl->size > ht->p.min_size;
}


static inline bool rht_grow_above_100(const struct rhashtable *ht,
				      const struct bucket_table *tbl)
{
	return atomic_read(&ht->nelems) > tbl->size &&
		(!ht->p.max_size || tbl->size < ht->p.max_size);
}


static inline bool rht_grow_above_max(const struct rhashtable *ht,
				      const struct bucket_table *tbl)
{
	return atomic_read(&ht->nelems) >= ht->max_elems;
}

#ifdef CONFIG_PROVE_LOCKING
int lockdep_rht_mutex_is_held(struct rhashtable *ht);
int lockdep_rht_bucket_is_held(const struct bucket_table *tbl, u32 hash);
#else
static inline int lockdep_rht_mutex_is_held(struct rhashtable *ht)
{
	return 1;
}

static inline int lockdep_rht_bucket_is_held(const struct bucket_table *tbl,
					     u32 hash)
{
	return 1;
}
#endif 

void *rhashtable_insert_slow(struct rhashtable *ht, const void *key,
			     struct rhash_head *obj);

void rhashtable_walk_enter(struct rhashtable *ht,
			   struct rhashtable_iter *iter);
void rhashtable_walk_exit(struct rhashtable_iter *iter);
int rhashtable_walk_start_check(struct rhashtable_iter *iter) __acquires(RCU);

static inline void rhashtable_walk_start(struct rhashtable_iter *iter)
{
	(void)rhashtable_walk_start_check(iter);
}

void *rhashtable_walk_next(struct rhashtable_iter *iter);
void *rhashtable_walk_peek(struct rhashtable_iter *iter);
void rhashtable_walk_stop(struct rhashtable_iter *iter) __releases(RCU);

void rhashtable_free_and_destroy(struct rhashtable *ht,
				 void (*free_fn)(void *ptr, void *arg),
				 void *arg);
void rhashtable_destroy(struct rhashtable *ht);

struct rhash_lock_head __rcu **rht_bucket_nested(
	const struct bucket_table *tbl, unsigned int hash);
struct rhash_lock_head __rcu **__rht_bucket_nested(
	const struct bucket_table *tbl, unsigned int hash);
struct rhash_lock_head __rcu **rht_bucket_nested_insert(
	struct rhashtable *ht, struct bucket_table *tbl, unsigned int hash);

#define rht_dereference(p, ht) \
	rcu_dereference_protected(p, lockdep_rht_mutex_is_held(ht))

#define rht_dereference_rcu(p, ht) \
	rcu_dereference_check(p, lockdep_rht_mutex_is_held(ht))

#define rht_dereference_bucket(p, tbl, hash) \
	rcu_dereference_protected(p, lockdep_rht_bucket_is_held(tbl, hash))

#define rht_dereference_bucket_rcu(p, tbl, hash) \
	rcu_dereference_check(p, lockdep_rht_bucket_is_held(tbl, hash))

#define rht_entry(tpos, pos, member) \
	({ tpos = container_of(pos, typeof(*tpos), member); 1; })

static inline struct rhash_lock_head __rcu *const *rht_bucket(
	const struct bucket_table *tbl, unsigned int hash)
{
	return unlikely(tbl->nest) ? rht_bucket_nested(tbl, hash) :
				     &tbl->buckets[hash];
}

static inline struct rhash_lock_head __rcu **rht_bucket_var(
	struct bucket_table *tbl, unsigned int hash)
{
	return unlikely(tbl->nest) ? __rht_bucket_nested(tbl, hash) :
				     &tbl->buckets[hash];
}

static inline struct rhash_lock_head __rcu **rht_bucket_insert(
	struct rhashtable *ht, struct bucket_table *tbl, unsigned int hash)
{
	return unlikely(tbl->nest) ? rht_bucket_nested_insert(ht, tbl, hash) :
				     &tbl->buckets[hash];
}



static inline unsigned long rht_lock(struct bucket_table *tbl,
				     struct rhash_lock_head __rcu **bkt)
{
	unsigned long flags;

	local_irq_save(flags);
	bit_spin_lock(0, (unsigned long *)bkt);
	lock_map_acquire(&tbl->dep_map);
	return flags;
}

static inline unsigned long rht_lock_nested(struct bucket_table *tbl,
					struct rhash_lock_head __rcu **bucket,
					unsigned int subclass)
{
	unsigned long flags;

	local_irq_save(flags);
	bit_spin_lock(0, (unsigned long *)bucket);
	lock_acquire_exclusive(&tbl->dep_map, subclass, 0, NULL, _THIS_IP_);
	return flags;
}

static inline void rht_unlock(struct bucket_table *tbl,
			      struct rhash_lock_head __rcu **bkt,
			      unsigned long flags)
{
	lock_map_release(&tbl->dep_map);
	bit_spin_unlock(0, (unsigned long *)bkt);
	local_irq_restore(flags);
}

static inline struct rhash_head *__rht_ptr(
	struct rhash_lock_head *p, struct rhash_lock_head __rcu *const *bkt)
{
	return (struct rhash_head *)
		((unsigned long)p & ~BIT(0) ?:
		 (unsigned long)RHT_NULLS_MARKER(bkt));
}


static inline struct rhash_head *rht_ptr_rcu(
	struct rhash_lock_head __rcu *const *bkt)
{
	return __rht_ptr(rcu_dereference(*bkt), bkt);
}

static inline struct rhash_head *rht_ptr(
	struct rhash_lock_head __rcu *const *bkt,
	struct bucket_table *tbl,
	unsigned int hash)
{
	return __rht_ptr(rht_dereference_bucket(*bkt, tbl, hash), bkt);
}

static inline struct rhash_head *rht_ptr_exclusive(
	struct rhash_lock_head __rcu *const *bkt)
{
	return __rht_ptr(rcu_dereference_protected(*bkt, 1), bkt);
}

static inline void rht_assign_locked(struct rhash_lock_head __rcu **bkt,
				     struct rhash_head *obj)
{
	if (rht_is_a_nulls(obj))
		obj = NULL;
	rcu_assign_pointer(*bkt, (void *)((unsigned long)obj | BIT(0)));
}

static inline void rht_assign_unlock(struct bucket_table *tbl,
				     struct rhash_lock_head __rcu **bkt,
				     struct rhash_head *obj,
				     unsigned long flags)
{
	if (rht_is_a_nulls(obj))
		obj = NULL;
	lock_map_release(&tbl->dep_map);
	rcu_assign_pointer(*bkt, (void *)obj);
	preempt_enable();
	__release(bitlock);
	local_irq_restore(flags);
}


#define rht_for_each_from(pos, head, tbl, hash) \
	for (pos = head;			\
	     !rht_is_a_nulls(pos);		\
	     pos = rht_dereference_bucket((pos)->next, tbl, hash))


#define rht_for_each(pos, tbl, hash) \
	rht_for_each_from(pos, rht_ptr(rht_bucket(tbl, hash), tbl, hash),  \
			  tbl, hash)


#define rht_for_each_entry_from(tpos, pos, head, tbl, hash, member)	\
	for (pos = head;						\
	     (!rht_is_a_nulls(pos)) && rht_entry(tpos, pos, member);	\
	     pos = rht_dereference_bucket((pos)->next, tbl, hash))


#define rht_for_each_entry(tpos, pos, tbl, hash, member)		\
	rht_for_each_entry_from(tpos, pos,				\
				rht_ptr(rht_bucket(tbl, hash), tbl, hash), \
				tbl, hash, member)


#define rht_for_each_entry_safe(tpos, pos, next, tbl, hash, member)	      \
	for (pos = rht_ptr(rht_bucket(tbl, hash), tbl, hash),		      \
	     next = !rht_is_a_nulls(pos) ?				      \
		       rht_dereference_bucket(pos->next, tbl, hash) : NULL;   \
	     (!rht_is_a_nulls(pos)) && rht_entry(tpos, pos, member);	      \
	     pos = next,						      \
	     next = !rht_is_a_nulls(pos) ?				      \
		       rht_dereference_bucket(pos->next, tbl, hash) : NULL)


#define rht_for_each_rcu_from(pos, head, tbl, hash)			\
	for (({barrier(); }),						\
	     pos = head;						\
	     !rht_is_a_nulls(pos);					\
	     pos = rcu_dereference_raw(pos->next))


#define rht_for_each_rcu(pos, tbl, hash)			\
	for (({barrier(); }),					\
	     pos = rht_ptr_rcu(rht_bucket(tbl, hash));		\
	     !rht_is_a_nulls(pos);				\
	     pos = rcu_dereference_raw(pos->next))


#define rht_for_each_entry_rcu_from(tpos, pos, head, tbl, hash, member) \
	for (({barrier(); }),						    \
	     pos = head;						    \
	     (!rht_is_a_nulls(pos)) && rht_entry(tpos, pos, member);	    \
	     pos = rht_dereference_bucket_rcu(pos->next, tbl, hash))


#define rht_for_each_entry_rcu(tpos, pos, tbl, hash, member)		   \
	rht_for_each_entry_rcu_from(tpos, pos,				   \
				    rht_ptr_rcu(rht_bucket(tbl, hash)),	   \
				    tbl, hash, member)


#define rhl_for_each_rcu(pos, list)					\
	for (pos = list; pos; pos = rcu_dereference_raw(pos->next))


#define rhl_for_each_entry_rcu(tpos, pos, list, member)			\
	for (pos = list; pos && rht_entry(tpos, pos, member);		\
	     pos = rcu_dereference_raw(pos->next))

static inline int rhashtable_compare(struct rhashtable_compare_arg *arg,
				     const void *obj)
{
	struct rhashtable *ht = arg->ht;
	const char *ptr = obj;

	return memcmp(ptr + ht->p.key_offset, arg->key, ht->p.key_len);
}


static inline struct rhash_head *__rhashtable_lookup(
	struct rhashtable *ht, const void *key,
	const struct rhashtable_params params)
{
	struct rhashtable_compare_arg arg = {
		.ht = ht,
		.key = key,
	};
	struct rhash_lock_head __rcu *const *bkt;
	struct bucket_table *tbl;
	struct rhash_head *he;
	unsigned int hash;

	tbl = rht_dereference_rcu(ht->tbl, ht);
restart:
	hash = rht_key_hashfn(ht, tbl, key, params);
	bkt = rht_bucket(tbl, hash);
	do {
		rht_for_each_rcu_from(he, rht_ptr_rcu(bkt), tbl, hash) {
			if (params.obj_cmpfn ?
			    params.obj_cmpfn(&arg, rht_obj(ht, he)) :
			    rhashtable_compare(&arg, rht_obj(ht, he)))
				continue;
			return he;
		}
		
	} while (he != RHT_NULLS_MARKER(bkt));

	
	smp_rmb();

	tbl = rht_dereference_rcu(tbl->future_tbl, ht);
	if (unlikely(tbl))
		goto restart;

	return NULL;
}


static inline void *rhashtable_lookup(
	struct rhashtable *ht, const void *key,
	const struct rhashtable_params params)
{
	struct rhash_head *he = __rhashtable_lookup(ht, key, params);

	return he ? rht_obj(ht, he) : NULL;
}


static inline void *rhashtable_lookup_fast(
	struct rhashtable *ht, const void *key,
	const struct rhashtable_params params)
{
	void *obj;

	rcu_read_lock();
	obj = rhashtable_lookup(ht, key, params);
	rcu_read_unlock();

	return obj;
}


static inline struct rhlist_head *rhltable_lookup(
	struct rhltable *hlt, const void *key,
	const struct rhashtable_params params)
{
	struct rhash_head *he = __rhashtable_lookup(&hlt->ht, key, params);

	return he ? container_of(he, struct rhlist_head, rhead) : NULL;
}


static inline void *__rhashtable_insert_fast(
	struct rhashtable *ht, const void *key, struct rhash_head *obj,
	const struct rhashtable_params params, bool rhlist)
{
	struct rhashtable_compare_arg arg = {
		.ht = ht,
		.key = key,
	};
	struct rhash_lock_head __rcu **bkt;
	struct rhash_head __rcu **pprev;
	struct bucket_table *tbl;
	struct rhash_head *head;
	unsigned long flags;
	unsigned int hash;
	int elasticity;
	void *data;

	rcu_read_lock();

	tbl = rht_dereference_rcu(ht->tbl, ht);
	hash = rht_head_hashfn(ht, tbl, obj, params);
	elasticity = RHT_ELASTICITY;
	bkt = rht_bucket_insert(ht, tbl, hash);
	data = ERR_PTR(-ENOMEM);
	if (!bkt)
		goto out;
	pprev = NULL;
	flags = rht_lock(tbl, bkt);

	if (unlikely(rcu_access_pointer(tbl->future_tbl))) {
slow_path:
		rht_unlock(tbl, bkt, flags);
		rcu_read_unlock();
		return rhashtable_insert_slow(ht, key, obj);
	}

	rht_for_each_from(head, rht_ptr(bkt, tbl, hash), tbl, hash) {
		struct rhlist_head *plist;
		struct rhlist_head *list;

		elasticity--;
		if (!key ||
		    (params.obj_cmpfn ?
		     params.obj_cmpfn(&arg, rht_obj(ht, head)) :
		     rhashtable_compare(&arg, rht_obj(ht, head)))) {
			pprev = &head->next;
			continue;
		}

		data = rht_obj(ht, head);

		if (!rhlist)
			goto out_unlock;


		list = container_of(obj, struct rhlist_head, rhead);
		plist = container_of(head, struct rhlist_head, rhead);

		RCU_INIT_POINTER(list->next, plist);
		head = rht_dereference_bucket(head->next, tbl, hash);
		RCU_INIT_POINTER(list->rhead.next, head);
		if (pprev) {
			rcu_assign_pointer(*pprev, obj);
			rht_unlock(tbl, bkt, flags);
		} else
			rht_assign_unlock(tbl, bkt, obj, flags);
		data = NULL;
		goto out;
	}

	if (elasticity <= 0)
		goto slow_path;

	data = ERR_PTR(-E2BIG);
	if (unlikely(rht_grow_above_max(ht, tbl)))
		goto out_unlock;

	if (unlikely(rht_grow_above_100(ht, tbl)))
		goto slow_path;

	
	head = rht_ptr(bkt, tbl, hash);

	RCU_INIT_POINTER(obj->next, head);
	if (rhlist) {
		struct rhlist_head *list;

		list = container_of(obj, struct rhlist_head, rhead);
		RCU_INIT_POINTER(list->next, NULL);
	}

	atomic_inc(&ht->nelems);
	rht_assign_unlock(tbl, bkt, obj, flags);

	if (rht_grow_above_75(ht, tbl))
		schedule_work(&ht->run_work);

	data = NULL;
out:
	rcu_read_unlock();

	return data;

out_unlock:
	rht_unlock(tbl, bkt, flags);
	goto out;
}


static inline int rhashtable_insert_fast(
	struct rhashtable *ht, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	void *ret;

	ret = __rhashtable_insert_fast(ht, NULL, obj, params, false);
	if (IS_ERR(ret))
		return PTR_ERR(ret);

	return ret == NULL ? 0 : -EEXIST;
}


static inline int rhltable_insert_key(
	struct rhltable *hlt, const void *key, struct rhlist_head *list,
	const struct rhashtable_params params)
{
	return PTR_ERR(__rhashtable_insert_fast(&hlt->ht, key, &list->rhead,
						params, true));
}


static inline int rhltable_insert(
	struct rhltable *hlt, struct rhlist_head *list,
	const struct rhashtable_params params)
{
	const char *key = rht_obj(&hlt->ht, &list->rhead);

	key += params.key_offset;

	return rhltable_insert_key(hlt, key, list, params);
}


static inline int rhashtable_lookup_insert_fast(
	struct rhashtable *ht, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	const char *key = rht_obj(ht, obj);
	void *ret;

	BUG_ON(ht->p.obj_hashfn);

	ret = __rhashtable_insert_fast(ht, key + ht->p.key_offset, obj, params,
				       false);
	if (IS_ERR(ret))
		return PTR_ERR(ret);

	return ret == NULL ? 0 : -EEXIST;
}


static inline void *rhashtable_lookup_get_insert_fast(
	struct rhashtable *ht, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	const char *key = rht_obj(ht, obj);

	BUG_ON(ht->p.obj_hashfn);

	return __rhashtable_insert_fast(ht, key + ht->p.key_offset, obj, params,
					false);
}


static inline int rhashtable_lookup_insert_key(
	struct rhashtable *ht, const void *key, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	void *ret;

	BUG_ON(!ht->p.obj_hashfn || !key);

	ret = __rhashtable_insert_fast(ht, key, obj, params, false);
	if (IS_ERR(ret))
		return PTR_ERR(ret);

	return ret == NULL ? 0 : -EEXIST;
}


static inline void *rhashtable_lookup_get_insert_key(
	struct rhashtable *ht, const void *key, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	BUG_ON(!ht->p.obj_hashfn || !key);

	return __rhashtable_insert_fast(ht, key, obj, params, false);
}


static inline int __rhashtable_remove_fast_one(
	struct rhashtable *ht, struct bucket_table *tbl,
	struct rhash_head *obj, const struct rhashtable_params params,
	bool rhlist)
{
	struct rhash_lock_head __rcu **bkt;
	struct rhash_head __rcu **pprev;
	struct rhash_head *he;
	unsigned long flags;
	unsigned int hash;
	int err = -ENOENT;

	hash = rht_head_hashfn(ht, tbl, obj, params);
	bkt = rht_bucket_var(tbl, hash);
	if (!bkt)
		return -ENOENT;
	pprev = NULL;
	flags = rht_lock(tbl, bkt);

	rht_for_each_from(he, rht_ptr(bkt, tbl, hash), tbl, hash) {
		struct rhlist_head *list;

		list = container_of(he, struct rhlist_head, rhead);

		if (he != obj) {
			struct rhlist_head __rcu **lpprev;

			pprev = &he->next;

			if (!rhlist)
				continue;

			do {
				lpprev = &list->next;
				list = rht_dereference_bucket(list->next,
							      tbl, hash);
			} while (list && obj != &list->rhead);

			if (!list)
				continue;

			list = rht_dereference_bucket(list->next, tbl, hash);
			RCU_INIT_POINTER(*lpprev, list);
			err = 0;
			break;
		}

		obj = rht_dereference_bucket(obj->next, tbl, hash);
		err = 1;

		if (rhlist) {
			list = rht_dereference_bucket(list->next, tbl, hash);
			if (list) {
				RCU_INIT_POINTER(list->rhead.next, obj);
				obj = &list->rhead;
				err = 0;
			}
		}

		if (pprev) {
			rcu_assign_pointer(*pprev, obj);
			rht_unlock(tbl, bkt, flags);
		} else {
			rht_assign_unlock(tbl, bkt, obj, flags);
		}
		goto unlocked;
	}

	rht_unlock(tbl, bkt, flags);
unlocked:
	if (err > 0) {
		atomic_dec(&ht->nelems);
		if (unlikely(ht->p.automatic_shrinking &&
			     rht_shrink_below_30(ht, tbl)))
			schedule_work(&ht->run_work);
		err = 0;
	}

	return err;
}


static inline int __rhashtable_remove_fast(
	struct rhashtable *ht, struct rhash_head *obj,
	const struct rhashtable_params params, bool rhlist)
{
	struct bucket_table *tbl;
	int err;

	rcu_read_lock();

	tbl = rht_dereference_rcu(ht->tbl, ht);

	
	while ((err = __rhashtable_remove_fast_one(ht, tbl, obj, params,
						   rhlist)) &&
	       (tbl = rht_dereference_rcu(tbl->future_tbl, ht)))
		;

	rcu_read_unlock();

	return err;
}


static inline int rhashtable_remove_fast(
	struct rhashtable *ht, struct rhash_head *obj,
	const struct rhashtable_params params)
{
	return __rhashtable_remove_fast(ht, obj, params, false);
}


static inline int rhltable_remove(
	struct rhltable *hlt, struct rhlist_head *list,
	const struct rhashtable_params params)
{
	return __rhashtable_remove_fast(&hlt->ht, &list->rhead, params, true);
}


static inline int __rhashtable_replace_fast(
	struct rhashtable *ht, struct bucket_table *tbl,
	struct rhash_head *obj_old, struct rhash_head *obj_new,
	const struct rhashtable_params params)
{
	struct rhash_lock_head __rcu **bkt;
	struct rhash_head __rcu **pprev;
	struct rhash_head *he;
	unsigned long flags;
	unsigned int hash;
	int err = -ENOENT;

	
	hash = rht_head_hashfn(ht, tbl, obj_old, params);
	if (hash != rht_head_hashfn(ht, tbl, obj_new, params))
		return -EINVAL;

	bkt = rht_bucket_var(tbl, hash);
	if (!bkt)
		return -ENOENT;

	pprev = NULL;
	flags = rht_lock(tbl, bkt);

	rht_for_each_from(he, rht_ptr(bkt, tbl, hash), tbl, hash) {
		if (he != obj_old) {
			pprev = &he->next;
			continue;
		}

		rcu_assign_pointer(obj_new->next, obj_old->next);
		if (pprev) {
			rcu_assign_pointer(*pprev, obj_new);
			rht_unlock(tbl, bkt, flags);
		} else {
			rht_assign_unlock(tbl, bkt, obj_new, flags);
		}
		err = 0;
		goto unlocked;
	}

	rht_unlock(tbl, bkt, flags);

unlocked:
	return err;
}


static inline int rhashtable_replace_fast(
	struct rhashtable *ht, struct rhash_head *obj_old,
	struct rhash_head *obj_new,
	const struct rhashtable_params params)
{
	struct bucket_table *tbl;
	int err;

	rcu_read_lock();

	tbl = rht_dereference_rcu(ht->tbl, ht);

	
	while ((err = __rhashtable_replace_fast(ht, tbl, obj_old,
						obj_new, params)) &&
	       (tbl = rht_dereference_rcu(tbl->future_tbl, ht)))
		;

	rcu_read_unlock();

	return err;
}


static inline void rhltable_walk_enter(struct rhltable *hlt,
				       struct rhashtable_iter *iter)
{
	return rhashtable_walk_enter(&hlt->ht, iter);
}


static inline void rhltable_free_and_destroy(struct rhltable *hlt,
					     void (*free_fn)(void *ptr,
							     void *arg),
					     void *arg)
{
	return rhashtable_free_and_destroy(&hlt->ht, free_fn, arg);
}

static inline void rhltable_destroy(struct rhltable *hlt)
{
	return rhltable_free_and_destroy(hlt, NULL, NULL);
}

#endif 
