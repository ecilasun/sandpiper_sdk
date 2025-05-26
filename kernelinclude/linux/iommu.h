/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_IOMMU_H
#define __LINUX_IOMMU_H

#include <linux/scatterlist.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/of.h>
#include <uapi/linux/iommu.h>

#define IOMMU_READ	(1 << 0)
#define IOMMU_WRITE	(1 << 1)
#define IOMMU_CACHE	(1 << 2) 
#define IOMMU_NOEXEC	(1 << 3)
#define IOMMU_MMIO	(1 << 4) 

#define IOMMU_PRIV	(1 << 5)

struct iommu_ops;
struct iommu_group;
struct bus_type;
struct device;
struct iommu_domain;
struct iommu_domain_ops;
struct notifier_block;
struct iommu_sva;
struct iommu_fault_event;
struct iommu_dma_cookie;


#define IOMMU_FAULT_READ	0x0
#define IOMMU_FAULT_WRITE	0x1

typedef int (*iommu_fault_handler_t)(struct iommu_domain *,
			struct device *, unsigned long, int, void *);
typedef int (*iommu_dev_fault_handler_t)(struct iommu_fault *, void *);

struct iommu_domain_geometry {
	dma_addr_t aperture_start; 
	dma_addr_t aperture_end;   
	bool force_aperture;       
};


#define __IOMMU_DOMAIN_PAGING	(1U << 0)  
#define __IOMMU_DOMAIN_DMA_API	(1U << 1)  
#define __IOMMU_DOMAIN_PT	(1U << 2)  
#define __IOMMU_DOMAIN_DMA_FQ	(1U << 3)  

#define __IOMMU_DOMAIN_SVA	(1U << 4)  

#define IOMMU_DOMAIN_ALLOC_FLAGS ~__IOMMU_DOMAIN_DMA_FQ

#define IOMMU_DOMAIN_BLOCKED	(0U)
#define IOMMU_DOMAIN_IDENTITY	(__IOMMU_DOMAIN_PT)
#define IOMMU_DOMAIN_UNMANAGED	(__IOMMU_DOMAIN_PAGING)
#define IOMMU_DOMAIN_DMA	(__IOMMU_DOMAIN_PAGING |	\
				 __IOMMU_DOMAIN_DMA_API)
#define IOMMU_DOMAIN_DMA_FQ	(__IOMMU_DOMAIN_PAGING |	\
				 __IOMMU_DOMAIN_DMA_API |	\
				 __IOMMU_DOMAIN_DMA_FQ)
#define IOMMU_DOMAIN_SVA	(__IOMMU_DOMAIN_SVA)

struct iommu_domain {
	unsigned type;
	const struct iommu_domain_ops *ops;
	unsigned long pgsize_bitmap;	
	struct iommu_domain_geometry geometry;
	struct iommu_dma_cookie *iova_cookie;
	enum iommu_page_response_code (*iopf_handler)(struct iommu_fault *fault,
						      void *data);
	void *fault_data;
	union {
		struct {
			iommu_fault_handler_t handler;
			void *handler_token;
		};
		struct {	
			struct mm_struct *mm;
			int users;
		};
	};
};

static inline bool iommu_is_dma_domain(struct iommu_domain *domain)
{
	return domain->type & __IOMMU_DOMAIN_DMA_API;
}

enum iommu_cap {
	IOMMU_CAP_CACHE_COHERENCY,	
	IOMMU_CAP_NOEXEC,		
	IOMMU_CAP_PRE_BOOT_PROTECTION,	
	
	IOMMU_CAP_ENFORCE_CACHE_COHERENCY,
	
	IOMMU_CAP_DEFERRED_FLUSH,
};


enum iommu_resv_type {
	
	IOMMU_RESV_DIRECT,
	
	IOMMU_RESV_DIRECT_RELAXABLE,
	
	IOMMU_RESV_RESERVED,
	
	IOMMU_RESV_MSI,
	
	IOMMU_RESV_SW_MSI,
};


struct iommu_resv_region {
	struct list_head	list;
	phys_addr_t		start;
	size_t			length;
	int			prot;
	enum iommu_resv_type	type;
	void (*free)(struct device *dev, struct iommu_resv_region *region);
};

struct iommu_iort_rmr_data {
	struct iommu_resv_region rr;

	
	const u32 *sids;
	u32 num_sids;
};


