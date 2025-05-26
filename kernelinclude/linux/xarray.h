/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef _LINUX_XARRAY_H
#define _LINUX_XARRAY_H


#include <linux/bitmap.h>
#include <linux/bug.h>
#include <linux/compiler.h>
#include <linux/gfp.h>
#include <linux/kconfig.h>
#include <linux/kernel.h>
#include <linux/rcupdate.h>
#include <linux/sched/mm.h>
#include <linux/spinlock.h>
#include <linux/types.h>



#define BITS_PER_XA_VALUE	(BITS_PER_LONG - 1)


static inline void *xa_mk_value(unsigned long v)
{
	WARN_ON((long)v < 0);
	return (void *)((v << 1) | 1);
}


static inline unsigned long xa_to_value(const void *entry)
{
	return (unsigned long)entry >> 1;
}


static inline bool xa_is_value(const void *entry)
{
	return (unsigned long)entry & 1;
}


static inline void *xa_tag_pointer(void *p, unsigned long tag)
{
	return (void *)((unsigned long)p | tag);
}


static inline void *xa_untag_pointer(void *entry)
{
	return (void *)((unsigned long)entry & ~3UL);
}


static inline unsigned int xa_pointer_tag(void *entry)
{
	return (unsigned long)entry & 3UL;
}


static inline void *xa_mk_internal(unsigned long v)
{
	return (void *)((v << 2) | 2);
}


static inline unsigned long xa_to_internal(const void *entry)
{
	return (unsigned long)entry >> 2;
}


static inline bool xa_is_internal(const void *entry)
{
	return ((unsigned long)entry & 3) == 2;
}

#define XA_ZERO_ENTRY		xa_mk_internal(257)


static inline bool xa_is_zero(const void *entry)
{
	return unlikely(entry == XA_ZERO_ENTRY);
}


static inline bool xa_is_err(const void *entry)
{
	return unlikely(xa_is_internal(entry) &&
			entry >= xa_mk_internal(-MAX_ERRNO));
}


static inline int xa_err(void *entry)
{
	
	if (xa_is_err(entry))
		return (long)entry >> 2;
	return 0;
}


struct xa_limit {
	u32 max;
	u32 min;
};

#define XA_LIMIT(_min, _max) (struct xa_limit) { .min = _min, .max = _max }

#define xa_limit_32b	XA_LIMIT(0, UINT_MAX)
#define xa_limit_31b	XA_LIMIT(0, INT_MAX)
#define xa_limit_16b	XA_LIMIT(0, USHRT_MAX)

typedef unsigned __bitwise xa_mark_t;
#define XA_MARK_0		((__force xa_mark_t)0U)
#define XA_MARK_1		((__force xa_mark_t)1U)
#define XA_MARK_2		((__force xa_mark_t)2U)
#define XA_PRESENT		((__force xa_mark_t)8U)
#define XA_MARK_MAX		XA_MARK_2
#define XA_FREE_MARK		XA_MARK_0

enum xa_lock_type {
	XA_LOCK_IRQ = 1,
	XA_LOCK_BH = 2,
};


#define XA_FLAGS_LOCK_IRQ	((__force gfp_t)XA_LOCK_IRQ)
#define XA_FLAGS_LOCK_BH	((__force gfp_t)XA_LOCK_BH)
#define XA_FLAGS_TRACK_FREE	((__force gfp_t)4U)
#define XA_FLAGS_ZERO_BUSY	((__force gfp_t)8U)
#define XA_FLAGS_ALLOC_WRAPPED	((__force gfp_t)16U)
#define XA_FLAGS_ACCOUNT	((__force gfp_t)32U)
#define XA_FLAGS_MARK(mark)	((__force gfp_t)((1U << __GFP_BITS_SHIFT) << \
						(__force unsigned)(mark)))


#define XA_FLAGS_ALLOC	(XA_FLAGS_TRACK_FREE | XA_FLAGS_MARK(XA_FREE_MARK))
#define XA_FLAGS_ALLOC1	(XA_FLAGS_TRACK_FREE | XA_FLAGS_ZERO_BUSY)



struct xarray {
	spinlock_t	xa_lock;

	gfp_t		xa_flags;
	void __rcu *	xa_head;
};

