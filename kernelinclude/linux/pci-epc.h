/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_PCI_EPC_H
#define __LINUX_PCI_EPC_H

#include <linux/pci-epf.h>

struct pci_epc;

enum pci_epc_interface_type {
	UNKNOWN_INTERFACE = -1,
	PRIMARY_INTERFACE,
	SECONDARY_INTERFACE,
};

enum pci_epc_irq_type {
	PCI_EPC_IRQ_UNKNOWN,
	PCI_EPC_IRQ_LEGACY,
	PCI_EPC_IRQ_MSI,
	PCI_EPC_IRQ_MSIX,
};

static inline const char *
pci_epc_interface_string(enum pci_epc_interface_type type)
{
	switch (type) {
	case PRIMARY_INTERFACE:
		return "primary";
	case SECONDARY_INTERFACE:
		return "secondary";
	default:
		return "UNKNOWN interface";
	}
}


struct pci_epc_ops {
	int	(*write_header)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
				struct pci_epf_header *hdr);
	int	(*set_bar)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			   struct pci_epf_bar *epf_bar);
	void	(*clear_bar)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			     struct pci_epf_bar *epf_bar);
	int	(*map_addr)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			    phys_addr_t addr, u64 pci_addr, size_t size);
	void	(*unmap_addr)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			      phys_addr_t addr);
	int	(*set_msi)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			   u8 interrupts);
	int	(*get_msi)(struct pci_epc *epc, u8 func_no, u8 vfunc_no);
	int	(*set_msix)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			    u16 interrupts, enum pci_barno, u32 offset);
	int	(*get_msix)(struct pci_epc *epc, u8 func_no, u8 vfunc_no);
	int	(*raise_irq)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			     enum pci_epc_irq_type type, u16 interrupt_num);
	int	(*map_msi_irq)(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			       phys_addr_t phys_addr, u8 interrupt_num,
			       u32 entry_size, u32 *msi_data,
			       u32 *msi_addr_offset);
	int	(*start)(struct pci_epc *epc);
	void	(*stop)(struct pci_epc *epc);
	const struct pci_epc_features* (*get_features)(struct pci_epc *epc,
						       u8 func_no, u8 vfunc_no);
	struct module *owner;
};


struct pci_epc_mem_window {
	phys_addr_t	phys_base;
	size_t		size;
	size_t		page_size;
};


struct pci_epc_mem {
	struct pci_epc_mem_window window;
	unsigned long	*bitmap;
	int		pages;
	
	struct mutex	lock;
};


struct pci_epc {
	struct device			dev;
	struct list_head		pci_epf;
	struct mutex			list_lock;
	const struct pci_epc_ops	*ops;
	struct pci_epc_mem		**windows;
	struct pci_epc_mem		*mem;
	unsigned int			num_windows;
	u8				max_functions;
	u8				*max_vfs;
	struct config_group		*group;
	
	struct mutex			lock;
	unsigned long			function_num_map;
};


struct pci_epc_features {
	unsigned int	linkup_notifier : 1;
	unsigned int	core_init_notifier : 1;
	unsigned int	msi_capable : 1;
	unsigned int	msix_capable : 1;
	u8	reserved_bar;
	u8	bar_fixed_64bit;
	u64	bar_fixed_size[PCI_STD_NUM_BARS];
	size_t	align;
};

#define to_pci_epc(device) container_of((device), struct pci_epc, dev)

#define pci_epc_create(dev, ops)    \
		__pci_epc_create((dev), (ops), THIS_MODULE)
#define devm_pci_epc_create(dev, ops)    \
		__devm_pci_epc_create((dev), (ops), THIS_MODULE)

static inline void epc_set_drvdata(struct pci_epc *epc, void *data)
{
	dev_set_drvdata(&epc->dev, data);
}

static inline void *epc_get_drvdata(struct pci_epc *epc)
{
	return dev_get_drvdata(&epc->dev);
}

struct pci_epc *
__devm_pci_epc_create(struct device *dev, const struct pci_epc_ops *ops,
		      struct module *owner);
struct pci_epc *
__pci_epc_create(struct device *dev, const struct pci_epc_ops *ops,
		 struct module *owner);
void devm_pci_epc_destroy(struct device *dev, struct pci_epc *epc);
void pci_epc_destroy(struct pci_epc *epc);
int pci_epc_add_epf(struct pci_epc *epc, struct pci_epf *epf,
		    enum pci_epc_interface_type type);
void pci_epc_linkup(struct pci_epc *epc);
void pci_epc_linkdown(struct pci_epc *epc);
void pci_epc_init_notify(struct pci_epc *epc);
void pci_epc_bme_notify(struct pci_epc *epc);
void pci_epc_remove_epf(struct pci_epc *epc, struct pci_epf *epf,
			enum pci_epc_interface_type type);
int pci_epc_write_header(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			 struct pci_epf_header *hdr);
int pci_epc_set_bar(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
		    struct pci_epf_bar *epf_bar);
void pci_epc_clear_bar(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
		       struct pci_epf_bar *epf_bar);
int pci_epc_map_addr(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
		     phys_addr_t phys_addr,
		     u64 pci_addr, size_t size);
void pci_epc_unmap_addr(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			phys_addr_t phys_addr);
int pci_epc_set_msi(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
		    u8 interrupts);
int pci_epc_get_msi(struct pci_epc *epc, u8 func_no, u8 vfunc_no);
int pci_epc_set_msix(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
		     u16 interrupts, enum pci_barno, u32 offset);
int pci_epc_get_msix(struct pci_epc *epc, u8 func_no, u8 vfunc_no);
int pci_epc_map_msi_irq(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
			phys_addr_t phys_addr, u8 interrupt_num,
			u32 entry_size, u32 *msi_data, u32 *msi_addr_offset);
int pci_epc_raise_irq(struct pci_epc *epc, u8 func_no, u8 vfunc_no,
		      enum pci_epc_irq_type type, u16 interrupt_num);
int pci_epc_start(struct pci_epc *epc);
void pci_epc_stop(struct pci_epc *epc);
const struct pci_epc_features *pci_epc_get_features(struct pci_epc *epc,
						    u8 func_no, u8 vfunc_no);
enum pci_barno
pci_epc_get_first_free_bar(const struct pci_epc_features *epc_features);
enum pci_barno pci_epc_get_next_free_bar(const struct pci_epc_features
					 *epc_features, enum pci_barno bar);
struct pci_epc *pci_epc_get(const char *epc_name);
void pci_epc_put(struct pci_epc *epc);

int pci_epc_mem_init(struct pci_epc *epc, phys_addr_t base,
		     size_t size, size_t page_size);
int pci_epc_multi_mem_init(struct pci_epc *epc,
			   struct pci_epc_mem_window *window,
			   unsigned int num_windows);
void pci_epc_mem_exit(struct pci_epc *epc);
void __iomem *pci_epc_mem_alloc_addr(struct pci_epc *epc,
				     phys_addr_t *phys_addr, size_t size);
void pci_epc_mem_free_addr(struct pci_epc *epc, phys_addr_t phys_addr,
			   void __iomem *virt_addr, size_t size);
#endif 
