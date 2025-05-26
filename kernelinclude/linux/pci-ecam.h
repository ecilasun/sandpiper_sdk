/* SPDX-License-Identifier: GPL-2.0 */

#ifndef DRIVERS_PCI_ECAM_H
#define DRIVERS_PCI_ECAM_H

#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>




#define PCIE_ECAM_BUS_SHIFT	20 
#define PCIE_ECAM_DEVFN_SHIFT	12 

#define PCIE_ECAM_BUS_MASK	0xff
#define PCIE_ECAM_DEVFN_MASK	0xff
#define PCIE_ECAM_REG_MASK	0xfff 

#define PCIE_ECAM_BUS(x)	(((x) & PCIE_ECAM_BUS_MASK) << PCIE_ECAM_BUS_SHIFT)
#define PCIE_ECAM_DEVFN(x)	(((x) & PCIE_ECAM_DEVFN_MASK) << PCIE_ECAM_DEVFN_SHIFT)
#define PCIE_ECAM_REG(x)	((x) & PCIE_ECAM_REG_MASK)

#define PCIE_ECAM_OFFSET(bus, devfn, where) \
	(PCIE_ECAM_BUS(bus) | \
	 PCIE_ECAM_DEVFN(devfn) | \
	 PCIE_ECAM_REG(where))


struct pci_config_window;
struct pci_ecam_ops {
	unsigned int			bus_shift;
	struct pci_ops			pci_ops;
	int				(*init)(struct pci_config_window *);
};


struct pci_config_window {
	struct resource			res;
	struct resource			busr;
	unsigned int			bus_shift;
	void				*priv;
	const struct pci_ecam_ops	*ops;
	union {
		void __iomem		*win;	
		void __iomem		**winp; 
	};
	struct device			*parent;
};


struct pci_config_window *pci_ecam_create(struct device *dev,
		struct resource *cfgres, struct resource *busr,
		const struct pci_ecam_ops *ops);
void pci_ecam_free(struct pci_config_window *cfg);


void __iomem *pci_ecam_map_bus(struct pci_bus *bus, unsigned int devfn,
			       int where);

extern const struct pci_ecam_ops pci_generic_ecam_ops;

#if defined(CONFIG_ACPI) && defined(CONFIG_PCI_QUIRKS)
extern const struct pci_ecam_ops pci_32b_ops;	
extern const struct pci_ecam_ops pci_32b_read_ops; 
extern const struct pci_ecam_ops hisi_pcie_ops;	
extern const struct pci_ecam_ops thunder_pem_ecam_ops; 
extern const struct pci_ecam_ops pci_thunder_ecam_ops; 
extern const struct pci_ecam_ops xgene_v1_pcie_ecam_ops; 
extern const struct pci_ecam_ops xgene_v2_pcie_ecam_ops; 
extern const struct pci_ecam_ops al_pcie_ops;	
extern const struct pci_ecam_ops tegra194_pcie_ops; 
extern const struct pci_ecam_ops loongson_pci_ecam_ops; 
#endif

#if IS_ENABLED(CONFIG_PCI_HOST_COMMON)

int pci_host_common_probe(struct platform_device *pdev);
int pci_host_common_remove(struct platform_device *pdev);
#endif
#endif