#define XARRAY_INIT(name, flags) {				\
	.xa_lock = __SPIN_LOCK_UNLOCKED(name.xa_lock),		\
	.xa_flags = flags,					\
	.xa_head = NULL,					\
}


#define DEFINE_XARRAY_FLAGS(name, flags)				\
	struct xarray name = XARRAY_INIT(name, flags)


#define DEFINE_XARRAY(name) DEFINE_XARRAY_FLAGS(name, 0)


#define DEFINE_XARRAY_ALLOC(name) DEFINE_XARRAY_FLAGS(name, XA_FLAGS_ALLOC)


#define DEFINE_XARRAY_ALLOC1(name) DEFINE_XARRAY_FLAGS(name, XA_FLAGS_ALLOC1)

void *xa_load(struct xarray *, unsigned long index);
void *xa_store(struct xarray *, unsigned long index, void *entry, gfp_t);
void *xa_erase(struct xarray *, unsigned long index);
void *xa_store_range(struct xarray *, unsigned long first, unsigned long last,
			void *entry, gfp_t);
bool xa_get_mark(struct xarray *, unsigned long index, xa_mark_t);
void xa_set_mark(struct xarray *, unsigned long index, xa_mark_t);
void xa_clear_mark(struct xarray *, unsigned long index, xa_mark_t);
void *xa_find(struct xarray *xa, unsigned long *index,
		unsigned long max, xa_mark_t) __attribute__((nonnull(2)));
void *xa_find_after(struct xarray *xa, unsigned long *index,
		unsigned long max, xa_mark_t) __attribute__((nonnull(2)));
unsigned int xa_extract(struct xarray *, void **dst, unsigned long start,
		unsigned long max, unsigned int n, xa_mark_t);
void xa_destroy(struct xarray *);


static inline void xa_init_flags(struct xarray *xa, gfp_t flags)
{
	spin_lock_init(&xa->xa_lock);
	xa->xa_flags = flags;
	xa->xa_head = NULL;
}


static inline void xa_init(struct xarray *xa)
{
	xa_init_flags(xa, 0);
}


static inline bool xa_empty(const struct xarray *xa)
{
	return xa->xa_head == NULL;
}


static inline bool xa_marked(const struct xarray *xa, xa_mark_t mark)
{
	return xa->xa_flags & XA_FLAGS_MARK(mark);
}


#define xa_for_each_range(xa, index, entry, start, last)		\
	for (index = start,						\
	     entry = xa_find(xa, &index, last, XA_PRESENT);		\
	     entry;							\
	     entry = xa_find_after(xa, &index, last, XA_PRESENT))


#define xa_for_each_start(xa, index, entry, start) \
	xa_for_each_range(xa, index, entry, start, ULONG_MAX)


#define xa_for_each(xa, index, entry) \
	xa_for_each_start(xa, index, entry, 0)


#define xa_for_each_marked(xa, index, entry, filter) \
	for (index = 0, entry = xa_find(xa, &index, ULONG_MAX, filter); \
	     entry; entry = xa_find_after(xa, &index, ULONG_MAX, filter))

#define xa_trylock(xa)		spin_trylock(&(xa)->xa_lock)
#define xa_lock(xa)		spin_lock(&(xa)->xa_lock)
#define xa_unlock(xa)		spin_unlock(&(xa)->xa_lock)
#define xa_lock_bh(xa)		spin_lock_bh(&(xa)->xa_lock)
#define xa_unlock_bh(xa)	spin_unlock_bh(&(xa)->xa_lock)
#define xa_lock_irq(xa)		spin_lock_irq(&(xa)->xa_lock)
#define xa_unlock_irq(xa)	spin_unlock_irq(&(xa)->xa_lock)
#define xa_lock_irqsave(xa, flags) \
				spin_lock_irqsave(&(xa)->xa_lock, flags)
#define xa_unlock_irqrestore(xa, flags) \
				spin_unlock_irqrestore(&(xa)->xa_lock, flags)
#define xa_lock_nested(xa, subclass) \
				spin_lock_nested(&(xa)->xa_lock, subclass)
#define xa_lock_bh_nested(xa, subclass) \
				spin_lock_bh_nested(&(xa)->xa_lock, subclass)
#define xa_lock_irq_nested(xa, subclass) \
				spin_lock_irq_nested(&(xa)->xa_lock, subclass)