enum iommu_dev_features {
	IOMMU_DEV_FEAT_SVA,
	IOMMU_DEV_FEAT_IOPF,
};

#define IOMMU_NO_PASID	(0U) 
#define IOMMU_FIRST_GLOBAL_PASID	(1U) 
#define IOMMU_PASID_INVALID	(-1U)
typedef unsigned int ioasid_t;

#ifdef CONFIG_IOMMU_API


struct iommu_iotlb_gather {
	unsigned long		start;
	unsigned long		end;
	size_t			pgsize;
	struct list_head	freelist;
	bool			queued;
};


struct iommu_ops {
	bool (*capable)(struct device *dev, enum iommu_cap);
	void *(*hw_info)(struct device *dev, u32 *length, u32 *type);

	
	struct iommu_domain *(*domain_alloc)(unsigned iommu_domain_type);

	struct iommu_device *(*probe_device)(struct device *dev);
	void (*release_device)(struct device *dev);
	void (*probe_finalize)(struct device *dev);
	void (*set_platform_dma_ops)(struct device *dev);
	struct iommu_group *(*device_group)(struct device *dev);

	
	void (*get_resv_regions)(struct device *dev, struct list_head *list);

	int (*of_xlate)(struct device *dev, struct of_phandle_args *args);
	bool (*is_attach_deferred)(struct device *dev);

	
	int (*dev_enable_feat)(struct device *dev, enum iommu_dev_features f);
	int (*dev_disable_feat)(struct device *dev, enum iommu_dev_features f);

	int (*page_response)(struct device *dev,
			     struct iommu_fault_event *evt,
			     struct iommu_page_response *msg);

	int (*def_domain_type)(struct device *dev);
	void (*remove_dev_pasid)(struct device *dev, ioasid_t pasid);

	const struct iommu_domain_ops *default_domain_ops;
	unsigned long pgsize_bitmap;
	struct module *owner;
};


struct iommu_domain_ops {
	int (*attach_dev)(struct iommu_domain *domain, struct device *dev);
	int (*set_dev_pasid)(struct iommu_domain *domain, struct device *dev,
			     ioasid_t pasid);

	int (*map)(struct iommu_domain *domain, unsigned long iova,
		   phys_addr_t paddr, size_t size, int prot, gfp_t gfp);
	int (*map_pages)(struct iommu_domain *domain, unsigned long iova,
			 phys_addr_t paddr, size_t pgsize, size_t pgcount,
			 int prot, gfp_t gfp, size_t *mapped);
	size_t (*unmap)(struct iommu_domain *domain, unsigned long iova,
			size_t size, struct iommu_iotlb_gather *iotlb_gather);
	size_t (*unmap_pages)(struct iommu_domain *domain, unsigned long iova,
			      size_t pgsize, size_t pgcount,
			      struct iommu_iotlb_gather *iotlb_gather);

	void (*flush_iotlb_all)(struct iommu_domain *domain);
	void (*iotlb_sync_map)(struct iommu_domain *domain, unsigned long iova,
			       size_t size);
	void (*iotlb_sync)(struct iommu_domain *domain,
			   struct iommu_iotlb_gather *iotlb_gather);

	phys_addr_t (*iova_to_phys)(struct iommu_domain *domain,
				    dma_addr_t iova);

	bool (*enforce_cache_coherency)(struct iommu_domain *domain);
	int (*enable_nesting)(struct iommu_domain *domain);
	int (*set_pgtable_quirks)(struct iommu_domain *domain,
				  unsigned long quirks);

	void (*free)(struct iommu_domain *domain);
};


struct iommu_device {
	struct list_head list;
	const struct iommu_ops *ops;
	struct fwnode_handle *fwnode;
	struct device *dev;
	u32 max_pasids;
};


struct iommu_fault_event {
	struct iommu_fault fault;
	struct list_head list;
};


struct iommu_fault_param {
	iommu_dev_fault_handler_t handler;
	void *data;
	struct list_head faults;
	struct mutex lock;
};


struct dev_iommu {
	struct mutex lock;
	struct iommu_fault_param	*fault_param;
	struct iopf_device_param	*iopf_param;
	struct iommu_fwspec		*fwspec;
	struct iommu_device		*iommu_dev;
	void				*priv;
	u32				max_pasids;
	u32				attach_deferred:1;
	u32				pci_32bit_workaround:1;
	u32				require_direct:1;
};

