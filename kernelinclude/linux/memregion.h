/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _MEMREGION_H_
#define _MEMREGION_H_
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/range.h>
#include <linux/bug.h>

struct memregion_info {
	int target_node;
	struct range range;
};

#ifdef CONFIG_MEMREGION
int memregion_alloc(gfp_t gfp);
void memregion_free(int id);
#else
static inline int memregion_alloc(gfp_t gfp)
{
	return -ENOMEM;
}
static inline void memregion_free(int id)
{
}
#endif


#ifdef CONFIG_ARCH_HAS_CPU_CACHE_INVALIDATE_MEMREGION
int cpu_cache_invalidate_memregion(int res_desc);
bool cpu_cache_has_invalidate_memregion(void);
#else
static inline bool cpu_cache_has_invalidate_memregion(void)
{
	return false;
}

static inline int cpu_cache_invalidate_memregion(int res_desc)
{
	WARN_ON_ONCE("CPU cache invalidation required");
	return -ENXIO;
}
#endif
#endif 
