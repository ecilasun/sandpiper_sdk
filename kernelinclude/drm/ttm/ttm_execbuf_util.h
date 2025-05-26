


#ifndef _TTM_EXECBUF_UTIL_H_
#define _TTM_EXECBUF_UTIL_H_

#include <linux/list.h>

struct ww_acquire_ctx;
struct dma_fence;
struct ttm_buffer_object;



struct ttm_validate_buffer {
	struct list_head head;
	struct ttm_buffer_object *bo;
	unsigned int num_shared;
};


void ttm_eu_backoff_reservation(struct ww_acquire_ctx *ticket,
				struct list_head *list);


int ttm_eu_reserve_buffers(struct ww_acquire_ctx *ticket,
			   struct list_head *list, bool intr,
			   struct list_head *dups);


void ttm_eu_fence_buffer_objects(struct ww_acquire_ctx *ticket,
				 struct list_head *list,
				 struct dma_fence *fence);

#endif
