/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_SLAB_H
#define	_LINUX_SLAB_H

#include <linux/cache.h>
#include <linux/gfp.h>
#include <linux/overflow.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/percpu-refcount.h>
#include <linux/cleanup.h>
#include <linux/hash.h>




#define SLAB_CONSISTENCY_CHECKS	((slab_flags_t __force)0x00000100U)

#define SLAB_RED_ZONE		((slab_flags_t __force)0x00000400U)

#define SLAB_POISON		((slab_flags_t __force)0x00000800U)

#define SLAB_KMALLOC		((slab_flags_t __force)0x00001000U)

#define SLAB_HWCACHE_ALIGN	((slab_flags_t __force)0x00002000U)

#define SLAB_CACHE_DMA		((slab_flags_t __force)0x00004000U)

#define SLAB_CACHE_DMA32	((slab_flags_t __force)0x00008000U)

#define SLAB_STORE_USER		((slab_flags_t __force)0x00010000U)

#define SLAB_PANIC		((slab_flags_t __force)0x00040000U)


#define SLAB_TYPESAFE_BY_RCU	((slab_flags_t __force)0x00080000U)

#define SLAB_MEM_SPREAD		((slab_flags_t __force)0x00100000U)

#define SLAB_TRACE		((slab_flags_t __force)0x00200000U)


#ifdef CONFIG_DEBUG_OBJECTS
# define SLAB_DEBUG_OBJECTS	((slab_flags_t __force)0x00400000U)
#else
# define SLAB_DEBUG_OBJECTS	0
#endif


#define SLAB_NOLEAKTRACE	((slab_flags_t __force)0x00800000U)


#define SLAB_NO_MERGE		((slab_flags_t __force)0x01000000U)


#ifdef CONFIG_FAILSLAB
# define SLAB_FAILSLAB		((slab_flags_t __force)0x02000000U)
#else
# define SLAB_FAILSLAB		0
#endif

#ifdef CONFIG_MEMCG_KMEM
# define SLAB_ACCOUNT		((slab_flags_t __force)0x04000000U)
#else
# define SLAB_ACCOUNT		0
#endif

#ifdef CONFIG_KASAN_GENERIC
#define SLAB_KASAN		((slab_flags_t __force)0x08000000U)
#else
#define SLAB_KASAN		0
#endif


#define SLAB_NO_USER_FLAGS	((slab_flags_t __force)0x10000000U)

#ifdef CONFIG_KFENCE
#define SLAB_SKIP_KFENCE	((slab_flags_t __force)0x20000000U)
#else
#define SLAB_SKIP_KFENCE	0
#endif



#ifndef CONFIG_SLUB_TINY
#define SLAB_RECLAIM_ACCOUNT	((slab_flags_t __force)0x00020000U)
#else
#define SLAB_RECLAIM_ACCOUNT	((slab_flags_t __force)0)
#endif
#define SLAB_TEMPORARY		SLAB_RECLAIM_ACCOUNT	


#define ZERO_SIZE_PTR ((void *)16)

#define ZERO_OR_NULL_PTR(x) ((unsigned long)(x) <= \
				(unsigned long)ZERO_SIZE_PTR)

#include <linux/kasan.h>

struct list_lru;
struct mem_cgroup;

bool slab_is_available(void);

struct kmem_cache *kmem_cache_create(const char *name, unsigned int size,
			unsigned int align, slab_flags_t flags,
			void (*ctor)(void *));
struct kmem_cache *kmem_cache_create_usercopy(const char *name,
			unsigned int size, unsigned int align,
			slab_flags_t flags,
			unsigned int useroffset, unsigned int usersize,
			void (*ctor)(void *));
void kmem_cache_destroy(struct kmem_cache *s);
int kmem_cache_shrink(struct kmem_cache *s);


#define KMEM_CACHE(__struct, __flags)					\
		kmem_cache_create(#__struct, sizeof(struct __struct),	\
			__alignof__(struct __struct), (__flags), NULL)


#define KMEM_CACHE_USERCOPY(__struct, __flags, __field)			\
		kmem_cache_create_usercopy(#__struct,			\
			sizeof(struct __struct),			\
			__alignof__(struct __struct), (__flags),	\
			offsetof(struct __struct, __field),		\
			sizeof_field(struct __struct, __field), NULL)


void * __must_check krealloc(const void *objp, size_t new_size, gfp_t flags) __realloc_size(2);
void kfree(const void *objp);
void kfree_sensitive(const void *objp);
size_t __ksize(const void *objp);

DEFINE_FREE(kfree, void *, if (!IS_ERR_OR_NULL(_T)) kfree(_T))


size_t ksize(const void *objp);

