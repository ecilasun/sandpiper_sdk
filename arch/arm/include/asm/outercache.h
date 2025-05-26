/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_OUTERCACHE_H
#define __ASM_OUTERCACHE_H

#include <linux/types.h>

struct l2x0_regs;

struct outer_cache_fns {
	void (*inv_range)(unsigned long, unsigned long);
	void (*clean_range)(unsigned long, unsigned long);
	void (*flush_range)(unsigned long, unsigned long);
	void (*flush_all)(void);
	void (*disable)(void);
#ifdef CONFIG_OUTER_CACHE_SYNC
	void (*sync)(void);
#endif
	void (*resume)(void);

	
	void (*write_sec)(unsigned long, unsigned);
	void (*configure)(const struct l2x0_regs *);
};

extern struct outer_cache_fns outer_cache;

#ifdef CONFIG_OUTER_CACHE

static inline void outer_inv_range(phys_addr_t start, phys_addr_t end)
{
	if (outer_cache.inv_range)
		outer_cache.inv_range(start, end);
}


static inline void outer_clean_range(phys_addr_t start, phys_addr_t end)
{
	if (outer_cache.clean_range)
		outer_cache.clean_range(start, end);
}


static inline void outer_flush_range(phys_addr_t start, phys_addr_t end)
{
	if (outer_cache.flush_range)
		outer_cache.flush_range(start, end);
}


static inline void outer_flush_all(void)
{
	if (outer_cache.flush_all)
		outer_cache.flush_all();
}


extern void outer_disable(void);


static inline void outer_resume(void)
{
	if (outer_cache.resume)
		outer_cache.resume();
}

#else

static inline void outer_inv_range(phys_addr_t start, phys_addr_t end)
{ }
static inline void outer_clean_range(phys_addr_t start, phys_addr_t end)
{ }
static inline void outer_flush_range(phys_addr_t start, phys_addr_t end)
{ }
static inline void outer_flush_all(void) { }
static inline void outer_disable(void) { }
static inline void outer_resume(void) { }

#endif

#endif	
