/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _COMEDI_PCI_H
#define _COMEDI_PCI_H

#include <linux/pci.h>
#include <linux/comedi/comedidev.h>


#define PCI_VENDOR_ID_KOLTER		0x1001
#define PCI_VENDOR_ID_ICP		0x104c
#define PCI_VENDOR_ID_DT		0x1116
#define PCI_VENDOR_ID_IOTECH		0x1616
#define PCI_VENDOR_ID_CONTEC		0x1221
#define PCI_VENDOR_ID_RTD		0x1435
#define PCI_VENDOR_ID_HUMUSOFT		0x186c

struct pci_dev *comedi_to_pci_dev(struct comedi_device *dev);

int comedi_pci_enable(struct comedi_device *dev);
void comedi_pci_disable(struct comedi_device *dev);
void comedi_pci_detach(struct comedi_device *dev);

int comedi_pci_auto_config(struct pci_dev *pcidev, struct comedi_driver *driver,
			   unsigned long context);
void comedi_pci_auto_unconfig(struct pci_dev *pcidev);

int comedi_pci_driver_register(struct comedi_driver *comedi_driver,
			       struct pci_driver *pci_driver);
void comedi_pci_driver_unregister(struct comedi_driver *comedi_driver,
				  struct pci_driver *pci_driver);


#define module_comedi_pci_driver(__comedi_driver, __pci_driver) \
	module_driver(__comedi_driver, comedi_pci_driver_register, \
			comedi_pci_driver_unregister, &(__pci_driver))

#endif 
