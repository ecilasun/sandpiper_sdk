/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_KFENCE_H
#define _LINUX_KFENCE_H

#include <linux/mm.h>
#include <linux/types.h>

#ifdef CONFIG_KFENCE

#include <linux/atomic.h>
#include <linux/static_key.h>

extern unsigned long kfence_sample_interval;


#define KFENCE_POOL_SIZE ((CONFIG_KFENCE_NUM_OBJECTS + 1) * 2 * PAGE_SIZE)
extern char *__kfence_pool;

DECLARE_STATIC_KEY_FALSE(kfence_allocation_key);
extern atomic_t kfence_allocation_gate;


static __always_inline bool is_kfence_address(const void *addr)
{
	
	return unlikely((unsigned long)((char *)addr - __kfence_pool) < KFENCE_POOL_SIZE && __kfence_pool);
}


void __init kfence_alloc_pool_and_metadata(void);


void __init kfence_init(void);


void kfence_shutdown_cache(struct kmem_cache *s);


void *__kfence_alloc(struct kmem_cache *s, size_t size, gfp_t flags);


static __always_inline void *kfence_alloc(struct kmem_cache *s, size_t size, gfp_t flags)
{
#if defined(CONFIG_KFENCE_STATIC_KEYS) || CONFIG_KFENCE_SAMPLE_INTERVAL == 0
	if (!static_branch_unlikely(&kfence_allocation_key))
		return NULL;
#else
	if (!static_branch_likely(&kfence_allocation_key))
		return NULL;
#endif
	if (likely(atomic_read(&kfence_allocation_gate)))
		return NULL;
	return __kfence_alloc(s, size, flags);
}


size_t kfence_ksize(const void *addr);


void *kfence_object_start(const void *addr);


void __kfence_free(void *addr);


static __always_inline __must_check bool kfence_free(void *addr)
{
	if (!is_kfence_address(addr))
		return false;
	__kfence_free(addr);
	return true;
}


bool __must_check kfence_handle_page_fault(unsigned long addr, bool is_write, struct pt_regs *regs);

#ifdef CONFIG_PRINTK
struct kmem_obj_info;

bool __kfence_obj_info(struct kmem_obj_info *kpp, void *object, struct slab *slab);
#endif

#else 

static inline bool is_kfence_address(const void *addr) { return false; }
static inline void kfence_alloc_pool_and_metadata(void) { }
static inline void kfence_init(void) { }
static inline void kfence_shutdown_cache(struct kmem_cache *s) { }
static inline void *kfence_alloc(struct kmem_cache *s, size_t size, gfp_t flags) { return NULL; }
static inline size_t kfence_ksize(const void *addr) { return 0; }
static inline void *kfence_object_start(const void *addr) { return NULL; }
static inline void __kfence_free(void *addr) { }
static inline bool __must_check kfence_free(void *addr) { return false; }
static inline bool __must_check kfence_handle_page_fault(unsigned long addr, bool is_write,
							 struct pt_regs *regs)
{
	return false;
}

#ifdef CONFIG_PRINTK
struct kmem_obj_info;
static inline bool __kfence_obj_info(struct kmem_obj_info *kpp, void *object, struct slab *slab)
{
	return false;
}
#endif

#endif

#endif 
