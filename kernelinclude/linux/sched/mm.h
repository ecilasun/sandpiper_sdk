/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_MM_H
#define _LINUX_SCHED_MM_H

#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/gfp.h>
#include <linux/sync_core.h>


extern struct mm_struct *mm_alloc(void);


static inline void mmgrab(struct mm_struct *mm)
{
	atomic_inc(&mm->mm_count);
}

static inline void smp_mb__after_mmgrab(void)
{
	smp_mb__after_atomic();
}

extern void __mmdrop(struct mm_struct *mm);

static inline void mmdrop(struct mm_struct *mm)
{
	
	if (unlikely(atomic_dec_and_test(&mm->mm_count)))
		__mmdrop(mm);
}

#ifdef CONFIG_PREEMPT_RT

static inline void __mmdrop_delayed(struct rcu_head *rhp)
{
	struct mm_struct *mm = container_of(rhp, struct mm_struct, delayed_drop);

	__mmdrop(mm);
}


static inline void mmdrop_sched(struct mm_struct *mm)
{
	
	if (atomic_dec_and_test(&mm->mm_count))
		call_rcu(&mm->delayed_drop, __mmdrop_delayed);
}
#else
static inline void mmdrop_sched(struct mm_struct *mm)
{
	mmdrop(mm);
}
#endif


static inline void mmgrab_lazy_tlb(struct mm_struct *mm)
{
	if (IS_ENABLED(CONFIG_MMU_LAZY_TLB_REFCOUNT))
		mmgrab(mm);
}

static inline void mmdrop_lazy_tlb(struct mm_struct *mm)
{
	if (IS_ENABLED(CONFIG_MMU_LAZY_TLB_REFCOUNT)) {
		mmdrop(mm);
	} else {
		
		smp_mb();
	}
}

static inline void mmdrop_lazy_tlb_sched(struct mm_struct *mm)
{
	if (IS_ENABLED(CONFIG_MMU_LAZY_TLB_REFCOUNT))
		mmdrop_sched(mm);
	else
		smp_mb(); 
}


static inline void mmget(struct mm_struct *mm)
{
	atomic_inc(&mm->mm_users);
}

static inline bool mmget_not_zero(struct mm_struct *mm)
{
	return atomic_inc_not_zero(&mm->mm_users);
}


extern void mmput(struct mm_struct *);
#ifdef CONFIG_MMU

void mmput_async(struct mm_struct *);
#endif


extern struct mm_struct *get_task_mm(struct task_struct *task);

extern struct mm_struct *mm_access(struct task_struct *task, unsigned int mode);

extern void exit_mm_release(struct task_struct *, struct mm_struct *);

extern void exec_mm_release(struct task_struct *, struct mm_struct *);

#ifdef CONFIG_MEMCG
extern void mm_update_next_owner(struct mm_struct *mm);
#else
static inline void mm_update_next_owner(struct mm_struct *mm)
{
}
#endif 

#ifdef CONFIG_MMU
#ifndef arch_get_mmap_end
#define arch_get_mmap_end(addr, len, flags)	(TASK_SIZE)
#endif

#ifndef arch_get_mmap_base
#define arch_get_mmap_base(addr, base) (base)
#endif

extern void arch_pick_mmap_layout(struct mm_struct *mm,
				  struct rlimit *rlim_stack);
extern unsigned long
arch_get_unmapped_area(struct file *, unsigned long, unsigned long,
		       unsigned long, unsigned long);
extern unsigned long
arch_get_unmapped_area_topdown(struct file *filp, unsigned long addr,
			  unsigned long len, unsigned long pgoff,
			  unsigned long flags);

unsigned long
generic_get_unmapped_area(struct file *filp, unsigned long addr,
			  unsigned long len, unsigned long pgoff,
			  unsigned long flags);
unsigned long
generic_get_unmapped_area_topdown(struct file *filp, unsigned long addr,
				  unsigned long len, unsigned long pgoff,
				  unsigned long flags);
#else
static inline void arch_pick_mmap_layout(struct mm_struct *mm,
					 struct rlimit *rlim_stack) {}
#endif

static inline bool in_vfork(struct task_struct *tsk)
{
	bool ret;

	
	rcu_read_lock();
	ret = tsk->vfork_done &&
			rcu_dereference(tsk->real_parent)->mm == tsk->mm;
	rcu_read_unlock();

	return ret;
}


static inline gfp_t current_gfp_context(gfp_t flags)
{
	unsigned int pflags = READ_ONCE(current->flags);

	if (unlikely(pflags & (PF_MEMALLOC_NOIO | PF_MEMALLOC_NOFS | PF_MEMALLOC_PIN))) {
		
		if (pflags & PF_MEMALLOC_NOIO)
			flags &= ~(__GFP_IO | __GFP_FS);
		else if (pflags & PF_MEMALLOC_NOFS)
			flags &= ~__GFP_FS;

		if (pflags & PF_MEMALLOC_PIN)
			flags &= ~__GFP_MOVABLE;
	}
	return flags;
}

#ifdef CONFIG_LOCKDEP
extern void __fs_reclaim_acquire(unsigned long ip);
extern void __fs_reclaim_release(unsigned long ip);
extern void fs_reclaim_acquire(gfp_t gfp_mask);
extern void fs_reclaim_release(gfp_t gfp_mask);
#else
static inline void __fs_reclaim_acquire(unsigned long ip) { }
static inline void __fs_reclaim_release(unsigned long ip) { }
static inline void fs_reclaim_acquire(gfp_t gfp_mask) { }
static inline void fs_reclaim_release(gfp_t gfp_mask) { }
#endif