#ifdef CONFIG_PRINTK
bool kmem_valid_obj(void *object);
void kmem_dump_obj(void *object);
#endif


#ifdef ARCH_HAS_DMA_MINALIGN
#if ARCH_DMA_MINALIGN > 8 && !defined(ARCH_KMALLOC_MINALIGN)
#define ARCH_KMALLOC_MINALIGN ARCH_DMA_MINALIGN
#endif
#endif

#ifndef ARCH_KMALLOC_MINALIGN
#define ARCH_KMALLOC_MINALIGN __alignof__(unsigned long long)
#elif ARCH_KMALLOC_MINALIGN > 8
#define KMALLOC_MIN_SIZE ARCH_KMALLOC_MINALIGN
#define KMALLOC_SHIFT_LOW ilog2(KMALLOC_MIN_SIZE)
#endif


#ifndef ARCH_SLAB_MINALIGN
#define ARCH_SLAB_MINALIGN __alignof__(unsigned long long)
#endif


#ifndef arch_slab_minalign
static inline unsigned int arch_slab_minalign(void)
{
	return ARCH_SLAB_MINALIGN;
}
#endif


#define __assume_kmalloc_alignment __assume_aligned(ARCH_KMALLOC_MINALIGN)
#define __assume_slab_alignment __assume_aligned(ARCH_SLAB_MINALIGN)
#define __assume_page_alignment __assume_aligned(PAGE_SIZE)



#ifdef CONFIG_SLAB

#define KMALLOC_SHIFT_HIGH	(PAGE_SHIFT + 1)
#define KMALLOC_SHIFT_MAX	(MAX_ORDER + PAGE_SHIFT)
#ifndef KMALLOC_SHIFT_LOW
#define KMALLOC_SHIFT_LOW	5
#endif
#endif

#ifdef CONFIG_SLUB
#define KMALLOC_SHIFT_HIGH	(PAGE_SHIFT + 1)
#define KMALLOC_SHIFT_MAX	(MAX_ORDER + PAGE_SHIFT)
#ifndef KMALLOC_SHIFT_LOW
#define KMALLOC_SHIFT_LOW	3
#endif
#endif


#define KMALLOC_MAX_SIZE	(1UL << KMALLOC_SHIFT_MAX)

#define KMALLOC_MAX_CACHE_SIZE	(1UL << KMALLOC_SHIFT_HIGH)

#define KMALLOC_MAX_ORDER	(KMALLOC_SHIFT_MAX - PAGE_SHIFT)


#ifndef KMALLOC_MIN_SIZE
#define KMALLOC_MIN_SIZE (1 << KMALLOC_SHIFT_LOW)
#endif


#define SLAB_OBJ_MIN_SIZE      (KMALLOC_MIN_SIZE < 16 ? \
                               (KMALLOC_MIN_SIZE) : 16)

#ifdef CONFIG_RANDOM_KMALLOC_CACHES
#define RANDOM_KMALLOC_CACHES_NR	15 // # of cache copies
#else
#define RANDOM_KMALLOC_CACHES_NR	0
#endif


enum kmalloc_cache_type {
	KMALLOC_NORMAL = 0,
#ifndef CONFIG_ZONE_DMA
	KMALLOC_DMA = KMALLOC_NORMAL,
#endif
#ifndef CONFIG_MEMCG_KMEM
	KMALLOC_CGROUP = KMALLOC_NORMAL,
#endif
	KMALLOC_RANDOM_START = KMALLOC_NORMAL,
	KMALLOC_RANDOM_END = KMALLOC_RANDOM_START + RANDOM_KMALLOC_CACHES_NR,
#ifdef CONFIG_SLUB_TINY
	KMALLOC_RECLAIM = KMALLOC_NORMAL,
#else
	KMALLOC_RECLAIM,
#endif
#ifdef CONFIG_ZONE_DMA
	KMALLOC_DMA,
#endif
#ifdef CONFIG_MEMCG_KMEM
	KMALLOC_CGROUP,
#endif
	NR_KMALLOC_TYPES
};

extern struct kmem_cache *
kmalloc_caches[NR_KMALLOC_TYPES][KMALLOC_SHIFT_HIGH + 1];


#define KMALLOC_NOT_NORMAL_BITS					\
	(__GFP_RECLAIMABLE |					\
	(IS_ENABLED(CONFIG_ZONE_DMA)   ? __GFP_DMA : 0) |	\
	(IS_ENABLED(CONFIG_MEMCG_KMEM) ? __GFP_ACCOUNT : 0))

extern unsigned long random_kmalloc_seed;

