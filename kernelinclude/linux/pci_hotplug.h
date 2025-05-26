/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef _PCI_HOTPLUG_H
#define _PCI_HOTPLUG_H


struct hotplug_slot_ops {
	int (*enable_slot)		(struct hotplug_slot *slot);
	int (*disable_slot)		(struct hotplug_slot *slot);
	int (*set_attention_status)	(struct hotplug_slot *slot, u8 value);
	int (*hardware_test)		(struct hotplug_slot *slot, u32 value);
	int (*get_power_status)		(struct hotplug_slot *slot, u8 *value);
	int (*get_attention_status)	(struct hotplug_slot *slot, u8 *value);
	int (*get_latch_status)		(struct hotplug_slot *slot, u8 *value);
	int (*get_adapter_status)	(struct hotplug_slot *slot, u8 *value);
	int (*reset_slot)		(struct hotplug_slot *slot, bool probe);
};


struct hotplug_slot {
	const struct hotplug_slot_ops	*ops;

	
	struct list_head		slot_list;
	struct pci_slot			*pci_slot;
	struct module			*owner;
	const char			*mod_name;
};

static inline const char *hotplug_slot_name(const struct hotplug_slot *slot)
{
	return pci_slot_name(slot->pci_slot);
}

int __pci_hp_register(struct hotplug_slot *slot, struct pci_bus *pbus, int nr,
		      const char *name, struct module *owner,
		      const char *mod_name);
int __pci_hp_initialize(struct hotplug_slot *slot, struct pci_bus *bus, int nr,
			const char *name, struct module *owner,
			const char *mod_name);
int pci_hp_add(struct hotplug_slot *slot);

void pci_hp_del(struct hotplug_slot *slot);
void pci_hp_destroy(struct hotplug_slot *slot);
void pci_hp_deregister(struct hotplug_slot *slot);


#define pci_hp_register(slot, pbus, devnr, name) \
	__pci_hp_register(slot, pbus, devnr, name, THIS_MODULE, KBUILD_MODNAME)
#define pci_hp_initialize(slot, bus, nr, name) \
	__pci_hp_initialize(slot, bus, nr, name, THIS_MODULE, KBUILD_MODNAME)

#ifdef CONFIG_ACPI
#include <linux/acpi.h>
bool pciehp_is_native(struct pci_dev *bridge);
int acpi_get_hp_hw_control_from_firmware(struct pci_dev *bridge);
bool shpchp_is_native(struct pci_dev *bridge);
int acpi_pci_check_ejectable(struct pci_bus *pbus, acpi_handle handle);
int acpi_pci_detect_ejectable(acpi_handle handle);
#else
static inline int acpi_get_hp_hw_control_from_firmware(struct pci_dev *bridge)
{
	return 0;
}
static inline bool pciehp_is_native(struct pci_dev *bridge) { return true; }
static inline bool shpchp_is_native(struct pci_dev *bridge) { return true; }
#endif

static inline bool hotplug_is_native(struct pci_dev *bridge)
{
	return pciehp_is_native(bridge) || shpchp_is_native(bridge);
}
#endif
