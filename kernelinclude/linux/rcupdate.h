/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __LINUX_RCUPDATE_H
#define __LINUX_RCUPDATE_H

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/atomic.h>
#include <linux/irqflags.h>
#include <linux/preempt.h>
#include <linux/bottom_half.h>
#include <linux/lockdep.h>
#include <linux/cleanup.h>
#include <asm/processor.h>
#include <linux/cpumask.h>
#include <linux/context_tracking_irq.h>

#define ULONG_CMP_GE(a, b)	(ULONG_MAX / 2 >= (a) - (b))
#define ULONG_CMP_LT(a, b)	(ULONG_MAX / 2 < (a) - (b))
#define ulong2long(a)		(*(long *)(&(a)))
#define USHORT_CMP_GE(a, b)	(USHRT_MAX / 2 >= (unsigned short)((a) - (b)))
#define USHORT_CMP_LT(a, b)	(USHRT_MAX / 2 < (unsigned short)((a) - (b)))


void call_rcu(struct rcu_head *head, rcu_callback_t func);
void rcu_barrier_tasks(void);
void rcu_barrier_tasks_rude(void);
void synchronize_rcu(void);

struct rcu_gp_oldstate;
unsigned long get_completed_synchronize_rcu(void);
void get_completed_synchronize_rcu_full(struct rcu_gp_oldstate *rgosp);

// Maximum number of unsigned long values corresponding to
// not-yet-completed RCU grace periods.
#define NUM_ACTIVE_RCU_POLL_OLDSTATE 2


static inline bool same_state_synchronize_rcu(unsigned long oldstate1, unsigned long oldstate2)
{
	return oldstate1 == oldstate2;
}

#ifdef CONFIG_PREEMPT_RCU

void __rcu_read_lock(void);
void __rcu_read_unlock(void);


#define rcu_preempt_depth() READ_ONCE(current->rcu_read_lock_nesting)

#else 

#ifdef CONFIG_TINY_RCU
#define rcu_read_unlock_strict() do { } while (0)
#else
void rcu_read_unlock_strict(void);
#endif

static inline void __rcu_read_lock(void)
{
	preempt_disable();
}

static inline void __rcu_read_unlock(void)
{
	preempt_enable();
	if (IS_ENABLED(CONFIG_RCU_STRICT_GRACE_PERIOD))
		rcu_read_unlock_strict();
}

static inline int rcu_preempt_depth(void)
{
	return 0;
}

#endif 

#ifdef CONFIG_RCU_LAZY
void call_rcu_hurry(struct rcu_head *head, rcu_callback_t func);
#else
static inline void call_rcu_hurry(struct rcu_head *head, rcu_callback_t func)
{
	call_rcu(head, func);
}
#endif


void rcu_init(void);
extern int rcu_scheduler_active;
void rcu_sched_clock_irq(int user);
void rcu_report_dead(unsigned int cpu);
void rcutree_migrate_callbacks(int cpu);

#ifdef CONFIG_TASKS_RCU_GENERIC
void rcu_init_tasks_generic(void);
#else
static inline void rcu_init_tasks_generic(void) { }
#endif

#ifdef CONFIG_RCU_STALL_COMMON
void rcu_sysrq_start(void);
void rcu_sysrq_end(void);
#else 
static inline void rcu_sysrq_start(void) { }
static inline void rcu_sysrq_end(void) { }
#endif 

#if defined(CONFIG_NO_HZ_FULL) && (!defined(CONFIG_GENERIC_ENTRY) || !defined(CONFIG_KVM_XFER_TO_GUEST_WORK))
void rcu_irq_work_resched(void);
#else
static inline void rcu_irq_work_resched(void) { }
#endif

#ifdef CONFIG_RCU_NOCB_CPU
void rcu_init_nohz(void);
int rcu_nocb_cpu_offload(int cpu);
int rcu_nocb_cpu_deoffload(int cpu);
void rcu_nocb_flush_deferred_wakeup(void);
#else 
static inline void rcu_init_nohz(void) { }
static inline int rcu_nocb_cpu_offload(int cpu) { return -EINVAL; }
static inline int rcu_nocb_cpu_deoffload(int cpu) { return 0; }
static inline void rcu_nocb_flush_deferred_wakeup(void) { }
#endif 


