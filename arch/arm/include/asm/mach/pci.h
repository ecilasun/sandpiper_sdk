/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_MACH_PCI_H
#define __ASM_MACH_PCI_H

#include <linux/ioport.h>

struct pci_sys_data;
struct pci_ops;
struct pci_bus;
struct pci_host_bridge;
struct device;

struct hw_pci {
	struct pci_ops	*ops;
	int		nr_controllers;
	void		**private_data;
	int		(*setup)(int nr, struct pci_sys_data *);
	int		(*scan)(int nr, struct pci_host_bridge *);
	void		(*preinit)(void);
	void		(*postinit)(void);
	u8		(*swizzle)(struct pci_dev *dev, u8 *pin);
	int		(*map_irq)(const struct pci_dev *dev, u8 slot, u8 pin);
};


struct pci_sys_data {
	struct list_head node;
	int		busnr;		
	u64		mem_offset;	
	unsigned long	io_offset;	
	struct pci_bus	*bus;		
	struct list_head resources;	
	struct resource io_res;
	char		io_res_name[12];
					
	u8		(*swizzle)(struct pci_dev *, u8 *);
					
	int		(*map_irq)(const struct pci_dev *, u8, u8);
	void		*private_data;	
};


void pci_common_init_dev(struct device *, struct hw_pci *);


static inline void pci_common_init(struct hw_pci *hw)
{
	pci_common_init_dev(NULL, hw);
}


#if defined(CONFIG_PCI)
extern void pci_map_io_early(unsigned long pfn);
#else
static inline void pci_map_io_early(unsigned long pfn) {}
#endif


extern struct pci_ops iop3xx_ops;
extern int iop3xx_pci_setup(int nr, struct pci_sys_data *);
extern void iop3xx_pci_preinit(void);
extern void iop3xx_pci_preinit_cond(void);

extern struct pci_ops dc21285_ops;
extern int dc21285_setup(int nr, struct pci_sys_data *);
extern void dc21285_preinit(void);
extern void dc21285_postinit(void);

#endif 