int iommu_device_register(struct iommu_device *iommu,
			  const struct iommu_ops *ops,
			  struct device *hwdev);
void iommu_device_unregister(struct iommu_device *iommu);
int  iommu_device_sysfs_add(struct iommu_device *iommu,
			    struct device *parent,
			    const struct attribute_group **groups,
			    const char *fmt, ...) __printf(4, 5);
void iommu_device_sysfs_remove(struct iommu_device *iommu);
int  iommu_device_link(struct iommu_device   *iommu, struct device *link);
void iommu_device_unlink(struct iommu_device *iommu, struct device *link);
int iommu_deferred_attach(struct device *dev, struct iommu_domain *domain);

static inline struct iommu_device *dev_to_iommu_device(struct device *dev)
{
	return (struct iommu_device *)dev_get_drvdata(dev);
}

static inline void iommu_iotlb_gather_init(struct iommu_iotlb_gather *gather)
{
	*gather = (struct iommu_iotlb_gather) {
		.start	= ULONG_MAX,
		.freelist = LIST_HEAD_INIT(gather->freelist),
	};
}

extern int bus_iommu_probe(const struct bus_type *bus);
extern bool iommu_present(const struct bus_type *bus);
extern bool device_iommu_capable(struct device *dev, enum iommu_cap cap);
extern bool iommu_group_has_isolated_msi(struct iommu_group *group);
extern struct iommu_domain *iommu_domain_alloc(const struct bus_type *bus);
extern void iommu_domain_free(struct iommu_domain *domain);
extern int iommu_attach_device(struct iommu_domain *domain,
			       struct device *dev);
extern void iommu_detach_device(struct iommu_domain *domain,
				struct device *dev);
extern int iommu_sva_unbind_gpasid(struct iommu_domain *domain,
				   struct device *dev, ioasid_t pasid);
extern struct iommu_domain *iommu_get_domain_for_dev(struct device *dev);
extern struct iommu_domain *iommu_get_dma_domain(struct device *dev);
extern int iommu_map(struct iommu_domain *domain, unsigned long iova,
		     phys_addr_t paddr, size_t size, int prot, gfp_t gfp);
extern size_t iommu_unmap(struct iommu_domain *domain, unsigned long iova,
			  size_t size);
extern size_t iommu_unmap_fast(struct iommu_domain *domain,
			       unsigned long iova, size_t size,
			       struct iommu_iotlb_gather *iotlb_gather);
extern ssize_t iommu_map_sg(struct iommu_domain *domain, unsigned long iova,
			    struct scatterlist *sg, unsigned int nents,
			    int prot, gfp_t gfp);
extern phys_addr_t iommu_iova_to_phys(struct iommu_domain *domain, dma_addr_t iova);
extern void iommu_set_fault_handler(struct iommu_domain *domain,
			iommu_fault_handler_t handler, void *token);

extern void iommu_get_resv_regions(struct device *dev, struct list_head *list);
extern void iommu_put_resv_regions(struct device *dev, struct list_head *list);
extern void iommu_set_default_passthrough(bool cmd_line);
extern void iommu_set_default_translated(bool cmd_line);
extern bool iommu_default_passthrough(void);
extern struct iommu_resv_region *
iommu_alloc_resv_region(phys_addr_t start, size_t length, int prot,
			enum iommu_resv_type type, gfp_t gfp);
extern int iommu_get_group_resv_regions(struct iommu_group *group,
					struct list_head *head);

extern int iommu_attach_group(struct iommu_domain *domain,
			      struct iommu_group *group);
extern void iommu_detach_group(struct iommu_domain *domain,
			       struct iommu_group *group);
extern struct iommu_group *iommu_group_alloc(void);
extern void *iommu_group_get_iommudata(struct iommu_group *group);
extern void iommu_group_set_iommudata(struct iommu_group *group,
				      void *iommu_data,
				      void (*release)(void *iommu_data));
extern int iommu_group_set_name(struct iommu_group *group, const char *name);
extern int iommu_group_add_device(struct iommu_group *group,
				  struct device *dev);
extern void iommu_group_remove_device(struct device *dev);
extern int iommu_group_for_each_dev(struct iommu_group *group, void *data,
				    int (*fn)(struct device *, void *));