#ifdef CONFIG_TASKS_RCU_GENERIC

# ifdef CONFIG_TASKS_RCU
# define rcu_tasks_classic_qs(t, preempt)				\
	do {								\
		if (!(preempt) && READ_ONCE((t)->rcu_tasks_holdout))	\
			WRITE_ONCE((t)->rcu_tasks_holdout, false);	\
	} while (0)
void call_rcu_tasks(struct rcu_head *head, rcu_callback_t func);
void synchronize_rcu_tasks(void);
# else
# define rcu_tasks_classic_qs(t, preempt) do { } while (0)
# define call_rcu_tasks call_rcu
# define synchronize_rcu_tasks synchronize_rcu
# endif

# ifdef CONFIG_TASKS_TRACE_RCU
// Bits for ->trc_reader_special.b.need_qs field.
#define TRC_NEED_QS		0x1  // Task needs a quiescent state.
#define TRC_NEED_QS_CHECKED	0x2  // Task has been checked for needing quiescent state.

u8 rcu_trc_cmpxchg_need_qs(struct task_struct *t, u8 old, u8 new);
void rcu_tasks_trace_qs_blkd(struct task_struct *t);

# define rcu_tasks_trace_qs(t)							\
	do {									\
		int ___rttq_nesting = READ_ONCE((t)->trc_reader_nesting);	\
										\
		if (unlikely(READ_ONCE((t)->trc_reader_special.b.need_qs) == TRC_NEED_QS) &&	\
		    likely(!___rttq_nesting)) {					\
			rcu_trc_cmpxchg_need_qs((t), TRC_NEED_QS, TRC_NEED_QS_CHECKED);	\
		} else if (___rttq_nesting && ___rttq_nesting != INT_MIN &&	\
			   !READ_ONCE((t)->trc_reader_special.b.blocked)) {	\
			rcu_tasks_trace_qs_blkd(t);				\
		}								\
	} while (0)
# else
# define rcu_tasks_trace_qs(t) do { } while (0)
# endif

#define rcu_tasks_qs(t, preempt)					\
do {									\
	rcu_tasks_classic_qs((t), (preempt));				\
	rcu_tasks_trace_qs(t);						\
} while (0)

# ifdef CONFIG_TASKS_RUDE_RCU
void call_rcu_tasks_rude(struct rcu_head *head, rcu_callback_t func);
void synchronize_rcu_tasks_rude(void);
# endif

#define rcu_note_voluntary_context_switch(t) rcu_tasks_qs(t, false)
void exit_tasks_rcu_start(void);
void exit_tasks_rcu_stop(void);
void exit_tasks_rcu_finish(void);
#else 
#define rcu_tasks_classic_qs(t, preempt) do { } while (0)
#define rcu_tasks_qs(t, preempt) do { } while (0)
#define rcu_note_voluntary_context_switch(t) do { } while (0)
#define call_rcu_tasks call_rcu
#define synchronize_rcu_tasks synchronize_rcu
static inline void exit_tasks_rcu_start(void) { }
static inline void exit_tasks_rcu_stop(void) { }
static inline void exit_tasks_rcu_finish(void) { }
#endif 


static inline bool rcu_trace_implies_rcu_gp(void) { return true; }


#define cond_resched_tasks_rcu_qs() \
do { \
	rcu_tasks_qs(current, false); \
	cond_resched(); \
} while (0)


#define rcu_softirq_qs_periodic(old_ts) \
do { \
	if (!IS_ENABLED(CONFIG_PREEMPT_RT) && \
	    time_after(jiffies, (old_ts) + HZ / 10)) { \
		preempt_disable(); \
		rcu_softirq_qs(); \
		preempt_enable(); \
		(old_ts) = jiffies; \
	} \
} while (0)



#if defined(CONFIG_TREE_RCU)
#include <linux/rcutree.h>
#elif defined(CONFIG_TINY_RCU)
#include <linux/rcutiny.h>
#else
#error "Unknown RCU implementation specified to kernel configuration"
#endif


