/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_RWSEM_H
#define _LINUX_RWSEM_H

#include <linux/linkage.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/atomic.h>
#include <linux/err.h>
#include <linux/cleanup.h>

#ifdef CONFIG_DEBUG_LOCK_ALLOC
# define __RWSEM_DEP_MAP_INIT(lockname)			\
	.dep_map = {					\
		.name = #lockname,			\
		.wait_type_inner = LD_WAIT_SLEEP,	\
	},
#else
# define __RWSEM_DEP_MAP_INIT(lockname)
#endif

#ifndef CONFIG_PREEMPT_RT

#ifdef CONFIG_RWSEM_SPIN_ON_OWNER
#include <linux/osq_lock.h>
#endif


struct rw_semaphore {
	atomic_long_t count;
	
	atomic_long_t owner;
#ifdef CONFIG_RWSEM_SPIN_ON_OWNER
	struct optimistic_spin_queue osq; 
#endif
	raw_spinlock_t wait_lock;
	struct list_head wait_list;
#ifdef CONFIG_DEBUG_RWSEMS
	void *magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
};


static inline int rwsem_is_locked(struct rw_semaphore *sem)
{
	return atomic_long_read(&sem->count) != 0;
}

#define RWSEM_UNLOCKED_VALUE		0L
#define __RWSEM_COUNT_INIT(name)	.count = ATOMIC_LONG_INIT(RWSEM_UNLOCKED_VALUE)



#ifdef CONFIG_DEBUG_RWSEMS
# define __RWSEM_DEBUG_INIT(lockname) .magic = &lockname,
#else
# define __RWSEM_DEBUG_INIT(lockname)
#endif

#ifdef CONFIG_RWSEM_SPIN_ON_OWNER
#define __RWSEM_OPT_INIT(lockname) .osq = OSQ_LOCK_UNLOCKED,
#else
#define __RWSEM_OPT_INIT(lockname)
#endif

#define __RWSEM_INITIALIZER(name)				\
	{ __RWSEM_COUNT_INIT(name),				\
	  .owner = ATOMIC_LONG_INIT(0),				\
	  __RWSEM_OPT_INIT(name)				\
	  .wait_lock = __RAW_SPIN_LOCK_UNLOCKED(name.wait_lock),\
	  .wait_list = LIST_HEAD_INIT((name).wait_list),	\
	  __RWSEM_DEBUG_INIT(name)				\
	  __RWSEM_DEP_MAP_INIT(name) }

#define DECLARE_RWSEM(name) \
	struct rw_semaphore name = __RWSEM_INITIALIZER(name)

extern void __init_rwsem(struct rw_semaphore *sem, const char *name,
			 struct lock_class_key *key);

#define init_rwsem(sem)						\
do {								\
	static struct lock_class_key __key;			\
								\
	__init_rwsem((sem), #sem, &__key);			\
} while (0)


static inline int rwsem_is_contended(struct rw_semaphore *sem)
{
	return !list_empty(&sem->wait_list);
}

#else 

#include <linux/rwbase_rt.h>

struct rw_semaphore {
	struct rwbase_rt	rwbase;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
};

#define __RWSEM_INITIALIZER(name)				\
	{							\
		.rwbase = __RWBASE_INITIALIZER(name),		\
		__RWSEM_DEP_MAP_INIT(name)			\
	}

#define DECLARE_RWSEM(lockname) \
	struct rw_semaphore lockname = __RWSEM_INITIALIZER(lockname)

extern void  __init_rwsem(struct rw_semaphore *rwsem, const char *name,
			  struct lock_class_key *key);

#define init_rwsem(sem)						\
do {								\
	static struct lock_class_key __key;			\
								\
	__init_rwsem((sem), #sem, &__key);			\
} while (0)

static __always_inline int rwsem_is_locked(struct rw_semaphore *sem)
{
	return rw_base_is_locked(&sem->rwbase);
}

static __always_inline int rwsem_is_contended(struct rw_semaphore *sem)
{
	return rw_base_is_contended(&sem->rwbase);
}

#endif 




extern void down_read(struct rw_semaphore *sem);
extern int __must_check down_read_interruptible(struct rw_semaphore *sem);
extern int __must_check down_read_killable(struct rw_semaphore *sem);


extern int down_read_trylock(struct rw_semaphore *sem);


extern void down_write(struct rw_semaphore *sem);
extern int __must_check down_write_killable(struct rw_semaphore *sem);


extern int down_write_trylock(struct rw_semaphore *sem);


extern void up_read(struct rw_semaphore *sem);


extern void up_write(struct rw_semaphore *sem);

DEFINE_GUARD(rwsem_read, struct rw_semaphore *, down_read(_T), up_read(_T))
DEFINE_GUARD(rwsem_write, struct rw_semaphore *, down_write(_T), up_write(_T))

DEFINE_FREE(up_read, struct rw_semaphore *, if (_T) up_read(_T))
DEFINE_FREE(up_write, struct rw_semaphore *, if (_T) up_write(_T))



extern void downgrade_write(struct rw_semaphore *sem);

#ifdef CONFIG_DEBUG_LOCK_ALLOC

extern void down_read_nested(struct rw_semaphore *sem, int subclass);
extern int __must_check down_read_killable_nested(struct rw_semaphore *sem, int subclass);
extern void down_write_nested(struct rw_semaphore *sem, int subclass);
extern int down_write_killable_nested(struct rw_semaphore *sem, int subclass);
extern void _down_write_nest_lock(struct rw_semaphore *sem, struct lockdep_map *nest_lock);

# define down_write_nest_lock(sem, nest_lock)			\
do {								\
	typecheck(struct lockdep_map *, &(nest_lock)->dep_map);	\
	_down_write_nest_lock(sem, &(nest_lock)->dep_map);	\
} while (0)


extern void down_read_non_owner(struct rw_semaphore *sem);
extern void up_read_non_owner(struct rw_semaphore *sem);
#else
# define down_read_nested(sem, subclass)		down_read(sem)
# define down_read_killable_nested(sem, subclass)	down_read_killable(sem)
# define down_write_nest_lock(sem, nest_lock)	down_write(sem)
# define down_write_nested(sem, subclass)	down_write(sem)
# define down_write_killable_nested(sem, subclass)	down_write_killable(sem)
# define down_read_non_owner(sem)		down_read(sem)
# define up_read_non_owner(sem)			up_read(sem)
#endif

#endif 