extern struct iommu_group *iommu_group_get(struct device *dev);
extern struct iommu_group *iommu_group_ref_get(struct iommu_group *group);
extern void iommu_group_put(struct iommu_group *group);
extern int iommu_register_device_fault_handler(struct device *dev,
					iommu_dev_fault_handler_t handler,
					void *data);

extern int iommu_unregister_device_fault_handler(struct device *dev);

extern int iommu_report_device_fault(struct device *dev,
				     struct iommu_fault_event *evt);
extern int iommu_page_response(struct device *dev,
			       struct iommu_page_response *msg);

extern int iommu_group_id(struct iommu_group *group);
extern struct iommu_domain *iommu_group_default_domain(struct iommu_group *);

int iommu_enable_nesting(struct iommu_domain *domain);
int iommu_set_pgtable_quirks(struct iommu_domain *domain,
		unsigned long quirks);

void iommu_set_dma_strict(void);

extern int report_iommu_fault(struct iommu_domain *domain, struct device *dev,
			      unsigned long iova, int flags);

static inline void iommu_flush_iotlb_all(struct iommu_domain *domain)
{
	if (domain->ops->flush_iotlb_all)
		domain->ops->flush_iotlb_all(domain);
}

static inline void iommu_iotlb_sync(struct iommu_domain *domain,
				  struct iommu_iotlb_gather *iotlb_gather)
{
	if (domain->ops->iotlb_sync)
		domain->ops->iotlb_sync(domain, iotlb_gather);

	iommu_iotlb_gather_init(iotlb_gather);
}


static inline
bool iommu_iotlb_gather_is_disjoint(struct iommu_iotlb_gather *gather,
				    unsigned long iova, size_t size)
{
	unsigned long start = iova, end = start + size - 1;

	return gather->end != 0 &&
		(end + 1 < gather->start || start > gather->end + 1);
}



static inline void iommu_iotlb_gather_add_range(struct iommu_iotlb_gather *gather,
						unsigned long iova, size_t size)
{
	unsigned long end = iova + size - 1;

	if (gather->start > iova)
		gather->start = iova;
	if (gather->end < end)
		gather->end = end;
}


static inline void iommu_iotlb_gather_add_page(struct iommu_domain *domain,
					       struct iommu_iotlb_gather *gather,
					       unsigned long iova, size_t size)
{
	
	if ((gather->pgsize && gather->pgsize != size) ||
	    iommu_iotlb_gather_is_disjoint(gather, iova, size))
		iommu_iotlb_sync(domain, gather);

	gather->pgsize = size;
	iommu_iotlb_gather_add_range(gather, iova, size);
}

static inline bool iommu_iotlb_gather_queued(struct iommu_iotlb_gather *gather)
{
	return gather && gather->queued;
}


extern struct iommu_group *pci_device_group(struct device *dev);

extern struct iommu_group *generic_device_group(struct device *dev);

struct iommu_group *fsl_mc_device_group(struct device *dev);


struct iommu_fwspec {
	const struct iommu_ops	*ops;
	struct fwnode_handle	*iommu_fwnode;
	u32			flags;
	unsigned int		num_ids;
	u32			ids[];
};


#define IOMMU_FWSPEC_PCI_RC_ATS			(1 << 0)


struct iommu_sva {
	struct device			*dev;
	struct iommu_domain		*domain;
};

int iommu_fwspec_init(struct device *dev, struct fwnode_handle *iommu_fwnode,
		      const struct iommu_ops *ops);
void iommu_fwspec_free(struct device *dev);
int iommu_fwspec_add_ids(struct device *dev, u32 *ids, int num_ids);
const struct iommu_ops *iommu_ops_from_fwnode(struct fwnode_handle *fwnode);

static inline struct iommu_fwspec *dev_iommu_fwspec_get(struct device *dev)
{
	if (dev->iommu)
		return dev->iommu->fwspec;
	else
		return NULL;
}

static inline void dev_iommu_fwspec_set(struct device *dev,
					struct iommu_fwspec *fwspec)
{
	dev->iommu->fwspec = fwspec;
}

static inline void *dev_iommu_priv_get(struct device *dev)
{
	if (dev->iommu)
		return dev->iommu->priv;
	else
		return NULL;
}

static inline void dev_iommu_priv_set(struct device *dev, void *priv)
{
	dev->iommu->priv = priv;
}

