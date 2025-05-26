/* SPDX-License-Identifier: MIT */

#ifndef __TTM_KMAP_ITER_H__
#define __TTM_KMAP_ITER_H__

#include <linux/types.h>

struct ttm_kmap_iter;
struct iosys_map;


struct ttm_kmap_iter_ops {
	
	void (*map_local)(struct ttm_kmap_iter *res_iter,
			  struct iosys_map *dmap, pgoff_t i);
	
	void (*unmap_local)(struct ttm_kmap_iter *res_iter,
			    struct iosys_map *dmap);
	bool maps_tt;
};


struct ttm_kmap_iter {
	const struct ttm_kmap_iter_ops *ops;
};

#endif 
