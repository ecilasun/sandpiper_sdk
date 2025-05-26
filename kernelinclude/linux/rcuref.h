/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _LINUX_RCUREF_H
#define _LINUX_RCUREF_H

#include <linux/atomic.h>
#include <linux/bug.h>
#include <linux/limits.h>
#include <linux/lockdep.h>
#include <linux/preempt.h>
#include <linux/rcupdate.h>

#define RCUREF_ONEREF		0x00000000U
#define RCUREF_MAXREF		0x7FFFFFFFU
#define RCUREF_SATURATED	0xA0000000U
#define RCUREF_RELEASED		0xC0000000U
#define RCUREF_DEAD		0xE0000000U
#define RCUREF_NOREF		0xFFFFFFFFU


static inline void rcuref_init(rcuref_t *ref, unsigned int cnt)
{
	atomic_set(&ref->refcnt, cnt - 1);
}


static inline unsigned int rcuref_read(rcuref_t *ref)
{
	unsigned int c = atomic_read(&ref->refcnt);

	
	return c >= RCUREF_RELEASED ? 0 : c + 1;
}

extern __must_check bool rcuref_get_slowpath(rcuref_t *ref);


static inline __must_check bool rcuref_get(rcuref_t *ref)
{
	
	if (likely(!atomic_add_negative_relaxed(1, &ref->refcnt)))
		return true;

	
	return rcuref_get_slowpath(ref);
}

extern __must_check bool rcuref_put_slowpath(rcuref_t *ref);


static __always_inline __must_check bool __rcuref_put(rcuref_t *ref)
{
	RCU_LOCKDEP_WARN(!rcu_read_lock_held() && preemptible(),
			 "suspicious rcuref_put_rcusafe() usage");
	
	if (likely(!atomic_add_negative_release(-1, &ref->refcnt)))
		return false;

	
	return rcuref_put_slowpath(ref);
}


static inline __must_check bool rcuref_put_rcusafe(rcuref_t *ref)
{
	return __rcuref_put(ref);
}


static inline __must_check bool rcuref_put(rcuref_t *ref)
{
	bool released;

	preempt_disable();
	released = __rcuref_put(ref);
	preempt_enable();
	return released;
}

#endif