#ifdef CONFIG_DEBUG_OBJECTS_RCU_HEAD
void init_rcu_head(struct rcu_head *head);
void destroy_rcu_head(struct rcu_head *head);
void init_rcu_head_on_stack(struct rcu_head *head);
void destroy_rcu_head_on_stack(struct rcu_head *head);
#else 
static inline void init_rcu_head(struct rcu_head *head) { }
static inline void destroy_rcu_head(struct rcu_head *head) { }
static inline void init_rcu_head_on_stack(struct rcu_head *head) { }
static inline void destroy_rcu_head_on_stack(struct rcu_head *head) { }
#endif	

#if defined(CONFIG_HOTPLUG_CPU) && defined(CONFIG_PROVE_RCU)
bool rcu_lockdep_current_cpu_online(void);
#else 
static inline bool rcu_lockdep_current_cpu_online(void) { return true; }
#endif 

extern struct lockdep_map rcu_lock_map;
extern struct lockdep_map rcu_bh_lock_map;
extern struct lockdep_map rcu_sched_lock_map;
extern struct lockdep_map rcu_callback_map;

#ifdef CONFIG_DEBUG_LOCK_ALLOC

static inline void rcu_lock_acquire(struct lockdep_map *map)
{
	lock_acquire(map, 0, 0, 2, 0, NULL, _THIS_IP_);
}

static inline void rcu_try_lock_acquire(struct lockdep_map *map)
{
	lock_acquire(map, 0, 1, 2, 0, NULL, _THIS_IP_);
}

static inline void rcu_lock_release(struct lockdep_map *map)
{
	lock_release(map, _THIS_IP_);
}

int debug_lockdep_rcu_enabled(void);
int rcu_read_lock_held(void);
int rcu_read_lock_bh_held(void);
int rcu_read_lock_sched_held(void);
int rcu_read_lock_any_held(void);

#else 

# define rcu_lock_acquire(a)		do { } while (0)
# define rcu_try_lock_acquire(a)	do { } while (0)
# define rcu_lock_release(a)		do { } while (0)

static inline int rcu_read_lock_held(void)
{
	return 1;
}

static inline int rcu_read_lock_bh_held(void)
{
	return 1;
}

static inline int rcu_read_lock_sched_held(void)
{
	return !preemptible();
}

static inline int rcu_read_lock_any_held(void)
{
	return !preemptible();
}

static inline int debug_lockdep_rcu_enabled(void)
{
	return 0;
}

#endif 

#ifdef CONFIG_PROVE_RCU


#define RCU_LOCKDEP_WARN(c, s)						\
	do {								\
		static bool __section(".data.unlikely") __warned;	\
		if (debug_lockdep_rcu_enabled() && (c) &&		\
		    debug_lockdep_rcu_enabled() && !__warned) {		\
			__warned = true;				\
			lockdep_rcu_suspicious(__FILE__, __LINE__, s);	\
		}							\
	} while (0)

#if defined(CONFIG_PROVE_RCU) && !defined(CONFIG_PREEMPT_RCU)
static inline void rcu_preempt_sleep_check(void)
{
	RCU_LOCKDEP_WARN(lock_is_held(&rcu_lock_map),
			 "Illegal context switch in RCU read-side critical section");
}
#else 
static inline void rcu_preempt_sleep_check(void) { }
#endif 

#define rcu_sleep_check()						\
	do {								\
		rcu_preempt_sleep_check();				\
		if (!IS_ENABLED(CONFIG_PREEMPT_RT))			\
		    RCU_LOCKDEP_WARN(lock_is_held(&rcu_bh_lock_map),	\
				 "Illegal context switch in RCU-bh read-side critical section"); \
		RCU_LOCKDEP_WARN(lock_is_held(&rcu_sched_lock_map),	\
				 "Illegal context switch in RCU-sched read-side critical section"); \
	} while (0)

#else 

#define RCU_LOCKDEP_WARN(c, s) do { } while (0 && (c))
#define rcu_sleep_check() do { } while (0)