extern struct mutex iommu_probe_device_lock;
int iommu_probe_device(struct device *dev);

int iommu_dev_enable_feature(struct device *dev, enum iommu_dev_features f);
int iommu_dev_disable_feature(struct device *dev, enum iommu_dev_features f);

int iommu_device_use_default_domain(struct device *dev);
void iommu_device_unuse_default_domain(struct device *dev);

int iommu_group_claim_dma_owner(struct iommu_group *group, void *owner);
void iommu_group_release_dma_owner(struct iommu_group *group);
bool iommu_group_dma_owner_claimed(struct iommu_group *group);

int iommu_device_claim_dma_owner(struct device *dev, void *owner);
void iommu_device_release_dma_owner(struct device *dev);

struct iommu_domain *iommu_sva_domain_alloc(struct device *dev,
					    struct mm_struct *mm);
int iommu_attach_device_pasid(struct iommu_domain *domain,
			      struct device *dev, ioasid_t pasid);
void iommu_detach_device_pasid(struct iommu_domain *domain,
			       struct device *dev, ioasid_t pasid);
struct iommu_domain *
iommu_get_domain_for_dev_pasid(struct device *dev, ioasid_t pasid,
			       unsigned int type);
ioasid_t iommu_alloc_global_pasid(struct device *dev);
void iommu_free_global_pasid(ioasid_t pasid);
#else 

struct iommu_ops {};
struct iommu_group {};
struct iommu_fwspec {};
struct iommu_device {};
struct iommu_fault_param {};
struct iommu_iotlb_gather {};

static inline bool iommu_present(const struct bus_type *bus)
{
	return false;
}

static inline bool device_iommu_capable(struct device *dev, enum iommu_cap cap)
{
	return false;
}

static inline struct iommu_domain *iommu_domain_alloc(const struct bus_type *bus)
{
	return NULL;
}

static inline void iommu_domain_free(struct iommu_domain *domain)
{
}

static inline int iommu_attach_device(struct iommu_domain *domain,
				      struct device *dev)
{
	return -ENODEV;
}

static inline void iommu_detach_device(struct iommu_domain *domain,
				       struct device *dev)
{
}

static inline struct iommu_domain *iommu_get_domain_for_dev(struct device *dev)
{
	return NULL;
}

static inline int iommu_map(struct iommu_domain *domain, unsigned long iova,
			    phys_addr_t paddr, size_t size, int prot, gfp_t gfp)
{
	return -ENODEV;
}

static inline size_t iommu_unmap(struct iommu_domain *domain,
				 unsigned long iova, size_t size)
{
	return 0;
}

static inline size_t iommu_unmap_fast(struct iommu_domain *domain,
				      unsigned long iova, int gfp_order,
				      struct iommu_iotlb_gather *iotlb_gather)
{
	return 0;
}

static inline ssize_t iommu_map_sg(struct iommu_domain *domain,
				   unsigned long iova, struct scatterlist *sg,
				   unsigned int nents, int prot, gfp_t gfp)
{
	return -ENODEV;
}

static inline void iommu_flush_iotlb_all(struct iommu_domain *domain)
{
}

static inline void iommu_iotlb_sync(struct iommu_domain *domain,
				  struct iommu_iotlb_gather *iotlb_gather)
{
}

static inline phys_addr_t iommu_iova_to_phys(struct iommu_domain *domain, dma_addr_t iova)
{
	return 0;
}

static inline void iommu_set_fault_handler(struct iommu_domain *domain,
				iommu_fault_handler_t handler, void *token)
{
}

static inline void iommu_get_resv_regions(struct device *dev,
					struct list_head *list)
{
}

static inline void iommu_put_resv_regions(struct device *dev,
					struct list_head *list)
{
}

static inline int iommu_get_group_resv_regions(struct iommu_group *group,
					       struct list_head *head)
{
	return -ENODEV;
}

static inline void iommu_set_default_passthrough(bool cmd_line)
{
}

static inline void iommu_set_default_translated(bool cmd_line)
{
}

static inline bool iommu_default_passthrough(void)
{
	return true;
}

static inline int iommu_attach_group(struct iommu_domain *domain,
				     struct iommu_group *group)
{
	return -ENODEV;
}

static inline void iommu_detach_group(struct iommu_domain *domain,
				      struct iommu_group *group)
{
}

