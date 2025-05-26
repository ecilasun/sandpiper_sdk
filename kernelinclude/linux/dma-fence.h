/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_DMA_FENCE_H
#define __LINUX_DMA_FENCE_H

#include <linux/err.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/bitops.h>
#include <linux/kref.h>
#include <linux/sched.h>
#include <linux/printk.h>
#include <linux/rcupdate.h>

struct dma_fence;
struct dma_fence_ops;
struct dma_fence_cb;


struct dma_fence {
	spinlock_t *lock;
	const struct dma_fence_ops *ops;
	
	union {
		struct list_head cb_list;
		
		ktime_t timestamp;
		
		struct rcu_head rcu;
	};
	u64 context;
	u64 seqno;
	unsigned long flags;
	struct kref refcount;
	int error;
};

enum dma_fence_flag_bits {
	DMA_FENCE_FLAG_SIGNALED_BIT,
	DMA_FENCE_FLAG_TIMESTAMP_BIT,
	DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT,
	DMA_FENCE_FLAG_USER_BITS, 
};

typedef void (*dma_fence_func_t)(struct dma_fence *fence,
				 struct dma_fence_cb *cb);


struct dma_fence_cb {
	struct list_head node;
	dma_fence_func_t func;
};


struct dma_fence_ops {
	
	bool use_64bit_seqno;

	
	const char * (*get_driver_name)(struct dma_fence *fence);

	
	const char * (*get_timeline_name)(struct dma_fence *fence);

	
	bool (*enable_signaling)(struct dma_fence *fence);

	
	bool (*signaled)(struct dma_fence *fence);

	
	signed long (*wait)(struct dma_fence *fence,
			    bool intr, signed long timeout);

	
	void (*release)(struct dma_fence *fence);

	
	void (*fence_value_str)(struct dma_fence *fence, char *str, int size);

	
	void (*timeline_value_str)(struct dma_fence *fence,
				   char *str, int size);

	
	void (*set_deadline)(struct dma_fence *fence, ktime_t deadline);
};

void dma_fence_init(struct dma_fence *fence, const struct dma_fence_ops *ops,
		    spinlock_t *lock, u64 context, u64 seqno);

void dma_fence_release(struct kref *kref);
void dma_fence_free(struct dma_fence *fence);
void dma_fence_describe(struct dma_fence *fence, struct seq_file *seq);


static inline void dma_fence_put(struct dma_fence *fence)
{
	if (fence)
		kref_put(&fence->refcount, dma_fence_release);
}


static inline struct dma_fence *dma_fence_get(struct dma_fence *fence)
{
	if (fence)
		kref_get(&fence->refcount);
	return fence;
}


static inline struct dma_fence *dma_fence_get_rcu(struct dma_fence *fence)
{
	if (kref_get_unless_zero(&fence->refcount))
		return fence;
	else
		return NULL;
}


static inline struct dma_fence *
dma_fence_get_rcu_safe(struct dma_fence __rcu **fencep)
{
	do {
		struct dma_fence *fence;

		fence = rcu_dereference(*fencep);
		if (!fence)
			return NULL;

		if (!dma_fence_get_rcu(fence))
			continue;

		
		if (fence == rcu_access_pointer(*fencep))
			return rcu_pointer_handoff(fence);

		dma_fence_put(fence);
	} while (1);
}

#ifdef CONFIG_LOCKDEP
bool dma_fence_begin_signalling(void);
void dma_fence_end_signalling(bool cookie);
void __dma_fence_might_wait(void);
#else
static inline bool dma_fence_begin_signalling(void)
{
	return true;
}
static inline void dma_fence_end_signalling(bool cookie) {}
static inline void __dma_fence_might_wait(void) {}
#endif

int dma_fence_signal(struct dma_fence *fence);
int dma_fence_signal_locked(struct dma_fence *fence);
int dma_fence_signal_timestamp(struct dma_fence *fence, ktime_t timestamp);
int dma_fence_signal_timestamp_locked(struct dma_fence *fence,
				      ktime_t timestamp);
signed long dma_fence_default_wait(struct dma_fence *fence,
				   bool intr, signed long timeout);
