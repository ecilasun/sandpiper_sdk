/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_PAGE_EXT_H
#define __LINUX_PAGE_EXT_H

#include <linux/types.h>
#include <linux/stacktrace.h>
#include <linux/stackdepot.h>

struct pglist_data;

#ifdef CONFIG_PAGE_EXTENSION

struct page_ext_operations {
	size_t offset;
	size_t size;
	bool (*need)(void);
	void (*init)(void);
	bool need_shared_flags;
};


enum page_ext_flags {
	PAGE_EXT_OWNER,
	PAGE_EXT_OWNER_ALLOCATED,
#if defined(CONFIG_PAGE_IDLE_FLAG) && !defined(CONFIG_64BIT)
	PAGE_EXT_YOUNG,
	PAGE_EXT_IDLE,
#endif
};


struct page_ext {
	unsigned long flags;
};

extern bool early_page_ext;
extern unsigned long page_ext_size;
extern void pgdat_page_ext_init(struct pglist_data *pgdat);

static inline bool early_page_ext_enabled(void)
{
	return early_page_ext;
}

#ifdef CONFIG_SPARSEMEM
static inline void page_ext_init_flatmem(void)
{
}
extern void page_ext_init(void);
static inline void page_ext_init_flatmem_late(void)
{
}
#else
extern void page_ext_init_flatmem(void);
extern void page_ext_init_flatmem_late(void);
static inline void page_ext_init(void)
{
}
#endif

extern struct page_ext *page_ext_get(struct page *page);
extern void page_ext_put(struct page_ext *page_ext);

static inline void *page_ext_data(struct page_ext *page_ext,
				  struct page_ext_operations *ops)
{
	return (void *)(page_ext) + ops->offset;
}

static inline struct page_ext *page_ext_next(struct page_ext *curr)
{
	void *next = curr;
	next += page_ext_size;
	return next;
}

#else 
struct page_ext;

static inline bool early_page_ext_enabled(void)
{
	return false;
}

static inline void pgdat_page_ext_init(struct pglist_data *pgdat)
{
}

static inline void page_ext_init(void)
{
}

static inline void page_ext_init_flatmem_late(void)
{
}

static inline void page_ext_init_flatmem(void)
{
}

static inline struct page_ext *page_ext_get(struct page *page)
{
	return NULL;
}

static inline void page_ext_put(struct page_ext *page_ext)
{
}
#endif 
#endif 