#define xa_lock_irqsave_nested(xa, flags, subclass) \
		spin_lock_irqsave_nested(&(xa)->xa_lock, flags, subclass)


void *__xa_erase(struct xarray *, unsigned long index);
void *__xa_store(struct xarray *, unsigned long index, void *entry, gfp_t);
void *__xa_cmpxchg(struct xarray *, unsigned long index, void *old,
		void *entry, gfp_t);
int __must_check __xa_insert(struct xarray *, unsigned long index,
		void *entry, gfp_t);
int __must_check __xa_alloc(struct xarray *, u32 *id, void *entry,
		struct xa_limit, gfp_t);
int __must_check __xa_alloc_cyclic(struct xarray *, u32 *id, void *entry,
		struct xa_limit, u32 *next, gfp_t);
void __xa_set_mark(struct xarray *, unsigned long index, xa_mark_t);
void __xa_clear_mark(struct xarray *, unsigned long index, xa_mark_t);


static inline void *xa_store_bh(struct xarray *xa, unsigned long index,
		void *entry, gfp_t gfp)
{
	void *curr;

	might_alloc(gfp);
	xa_lock_bh(xa);
	curr = __xa_store(xa, index, entry, gfp);
	xa_unlock_bh(xa);

	return curr;
}


static inline void *xa_store_irq(struct xarray *xa, unsigned long index,
		void *entry, gfp_t gfp)
{
	void *curr;

	might_alloc(gfp);
	xa_lock_irq(xa);
	curr = __xa_store(xa, index, entry, gfp);
	xa_unlock_irq(xa);

	return curr;
}


static inline void *xa_erase_bh(struct xarray *xa, unsigned long index)
{
	void *entry;

	xa_lock_bh(xa);
	entry = __xa_erase(xa, index);
	xa_unlock_bh(xa);

	return entry;
}


static inline void *xa_erase_irq(struct xarray *xa, unsigned long index)
{
	void *entry;

	xa_lock_irq(xa);
	entry = __xa_erase(xa, index);
	xa_unlock_irq(xa);

	return entry;
}


static inline void *xa_cmpxchg(struct xarray *xa, unsigned long index,
			void *old, void *entry, gfp_t gfp)
{
	void *curr;

	might_alloc(gfp);
	xa_lock(xa);
	curr = __xa_cmpxchg(xa, index, old, entry, gfp);
	xa_unlock(xa);

	return curr;
}


static inline void *xa_cmpxchg_bh(struct xarray *xa, unsigned long index,
			void *old, void *entry, gfp_t gfp)
{
	void *curr;

	might_alloc(gfp);
	xa_lock_bh(xa);
	curr = __xa_cmpxchg(xa, index, old, entry, gfp);
	xa_unlock_bh(xa);

	return curr;
}


static inline void *xa_cmpxchg_irq(struct xarray *xa, unsigned long index,
			void *old, void *entry, gfp_t gfp)
{
	void *curr;

	might_alloc(gfp);
	xa_lock_irq(xa);
	curr = __xa_cmpxchg(xa, index, old, entry, gfp);
	xa_unlock_irq(xa);

	return curr;
}


static inline int __must_check xa_insert(struct xarray *xa,
		unsigned long index, void *entry, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock(xa);
	err = __xa_insert(xa, index, entry, gfp);
	xa_unlock(xa);

	return err;
}


static inline int __must_check xa_insert_bh(struct xarray *xa,
		unsigned long index, void *entry, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock_bh(xa);
	err = __xa_insert(xa, index, entry, gfp);
	xa_unlock_bh(xa);

	return err;
}


static inline int __must_check xa_insert_irq(struct xarray *xa,
		unsigned long index, void *entry, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock_irq(xa);
	err = __xa_insert(xa, index, entry, gfp);
	xa_unlock_irq(xa);

	return err;
}


static inline __must_check int xa_alloc(struct xarray *xa, u32 *id,
		void *entry, struct xa_limit limit, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock(xa);
	err = __xa_alloc(xa, id, entry, limit, gfp);
	xa_unlock(xa);

	return err;
}


static inline int __must_check xa_alloc_bh(struct xarray *xa, u32 *id,
		void *entry, struct xa_limit limit, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock_bh(xa);
	err = __xa_alloc(xa, id, entry, limit, gfp);
	xa_unlock_bh(xa);

	return err;
}


