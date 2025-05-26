/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_PCI_EPF_H
#define __LINUX_PCI_EPF_H

#include <linux/configfs.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/pci.h>

struct pci_epf;
enum pci_epc_interface_type;

enum pci_barno {
	NO_BAR = -1,
	BAR_0,
	BAR_1,
	BAR_2,
	BAR_3,
	BAR_4,
	BAR_5,
};


struct pci_epf_header {
	u16	vendorid;
	u16	deviceid;
	u8	revid;
	u8	progif_code;
	u8	subclass_code;
	u8	baseclass_code;
	u8	cache_line_size;
	u16	subsys_vendor_id;
	u16	subsys_id;
	enum pci_interrupt_pin interrupt_pin;
};


struct pci_epf_ops {
	int	(*bind)(struct pci_epf *epf);
	void	(*unbind)(struct pci_epf *epf);
	struct config_group *(*add_cfs)(struct pci_epf *epf,
					struct config_group *group);
};


struct pci_epc_event_ops {
	int (*core_init)(struct pci_epf *epf);
	int (*link_up)(struct pci_epf *epf);
	int (*link_down)(struct pci_epf *epf);
	int (*bme)(struct pci_epf *epf);
};


struct pci_epf_driver {
	int	(*probe)(struct pci_epf *epf,
			 const struct pci_epf_device_id *id);
	void	(*remove)(struct pci_epf *epf);

	struct device_driver	driver;
	struct pci_epf_ops	*ops;
	struct module		*owner;
	struct list_head	epf_group;
	const struct pci_epf_device_id	*id_table;
};

#define to_pci_epf_driver(drv) (container_of((drv), struct pci_epf_driver, \
				driver))


struct pci_epf_bar {
	dma_addr_t	phys_addr;
	void		*addr;
	size_t		size;
	enum pci_barno	barno;
	int		flags;
};


struct pci_epf {
	struct device		dev;
	const char		*name;
	struct pci_epf_header	*header;
	struct pci_epf_bar	bar[6];
	u8			msi_interrupts;
	u16			msix_interrupts;
	u8			func_no;
	u8			vfunc_no;

	struct pci_epc		*epc;
	struct pci_epf		*epf_pf;
	struct pci_epf_driver	*driver;
	const struct pci_epf_device_id *id;
	struct list_head	list;
	
	struct mutex		lock;

	
	struct pci_epc		*sec_epc;
	struct list_head	sec_epc_list;
	struct pci_epf_bar	sec_epc_bar[6];
	u8			sec_epc_func_no;
	struct config_group	*group;
	unsigned int		is_bound;
	unsigned int		is_vf;
	unsigned long		vfunction_num_map;
	struct list_head	pci_vepf;
	const struct pci_epc_event_ops *event_ops;
};


struct pci_epf_msix_tbl {
	u64 msg_addr;
	u32 msg_data;
	u32 vector_ctrl;
};

#define to_pci_epf(epf_dev) container_of((epf_dev), struct pci_epf, dev)

#define pci_epf_register_driver(driver)    \
		__pci_epf_register_driver((driver), THIS_MODULE)

static inline void epf_set_drvdata(struct pci_epf *epf, void *data)
{
	dev_set_drvdata(&epf->dev, data);
}

static inline void *epf_get_drvdata(struct pci_epf *epf)
{
	return dev_get_drvdata(&epf->dev);
}

struct pci_epf *pci_epf_create(const char *name);
void pci_epf_destroy(struct pci_epf *epf);
int __pci_epf_register_driver(struct pci_epf_driver *driver,
			      struct module *owner);
void pci_epf_unregister_driver(struct pci_epf_driver *driver);
void *pci_epf_alloc_space(struct pci_epf *epf, size_t size, enum pci_barno bar,
			  size_t align, enum pci_epc_interface_type type);
void pci_epf_free_space(struct pci_epf *epf, void *addr, enum pci_barno bar,
			enum pci_epc_interface_type type);
int pci_epf_bind(struct pci_epf *epf);
void pci_epf_unbind(struct pci_epf *epf);
int pci_epf_add_vepf(struct pci_epf *epf_pf, struct pci_epf *epf_vf);
void pci_epf_remove_vepf(struct pci_epf *epf_pf, struct pci_epf *epf_vf);
#endif 
