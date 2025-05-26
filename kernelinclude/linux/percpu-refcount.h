/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_PERCPU_REFCOUNT_H
#define _LINUX_PERCPU_REFCOUNT_H

#include <linux/atomic.h>
#include <linux/percpu.h>
#include <linux/rcupdate.h>
#include <linux/types.h>
#include <linux/gfp.h>

struct percpu_ref;
typedef void (percpu_ref_func_t)(struct percpu_ref *);


enum {
	__PERCPU_REF_ATOMIC	= 1LU << 0,	
	__PERCPU_REF_DEAD	= 1LU << 1,	
	__PERCPU_REF_ATOMIC_DEAD = __PERCPU_REF_ATOMIC | __PERCPU_REF_DEAD,

	__PERCPU_REF_FLAG_BITS	= 2,
};


enum {
	
	PERCPU_REF_INIT_ATOMIC	= 1 << 0,

	
	PERCPU_REF_INIT_DEAD	= 1 << 1,

	
	PERCPU_REF_ALLOW_REINIT	= 1 << 2,
};

struct percpu_ref_data {
	atomic_long_t		count;
	percpu_ref_func_t	*release;
	percpu_ref_func_t	*confirm_switch;
	bool			force_atomic:1;
	bool			allow_reinit:1;
	struct rcu_head		rcu;
	struct percpu_ref	*ref;
};

struct percpu_ref {
	
	unsigned long		percpu_count_ptr;

	
	struct percpu_ref_data  *data;
};

int __must_check percpu_ref_init(struct percpu_ref *ref,
				 percpu_ref_func_t *release, unsigned int flags,
				 gfp_t gfp);
void percpu_ref_exit(struct percpu_ref *ref);
void percpu_ref_switch_to_atomic(struct percpu_ref *ref,
				 percpu_ref_func_t *confirm_switch);
void percpu_ref_switch_to_atomic_sync(struct percpu_ref *ref);
void percpu_ref_switch_to_percpu(struct percpu_ref *ref);
void percpu_ref_kill_and_confirm(struct percpu_ref *ref,
				 percpu_ref_func_t *confirm_kill);
void percpu_ref_resurrect(struct percpu_ref *ref);
void percpu_ref_reinit(struct percpu_ref *ref);
bool percpu_ref_is_zero(struct percpu_ref *ref);


static inline void percpu_ref_kill(struct percpu_ref *ref)
{
	percpu_ref_kill_and_confirm(ref, NULL);
}


static inline bool __ref_is_percpu(struct percpu_ref *ref,
					  unsigned long __percpu **percpu_countp)
{
	unsigned long percpu_ptr;

	
	percpu_ptr = READ_ONCE(ref->percpu_count_ptr);

	
	if (unlikely(percpu_ptr & __PERCPU_REF_ATOMIC_DEAD))
		return false;

	*percpu_countp = (unsigned long __percpu *)percpu_ptr;
	return true;
}


static inline void percpu_ref_get_many(struct percpu_ref *ref, unsigned long nr)
{
	unsigned long __percpu *percpu_count;

	rcu_read_lock();

	if (__ref_is_percpu(ref, &percpu_count))
		this_cpu_add(*percpu_count, nr);
	else
		atomic_long_add(nr, &ref->data->count);

	rcu_read_unlock();
}


static inline void percpu_ref_get(struct percpu_ref *ref)
{
	percpu_ref_get_many(ref, 1);
}


static inline bool percpu_ref_tryget_many(struct percpu_ref *ref,
					  unsigned long nr)
{
	unsigned long __percpu *percpu_count;
	bool ret;

	rcu_read_lock();

	if (__ref_is_percpu(ref, &percpu_count)) {
		this_cpu_add(*percpu_count, nr);
		ret = true;
	} else {
		ret = atomic_long_add_unless(&ref->data->count, nr, 0);
	}

	rcu_read_unlock();

	return ret;
}


static inline bool percpu_ref_tryget(struct percpu_ref *ref)
{
	return percpu_ref_tryget_many(ref, 1);
}


static inline bool percpu_ref_tryget_live_rcu(struct percpu_ref *ref)
{
	unsigned long __percpu *percpu_count;
	bool ret = false;

	WARN_ON_ONCE(!rcu_read_lock_held());

	if (likely(__ref_is_percpu(ref, &percpu_count))) {
		this_cpu_inc(*percpu_count);
		ret = true;
	} else if (!(ref->percpu_count_ptr & __PERCPU_REF_DEAD)) {
		ret = atomic_long_inc_not_zero(&ref->data->count);
	}
	return ret;
}


static inline bool percpu_ref_tryget_live(struct percpu_ref *ref)
{
	bool ret = false;

	rcu_read_lock();
	ret = percpu_ref_tryget_live_rcu(ref);
	rcu_read_unlock();
	return ret;
}


static inline void percpu_ref_put_many(struct percpu_ref *ref, unsigned long nr)
{
	unsigned long __percpu *percpu_count;

	rcu_read_lock();

	if (__ref_is_percpu(ref, &percpu_count))
		this_cpu_sub(*percpu_count, nr);
	else if (unlikely(atomic_long_sub_and_test(nr, &ref->data->count)))
		ref->data->release(ref);

	rcu_read_unlock();
}


static inline void percpu_ref_put(struct percpu_ref *ref)
{
	percpu_ref_put_many(ref, 1);
}


static inline bool percpu_ref_is_dying(struct percpu_ref *ref)
{
	return ref->percpu_count_ptr & __PERCPU_REF_DEAD;
}

#endif