static inline void memalloc_retry_wait(gfp_t gfp_flags)
{
	
	__set_current_state(TASK_UNINTERRUPTIBLE);
	gfp_flags = current_gfp_context(gfp_flags);
	if (gfpflags_allow_blocking(gfp_flags) &&
	    !(gfp_flags & __GFP_NORETRY))
		
		io_schedule_timeout(1);
	else
		
		io_schedule_timeout(HZ/50);
}


static inline void might_alloc(gfp_t gfp_mask)
{
	fs_reclaim_acquire(gfp_mask);
	fs_reclaim_release(gfp_mask);

	might_sleep_if(gfpflags_allow_blocking(gfp_mask));
}


static inline unsigned int memalloc_noio_save(void)
{
	unsigned int flags = current->flags & PF_MEMALLOC_NOIO;
	current->flags |= PF_MEMALLOC_NOIO;
	return flags;
}


static inline void memalloc_noio_restore(unsigned int flags)
{
	current->flags = (current->flags & ~PF_MEMALLOC_NOIO) | flags;
}


static inline unsigned int memalloc_nofs_save(void)
{
	unsigned int flags = current->flags & PF_MEMALLOC_NOFS;
	current->flags |= PF_MEMALLOC_NOFS;
	return flags;
}


static inline void memalloc_nofs_restore(unsigned int flags)
{
	current->flags = (current->flags & ~PF_MEMALLOC_NOFS) | flags;
}

static inline unsigned int memalloc_noreclaim_save(void)
{
	unsigned int flags = current->flags & PF_MEMALLOC;
	current->flags |= PF_MEMALLOC;
	return flags;
}

static inline void memalloc_noreclaim_restore(unsigned int flags)
{
	current->flags = (current->flags & ~PF_MEMALLOC) | flags;
}

static inline unsigned int memalloc_pin_save(void)
{
	unsigned int flags = current->flags & PF_MEMALLOC_PIN;

	current->flags |= PF_MEMALLOC_PIN;
	return flags;
}

static inline void memalloc_pin_restore(unsigned int flags)
{
	current->flags = (current->flags & ~PF_MEMALLOC_PIN) | flags;
}

#ifdef CONFIG_MEMCG
DECLARE_PER_CPU(struct mem_cgroup *, int_active_memcg);

static inline struct mem_cgroup *
set_active_memcg(struct mem_cgroup *memcg)
{
	struct mem_cgroup *old;

	if (!in_task()) {
		old = this_cpu_read(int_active_memcg);
		this_cpu_write(int_active_memcg, memcg);
	} else {
		old = current->active_memcg;
		current->active_memcg = memcg;
	}

	return old;
}
#else
static inline struct mem_cgroup *
set_active_memcg(struct mem_cgroup *memcg)
{
	return NULL;
}
#endif

#ifdef CONFIG_MEMBARRIER
enum {
	MEMBARRIER_STATE_PRIVATE_EXPEDITED_READY		= (1U << 0),
	MEMBARRIER_STATE_PRIVATE_EXPEDITED			= (1U << 1),
	MEMBARRIER_STATE_GLOBAL_EXPEDITED_READY			= (1U << 2),
	MEMBARRIER_STATE_GLOBAL_EXPEDITED			= (1U << 3),
	MEMBARRIER_STATE_PRIVATE_EXPEDITED_SYNC_CORE_READY	= (1U << 4),
	MEMBARRIER_STATE_PRIVATE_EXPEDITED_SYNC_CORE		= (1U << 5),
	MEMBARRIER_STATE_PRIVATE_EXPEDITED_RSEQ_READY		= (1U << 6),
	MEMBARRIER_STATE_PRIVATE_EXPEDITED_RSEQ			= (1U << 7),
};

enum {
	MEMBARRIER_FLAG_SYNC_CORE	= (1U << 0),
	MEMBARRIER_FLAG_RSEQ		= (1U << 1),
};

#ifdef CONFIG_ARCH_HAS_MEMBARRIER_CALLBACKS
#include <asm/membarrier.h>
#endif

static inline void membarrier_mm_sync_core_before_usermode(struct mm_struct *mm)
{
	if (current->mm != mm)
		return;
	if (likely(!(atomic_read(&mm->membarrier_state) &
		     MEMBARRIER_STATE_PRIVATE_EXPEDITED_SYNC_CORE)))
		return;
	sync_core_before_usermode();
}

extern void membarrier_exec_mmap(struct mm_struct *mm);

extern void membarrier_update_current_mm(struct mm_struct *next_mm);

#else
#ifdef CONFIG_ARCH_HAS_MEMBARRIER_CALLBACKS
static inline void membarrier_arch_switch_mm(struct mm_struct *prev,
					     struct mm_struct *next,
					     struct task_struct *tsk)
{
}
#endif
static inline void membarrier_exec_mmap(struct mm_struct *mm)
{
}
static inline void membarrier_mm_sync_core_before_usermode(struct mm_struct *mm)
{
}
static inline void membarrier_update_current_mm(struct mm_struct *next_mm)
{
}
#endif

#endif 
