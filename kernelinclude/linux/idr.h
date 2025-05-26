/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __IDR_H__
#define __IDR_H__

#include <linux/radix-tree.h>
#include <linux/gfp.h>
#include <linux/percpu.h>

struct idr {
	struct radix_tree_root	idr_rt;
	unsigned int		idr_base;
	unsigned int		idr_next;
};


#define IDR_FREE	0


#define IDR_RT_MARKER	(ROOT_IS_IDR | (__force gfp_t)			\
					(1 << (ROOT_TAG_SHIFT + IDR_FREE)))

#define IDR_INIT_BASE(name, base) {					\
	.idr_rt = RADIX_TREE_INIT(name, IDR_RT_MARKER),			\
	.idr_base = (base),						\
	.idr_next = 0,							\
}


#define IDR_INIT(name)	IDR_INIT_BASE(name, 0)


#define DEFINE_IDR(name)	struct idr name = IDR_INIT(name)


static inline unsigned int idr_get_cursor(const struct idr *idr)
{
	return READ_ONCE(idr->idr_next);
}


static inline void idr_set_cursor(struct idr *idr, unsigned int val)
{
	WRITE_ONCE(idr->idr_next, val);
}



#define idr_lock(idr)		xa_lock(&(idr)->idr_rt)
#define idr_unlock(idr)		xa_unlock(&(idr)->idr_rt)
#define idr_lock_bh(idr)	xa_lock_bh(&(idr)->idr_rt)
#define idr_unlock_bh(idr)	xa_unlock_bh(&(idr)->idr_rt)
#define idr_lock_irq(idr)	xa_lock_irq(&(idr)->idr_rt)
#define idr_unlock_irq(idr)	xa_unlock_irq(&(idr)->idr_rt)
#define idr_lock_irqsave(idr, flags) \
				xa_lock_irqsave(&(idr)->idr_rt, flags)
#define idr_unlock_irqrestore(idr, flags) \
				xa_unlock_irqrestore(&(idr)->idr_rt, flags)

void idr_preload(gfp_t gfp_mask);

int idr_alloc(struct idr *, void *ptr, int start, int end, gfp_t);
int __must_check idr_alloc_u32(struct idr *, void *ptr, u32 *id,
				unsigned long max, gfp_t);
int idr_alloc_cyclic(struct idr *, void *ptr, int start, int end, gfp_t);
void *idr_remove(struct idr *, unsigned long id);
void *idr_find(const struct idr *, unsigned long id);
int idr_for_each(const struct idr *,
		 int (*fn)(int id, void *p, void *data), void *data);
void *idr_get_next(struct idr *, int *nextid);
void *idr_get_next_ul(struct idr *, unsigned long *nextid);
void *idr_replace(struct idr *, void *, unsigned long id);
void idr_destroy(struct idr *);


static inline void idr_init_base(struct idr *idr, int base)
{
	INIT_RADIX_TREE(&idr->idr_rt, IDR_RT_MARKER);
	idr->idr_base = base;
	idr->idr_next = 0;
}


static inline void idr_init(struct idr *idr)
{
	idr_init_base(idr, 0);
}


static inline bool idr_is_empty(const struct idr *idr)
{
	return radix_tree_empty(&idr->idr_rt) &&
		radix_tree_tagged(&idr->idr_rt, IDR_FREE);
}


static inline void idr_preload_end(void)
{
	local_unlock(&radix_tree_preloads.lock);
}


#define idr_for_each_entry(idr, entry, id)			\
	for (id = 0; ((entry) = idr_get_next(idr, &(id))) != NULL; id += 1U)


#define idr_for_each_entry_ul(idr, entry, tmp, id)			\
	for (tmp = 0, id = 0;						\
	     ((entry) = tmp <= id ? idr_get_next_ul(idr, &(id)) : NULL) != NULL; \
	     tmp = id, ++id)


#define idr_for_each_entry_continue(idr, entry, id)			\
	for ((entry) = idr_get_next((idr), &(id));			\
	     entry;							\
	     ++id, (entry) = idr_get_next((idr), &(id)))


#define idr_for_each_entry_continue_ul(idr, entry, tmp, id)		\
	for (tmp = id;							\
	     ((entry) = tmp <= id ? idr_get_next_ul(idr, &(id)) : NULL) != NULL; \
	     tmp = id, ++id)


#define IDA_CHUNK_SIZE		128	
#define IDA_BITMAP_LONGS	(IDA_CHUNK_SIZE / sizeof(long))
#define IDA_BITMAP_BITS 	(IDA_BITMAP_LONGS * sizeof(long) * 8)

struct ida_bitmap {
	unsigned long		bitmap[IDA_BITMAP_LONGS];
};

struct ida {
	struct xarray xa;
};

#define IDA_INIT_FLAGS	(XA_FLAGS_LOCK_IRQ | XA_FLAGS_ALLOC)

#define IDA_INIT(name)	{						\
	.xa = XARRAY_INIT(name, IDA_INIT_FLAGS)				\
}
#define DEFINE_IDA(name)	struct ida name = IDA_INIT(name)

int ida_alloc_range(struct ida *, unsigned int min, unsigned int max, gfp_t);
void ida_free(struct ida *, unsigned int id);
void ida_destroy(struct ida *ida);


static inline int ida_alloc(struct ida *ida, gfp_t gfp)
{
	return ida_alloc_range(ida, 0, ~0, gfp);
}


static inline int ida_alloc_min(struct ida *ida, unsigned int min, gfp_t gfp)
{
	return ida_alloc_range(ida, min, ~0, gfp);
}


static inline int ida_alloc_max(struct ida *ida, unsigned int max, gfp_t gfp)
{
	return ida_alloc_range(ida, 0, max, gfp);
}

static inline void ida_init(struct ida *ida)
{
	xa_init_flags(&ida->xa, IDA_INIT_FLAGS);
}


#define ida_simple_get(ida, start, end, gfp)	\
			ida_alloc_range(ida, start, (end) - 1, gfp)
#define ida_simple_remove(ida, id)	ida_free(ida, id)

static inline bool ida_is_empty(const struct ida *ida)
{
	return xa_empty(&ida->xa);
}
#endif 
