/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MMU_NOTIFIER_H
#define _LINUX_MMU_NOTIFIER_H

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/mm_types.h>
#include <linux/mmap_lock.h>
#include <linux/srcu.h>
#include <linux/interval_tree.h>

struct mmu_notifier_subscriptions;
struct mmu_notifier;
struct mmu_notifier_range;
struct mmu_interval_notifier;


enum mmu_notifier_event {
	MMU_NOTIFY_UNMAP = 0,
	MMU_NOTIFY_CLEAR,
	MMU_NOTIFY_PROTECTION_VMA,
	MMU_NOTIFY_PROTECTION_PAGE,
	MMU_NOTIFY_SOFT_DIRTY,
	MMU_NOTIFY_RELEASE,
	MMU_NOTIFY_MIGRATE,
	MMU_NOTIFY_EXCLUSIVE,
};

#define MMU_NOTIFIER_RANGE_BLOCKABLE (1 << 0)

struct mmu_notifier_ops {
	
	void (*release)(struct mmu_notifier *subscription,
			struct mm_struct *mm);

	
	int (*clear_flush_young)(struct mmu_notifier *subscription,
				 struct mm_struct *mm,
				 unsigned long start,
				 unsigned long end);

	
	int (*clear_young)(struct mmu_notifier *subscription,
			   struct mm_struct *mm,
			   unsigned long start,
			   unsigned long end);

	
	int (*test_young)(struct mmu_notifier *subscription,
			  struct mm_struct *mm,
			  unsigned long address);

	
	void (*change_pte)(struct mmu_notifier *subscription,
			   struct mm_struct *mm,
			   unsigned long address,
			   pte_t pte);

	
	int (*invalidate_range_start)(struct mmu_notifier *subscription,
				      const struct mmu_notifier_range *range);
	void (*invalidate_range_end)(struct mmu_notifier *subscription,
				     const struct mmu_notifier_range *range);

	
	void (*arch_invalidate_secondary_tlbs)(
					struct mmu_notifier *subscription,
					struct mm_struct *mm,
					unsigned long start,
					unsigned long end);

	
	struct mmu_notifier *(*alloc_notifier)(struct mm_struct *mm);
	void (*free_notifier)(struct mmu_notifier *subscription);
};


struct mmu_notifier {
	struct hlist_node hlist;
	const struct mmu_notifier_ops *ops;
	struct mm_struct *mm;
	struct rcu_head rcu;
	unsigned int users;
};


struct mmu_interval_notifier_ops {
	bool (*invalidate)(struct mmu_interval_notifier *interval_sub,
			   const struct mmu_notifier_range *range,
			   unsigned long cur_seq);
};

struct mmu_interval_notifier {
	struct interval_tree_node interval_tree;
	const struct mmu_interval_notifier_ops *ops;
	struct mm_struct *mm;
	struct hlist_node deferred_item;
	unsigned long invalidate_seq;
};

#ifdef CONFIG_MMU_NOTIFIER

#ifdef CONFIG_LOCKDEP
extern struct lockdep_map __mmu_notifier_invalidate_range_start_map;
#endif

struct mmu_notifier_range {
	struct mm_struct *mm;
	unsigned long start;
	unsigned long end;
	unsigned flags;
	enum mmu_notifier_event event;
	void *owner;
};

static inline int mm_has_notifiers(struct mm_struct *mm)
{
	return unlikely(mm->notifier_subscriptions);
}

struct mmu_notifier *mmu_notifier_get_locked(const struct mmu_notifier_ops *ops,
					     struct mm_struct *mm);
static inline struct mmu_notifier *
mmu_notifier_get(const struct mmu_notifier_ops *ops, struct mm_struct *mm)
{
	struct mmu_notifier *ret;

	mmap_write_lock(mm);
	ret = mmu_notifier_get_locked(ops, mm);
	mmap_write_unlock(mm);
	return ret;
}
void mmu_notifier_put(struct mmu_notifier *subscription);
void mmu_notifier_synchronize(void);

extern int mmu_notifier_register(struct mmu_notifier *subscription,
				 struct mm_struct *mm);
extern int __mmu_notifier_register(struct mmu_notifier *subscription,
				   struct mm_struct *mm);
extern void mmu_notifier_unregister(struct mmu_notifier *subscription,
				    struct mm_struct *mm);

unsigned long
mmu_interval_read_begin(struct mmu_interval_notifier *interval_sub);
int mmu_interval_notifier_insert(struct mmu_interval_notifier *interval_sub,
				 struct mm_struct *mm, unsigned long start,
				 unsigned long length,
				 const struct mmu_interval_notifier_ops *ops);
int mmu_interval_notifier_insert_locked(
	struct mmu_interval_notifier *interval_sub, struct mm_struct *mm,
	unsigned long start, unsigned long length,
	const struct mmu_interval_notifier_ops *ops);
void mmu_interval_notifier_remove(struct mmu_interval_notifier *interval_sub);


static inline void
mmu_interval_set_seq(struct mmu_interval_notifier *interval_sub,
		     unsigned long cur_seq)
{
	WRITE_ONCE(interval_sub->invalidate_seq, cur_seq);
}