int dma_fence_add_callback(struct dma_fence *fence,
			   struct dma_fence_cb *cb,
			   dma_fence_func_t func);
bool dma_fence_remove_callback(struct dma_fence *fence,
			       struct dma_fence_cb *cb);
void dma_fence_enable_sw_signaling(struct dma_fence *fence);


static inline bool
dma_fence_is_signaled_locked(struct dma_fence *fence)
{
	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return true;

	if (fence->ops->signaled && fence->ops->signaled(fence)) {
		dma_fence_signal_locked(fence);
		return true;
	}

	return false;
}


static inline bool
dma_fence_is_signaled(struct dma_fence *fence)
{
	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return true;

	if (fence->ops->signaled && fence->ops->signaled(fence)) {
		dma_fence_signal(fence);
		return true;
	}

	return false;
}


static inline bool __dma_fence_is_later(u64 f1, u64 f2,
					const struct dma_fence_ops *ops)
{
	
	if (ops->use_64bit_seqno)
		return f1 > f2;

	return (int)(lower_32_bits(f1) - lower_32_bits(f2)) > 0;
}


static inline bool dma_fence_is_later(struct dma_fence *f1,
				      struct dma_fence *f2)
{
	if (WARN_ON(f1->context != f2->context))
		return false;

	return __dma_fence_is_later(f1->seqno, f2->seqno, f1->ops);
}


static inline bool dma_fence_is_later_or_same(struct dma_fence *f1,
					      struct dma_fence *f2)
{
	return f1 == f2 || dma_fence_is_later(f1, f2);
}


static inline struct dma_fence *dma_fence_later(struct dma_fence *f1,
						struct dma_fence *f2)
{
	if (WARN_ON(f1->context != f2->context))
		return NULL;

	
	if (dma_fence_is_later(f1, f2))
		return dma_fence_is_signaled(f1) ? NULL : f1;
	else
		return dma_fence_is_signaled(f2) ? NULL : f2;
}


static inline int dma_fence_get_status_locked(struct dma_fence *fence)
{
	if (dma_fence_is_signaled_locked(fence))
		return fence->error ?: 1;
	else
		return 0;
}

int dma_fence_get_status(struct dma_fence *fence);


static inline void dma_fence_set_error(struct dma_fence *fence,
				       int error)
{
	WARN_ON(test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags));
	WARN_ON(error >= 0 || error < -MAX_ERRNO);

	fence->error = error;
}


static inline ktime_t dma_fence_timestamp(struct dma_fence *fence)
{
	if (WARN_ON(!test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags)))
		return ktime_get();

	while (!test_bit(DMA_FENCE_FLAG_TIMESTAMP_BIT, &fence->flags))
		cpu_relax();

	return fence->timestamp;
}

signed long dma_fence_wait_timeout(struct dma_fence *,
				   bool intr, signed long timeout);
signed long dma_fence_wait_any_timeout(struct dma_fence **fences,
				       uint32_t count,
				       bool intr, signed long timeout,
				       uint32_t *idx);


static inline signed long dma_fence_wait(struct dma_fence *fence, bool intr)
{
	signed long ret;

	
	ret = dma_fence_wait_timeout(fence, intr, MAX_SCHEDULE_TIMEOUT);

	return ret < 0 ? ret : 0;
}

void dma_fence_set_deadline(struct dma_fence *fence, ktime_t deadline);

struct dma_fence *dma_fence_get_stub(void);
struct dma_fence *dma_fence_allocate_private_stub(ktime_t timestamp);
u64 dma_fence_context_alloc(unsigned num);

extern const struct dma_fence_ops dma_fence_array_ops;
extern const struct dma_fence_ops dma_fence_chain_ops;


static inline bool dma_fence_is_array(struct dma_fence *fence)
{
	return fence->ops == &dma_fence_array_ops;
}


static inline bool dma_fence_is_chain(struct dma_fence *fence)
{
	return fence->ops == &dma_fence_chain_ops;
}


static inline bool dma_fence_is_container(struct dma_fence *fence)
{
	return dma_fence_is_array(fence) || dma_fence_is_chain(fence);
}

#endif 
