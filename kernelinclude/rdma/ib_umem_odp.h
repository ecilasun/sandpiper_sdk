/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */


#ifndef IB_UMEM_ODP_H
#define IB_UMEM_ODP_H

#include <rdma/ib_umem.h>
#include <rdma/ib_verbs.h>

struct ib_umem_odp {
	struct ib_umem umem;
	struct mmu_interval_notifier notifier;
	struct pid *tgid;

	
	unsigned long *pfn_list;

	
	dma_addr_t		*dma_list;
	
	struct mutex		umem_mutex;
	void			*private; 

	int npages;

	
	bool is_implicit_odp;

	unsigned int		page_shift;
};

static inline struct ib_umem_odp *to_ib_umem_odp(struct ib_umem *umem)
{
	return container_of(umem, struct ib_umem_odp, umem);
}


static inline unsigned long ib_umem_start(struct ib_umem_odp *umem_odp)
{
	return umem_odp->notifier.interval_tree.start;
}


static inline unsigned long ib_umem_end(struct ib_umem_odp *umem_odp)
{
	return umem_odp->notifier.interval_tree.last + 1;
}

static inline size_t ib_umem_odp_num_pages(struct ib_umem_odp *umem_odp)
{
	return (ib_umem_end(umem_odp) - ib_umem_start(umem_odp)) >>
	       umem_odp->page_shift;
}


#define ODP_READ_ALLOWED_BIT  (1<<0ULL)
#define ODP_WRITE_ALLOWED_BIT (1<<1ULL)

#define ODP_DMA_ADDR_MASK (~(ODP_READ_ALLOWED_BIT | ODP_WRITE_ALLOWED_BIT))

#ifdef CONFIG_INFINIBAND_ON_DEMAND_PAGING

struct ib_umem_odp *
ib_umem_odp_get(struct ib_device *device, unsigned long addr, size_t size,
		int access, const struct mmu_interval_notifier_ops *ops);
struct ib_umem_odp *ib_umem_odp_alloc_implicit(struct ib_device *device,
					       int access);
struct ib_umem_odp *
ib_umem_odp_alloc_child(struct ib_umem_odp *root_umem, unsigned long addr,
			size_t size,
			const struct mmu_interval_notifier_ops *ops);
void ib_umem_odp_release(struct ib_umem_odp *umem_odp);

int ib_umem_odp_map_dma_and_lock(struct ib_umem_odp *umem_odp, u64 start_offset,
				 u64 bcnt, u64 access_mask, bool fault);

void ib_umem_odp_unmap_dma_pages(struct ib_umem_odp *umem_odp, u64 start_offset,
				 u64 bound);

#else 

static inline struct ib_umem_odp *
ib_umem_odp_get(struct ib_device *device, unsigned long addr, size_t size,
		int access, const struct mmu_interval_notifier_ops *ops)
{
	return ERR_PTR(-EINVAL);
}

static inline void ib_umem_odp_release(struct ib_umem_odp *umem_odp) {}

#endif 

#endif 
