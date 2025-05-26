/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SWIOTLB_H
#define __LINUX_SWIOTLB_H

#include <linux/device.h>
#include <linux/dma-direction.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/limits.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

struct device;
struct page;
struct scatterlist;

#define SWIOTLB_VERBOSE	(1 << 0) 
#define SWIOTLB_FORCE	(1 << 1) 
#define SWIOTLB_ANY	(1 << 2) 


#define IO_TLB_SEGSIZE	128


#define IO_TLB_SHIFT 11
#define IO_TLB_SIZE (1 << IO_TLB_SHIFT)


#define IO_TLB_DEFAULT_SIZE (64UL<<20)

unsigned long swiotlb_size_or_default(void);
void __init swiotlb_init_remap(bool addressing_limit, unsigned int flags,
	int (*remap)(void *tlb, unsigned long nslabs));
int swiotlb_init_late(size_t size, gfp_t gfp_mask,
	int (*remap)(void *tlb, unsigned long nslabs));
extern void __init swiotlb_update_mem_attributes(void);

phys_addr_t swiotlb_tbl_map_single(struct device *hwdev, phys_addr_t phys,
		size_t mapping_size, size_t alloc_size,
		unsigned int alloc_aligned_mask, enum dma_data_direction dir,
		unsigned long attrs);

extern void swiotlb_tbl_unmap_single(struct device *hwdev,
				     phys_addr_t tlb_addr,
				     size_t mapping_size,
				     enum dma_data_direction dir,
				     unsigned long attrs);

void swiotlb_sync_single_for_device(struct device *dev, phys_addr_t tlb_addr,
		size_t size, enum dma_data_direction dir);
void swiotlb_sync_single_for_cpu(struct device *dev, phys_addr_t tlb_addr,
		size_t size, enum dma_data_direction dir);
dma_addr_t swiotlb_map(struct device *dev, phys_addr_t phys,
		size_t size, enum dma_data_direction dir, unsigned long attrs);

#ifdef CONFIG_SWIOTLB


struct io_tlb_pool {
	phys_addr_t start;
	phys_addr_t end;
	void *vaddr;
	unsigned long nslabs;
	bool late_alloc;
	unsigned int nareas;
	unsigned int area_nslabs;
	struct io_tlb_area *areas;
	struct io_tlb_slot *slots;
#ifdef CONFIG_SWIOTLB_DYNAMIC
	struct list_head node;
	struct rcu_head rcu;
	bool transient;
#endif
};


struct io_tlb_mem {
	struct io_tlb_pool defpool;
	unsigned long nslabs;
	struct dentry *debugfs;
	bool force_bounce;
	bool for_alloc;
#ifdef CONFIG_SWIOTLB_DYNAMIC
	bool can_grow;
	u64 phys_limit;
	spinlock_t lock;
	struct list_head pools;
	struct work_struct dyn_alloc;
#endif
#ifdef CONFIG_DEBUG_FS
	atomic_long_t total_used;
	atomic_long_t used_hiwater;
#endif
};

#ifdef CONFIG_SWIOTLB_DYNAMIC

struct io_tlb_pool *swiotlb_find_pool(struct device *dev, phys_addr_t paddr);

#else

static inline struct io_tlb_pool *swiotlb_find_pool(struct device *dev,
						    phys_addr_t paddr)
{
	return &dev->dma_io_tlb_mem->defpool;
}

#endif


static inline bool is_swiotlb_buffer(struct device *dev, phys_addr_t paddr)
{
	struct io_tlb_mem *mem = dev->dma_io_tlb_mem;

	if (!mem)
		return false;

#ifdef CONFIG_SWIOTLB_DYNAMIC
	
	smp_rmb();
	return READ_ONCE(dev->dma_uses_io_tlb) &&
		swiotlb_find_pool(dev, paddr);
#else
	return paddr >= mem->defpool.start && paddr < mem->defpool.end;
#endif
}

static inline bool is_swiotlb_force_bounce(struct device *dev)
{
	struct io_tlb_mem *mem = dev->dma_io_tlb_mem;

	return mem && mem->force_bounce;
}

void swiotlb_init(bool addressing_limited, unsigned int flags);
void __init swiotlb_exit(void);
void swiotlb_dev_init(struct device *dev);
size_t swiotlb_max_mapping_size(struct device *dev);
bool is_swiotlb_allocated(void);
bool is_swiotlb_active(struct device *dev);
void __init swiotlb_adjust_size(unsigned long size);
phys_addr_t default_swiotlb_base(void);
phys_addr_t default_swiotlb_limit(void);
#else
static inline void swiotlb_init(bool addressing_limited, unsigned int flags)
{
}

static inline void swiotlb_dev_init(struct device *dev)
{
}

static inline bool is_swiotlb_buffer(struct device *dev, phys_addr_t paddr)
{
	return false;
}
static inline bool is_swiotlb_force_bounce(struct device *dev)
{
	return false;
}
static inline void swiotlb_exit(void)
{
}
static inline size_t swiotlb_max_mapping_size(struct device *dev)
{
	return SIZE_MAX;
}

static inline bool is_swiotlb_allocated(void)
{
	return false;
}

static inline bool is_swiotlb_active(struct device *dev)
{
	return false;
}

static inline void swiotlb_adjust_size(unsigned long size)
{
}

static inline phys_addr_t default_swiotlb_base(void)
{
	return 0;
}

static inline phys_addr_t default_swiotlb_limit(void)
{
	return 0;
}
#endif 

extern void swiotlb_print_info(void);

#ifdef CONFIG_DMA_RESTRICTED_POOL
struct page *swiotlb_alloc(struct device *dev, size_t size);
bool swiotlb_free(struct device *dev, struct page *page, size_t size);

static inline bool is_swiotlb_for_alloc(struct device *dev)
{
	return dev->dma_io_tlb_mem->for_alloc;
}
#else
static inline struct page *swiotlb_alloc(struct device *dev, size_t size)
{
	return NULL;
}
static inline bool swiotlb_free(struct device *dev, struct page *page,
				size_t size)
{
	return false;
}
static inline bool is_swiotlb_for_alloc(struct device *dev)
{
	return false;
}
#endif 

#endif 