static inline bool
mmu_interval_read_retry(struct mmu_interval_notifier *interval_sub,
			unsigned long seq)
{
	return interval_sub->invalidate_seq != seq;
}


static inline bool
mmu_interval_check_retry(struct mmu_interval_notifier *interval_sub,
			 unsigned long seq)
{
	
	return READ_ONCE(interval_sub->invalidate_seq) != seq;
}

extern void __mmu_notifier_subscriptions_destroy(struct mm_struct *mm);
extern void __mmu_notifier_release(struct mm_struct *mm);
extern int __mmu_notifier_clear_flush_young(struct mm_struct *mm,
					  unsigned long start,
					  unsigned long end);
extern int __mmu_notifier_clear_young(struct mm_struct *mm,
				      unsigned long start,
				      unsigned long end);
extern int __mmu_notifier_test_young(struct mm_struct *mm,
				     unsigned long address);
extern void __mmu_notifier_change_pte(struct mm_struct *mm,
				      unsigned long address, pte_t pte);
extern int __mmu_notifier_invalidate_range_start(struct mmu_notifier_range *r);
extern void __mmu_notifier_invalidate_range_end(struct mmu_notifier_range *r);
extern void __mmu_notifier_arch_invalidate_secondary_tlbs(struct mm_struct *mm,
					unsigned long start, unsigned long end);
extern bool
mmu_notifier_range_update_to_read_only(const struct mmu_notifier_range *range);

static inline bool
mmu_notifier_range_blockable(const struct mmu_notifier_range *range)
{
	return (range->flags & MMU_NOTIFIER_RANGE_BLOCKABLE);
}

static inline void mmu_notifier_release(struct mm_struct *mm)
{
	if (mm_has_notifiers(mm))
		__mmu_notifier_release(mm);
}

static inline int mmu_notifier_clear_flush_young(struct mm_struct *mm,
					  unsigned long start,
					  unsigned long end)
{
	if (mm_has_notifiers(mm))
		return __mmu_notifier_clear_flush_young(mm, start, end);
	return 0;
}

static inline int mmu_notifier_clear_young(struct mm_struct *mm,
					   unsigned long start,
					   unsigned long end)
{
	if (mm_has_notifiers(mm))
		return __mmu_notifier_clear_young(mm, start, end);
	return 0;
}

static inline int mmu_notifier_test_young(struct mm_struct *mm,
					  unsigned long address)
{
	if (mm_has_notifiers(mm))
		return __mmu_notifier_test_young(mm, address);
	return 0;
}

static inline void mmu_notifier_change_pte(struct mm_struct *mm,
					   unsigned long address, pte_t pte)
{
	if (mm_has_notifiers(mm))
		__mmu_notifier_change_pte(mm, address, pte);
}

static inline void
mmu_notifier_invalidate_range_start(struct mmu_notifier_range *range)
{
	might_sleep();

	lock_map_acquire(&__mmu_notifier_invalidate_range_start_map);
	if (mm_has_notifiers(range->mm)) {
		range->flags |= MMU_NOTIFIER_RANGE_BLOCKABLE;
		__mmu_notifier_invalidate_range_start(range);
	}
	lock_map_release(&__mmu_notifier_invalidate_range_start_map);
}

static inline int
mmu_notifier_invalidate_range_start_nonblock(struct mmu_notifier_range *range)
{
	int ret = 0;

	lock_map_acquire(&__mmu_notifier_invalidate_range_start_map);
	if (mm_has_notifiers(range->mm)) {
		range->flags &= ~MMU_NOTIFIER_RANGE_BLOCKABLE;
		ret = __mmu_notifier_invalidate_range_start(range);
	}
	lock_map_release(&__mmu_notifier_invalidate_range_start_map);
	return ret;
}

static inline void
mmu_notifier_invalidate_range_end(struct mmu_notifier_range *range)
{
	if (mmu_notifier_range_blockable(range))
		might_sleep();

	if (mm_has_notifiers(range->mm))
		__mmu_notifier_invalidate_range_end(range);
}

static inline void mmu_notifier_arch_invalidate_secondary_tlbs(struct mm_struct *mm,
					unsigned long start, unsigned long end)
{
	if (mm_has_notifiers(mm))
		__mmu_notifier_arch_invalidate_secondary_tlbs(mm, start, end);
}

static inline void mmu_notifier_subscriptions_init(struct mm_struct *mm)
{
	mm->notifier_subscriptions = NULL;
}

static inline void mmu_notifier_subscriptions_destroy(struct mm_struct *mm)
{
	if (mm_has_notifiers(mm))
		__mmu_notifier_subscriptions_destroy(mm);
}


static inline void mmu_notifier_range_init(struct mmu_notifier_range *range,
					   enum mmu_notifier_event event,
					   unsigned flags,
					   struct mm_struct *mm,
					   unsigned long start,
					   unsigned long end)
{
	range->event = event;
	range->mm = mm;
	range->start = start;
	range->end = end;
	range->flags = flags;
}

