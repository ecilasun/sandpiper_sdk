/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_MSI_H
#define LINUX_MSI_H



#include <linux/irqdomain_defs.h>
#include <linux/cpumask.h>
#include <linux/msi_api.h>
#include <linux/xarray.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/irq.h>
#include <linux/bits.h>

#include <asm/msi.h>


#ifndef arch_msi_msg_addr_lo
typedef struct arch_msi_msg_addr_lo {
	u32	address_lo;
} __attribute__ ((packed)) arch_msi_msg_addr_lo_t;
#endif

#ifndef arch_msi_msg_addr_hi
typedef struct arch_msi_msg_addr_hi {
	u32	address_hi;
} __attribute__ ((packed)) arch_msi_msg_addr_hi_t;
#endif

#ifndef arch_msi_msg_data
typedef struct arch_msi_msg_data {
	u32	data;
} __attribute__ ((packed)) arch_msi_msg_data_t;
#endif

#ifndef arch_is_isolated_msi
#define arch_is_isolated_msi() false
#endif


struct msi_msg {
	union {
		u32			address_lo;
		arch_msi_msg_addr_lo_t	arch_addr_lo;
	};
	union {
		u32			address_hi;
		arch_msi_msg_addr_hi_t	arch_addr_hi;
	};
	union {
		u32			data;
		arch_msi_msg_data_t	arch_data;
	};
};

extern int pci_msi_ignore_mask;

struct msi_desc;
struct pci_dev;
struct platform_msi_priv_data;
struct device_attribute;
struct irq_domain;
struct irq_affinity_desc;

void __get_cached_msi_msg(struct msi_desc *entry, struct msi_msg *msg);
#ifdef CONFIG_GENERIC_MSI_IRQ
void get_cached_msi_msg(unsigned int irq, struct msi_msg *msg);
#else
static inline void get_cached_msi_msg(unsigned int irq, struct msi_msg *msg) { }
#endif

typedef void (*irq_write_msi_msg_t)(struct msi_desc *desc,
				    struct msi_msg *msg);


struct pci_msi_desc {
	union {
		u32 msi_mask;
		u32 msix_ctrl;
	};
	struct {
		u8	is_msix		: 1;
		u8	multiple	: 3;
		u8	multi_cap	: 3;
		u8	can_mask	: 1;
		u8	is_64		: 1;
		u8	is_virtual	: 1;
		unsigned default_irq;
	} msi_attrib;
	union {
		u8	mask_pos;
		void __iomem *mask_base;
	};
};


union msi_domain_cookie {
	u64	value;
	void	*ptr;
	void	__iomem *iobase;
};


struct msi_desc_data {
	union msi_domain_cookie		dcookie;
	union msi_instance_cookie	icookie;
};

#define MSI_MAX_INDEX		((unsigned int)USHRT_MAX)


struct msi_desc {
	
	unsigned int			irq;
	unsigned int			nvec_used;
	struct device			*dev;
	struct msi_msg			msg;
	struct irq_affinity_desc	*affinity;
#ifdef CONFIG_IRQ_MSI_IOMMU
	const void			*iommu_cookie;
#endif
#ifdef CONFIG_SYSFS
	struct device_attribute		*sysfs_attrs;
#endif

	void (*write_msi_msg)(struct msi_desc *entry, void *data);
	void *write_msi_msg_data;

	u16				msi_index;
	union {
		struct pci_msi_desc	pci;
		struct msi_desc_data	data;
	};
};


enum msi_desc_filter {
	
	MSI_DESC_ALL,
	
	MSI_DESC_NOTASSOCIATED,
	
	MSI_DESC_ASSOCIATED,
};



struct msi_dev_domain {
	struct xarray		store;
	struct irq_domain	*domain;
};


struct msi_device_data {
	unsigned long			properties;
	struct platform_msi_priv_data	*platform_data;
	struct mutex			mutex;
	struct msi_dev_domain		__domains[MSI_MAX_DEVICE_IRQDOMAINS];
	unsigned long			__iter_idx;
};