static __always_inline enum kmalloc_cache_type kmalloc_type(gfp_t flags, unsigned long caller)
{
	
	if (likely((flags & KMALLOC_NOT_NORMAL_BITS) == 0))
#ifdef CONFIG_RANDOM_KMALLOC_CACHES
		
		return KMALLOC_RANDOM_START + hash_64(caller ^ random_kmalloc_seed,
						      ilog2(RANDOM_KMALLOC_CACHES_NR + 1));
#else
		return KMALLOC_NORMAL;
#endif

	
	if (IS_ENABLED(CONFIG_ZONE_DMA) && (flags & __GFP_DMA))
		return KMALLOC_DMA;
	if (!IS_ENABLED(CONFIG_MEMCG_KMEM) || (flags & __GFP_RECLAIMABLE))
		return KMALLOC_RECLAIM;
	else
		return KMALLOC_CGROUP;
}


static __always_inline unsigned int __kmalloc_index(size_t size,
						    bool size_is_constant)
{
	if (!size)
		return 0;

	if (size <= KMALLOC_MIN_SIZE)
		return KMALLOC_SHIFT_LOW;

	if (KMALLOC_MIN_SIZE <= 32 && size > 64 && size <= 96)
		return 1;
	if (KMALLOC_MIN_SIZE <= 64 && size > 128 && size <= 192)
		return 2;
	if (size <=          8) return 3;
	if (size <=         16) return 4;
	if (size <=         32) return 5;
	if (size <=         64) return 6;
	if (size <=        128) return 7;
	if (size <=        256) return 8;
	if (size <=        512) return 9;
	if (size <=       1024) return 10;
	if (size <=   2 * 1024) return 11;
	if (size <=   4 * 1024) return 12;
	if (size <=   8 * 1024) return 13;
	if (size <=  16 * 1024) return 14;
	if (size <=  32 * 1024) return 15;
	if (size <=  64 * 1024) return 16;
	if (size <= 128 * 1024) return 17;
	if (size <= 256 * 1024) return 18;
	if (size <= 512 * 1024) return 19;
	if (size <= 1024 * 1024) return 20;
	if (size <=  2 * 1024 * 1024) return 21;

	if (!IS_ENABLED(CONFIG_PROFILE_ALL_BRANCHES) && size_is_constant)
		BUILD_BUG_ON_MSG(1, "unexpected size in kmalloc_index()");
	else
		BUG();

	
	return -1;
}
static_assert(PAGE_SHIFT <= 20);
#define kmalloc_index(s) __kmalloc_index(s, true)

void *__kmalloc(size_t size, gfp_t flags) __assume_kmalloc_alignment __alloc_size(1);


void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t flags) __assume_slab_alignment __malloc;
void *kmem_cache_alloc_lru(struct kmem_cache *s, struct list_lru *lru,
			   gfp_t gfpflags) __assume_slab_alignment __malloc;
void kmem_cache_free(struct kmem_cache *s, void *objp);


void kmem_cache_free_bulk(struct kmem_cache *s, size_t size, void **p);
int kmem_cache_alloc_bulk(struct kmem_cache *s, gfp_t flags, size_t size, void **p);

static __always_inline void kfree_bulk(size_t size, void **p)
{
	kmem_cache_free_bulk(NULL, size, p);
}

void *__kmalloc_node(size_t size, gfp_t flags, int node) __assume_kmalloc_alignment
							 __alloc_size(1);
void *kmem_cache_alloc_node(struct kmem_cache *s, gfp_t flags, int node) __assume_slab_alignment
									 __malloc;

void *kmalloc_trace(struct kmem_cache *s, gfp_t flags, size_t size)
		    __assume_kmalloc_alignment __alloc_size(3);

void *kmalloc_node_trace(struct kmem_cache *s, gfp_t gfpflags,
			 int node, size_t size) __assume_kmalloc_alignment
						__alloc_size(4);
void *kmalloc_large(size_t size, gfp_t flags) __assume_page_alignment
					      __alloc_size(1);

void *kmalloc_large_node(size_t size, gfp_t flags, int node) __assume_page_alignment
							     __alloc_size(1);


static __always_inline __alloc_size(1) void *kmalloc(size_t size, gfp_t flags)
{
	if (__builtin_constant_p(size) && size) {
		unsigned int index;

		if (size > KMALLOC_MAX_CACHE_SIZE)
			return kmalloc_large(size, flags);

		index = kmalloc_index(size);
		return kmalloc_trace(
				kmalloc_caches[kmalloc_type(flags, _RET_IP_)][index],
				flags, size);
	}
	return __kmalloc(size, flags);
}

