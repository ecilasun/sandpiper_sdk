/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_SRCU_H
#define _LINUX_SRCU_H

#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <linux/workqueue.h>
#include <linux/rcu_segcblist.h>

struct srcu_struct;

#ifdef CONFIG_DEBUG_LOCK_ALLOC

int __init_srcu_struct(struct srcu_struct *ssp, const char *name,
		       struct lock_class_key *key);

#define init_srcu_struct(ssp) \
({ \
	static struct lock_class_key __srcu_key; \
	\
	__init_srcu_struct((ssp), #ssp, &__srcu_key); \
})

#define __SRCU_DEP_MAP_INIT(srcu_name)	.dep_map = { .name = #srcu_name },
#else 

int init_srcu_struct(struct srcu_struct *ssp);

#define __SRCU_DEP_MAP_INIT(srcu_name)
#endif 

#ifdef CONFIG_TINY_SRCU
#include <linux/srcutiny.h>
#elif defined(CONFIG_TREE_SRCU)
#include <linux/srcutree.h>
#else
#error "Unknown SRCU implementation specified to kernel configuration"
#endif

void call_srcu(struct srcu_struct *ssp, struct rcu_head *head,
		void (*func)(struct rcu_head *head));
void cleanup_srcu_struct(struct srcu_struct *ssp);
int __srcu_read_lock(struct srcu_struct *ssp) __acquires(ssp);
void __srcu_read_unlock(struct srcu_struct *ssp, int idx) __releases(ssp);
void synchronize_srcu(struct srcu_struct *ssp);
unsigned long get_state_synchronize_srcu(struct srcu_struct *ssp);
unsigned long start_poll_synchronize_srcu(struct srcu_struct *ssp);
bool poll_state_synchronize_srcu(struct srcu_struct *ssp, unsigned long cookie);

#ifdef CONFIG_NEED_SRCU_NMI_SAFE
int __srcu_read_lock_nmisafe(struct srcu_struct *ssp) __acquires(ssp);
void __srcu_read_unlock_nmisafe(struct srcu_struct *ssp, int idx) __releases(ssp);
#else
static inline int __srcu_read_lock_nmisafe(struct srcu_struct *ssp)
{
	return __srcu_read_lock(ssp);
}
static inline void __srcu_read_unlock_nmisafe(struct srcu_struct *ssp, int idx)
{
	__srcu_read_unlock(ssp, idx);
}
#endif 

void srcu_init(void);

#ifdef CONFIG_DEBUG_LOCK_ALLOC


static inline int srcu_read_lock_held(const struct srcu_struct *ssp)
{
	if (!debug_lockdep_rcu_enabled())
		return 1;
	return lock_is_held(&ssp->dep_map);
}




static inline void srcu_lock_acquire(struct lockdep_map *map)
{
	lock_map_acquire_read(map);
}


static inline void srcu_lock_release(struct lockdep_map *map)
{
	lock_map_release(map);
}


static inline void srcu_lock_sync(struct lockdep_map *map)
{
	lock_map_sync(map);
}

#else 

static inline int srcu_read_lock_held(const struct srcu_struct *ssp)
{
	return 1;
}

#define srcu_lock_acquire(m) do { } while (0)
#define srcu_lock_release(m) do { } while (0)
#define srcu_lock_sync(m) do { } while (0)

#endif 

#define SRCU_NMI_UNKNOWN	0x0
#define SRCU_NMI_UNSAFE		0x1
#define SRCU_NMI_SAFE		0x2

#if defined(CONFIG_PROVE_RCU) && defined(CONFIG_TREE_SRCU)
void srcu_check_nmi_safety(struct srcu_struct *ssp, bool nmi_safe);
#else
static inline void srcu_check_nmi_safety(struct srcu_struct *ssp,
					 bool nmi_safe) { }
#endif



#define srcu_dereference_check(p, ssp, c) \
	__rcu_dereference_check((p), __UNIQUE_ID(rcu), \
				(c) || srcu_read_lock_held(ssp), __rcu)


#define srcu_dereference(p, ssp) srcu_dereference_check((p), (ssp), 0)


#define srcu_dereference_notrace(p, ssp) srcu_dereference_check((p), (ssp), 1)


static inline int srcu_read_lock(struct srcu_struct *ssp) __acquires(ssp)
{
	int retval;

	srcu_check_nmi_safety(ssp, false);
	retval = __srcu_read_lock(ssp);
	srcu_lock_acquire(&ssp->dep_map);
	return retval;
}


static inline int srcu_read_lock_nmisafe(struct srcu_struct *ssp) __acquires(ssp)
{
	int retval;

	srcu_check_nmi_safety(ssp, true);
	retval = __srcu_read_lock_nmisafe(ssp);
	rcu_try_lock_acquire(&ssp->dep_map);
	return retval;
}


static inline notrace int
srcu_read_lock_notrace(struct srcu_struct *ssp) __acquires(ssp)
{
	int retval;

	srcu_check_nmi_safety(ssp, false);
	retval = __srcu_read_lock(ssp);
	return retval;
}


static inline int srcu_down_read(struct srcu_struct *ssp) __acquires(ssp)
{
	WARN_ON_ONCE(in_nmi());
	srcu_check_nmi_safety(ssp, false);
	return __srcu_read_lock(ssp);
}


static inline void srcu_read_unlock(struct srcu_struct *ssp, int idx)
	__releases(ssp)
{
	WARN_ON_ONCE(idx & ~0x1);
	srcu_check_nmi_safety(ssp, false);
	srcu_lock_release(&ssp->dep_map);
	__srcu_read_unlock(ssp, idx);
}


static inline void srcu_read_unlock_nmisafe(struct srcu_struct *ssp, int idx)
	__releases(ssp)
{
	WARN_ON_ONCE(idx & ~0x1);
	srcu_check_nmi_safety(ssp, true);
	rcu_lock_release(&ssp->dep_map);
	__srcu_read_unlock_nmisafe(ssp, idx);
}


static inline notrace void
srcu_read_unlock_notrace(struct srcu_struct *ssp, int idx) __releases(ssp)
{
	srcu_check_nmi_safety(ssp, false);
	__srcu_read_unlock(ssp, idx);
}


static inline void srcu_up_read(struct srcu_struct *ssp, int idx)
	__releases(ssp)
{
	WARN_ON_ONCE(idx & ~0x1);
	WARN_ON_ONCE(in_nmi());
	srcu_check_nmi_safety(ssp, false);
	__srcu_read_unlock(ssp, idx);
}


static inline void smp_mb__after_srcu_read_unlock(void)
{
	
}

DEFINE_LOCK_GUARD_1(srcu, struct srcu_struct,
		    _T->idx = srcu_read_lock(_T->lock),
		    srcu_read_unlock(_T->lock, _T->idx),
		    int idx)

#endif