static inline int __must_check xa_alloc_irq(struct xarray *xa, u32 *id,
		void *entry, struct xa_limit limit, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock_irq(xa);
	err = __xa_alloc(xa, id, entry, limit, gfp);
	xa_unlock_irq(xa);

	return err;
}


static inline int xa_alloc_cyclic(struct xarray *xa, u32 *id, void *entry,
		struct xa_limit limit, u32 *next, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock(xa);
	err = __xa_alloc_cyclic(xa, id, entry, limit, next, gfp);
	xa_unlock(xa);

	return err;
}


static inline int xa_alloc_cyclic_bh(struct xarray *xa, u32 *id, void *entry,
		struct xa_limit limit, u32 *next, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock_bh(xa);
	err = __xa_alloc_cyclic(xa, id, entry, limit, next, gfp);
	xa_unlock_bh(xa);

	return err;
}


static inline int xa_alloc_cyclic_irq(struct xarray *xa, u32 *id, void *entry,
		struct xa_limit limit, u32 *next, gfp_t gfp)
{
	int err;

	might_alloc(gfp);
	xa_lock_irq(xa);
	err = __xa_alloc_cyclic(xa, id, entry, limit, next, gfp);
	xa_unlock_irq(xa);

	return err;
}


static inline __must_check
int xa_reserve(struct xarray *xa, unsigned long index, gfp_t gfp)
{
	return xa_err(xa_cmpxchg(xa, index, NULL, XA_ZERO_ENTRY, gfp));
}


static inline __must_check
int xa_reserve_bh(struct xarray *xa, unsigned long index, gfp_t gfp)
{
	return xa_err(xa_cmpxchg_bh(xa, index, NULL, XA_ZERO_ENTRY, gfp));
}


static inline __must_check
int xa_reserve_irq(struct xarray *xa, unsigned long index, gfp_t gfp)
{
	return xa_err(xa_cmpxchg_irq(xa, index, NULL, XA_ZERO_ENTRY, gfp));
}


static inline void xa_release(struct xarray *xa, unsigned long index)
{
	xa_cmpxchg(xa, index, XA_ZERO_ENTRY, NULL, 0);
}




#ifndef XA_CHUNK_SHIFT
#define XA_CHUNK_SHIFT		(CONFIG_BASE_SMALL ? 4 : 6)
#endif
#define XA_CHUNK_SIZE		(1UL << XA_CHUNK_SHIFT)
#define XA_CHUNK_MASK		(XA_CHUNK_SIZE - 1)
#define XA_MAX_MARKS		3
#define XA_MARK_LONGS		DIV_ROUND_UP(XA_CHUNK_SIZE, BITS_PER_LONG)


struct xa_node {
	unsigned char	shift;		
	unsigned char	offset;		
	unsigned char	count;		
	unsigned char	nr_values;	
	struct xa_node __rcu *parent;	
	struct xarray	*array;		
	union {
		struct list_head private_list;	
		struct rcu_head	rcu_head;	
	};
	void __rcu	*slots[XA_CHUNK_SIZE];
	union {
		unsigned long	tags[XA_MAX_MARKS][XA_MARK_LONGS];
		unsigned long	marks[XA_MAX_MARKS][XA_MARK_LONGS];
	};
};

void xa_dump(const struct xarray *);
void xa_dump_node(const struct xa_node *);

#ifdef XA_DEBUG
#define XA_BUG_ON(xa, x) do {					\
		if (x) {					\
			xa_dump(xa);				\
			BUG();					\
		}						\
	} while (0)
#define XA_NODE_BUG_ON(node, x) do {				\
		if (x) {					\
			if (node) xa_dump_node(node);		\
			BUG();					\
		}						\
	} while (0)
#else
#define XA_BUG_ON(xa, x)	do { } while (0)
#define XA_NODE_BUG_ON(node, x)	do { } while (0)
#endif


static inline void *xa_head(const struct xarray *xa)
{
	return rcu_dereference_check(xa->xa_head,
						lockdep_is_held(&xa->xa_lock));
}


static inline void *xa_head_locked(const struct xarray *xa)
{
	return rcu_dereference_protected(xa->xa_head,
						lockdep_is_held(&xa->xa_lock));
}


