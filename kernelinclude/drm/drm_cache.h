


#ifndef _DRM_CACHE_H_
#define _DRM_CACHE_H_

#include <linux/scatterlist.h>

struct iosys_map;

void drm_clflush_pages(struct page *pages[], unsigned long num_pages);
void drm_clflush_sg(struct sg_table *st);
void drm_clflush_virt_range(void *addr, unsigned long length);
bool drm_need_swiotlb(int dma_bits);


static inline bool drm_arch_can_wc_memory(void)
{
#if defined(CONFIG_PPC) && !defined(CONFIG_NOT_COHERENT_CACHE)
	return false;
#elif defined(CONFIG_MIPS) && defined(CONFIG_CPU_LOONGSON64)
	return false;
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
	
	return false;
#elif defined(CONFIG_LOONGARCH)
	
	return false;
#else
	return true;
#endif
}

void drm_memcpy_init_early(void);

void drm_memcpy_from_wc(struct iosys_map *dst,
			const struct iosys_map *src,
			unsigned long len);
#endif