int msi_setup_device_data(struct device *dev);

void msi_lock_descs(struct device *dev);
void msi_unlock_descs(struct device *dev);

struct msi_desc *msi_domain_first_desc(struct device *dev, unsigned int domid,
				       enum msi_desc_filter filter);


static inline struct msi_desc *msi_first_desc(struct device *dev,
					      enum msi_desc_filter filter)
{
	return msi_domain_first_desc(dev, MSI_DEFAULT_DOMAIN, filter);
}

struct msi_desc *msi_next_desc(struct device *dev, unsigned int domid,
			       enum msi_desc_filter filter);


#define msi_domain_for_each_desc(desc, dev, domid, filter)			\
	for ((desc) = msi_domain_first_desc((dev), (domid), (filter)); (desc);	\
	     (desc) = msi_next_desc((dev), (domid), (filter)))


#define msi_for_each_desc(desc, dev, filter)					\
	msi_domain_for_each_desc((desc), (dev), MSI_DEFAULT_DOMAIN, (filter))

#define msi_desc_to_dev(desc)		((desc)->dev)

#ifdef CONFIG_IRQ_MSI_IOMMU
static inline const void *msi_desc_get_iommu_cookie(struct msi_desc *desc)
{
	return desc->iommu_cookie;
}

static inline void msi_desc_set_iommu_cookie(struct msi_desc *desc,
					     const void *iommu_cookie)
{
	desc->iommu_cookie = iommu_cookie;
}
#else
static inline const void *msi_desc_get_iommu_cookie(struct msi_desc *desc)
{
	return NULL;
}

static inline void msi_desc_set_iommu_cookie(struct msi_desc *desc,
					     const void *iommu_cookie)
{
}
#endif

int msi_domain_insert_msi_desc(struct device *dev, unsigned int domid,
			       struct msi_desc *init_desc);

static inline int msi_insert_msi_desc(struct device *dev, struct msi_desc *init_desc)
{
	return msi_domain_insert_msi_desc(dev, MSI_DEFAULT_DOMAIN, init_desc);
}

void msi_domain_free_msi_descs_range(struct device *dev, unsigned int domid,
				     unsigned int first, unsigned int last);


static inline void msi_free_msi_descs_range(struct device *dev, unsigned int first,
					    unsigned int last)
{
	msi_domain_free_msi_descs_range(dev, MSI_DEFAULT_DOMAIN, first, last);
}


static inline void msi_free_msi_descs(struct device *dev)
{
	msi_free_msi_descs_range(dev, 0, MSI_MAX_INDEX);
}


#ifdef CONFIG_PCI_MSI_ARCH_FALLBACKS
int arch_setup_msi_irq(struct pci_dev *dev, struct msi_desc *desc);
void arch_teardown_msi_irq(unsigned int irq);
int arch_setup_msi_irqs(struct pci_dev *dev, int nvec, int type);
void arch_teardown_msi_irqs(struct pci_dev *dev);
#endif 


#if defined(CONFIG_PCI_XEN) || defined(CONFIG_PCI_MSI_ARCH_FALLBACKS)
#ifdef CONFIG_SYSFS
int msi_device_populate_sysfs(struct device *dev);
void msi_device_destroy_sysfs(struct device *dev);
#else 
static inline int msi_device_populate_sysfs(struct device *dev) { return 0; }
static inline void msi_device_destroy_sysfs(struct device *dev) { }
#endif 
#endif 


bool arch_restore_msi_irqs(struct pci_dev *dev);

#ifdef CONFIG_GENERIC_MSI_IRQ

#include <linux/irqhandler.h>

struct irq_domain;
struct irq_domain_ops;
struct irq_chip;
struct device_node;
struct fwnode_handle;
struct msi_domain_info;