static inline void *xa_entry(const struct xarray *xa,
				const struct xa_node *node, unsigned int offset)
{
	XA_NODE_BUG_ON(node, offset >= XA_CHUNK_SIZE);
	return rcu_dereference_check(node->slots[offset],
						lockdep_is_held(&xa->xa_lock));
}


static inline void *xa_entry_locked(const struct xarray *xa,
				const struct xa_node *node, unsigned int offset)
{
	XA_NODE_BUG_ON(node, offset >= XA_CHUNK_SIZE);
	return rcu_dereference_protected(node->slots[offset],
						lockdep_is_held(&xa->xa_lock));
}


static inline struct xa_node *xa_parent(const struct xarray *xa,
					const struct xa_node *node)
{
	return rcu_dereference_check(node->parent,
						lockdep_is_held(&xa->xa_lock));
}


static inline struct xa_node *xa_parent_locked(const struct xarray *xa,
					const struct xa_node *node)
{
	return rcu_dereference_protected(node->parent,
						lockdep_is_held(&xa->xa_lock));
}


static inline void *xa_mk_node(const struct xa_node *node)
{
	return (void *)((unsigned long)node | 2);
}


static inline struct xa_node *xa_to_node(const void *entry)
{
	return (struct xa_node *)((unsigned long)entry - 2);
}


static inline bool xa_is_node(const void *entry)
{
	return xa_is_internal(entry) && (unsigned long)entry > 4096;
}


static inline void *xa_mk_sibling(unsigned int offset)
{
	return xa_mk_internal(offset);
}


static inline unsigned long xa_to_sibling(const void *entry)
{
	return xa_to_internal(entry);
}


static inline bool xa_is_sibling(const void *entry)
{
	return IS_ENABLED(CONFIG_XARRAY_MULTI) && xa_is_internal(entry) &&
		(entry < xa_mk_sibling(XA_CHUNK_SIZE - 1));
}

#define XA_RETRY_ENTRY		xa_mk_internal(256)


static inline bool xa_is_retry(const void *entry)
{
	return unlikely(entry == XA_RETRY_ENTRY);
}


static inline bool xa_is_advanced(const void *entry)
{
	return xa_is_internal(entry) && (entry <= XA_RETRY_ENTRY);
}


typedef void (*xa_update_node_t)(struct xa_node *node);

void xa_delete_node(struct xa_node *, xa_update_node_t);


struct xa_state {
	struct xarray *xa;
	unsigned long xa_index;
	unsigned char xa_shift;
	unsigned char xa_sibs;
	unsigned char xa_offset;
	unsigned char xa_pad;		
	struct xa_node *xa_node;
	struct xa_node *xa_alloc;
	xa_update_node_t xa_update;
	struct list_lru *xa_lru;
};


#define XA_ERROR(errno) ((struct xa_node *)(((unsigned long)errno << 2) | 2UL))
#define XAS_BOUNDS	((struct xa_node *)1UL)
#define XAS_RESTART	((struct xa_node *)3UL)

#define __XA_STATE(array, index, shift, sibs)  {	\
	.xa = array,					\
	.xa_index = index,				\
	.xa_shift = shift,				\
	.xa_sibs = sibs,				\
	.xa_offset = 0,					\
	.xa_pad = 0,					\
	.xa_node = XAS_RESTART,				\
	.xa_alloc = NULL,				\
	.xa_update = NULL,				\
	.xa_lru = NULL,					\
}


#define XA_STATE(name, array, index)				\
	struct xa_state name = __XA_STATE(array, index, 0, 0)


#define XA_STATE_ORDER(name, array, index, order)		\
	struct xa_state name = __XA_STATE(array,		\
			(index >> order) << order,		\
			order - (order % XA_CHUNK_SHIFT),	\
			(1U << (order % XA_CHUNK_SHIFT)) - 1)

#define xas_marked(xas, mark)	xa_marked((xas)->xa, (mark))
#define xas_trylock(xas)	xa_trylock((xas)->xa)
#define xas_lock(xas)		xa_lock((xas)->xa)
#define xas_unlock(xas)		xa_unlock((xas)->xa)
#define xas_lock_bh(xas)	xa_lock_bh((xas)->xa)
#define xas_unlock_bh(xas)	xa_unlock_bh((xas)->xa)
#define xas_lock_irq(xas)	xa_lock_irq((xas)->xa)
#define xas_unlock_irq(xas)	xa_unlock_irq((xas)->xa)
#define xas_lock_irqsave(xas, flags) \
				xa_lock_irqsave((xas)->xa, flags)