static inline void mmu_notifier_range_init_owner(
			struct mmu_notifier_range *range,
			enum mmu_notifier_event event, unsigned int flags,
			struct mm_struct *mm, unsigned long start,
			unsigned long end, void *owner)
{
	mmu_notifier_range_init(range, event, flags, mm, start, end);
	range->owner = owner;
}

#define ptep_clear_flush_young_notify(__vma, __address, __ptep)		\
({									\
	int __young;							\
	struct vm_area_struct *___vma = __vma;				\
	unsigned long ___address = __address;				\
	__young = ptep_clear_flush_young(___vma, ___address, __ptep);	\
	__young |= mmu_notifier_clear_flush_young(___vma->vm_mm,	\
						  ___address,		\
						  ___address +		\
							PAGE_SIZE);	\
	__young;							\
})

#define pmdp_clear_flush_young_notify(__vma, __address, __pmdp)		\
({									\
	int __young;							\
	struct vm_area_struct *___vma = __vma;				\
	unsigned long ___address = __address;				\
	__young = pmdp_clear_flush_young(___vma, ___address, __pmdp);	\
	__young |= mmu_notifier_clear_flush_young(___vma->vm_mm,	\
						  ___address,		\
						  ___address +		\
							PMD_SIZE);	\
	__young;							\
})

#define ptep_clear_young_notify(__vma, __address, __ptep)		\
({									\
	int __young;							\
	struct vm_area_struct *___vma = __vma;				\
	unsigned long ___address = __address;				\
	__young = ptep_test_and_clear_young(___vma, ___address, __ptep);\
	__young |= mmu_notifier_clear_young(___vma->vm_mm, ___address,	\
					    ___address + PAGE_SIZE);	\
	__young;							\
})

#define pmdp_clear_young_notify(__vma, __address, __pmdp)		\
({									\
	int __young;							\
	struct vm_area_struct *___vma = __vma;				\
	unsigned long ___address = __address;				\
	__young = pmdp_test_and_clear_young(___vma, ___address, __pmdp);\
	__young |= mmu_notifier_clear_young(___vma->vm_mm, ___address,	\
					    ___address + PMD_SIZE);	\
	__young;							\
})


#define set_pte_at_notify(__mm, __address, __ptep, __pte)		\
({									\
	struct mm_struct *___mm = __mm;					\
	unsigned long ___address = __address;				\
	pte_t ___pte = __pte;						\
									\
	mmu_notifier_change_pte(___mm, ___address, ___pte);		\
	set_pte_at(___mm, ___address, __ptep, ___pte);			\
})

#else 

struct mmu_notifier_range {
	unsigned long start;
	unsigned long end;
};

static inline void _mmu_notifier_range_init(struct mmu_notifier_range *range,
					    unsigned long start,
					    unsigned long end)
{
	range->start = start;
	range->end = end;
}

#define mmu_notifier_range_init(range,event,flags,mm,start,end)  \
	_mmu_notifier_range_init(range, start, end)
#define mmu_notifier_range_init_owner(range, event, flags, mm, start, \
					end, owner) \
	_mmu_notifier_range_init(range, start, end)

static inline bool
mmu_notifier_range_blockable(const struct mmu_notifier_range *range)
{
	return true;
}

static inline int mm_has_notifiers(struct mm_struct *mm)
{
	return 0;
}

static inline void mmu_notifier_release(struct mm_struct *mm)
{
}

static inline int mmu_notifier_clear_flush_young(struct mm_struct *mm,
					  unsigned long start,
					  unsigned long end)
{
	return 0;
}

static inline int mmu_notifier_test_young(struct mm_struct *mm,
					  unsigned long address)
{
	return 0;
}

static inline void mmu_notifier_change_pte(struct mm_struct *mm,
					   unsigned long address, pte_t pte)
{
}

static inline void
mmu_notifier_invalidate_range_start(struct mmu_notifier_range *range)
{
}

static inline int
mmu_notifier_invalidate_range_start_nonblock(struct mmu_notifier_range *range)
{
	return 0;
}

static inline
void mmu_notifier_invalidate_range_end(struct mmu_notifier_range *range)
{
}

static inline void mmu_notifier_arch_invalidate_secondary_tlbs(struct mm_struct *mm,
				  unsigned long start, unsigned long end)
{
}

static inline void mmu_notifier_subscriptions_init(struct mm_struct *mm)
{
}

static inline void mmu_notifier_subscriptions_destroy(struct mm_struct *mm)
{
}

#define mmu_notifier_range_update_to_read_only(r) false

#define ptep_clear_flush_young_notify ptep_clear_flush_young
#define pmdp_clear_flush_young_notify pmdp_clear_flush_young
#define ptep_clear_young_notify ptep_test_and_clear_young
#define pmdp_clear_young_notify pmdp_test_and_clear_young
#define	ptep_clear_flush_notify ptep_clear_flush
#define pmdp_huge_clear_flush_notify pmdp_huge_clear_flush
#define pudp_huge_clear_flush_notify pudp_huge_clear_flush
#define set_pte_at_notify set_pte_at

static inline void mmu_notifier_synchronize(void)
{
}

#endif 

#endif 
