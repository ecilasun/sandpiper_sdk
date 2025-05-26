/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SLUB_DEF_H
#define _LINUX_SLUB_DEF_H


#include <linux/kfence.h>
#include <linux/kobject.h>
#include <linux/reciprocal_div.h>
#include <linux/local_lock.h>

enum stat_item {
	ALLOC_FASTPATH,		
	ALLOC_SLOWPATH,		
	FREE_FASTPATH,		
	FREE_SLOWPATH,		
	FREE_FROZEN,		
	FREE_ADD_PARTIAL,	
	FREE_REMOVE_PARTIAL,	
	ALLOC_FROM_PARTIAL,	
	ALLOC_SLAB,		
	ALLOC_REFILL,		
	ALLOC_NODE_MISMATCH,	
	FREE_SLAB,		
	CPUSLAB_FLUSH,		
	DEACTIVATE_FULL,	
	DEACTIVATE_EMPTY,	
	DEACTIVATE_TO_HEAD,	
	DEACTIVATE_TO_TAIL,	
	DEACTIVATE_REMOTE_FREES,
	DEACTIVATE_BYPASS,	
	ORDER_FALLBACK,		
	CMPXCHG_DOUBLE_CPU_FAIL,
	CMPXCHG_DOUBLE_FAIL,	
	CPU_PARTIAL_ALLOC,	
	CPU_PARTIAL_FREE,	
	CPU_PARTIAL_NODE,	
	CPU_PARTIAL_DRAIN,	
	NR_SLUB_STAT_ITEMS
};

#ifndef CONFIG_SLUB_TINY

struct kmem_cache_cpu {
	union {
		struct {
			void **freelist;	
			unsigned long tid;	
		};
		freelist_aba_t freelist_tid;
	};
	struct slab *slab;	
#ifdef CONFIG_SLUB_CPU_PARTIAL
	struct slab *partial;	
#endif
	local_lock_t lock;	
#ifdef CONFIG_SLUB_STATS
	unsigned stat[NR_SLUB_STAT_ITEMS];
#endif
};
#endif 

#ifdef CONFIG_SLUB_CPU_PARTIAL
#define slub_percpu_partial(c)		((c)->partial)

#define slub_set_percpu_partial(c, p)		\
({						\
	slub_percpu_partial(c) = (p)->next;	\
})

#define slub_percpu_partial_read_once(c)     READ_ONCE(slub_percpu_partial(c))
#else
#define slub_percpu_partial(c)			NULL

#define slub_set_percpu_partial(c, p)

#define slub_percpu_partial_read_once(c)	NULL
#endif // CONFIG_SLUB_CPU_PARTIAL


struct kmem_cache_order_objects {
	unsigned int x;
};


struct kmem_cache {
#ifndef CONFIG_SLUB_TINY
	struct kmem_cache_cpu __percpu *cpu_slab;
#endif
	
	slab_flags_t flags;
	unsigned long min_partial;
	unsigned int size;	
	unsigned int object_size;
	struct reciprocal_value reciprocal_size;
	unsigned int offset;	
#ifdef CONFIG_SLUB_CPU_PARTIAL
	
	unsigned int cpu_partial;
	
	unsigned int cpu_partial_slabs;
#endif
	struct kmem_cache_order_objects oo;

	
	struct kmem_cache_order_objects min;
	gfp_t allocflags;	
	int refcount;		
	void (*ctor)(void *);
	unsigned int inuse;		
	unsigned int align;		
	unsigned int red_left_pad;	
	const char *name;	
	struct list_head list;	
#ifdef CONFIG_SYSFS
	struct kobject kobj;	
#endif
#ifdef CONFIG_SLAB_FREELIST_HARDENED
	unsigned long random;
#endif

#ifdef CONFIG_NUMA
	
	unsigned int remote_node_defrag_ratio;
#endif

#ifdef CONFIG_SLAB_FREELIST_RANDOM
	unsigned int *random_seq;
#endif

#ifdef CONFIG_KASAN_GENERIC
	struct kasan_cache kasan_info;
#endif

#ifdef CONFIG_HARDENED_USERCOPY
	unsigned int useroffset;	
	unsigned int usersize;		
#endif

	struct kmem_cache_node *node[MAX_NUMNODES];
};

#if defined(CONFIG_SYSFS) && !defined(CONFIG_SLUB_TINY)
#define SLAB_SUPPORTS_SYSFS
void sysfs_slab_unlink(struct kmem_cache *);
void sysfs_slab_release(struct kmem_cache *);
#else
static inline void sysfs_slab_unlink(struct kmem_cache *s)
{
}
static inline void sysfs_slab_release(struct kmem_cache *s)
{
}
#endif

void *fixup_red_left(struct kmem_cache *s, void *p);

static inline void *nearest_obj(struct kmem_cache *cache, const struct slab *slab,
				void *x) {
	void *object = x - (x - slab_address(slab)) % cache->size;
	void *last_object = slab_address(slab) +
		(slab->objects - 1) * cache->size;
	void *result = (unlikely(object > last_object)) ? last_object : object;

	result = fixup_red_left(cache, result);
	return result;
}


static inline unsigned int __obj_to_index(const struct kmem_cache *cache,
					  void *addr, void *obj)
{
	return reciprocal_divide(kasan_reset_tag(obj) - addr,
				 cache->reciprocal_size);
}

static inline unsigned int obj_to_index(const struct kmem_cache *cache,
					const struct slab *slab, void *obj)
{
	if (is_kfence_address(obj))
		return 0;
	return __obj_to_index(cache, slab_address(slab), obj);
}

static inline int objs_per_slab(const struct kmem_cache *cache,
				     const struct slab *slab)
{
	return slab->objects;
}
#endif 
