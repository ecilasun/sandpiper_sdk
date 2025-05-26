/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _DMA_HEAPS_H
#define _DMA_HEAPS_H

#include <linux/cdev.h>
#include <linux/types.h>

struct dma_heap;


struct dma_heap_ops {
	struct dma_buf *(*allocate)(struct dma_heap *heap,
				    unsigned long len,
				    unsigned long fd_flags,
				    unsigned long heap_flags);
};


struct dma_heap_export_info {
	const char *name;
	const struct dma_heap_ops *ops;
	void *priv;
};


void *dma_heap_get_drvdata(struct dma_heap *heap);


const char *dma_heap_get_name(struct dma_heap *heap);


struct dma_heap *dma_heap_add(const struct dma_heap_export_info *exp_info);

#endif 
