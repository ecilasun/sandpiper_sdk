/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __SOUND_MEMALLOC_H
#define __SOUND_MEMALLOC_H

#include <linux/dma-direction.h>
#include <asm/page.h>

struct device;
struct vm_area_struct;
struct sg_table;


struct snd_dma_device {
	int type;			
	enum dma_data_direction dir;	
	bool need_sync;			
	struct device *dev;		
};


#define SNDRV_DMA_TYPE_UNKNOWN		0	
#define SNDRV_DMA_TYPE_CONTINUOUS	1	
#define SNDRV_DMA_TYPE_DEV		2	
#define SNDRV_DMA_TYPE_DEV_WC		5	
#ifdef CONFIG_GENERIC_ALLOCATOR
#define SNDRV_DMA_TYPE_DEV_IRAM		4	
#else
#define SNDRV_DMA_TYPE_DEV_IRAM	SNDRV_DMA_TYPE_DEV
#endif
#define SNDRV_DMA_TYPE_VMALLOC		7	
#define SNDRV_DMA_TYPE_NONCONTIG	8	
#define SNDRV_DMA_TYPE_NONCOHERENT	9	
#ifdef CONFIG_SND_DMA_SGBUF
#define SNDRV_DMA_TYPE_DEV_SG		SNDRV_DMA_TYPE_NONCONTIG
#define SNDRV_DMA_TYPE_DEV_WC_SG	6	
#else
#define SNDRV_DMA_TYPE_DEV_SG	SNDRV_DMA_TYPE_DEV 
#define SNDRV_DMA_TYPE_DEV_WC_SG	SNDRV_DMA_TYPE_DEV_WC
#endif

#ifdef CONFIG_SND_DMA_SGBUF
#define SNDRV_DMA_TYPE_DEV_SG_FALLBACK		10
#define SNDRV_DMA_TYPE_DEV_WC_SG_FALLBACK	11
#endif


struct snd_dma_buffer {
	struct snd_dma_device dev;	
	unsigned char *area;	
	dma_addr_t addr;	
	size_t bytes;		
	void *private_data;	
};


static inline unsigned int snd_sgbuf_aligned_pages(size_t size)
{
	return (size + PAGE_SIZE - 1) >> PAGE_SHIFT;
}


int snd_dma_alloc_dir_pages(int type, struct device *dev,
			    enum dma_data_direction dir, size_t size,
			    struct snd_dma_buffer *dmab);

static inline int snd_dma_alloc_pages(int type, struct device *dev,
				      size_t size, struct snd_dma_buffer *dmab)
{
	return snd_dma_alloc_dir_pages(type, dev, DMA_BIDIRECTIONAL, size, dmab);
}

int snd_dma_alloc_pages_fallback(int type, struct device *dev, size_t size,
                                 struct snd_dma_buffer *dmab);
void snd_dma_free_pages(struct snd_dma_buffer *dmab);
int snd_dma_buffer_mmap(struct snd_dma_buffer *dmab,
			struct vm_area_struct *area);

enum snd_dma_sync_mode { SNDRV_DMA_SYNC_CPU, SNDRV_DMA_SYNC_DEVICE };
#ifdef CONFIG_HAS_DMA
void snd_dma_buffer_sync(struct snd_dma_buffer *dmab,
			 enum snd_dma_sync_mode mode);
#else
static inline void snd_dma_buffer_sync(struct snd_dma_buffer *dmab,
				       enum snd_dma_sync_mode mode) {}
#endif

dma_addr_t snd_sgbuf_get_addr(struct snd_dma_buffer *dmab, size_t offset);
struct page *snd_sgbuf_get_page(struct snd_dma_buffer *dmab, size_t offset);
unsigned int snd_sgbuf_get_chunk_size(struct snd_dma_buffer *dmab,
				      unsigned int ofs, unsigned int size);


struct snd_dma_buffer *snd_devm_alloc_dir_pages(struct device *dev, int type,
						enum dma_data_direction dir,
						size_t size);

static inline struct snd_dma_buffer *
snd_devm_alloc_pages(struct device *dev, int type, size_t size)
{
	return snd_devm_alloc_dir_pages(dev, type, DMA_BIDIRECTIONAL, size);
}

static inline struct sg_table *
snd_dma_noncontig_sg_table(struct snd_dma_buffer *dmab)
{
	return dmab->private_data;
}

#endif 