#endif 



#ifdef __CHECKER__
#define rcu_check_sparse(p, space) \
	((void)(((typeof(*p) space *)p) == p))
#else 
#define rcu_check_sparse(p, space)
#endif 

#define __unrcu_pointer(p, local)					\
({									\
	typeof(*p) *local = (typeof(*p) *__force)(p);			\
	rcu_check_sparse(p, __rcu);					\
	((typeof(*p) __force __kernel *)(local)); 			\
})

#define unrcu_pointer(p) __unrcu_pointer(p, __UNIQUE_ID(rcu))

#define __rcu_access_pointer(p, local, space) \
({ \
	typeof(*p) *local = (typeof(*p) *__force)READ_ONCE(p); \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(local)); \
})
#define __rcu_dereference_check(p, local, c, space) \
({ \
	 \
	typeof(*p) *local = (typeof(*p) *__force)READ_ONCE(p); \
	RCU_LOCKDEP_WARN(!(c), "suspicious rcu_dereference_check() usage"); \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(local)); \
})
#define __rcu_dereference_protected(p, local, c, space) \
({ \
	RCU_LOCKDEP_WARN(!(c), "suspicious rcu_dereference_protected() usage"); \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(p)); \
})
#define __rcu_dereference_raw(p, local) \
({ \
	 \
	typeof(p) local = READ_ONCE(p); \
	((typeof(*p) __force __kernel *)(local)); \
})
#define rcu_dereference_raw(p) __rcu_dereference_raw(p, __UNIQUE_ID(rcu))


#define RCU_INITIALIZER(v) (typeof(*(v)) __force __rcu *)(v)


#define rcu_assign_pointer(p, v)					      \
do {									      \
	uintptr_t _r_a_p__v = (uintptr_t)(v);				      \
	rcu_check_sparse(p, __rcu);					      \
									      \
	if (__builtin_constant_p(v) && (_r_a_p__v) == (uintptr_t)NULL)	      \
		WRITE_ONCE((p), (typeof(p))(_r_a_p__v));		      \
	else								      \
		smp_store_release(&p, RCU_INITIALIZER((typeof(p))_r_a_p__v)); \
} while (0)


#define rcu_replace_pointer(rcu_ptr, ptr, c)				\
({									\
	typeof(ptr) __tmp = rcu_dereference_protected((rcu_ptr), (c));	\
	rcu_assign_pointer((rcu_ptr), (ptr));				\
	__tmp;								\
})


#define rcu_access_pointer(p) __rcu_access_pointer((p), __UNIQUE_ID(rcu), __rcu)


#define rcu_dereference_check(p, c) \
	__rcu_dereference_check((p), __UNIQUE_ID(rcu), \
				(c) || rcu_read_lock_held(), __rcu)


#define rcu_dereference_bh_check(p, c) \
	__rcu_dereference_check((p), __UNIQUE_ID(rcu), \
				(c) || rcu_read_lock_bh_held(), __rcu)


#define rcu_dereference_sched_check(p, c) \
	__rcu_dereference_check((p), __UNIQUE_ID(rcu), \
				(c) || rcu_read_lock_sched_held(), \
				__rcu)


#define rcu_dereference_raw_check(p) \
	__rcu_dereference_check((p), __UNIQUE_ID(rcu), 1, __rcu)


#define rcu_dereference_protected(p, c) \
	__rcu_dereference_protected((p), __UNIQUE_ID(rcu), (c), __rcu)



#define rcu_dereference(p) rcu_dereference_check(p, 0)


#define rcu_dereference_bh(p) rcu_dereference_bh_check(p, 0)


#define rcu_dereference_sched(p) rcu_dereference_sched_check(p, 0)


#define rcu_pointer_handoff(p) (p)


static __always_inline void rcu_read_lock(void)
{
	__rcu_read_lock();
	__acquire(RCU);
	rcu_lock_acquire(&rcu_lock_map);
	RCU_LOCKDEP_WARN(!rcu_is_watching(),
			 "rcu_read_lock() used illegally while idle");
}