struct msi_domain_ops {
	irq_hw_number_t	(*get_hwirq)(struct msi_domain_info *info,
				     msi_alloc_info_t *arg);
	int		(*msi_init)(struct irq_domain *domain,
				    struct msi_domain_info *info,
				    unsigned int virq, irq_hw_number_t hwirq,
				    msi_alloc_info_t *arg);
	void		(*msi_free)(struct irq_domain *domain,
				    struct msi_domain_info *info,
				    unsigned int virq);
	int		(*msi_prepare)(struct irq_domain *domain,
				       struct device *dev, int nvec,
				       msi_alloc_info_t *arg);
	void		(*prepare_desc)(struct irq_domain *domain, msi_alloc_info_t *arg,
					struct msi_desc *desc);
	void		(*set_desc)(msi_alloc_info_t *arg,
				    struct msi_desc *desc);
	int		(*domain_alloc_irqs)(struct irq_domain *domain,
					     struct device *dev, int nvec);
	void		(*domain_free_irqs)(struct irq_domain *domain,
					    struct device *dev);
	void		(*msi_post_free)(struct irq_domain *domain,
					 struct device *dev);
};


struct msi_domain_info {
	u32				flags;
	enum irq_domain_bus_token	bus_token;
	unsigned int			hwsize;
	struct msi_domain_ops		*ops;
	struct irq_chip			*chip;
	void				*chip_data;
	irq_flow_handler_t		handler;
	void				*handler_data;
	const char			*handler_name;
	void				*data;
};


struct msi_domain_template {
	char			name[48];
	struct irq_chip		chip;
	struct msi_domain_ops	ops;
	struct msi_domain_info	info;
};


enum {
	
	MSI_FLAG_USE_DEF_DOM_OPS	= (1 << 0),
	
	MSI_FLAG_USE_DEF_CHIP_OPS	= (1 << 1),
	
	MSI_FLAG_ACTIVATE_EARLY		= (1 << 2),
	
	MSI_FLAG_MUST_REACTIVATE	= (1 << 3),
	
	MSI_FLAG_DEV_SYSFS		= (1 << 4),
	
	MSI_FLAG_ALLOC_SIMPLE_MSI_DESCS	= (1 << 5),
	
	MSI_FLAG_FREE_MSI_DESCS		= (1 << 6),

	
	MSI_GENERIC_FLAGS_MASK		= GENMASK(15, 0),

	
	MSI_DOMAIN_FLAGS_MASK		= GENMASK(31, 16),

	
	MSI_FLAG_MULTI_PCI_MSI		= (1 << 16),
	
	MSI_FLAG_PCI_MSIX		= (1 << 17),
	
	MSI_FLAG_LEVEL_CAPABLE		= (1 << 18),
	
	MSI_FLAG_MSIX_CONTIGUOUS	= (1 << 19),
	
	MSI_FLAG_PCI_MSIX_ALLOC_DYN	= (1 << 20),
	
	MSI_FLAG_PCI_IMS		= (1 << 21),
};


struct msi_parent_ops {
	u32		supported_flags;
	const char	*prefix;
	bool		(*init_dev_msi_info)(struct device *dev, struct irq_domain *domain,
					     struct irq_domain *msi_parent_domain,
					     struct msi_domain_info *msi_child_info);
};

bool msi_parent_init_dev_msi_info(struct device *dev, struct irq_domain *domain,
				  struct irq_domain *msi_parent_domain,
				  struct msi_domain_info *msi_child_info);

int msi_domain_set_affinity(struct irq_data *data, const struct cpumask *mask,
			    bool force);

struct irq_domain *msi_create_irq_domain(struct fwnode_handle *fwnode,
					 struct msi_domain_info *info,
					 struct irq_domain *parent);

bool msi_create_device_irq_domain(struct device *dev, unsigned int domid,
				  const struct msi_domain_template *template,
				  unsigned int hwsize, void *domain_data,
				  void *chip_data);
void msi_remove_device_irq_domain(struct device *dev, unsigned int domid);

bool msi_match_device_irq_domain(struct device *dev, unsigned int domid,
				 enum irq_domain_bus_token bus_token);