static __always_inline __alloc_size(1) void *kmalloc_node(size_t size, gfp_t flags, int node)
{
	if (__builtin_constant_p(size) && size) {
		unsigned int index;

		if (size > KMALLOC_MAX_CACHE_SIZE)
			return kmalloc_large_node(size, flags, node);

		index = kmalloc_index(size);
		return kmalloc_node_trace(
				kmalloc_caches[kmalloc_type(flags, _RET_IP_)][index],
				flags, node, size);
	}
	return __kmalloc_node(size, flags, node);
}


static inline __alloc_size(1, 2) void *kmalloc_array(size_t n, size_t size, gfp_t flags)
{
	size_t bytes;

	if (unlikely(check_mul_overflow(n, size, &bytes)))
		return NULL;
	if (__builtin_constant_p(n) && __builtin_constant_p(size))
		return kmalloc(bytes, flags);
	return __kmalloc(bytes, flags);
}


static inline __realloc_size(2, 3) void * __must_check krealloc_array(void *p,
								      size_t new_n,
								      size_t new_size,
								      gfp_t flags)
{
	size_t bytes;

	if (unlikely(check_mul_overflow(new_n, new_size, &bytes)))
		return NULL;

	return krealloc(p, bytes, flags);
}


static inline __alloc_size(1, 2) void *kcalloc(size_t n, size_t size, gfp_t flags)
{
	return kmalloc_array(n, size, flags | __GFP_ZERO);
}

void *__kmalloc_node_track_caller(size_t size, gfp_t flags, int node,
				  unsigned long caller) __alloc_size(1);
#define kmalloc_node_track_caller(size, flags, node) \
	__kmalloc_node_track_caller(size, flags, node, \
				    _RET_IP_)


#define kmalloc_track_caller(size, flags) \
	__kmalloc_node_track_caller(size, flags, \
				    NUMA_NO_NODE, _RET_IP_)

static inline __alloc_size(1, 2) void *kmalloc_array_node(size_t n, size_t size, gfp_t flags,
							  int node)
{
	size_t bytes;

	if (unlikely(check_mul_overflow(n, size, &bytes)))
		return NULL;
	if (__builtin_constant_p(n) && __builtin_constant_p(size))
		return kmalloc_node(bytes, flags, node);
	return __kmalloc_node(bytes, flags, node);
}

static inline __alloc_size(1, 2) void *kcalloc_node(size_t n, size_t size, gfp_t flags, int node)
{
	return kmalloc_array_node(n, size, flags | __GFP_ZERO, node);
}


static inline void *kmem_cache_zalloc(struct kmem_cache *k, gfp_t flags)
{
	return kmem_cache_alloc(k, flags | __GFP_ZERO);
}


static inline __alloc_size(1) void *kzalloc(size_t size, gfp_t flags)
{
	return kmalloc(size, flags | __GFP_ZERO);
}


static inline __alloc_size(1) void *kzalloc_node(size_t size, gfp_t flags, int node)
{
	return kmalloc_node(size, flags | __GFP_ZERO, node);
}

extern void *kvmalloc_node(size_t size, gfp_t flags, int node) __alloc_size(1);
static inline __alloc_size(1) void *kvmalloc(size_t size, gfp_t flags)
{
	return kvmalloc_node(size, flags, NUMA_NO_NODE);
}
static inline __alloc_size(1) void *kvzalloc_node(size_t size, gfp_t flags, int node)
{
	return kvmalloc_node(size, flags | __GFP_ZERO, node);
}
static inline __alloc_size(1) void *kvzalloc(size_t size, gfp_t flags)
{
	return kvmalloc(size, flags | __GFP_ZERO);
}

static inline __alloc_size(1, 2) void *kvmalloc_array(size_t n, size_t size, gfp_t flags)
{
	size_t bytes;

	if (unlikely(check_mul_overflow(n, size, &bytes)))
		return NULL;

	return kvmalloc(bytes, flags);
}

static inline __alloc_size(1, 2) void *kvcalloc(size_t n, size_t size, gfp_t flags)
{
	return kvmalloc_array(n, size, flags | __GFP_ZERO);
}

extern void *kvrealloc(const void *p, size_t oldsize, size_t newsize, gfp_t flags)
		      __realloc_size(3);
extern void kvfree(const void *addr);
extern void kvfree_sensitive(const void *addr, size_t len);

unsigned int kmem_cache_size(struct kmem_cache *s);


size_t kmalloc_size_roundup(size_t size);

void __init kmem_cache_init_late(void);

#if defined(CONFIG_SMP) && defined(CONFIG_SLAB)
int slab_prepare_cpu(unsigned int cpu);
int slab_dead_cpu(unsigned int cpu);
#else
#define slab_prepare_cpu	NULL
#define slab_dead_cpu		NULL
#endif

#endif	