#define xas_unlock_irqrestore(xas, flags) \
				xa_unlock_irqrestore((xas)->xa, flags)


static inline int xas_error(const struct xa_state *xas)
{
	return xa_err(xas->xa_node);
}


static inline void xas_set_err(struct xa_state *xas, long err)
{
	xas->xa_node = XA_ERROR(err);
}


static inline bool xas_invalid(const struct xa_state *xas)
{
	return (unsigned long)xas->xa_node & 3;
}


static inline bool xas_valid(const struct xa_state *xas)
{
	return !xas_invalid(xas);
}


static inline bool xas_is_node(const struct xa_state *xas)
{
	return xas_valid(xas) && xas->xa_node;
}


static inline bool xas_not_node(struct xa_node *node)
{
	return ((unsigned long)node & 3) || !node;
}


static inline bool xas_frozen(struct xa_node *node)
{
	return (unsigned long)node & 2;
}


static inline bool xas_top(struct xa_node *node)
{
	return node <= XAS_RESTART;
}


static inline void xas_reset(struct xa_state *xas)
{
	xas->xa_node = XAS_RESTART;
}


static inline bool xas_retry(struct xa_state *xas, const void *entry)
{
	if (xa_is_zero(entry))
		return true;
	if (!xa_is_retry(entry))
		return false;
	xas_reset(xas);
	return true;
}

void *xas_load(struct xa_state *);
void *xas_store(struct xa_state *, void *entry);
void *xas_find(struct xa_state *, unsigned long max);
void *xas_find_conflict(struct xa_state *);

bool xas_get_mark(const struct xa_state *, xa_mark_t);
void xas_set_mark(const struct xa_state *, xa_mark_t);
void xas_clear_mark(const struct xa_state *, xa_mark_t);
void *xas_find_marked(struct xa_state *, unsigned long max, xa_mark_t);
void xas_init_marks(const struct xa_state *);

bool xas_nomem(struct xa_state *, gfp_t);
void xas_destroy(struct xa_state *);
void xas_pause(struct xa_state *);

void xas_create_range(struct xa_state *);

#ifdef CONFIG_XARRAY_MULTI
int xa_get_order(struct xarray *, unsigned long index);
void xas_split(struct xa_state *, void *entry, unsigned int order);
void xas_split_alloc(struct xa_state *, void *entry, unsigned int order, gfp_t);
#else
static inline int xa_get_order(struct xarray *xa, unsigned long index)
{
	return 0;
}

static inline void xas_split(struct xa_state *xas, void *entry,
		unsigned int order)
{
	xas_store(xas, entry);
}

static inline void xas_split_alloc(struct xa_state *xas, void *entry,
		unsigned int order, gfp_t gfp)
{
}
#endif


static inline void *xas_reload(struct xa_state *xas)
{
	struct xa_node *node = xas->xa_node;
	void *entry;
	char offset;

	if (!node)
		return xa_head(xas->xa);
	if (IS_ENABLED(CONFIG_XARRAY_MULTI)) {
		offset = (xas->xa_index >> node->shift) & XA_CHUNK_MASK;
		entry = xa_entry(xas->xa, node, offset);
		if (!xa_is_sibling(entry))
			return entry;
		offset = xa_to_sibling(entry);
	} else {
		offset = xas->xa_offset;
	}
	return xa_entry(xas->xa, node, offset);
}


static inline void xas_set(struct xa_state *xas, unsigned long index)
{
	xas->xa_index = index;
	xas->xa_node = XAS_RESTART;
}


static inline void xas_advance(struct xa_state *xas, unsigned long index)
{
	unsigned char shift = xas_is_node(xas) ? xas->xa_node->shift : 0;

	xas->xa_index = index;
	xas->xa_offset = (index >> shift) & XA_CHUNK_MASK;
}