static inline void rcu_read_unlock(void)
{
	RCU_LOCKDEP_WARN(!rcu_is_watching(),
			 "rcu_read_unlock() used illegally while idle");
	__release(RCU);
	__rcu_read_unlock();
	rcu_lock_release(&rcu_lock_map); 
}


static inline void rcu_read_lock_bh(void)
{
	local_bh_disable();
	__acquire(RCU_BH);
	rcu_lock_acquire(&rcu_bh_lock_map);
	RCU_LOCKDEP_WARN(!rcu_is_watching(),
			 "rcu_read_lock_bh() used illegally while idle");
}


static inline void rcu_read_unlock_bh(void)
{
	RCU_LOCKDEP_WARN(!rcu_is_watching(),
			 "rcu_read_unlock_bh() used illegally while idle");
	rcu_lock_release(&rcu_bh_lock_map);
	__release(RCU_BH);
	local_bh_enable();
}


static inline void rcu_read_lock_sched(void)
{
	preempt_disable();
	__acquire(RCU_SCHED);
	rcu_lock_acquire(&rcu_sched_lock_map);
	RCU_LOCKDEP_WARN(!rcu_is_watching(),
			 "rcu_read_lock_sched() used illegally while idle");
}


static inline notrace void rcu_read_lock_sched_notrace(void)
{
	preempt_disable_notrace();
	__acquire(RCU_SCHED);
}


static inline void rcu_read_unlock_sched(void)
{
	RCU_LOCKDEP_WARN(!rcu_is_watching(),
			 "rcu_read_unlock_sched() used illegally while idle");
	rcu_lock_release(&rcu_sched_lock_map);
	__release(RCU_SCHED);
	preempt_enable();
}


static inline notrace void rcu_read_unlock_sched_notrace(void)
{
	__release(RCU_SCHED);
	preempt_enable_notrace();
}


#define RCU_INIT_POINTER(p, v) \
	do { \
		rcu_check_sparse(p, __rcu); \
		WRITE_ONCE(p, RCU_INITIALIZER(v)); \
	} while (0)


#define RCU_POINTER_INITIALIZER(p, v) \
		.p = RCU_INITIALIZER(v)


#define __is_kvfree_rcu_offset(offset) ((offset) < 4096)


#define kfree_rcu(ptr, rhf) kvfree_rcu_arg_2(ptr, rhf)
#define kvfree_rcu(ptr, rhf) kvfree_rcu_arg_2(ptr, rhf)


#define kfree_rcu_mightsleep(ptr) kvfree_rcu_arg_1(ptr)
#define kvfree_rcu_mightsleep(ptr) kvfree_rcu_arg_1(ptr)

#define kvfree_rcu_arg_2(ptr, rhf)					\
do {									\
	typeof (ptr) ___p = (ptr);					\
									\
	if (___p) {									\
		BUILD_BUG_ON(!__is_kvfree_rcu_offset(offsetof(typeof(*(ptr)), rhf)));	\
		kvfree_call_rcu(&((___p)->rhf), (void *) (___p));			\
	}										\
} while (0)

#define kvfree_rcu_arg_1(ptr)					\
do {								\
	typeof(ptr) ___p = (ptr);				\
								\
	if (___p)						\
		kvfree_call_rcu(NULL, (void *) (___p));		\
} while (0)


#ifdef CONFIG_ARCH_WEAK_RELEASE_ACQUIRE
#define smp_mb__after_unlock_lock()	smp_mb()  
#else 
#define smp_mb__after_unlock_lock()	do { } while (0)
#endif 





static inline void rcu_head_init(struct rcu_head *rhp)
{
	rhp->func = (rcu_callback_t)~0L;
}


static inline bool
rcu_head_after_call_rcu(struct rcu_head *rhp, rcu_callback_t f)
{
	rcu_callback_t func = READ_ONCE(rhp->func);

	if (func == f)
		return true;
	WARN_ON_ONCE(func != (rcu_callback_t)~0L);
	return false;
}


extern int rcu_expedited;
extern int rcu_normal;

DEFINE_LOCK_GUARD_0(rcu, rcu_read_lock(), rcu_read_unlock())

#endif 
