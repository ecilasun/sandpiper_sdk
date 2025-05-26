/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __ACPI_DRIVERS_H__
#define __ACPI_DRIVERS_H__

#define ACPI_MAX_STRING			80



#define ACPI_POWER_HID			"LNXPOWER"
#define ACPI_PROCESSOR_OBJECT_HID	"LNXCPU"
#define ACPI_SYSTEM_HID			"LNXSYSTM"
#define ACPI_THERMAL_HID		"LNXTHERM"
#define ACPI_BUTTON_HID_POWERF		"LNXPWRBN"
#define ACPI_BUTTON_HID_SLEEPF		"LNXSLPBN"
#define ACPI_VIDEO_HID			"LNXVIDEO"
#define ACPI_BAY_HID			"LNXIOBAY"
#define ACPI_DOCK_HID			"LNXDOCK"
#define ACPI_ECDT_HID			"LNXEC"

#define ACPI_SMBUS_MS_HID		"SMB0001"

#define ACPI_SMBUS_IBM_HID		"SMBUSIBM"


#define ACPI_FIXED_HARDWARE_EVENT	0x100






int acpi_irq_penalty_init(void);
int acpi_pci_link_allocate_irq(acpi_handle handle, int index, int *triggering,
			       int *polarity, char **name);
int acpi_pci_link_free_irq(acpi_handle handle);



struct pci_bus;

#ifdef CONFIG_PCI
struct pci_dev *acpi_get_pci_dev(acpi_handle);
#else
static inline struct pci_dev *acpi_get_pci_dev(acpi_handle handle)
{
	return NULL;
}
#endif



struct pci_bus *pci_acpi_scan_root(struct acpi_pci_root *root);

#ifdef CONFIG_X86
void pci_acpi_crs_quirks(void);
#else
static inline void pci_acpi_crs_quirks(void) { }
#endif



#ifdef CONFIG_ACPI_DOCK
extern int is_dock_device(struct acpi_device *adev);
#else
static inline int is_dock_device(struct acpi_device *adev)
{
	return 0;
}
#endif 

#endif 