static inline struct iommu_group *iommu_group_alloc(void)
{
	return ERR_PTR(-ENODEV);
}

static inline void *iommu_group_get_iommudata(struct iommu_group *group)
{
	return NULL;
}

static inline void iommu_group_set_iommudata(struct iommu_group *group,
					     void *iommu_data,
					     void (*release)(void *iommu_data))
{
}

static inline int iommu_group_set_name(struct iommu_group *group,
				       const char *name)
{
	return -ENODEV;
}

static inline int iommu_group_add_device(struct iommu_group *group,
					 struct device *dev)
{
	return -ENODEV;
}

static inline void iommu_group_remove_device(struct device *dev)
{
}

static inline int iommu_group_for_each_dev(struct iommu_group *group,
					   void *data,
					   int (*fn)(struct device *, void *))
{
	return -ENODEV;
}

static inline struct iommu_group *iommu_group_get(struct device *dev)
{
	return NULL;
}

static inline void iommu_group_put(struct iommu_group *group)
{
}

static inline
int iommu_register_device_fault_handler(struct device *dev,
					iommu_dev_fault_handler_t handler,
					void *data)
{
	return -ENODEV;
}

static inline int iommu_unregister_device_fault_handler(struct device *dev)
{
	return 0;
}

static inline
int iommu_report_device_fault(struct device *dev, struct iommu_fault_event *evt)
{
	return -ENODEV;
}

static inline int iommu_page_response(struct device *dev,
				      struct iommu_page_response *msg)
{
	return -ENODEV;
}

static inline int iommu_group_id(struct iommu_group *group)
{
	return -ENODEV;
}

static inline int iommu_set_pgtable_quirks(struct iommu_domain *domain,
		unsigned long quirks)
{
	return 0;
}

static inline int iommu_device_register(struct iommu_device *iommu,
					const struct iommu_ops *ops,
					struct device *hwdev)
{
	return -ENODEV;
}

static inline struct iommu_device *dev_to_iommu_device(struct device *dev)
{
	return NULL;
}

static inline void iommu_iotlb_gather_init(struct iommu_iotlb_gather *gather)
{
}

static inline void iommu_iotlb_gather_add_page(struct iommu_domain *domain,
					       struct iommu_iotlb_gather *gather,
					       unsigned long iova, size_t size)
{
}

static inline bool iommu_iotlb_gather_queued(struct iommu_iotlb_gather *gather)
{
	return false;
}

static inline void iommu_device_unregister(struct iommu_device *iommu)
{
}

static inline int  iommu_device_sysfs_add(struct iommu_device *iommu,
					  struct device *parent,
					  const struct attribute_group **groups,
					  const char *fmt, ...)
{
	return -ENODEV;
}

static inline void iommu_device_sysfs_remove(struct iommu_device *iommu)
{
}

static inline int iommu_device_link(struct device *dev, struct device *link)
{
	return -EINVAL;
}

static inline void iommu_device_unlink(struct device *dev, struct device *link)
{
}

static inline int iommu_fwspec_init(struct device *dev,
				    struct fwnode_handle *iommu_fwnode,
				    const struct iommu_ops *ops)
{
	return -ENODEV;
}

static inline void iommu_fwspec_free(struct device *dev)
{
}

static inline int iommu_fwspec_add_ids(struct device *dev, u32 *ids,
				       int num_ids)
{
	return -ENODEV;
}

static inline
const struct iommu_ops *iommu_ops_from_fwnode(struct fwnode_handle *fwnode)
{
	return NULL;
}

static inline int
iommu_dev_enable_feature(struct device *dev, enum iommu_dev_features feat)
{
	return -ENODEV;
}

static inline int
iommu_dev_disable_feature(struct device *dev, enum iommu_dev_features feat)
{
	return -ENODEV;
}

static inline struct iommu_fwspec *dev_iommu_fwspec_get(struct device *dev)
{
	return NULL;
}

static inline int iommu_device_use_default_domain(struct device *dev)
{
	return 0;
}

static inline void iommu_device_unuse_default_domain(struct device *dev)
{
}

static inline int
iommu_group_claim_dma_owner(struct iommu_group *group, void *owner)
{
	return -ENODEV;
}

static inline void iommu_group_release_dma_owner(struct iommu_group *group)
{
}

static inline bool iommu_group_dma_owner_claimed(struct iommu_group *group)
{
	return false;
}

