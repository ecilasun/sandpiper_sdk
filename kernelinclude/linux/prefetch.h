/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_PREFETCH_H
#define _LINUX_PREFETCH_H

#include <linux/types.h>
#include <asm/processor.h>
#include <asm/cache.h>

struct page;


#ifndef ARCH_HAS_PREFETCH
#define prefetch(x) __builtin_prefetch(x)
#endif

#ifndef ARCH_HAS_PREFETCHW
#define prefetchw(x) __builtin_prefetch(x,1)
#endif

#ifndef PREFETCH_STRIDE
#define PREFETCH_STRIDE (4*L1_CACHE_BYTES)
#endif

static inline void prefetch_range(void *addr, size_t len)
{
#ifdef ARCH_HAS_PREFETCH
	char *cp;
	char *end = addr + len;

	for (cp = addr; cp < end; cp += PREFETCH_STRIDE)
		prefetch(cp);
#endif
}

static inline void prefetch_page_address(struct page *page)
{
#if defined(WANT_PAGE_VIRTUAL) || defined(HASHED_PAGE_VIRTUAL)
	prefetch(page);
#endif
}

#endif