static inline void xas_set_order(struct xa_state *xas, unsigned long index,
					unsigned int order)
{
#ifdef CONFIG_XARRAY_MULTI
	xas->xa_index = order < BITS_PER_LONG ? (index >> order) << order : 0;
	xas->xa_shift = order - (order % XA_CHUNK_SHIFT);
	xas->xa_sibs = (1 << (order % XA_CHUNK_SHIFT)) - 1;
	xas->xa_node = XAS_RESTART;
#else
	BUG_ON(order > 0);
	xas_set(xas, index);
#endif
}


static inline void xas_set_update(struct xa_state *xas, xa_update_node_t update)
{
	xas->xa_update = update;
}

static inline void xas_set_lru(struct xa_state *xas, struct list_lru *lru)
{
	xas->xa_lru = lru;
}


static inline void *xas_next_entry(struct xa_state *xas, unsigned long max)
{
	struct xa_node *node = xas->xa_node;
	void *entry;

	if (unlikely(xas_not_node(node) || node->shift ||
			xas->xa_offset != (xas->xa_index & XA_CHUNK_MASK)))
		return xas_find(xas, max);

	do {
		if (unlikely(xas->xa_index >= max))
			return xas_find(xas, max);
		if (unlikely(xas->xa_offset == XA_CHUNK_MASK))
			return xas_find(xas, max);
		entry = xa_entry(xas->xa, node, xas->xa_offset + 1);
		if (unlikely(xa_is_internal(entry)))
			return xas_find(xas, max);
		xas->xa_offset++;
		xas->xa_index++;
	} while (!entry);

	return entry;
}


static inline unsigned int xas_find_chunk(struct xa_state *xas, bool advance,
		xa_mark_t mark)
{
	unsigned long *addr = xas->xa_node->marks[(__force unsigned)mark];
	unsigned int offset = xas->xa_offset;

	if (advance)
		offset++;
	if (XA_CHUNK_SIZE == BITS_PER_LONG) {
		if (offset < XA_CHUNK_SIZE) {
			unsigned long data = *addr & (~0UL << offset);
			if (data)
				return __ffs(data);
		}
		return XA_CHUNK_SIZE;
	}

	return find_next_bit(addr, XA_CHUNK_SIZE, offset);
}


static inline void *xas_next_marked(struct xa_state *xas, unsigned long max,
								xa_mark_t mark)
{
	struct xa_node *node = xas->xa_node;
	void *entry;
	unsigned int offset;

	if (unlikely(xas_not_node(node) || node->shift))
		return xas_find_marked(xas, max, mark);
	offset = xas_find_chunk(xas, true, mark);
	xas->xa_offset = offset;
	xas->xa_index = (xas->xa_index & ~XA_CHUNK_MASK) + offset;
	if (xas->xa_index > max)
		return NULL;
	if (offset == XA_CHUNK_SIZE)
		return xas_find_marked(xas, max, mark);
	entry = xa_entry(xas->xa, node, offset);
	if (!entry)
		return xas_find_marked(xas, max, mark);
	return entry;
}


enum {
	XA_CHECK_SCHED = 4096,
};


#define xas_for_each(xas, entry, max) \
	for (entry = xas_find(xas, max); entry; \
	     entry = xas_next_entry(xas, max))


#define xas_for_each_marked(xas, entry, max, mark) \
	for (entry = xas_find_marked(xas, max, mark); entry; \
	     entry = xas_next_marked(xas, max, mark))


#define xas_for_each_conflict(xas, entry) \
	while ((entry = xas_find_conflict(xas)))

void *__xas_next(struct xa_state *);
void *__xas_prev(struct xa_state *);


static inline void *xas_prev(struct xa_state *xas)
{
	struct xa_node *node = xas->xa_node;

	if (unlikely(xas_not_node(node) || node->shift ||
				xas->xa_offset == 0))
		return __xas_prev(xas);

	xas->xa_index--;
	xas->xa_offset--;
	return xa_entry(xas->xa, node, xas->xa_offset);
}


static inline void *xas_next(struct xa_state *xas)
{
	struct xa_node *node = xas->xa_node;

	if (unlikely(xas_not_node(node) || node->shift ||
				xas->xa_offset == XA_CHUNK_MASK))
		return __xas_next(xas);

	xas->xa_index++;
	xas->xa_offset++;
	return xa_entry(xas->xa, node, xas->xa_offset);
}

#endif 