int msi_domain_alloc_irqs_range_locked(struct device *dev, unsigned int domid,
				       unsigned int first, unsigned int last);
int msi_domain_alloc_irqs_range(struct device *dev, unsigned int domid,
				unsigned int first, unsigned int last);
int msi_domain_alloc_irqs_all_locked(struct device *dev, unsigned int domid, int nirqs);

struct msi_map msi_domain_alloc_irq_at(struct device *dev, unsigned int domid, unsigned int index,
				       const struct irq_affinity_desc *affdesc,
				       union msi_instance_cookie *cookie);

void msi_domain_free_irqs_range_locked(struct device *dev, unsigned int domid,
				       unsigned int first, unsigned int last);
void msi_domain_free_irqs_range(struct device *dev, unsigned int domid,
				unsigned int first, unsigned int last);
void msi_domain_free_irqs_all_locked(struct device *dev, unsigned int domid);
void msi_domain_free_irqs_all(struct device *dev, unsigned int domid);

struct msi_domain_info *msi_get_domain_info(struct irq_domain *domain);

struct irq_domain *platform_msi_create_irq_domain(struct fwnode_handle *fwnode,
						  struct msi_domain_info *info,
						  struct irq_domain *parent);
int platform_msi_domain_alloc_irqs(struct device *dev, unsigned int nvec,
				   irq_write_msi_msg_t write_msi_msg);
void platform_msi_domain_free_irqs(struct device *dev);


int msi_domain_prepare_irqs(struct irq_domain *domain, struct device *dev,
			    int nvec, msi_alloc_info_t *args);
int msi_domain_populate_irqs(struct irq_domain *domain, struct device *dev,
			     int virq, int nvec, msi_alloc_info_t *args);
void msi_domain_depopulate_descs(struct device *dev, int virq, int nvec);

struct irq_domain *
__platform_msi_create_device_domain(struct device *dev,
				    unsigned int nvec,
				    bool is_tree,
				    irq_write_msi_msg_t write_msi_msg,
				    const struct irq_domain_ops *ops,
				    void *host_data);

#define platform_msi_create_device_domain(dev, nvec, write, ops, data)	\
	__platform_msi_create_device_domain(dev, nvec, false, write, ops, data)
#define platform_msi_create_device_tree_domain(dev, nvec, write, ops, data) \
	__platform_msi_create_device_domain(dev, nvec, true, write, ops, data)

int platform_msi_device_domain_alloc(struct irq_domain *domain, unsigned int virq,
				     unsigned int nr_irqs);
void platform_msi_device_domain_free(struct irq_domain *domain, unsigned int virq,
				     unsigned int nvec);
void *platform_msi_get_host_data(struct irq_domain *domain);

bool msi_device_has_isolated_msi(struct device *dev);
#else 
static inline bool msi_device_has_isolated_msi(struct device *dev)
{
	
	return arch_is_isolated_msi();
}
#endif 


#ifdef CONFIG_PCI_MSI
struct pci_dev *msi_desc_to_pci_dev(struct msi_desc *desc);
void pci_write_msi_msg(unsigned int irq, struct msi_msg *msg);
void __pci_read_msi_msg(struct msi_desc *entry, struct msi_msg *msg);
void __pci_write_msi_msg(struct msi_desc *entry, struct msi_msg *msg);
void pci_msi_mask_irq(struct irq_data *data);
void pci_msi_unmask_irq(struct irq_data *data);
struct irq_domain *pci_msi_create_irq_domain(struct fwnode_handle *fwnode,
					     struct msi_domain_info *info,
					     struct irq_domain *parent);
u32 pci_msi_domain_get_msi_rid(struct irq_domain *domain, struct pci_dev *pdev);
struct irq_domain *pci_msi_get_device_domain(struct pci_dev *pdev);
#else 
static inline struct irq_domain *pci_msi_get_device_domain(struct pci_dev *pdev)
{
	return NULL;
}
static inline void pci_write_msi_msg(unsigned int irq, struct msi_msg *msg) { }
#endif 

#endif 