static inline void iommu_device_release_dma_owner(struct device *dev)
{
}

static inline int iommu_device_claim_dma_owner(struct device *dev, void *owner)
{
	return -ENODEV;
}

static inline struct iommu_domain *
iommu_sva_domain_alloc(struct device *dev, struct mm_struct *mm)
{
	return NULL;
}

static inline int iommu_attach_device_pasid(struct iommu_domain *domain,
					    struct device *dev, ioasid_t pasid)
{
	return -ENODEV;
}

static inline void iommu_detach_device_pasid(struct iommu_domain *domain,
					     struct device *dev, ioasid_t pasid)
{
}

static inline struct iommu_domain *
iommu_get_domain_for_dev_pasid(struct device *dev, ioasid_t pasid,
			       unsigned int type)
{
	return NULL;
}

static inline ioasid_t iommu_alloc_global_pasid(struct device *dev)
{
	return IOMMU_PASID_INVALID;
}

static inline void iommu_free_global_pasid(ioasid_t pasid) {}
#endif 


static inline size_t iommu_map_sgtable(struct iommu_domain *domain,
			unsigned long iova, struct sg_table *sgt, int prot)
{
	return iommu_map_sg(domain, iova, sgt->sgl, sgt->orig_nents, prot,
			    GFP_KERNEL);
}

#ifdef CONFIG_IOMMU_DEBUGFS
extern	struct dentry *iommu_debugfs_dir;
void iommu_debugfs_setup(void);
#else
static inline void iommu_debugfs_setup(void) {}
#endif

#ifdef CONFIG_IOMMU_DMA
#include <linux/msi.h>


void iommu_setup_dma_ops(struct device *dev, u64 dma_base, u64 dma_limit);

int iommu_get_msi_cookie(struct iommu_domain *domain, dma_addr_t base);

int iommu_dma_prepare_msi(struct msi_desc *desc, phys_addr_t msi_addr);
void iommu_dma_compose_msi_msg(struct msi_desc *desc, struct msi_msg *msg);

#else 

struct msi_desc;
struct msi_msg;

static inline void iommu_setup_dma_ops(struct device *dev, u64 dma_base, u64 dma_limit)
{
}

static inline int iommu_get_msi_cookie(struct iommu_domain *domain, dma_addr_t base)
{
	return -ENODEV;
}

static inline int iommu_dma_prepare_msi(struct msi_desc *desc, phys_addr_t msi_addr)
{
	return 0;
}

static inline void iommu_dma_compose_msi_msg(struct msi_desc *desc, struct msi_msg *msg)
{
}

#endif	


#define TEGRA_STREAM_ID_BYPASS 0x7f

static inline bool tegra_dev_iommu_get_stream_id(struct device *dev, u32 *stream_id)
{
#ifdef CONFIG_IOMMU_API
	struct iommu_fwspec *fwspec = dev_iommu_fwspec_get(dev);

	if (fwspec && fwspec->num_ids == 1) {
		*stream_id = fwspec->ids[0] & 0xffff;
		return true;
	}
#endif

	return false;
}

#ifdef CONFIG_IOMMU_SVA
static inline void mm_pasid_init(struct mm_struct *mm)
{
	mm->pasid = IOMMU_PASID_INVALID;
}
static inline bool mm_valid_pasid(struct mm_struct *mm)
{
	return mm->pasid != IOMMU_PASID_INVALID;
}
void mm_pasid_drop(struct mm_struct *mm);
struct iommu_sva *iommu_sva_bind_device(struct device *dev,
					struct mm_struct *mm);
void iommu_sva_unbind_device(struct iommu_sva *handle);
u32 iommu_sva_get_pasid(struct iommu_sva *handle);
#else
static inline struct iommu_sva *
iommu_sva_bind_device(struct device *dev, struct mm_struct *mm)
{
	return ERR_PTR(-ENODEV);
}

static inline void iommu_sva_unbind_device(struct iommu_sva *handle)
{
}

static inline u32 iommu_sva_get_pasid(struct iommu_sva *handle)
{
	return IOMMU_PASID_INVALID;
}
static inline void mm_pasid_init(struct mm_struct *mm) {}
static inline bool mm_valid_pasid(struct mm_struct *mm) { return false; }
static inline void mm_pasid_drop(struct mm_struct *mm) {}
#endif 

#endif 
