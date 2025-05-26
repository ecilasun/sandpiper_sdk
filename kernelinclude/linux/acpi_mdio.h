/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_ACPI_MDIO_H
#define __LINUX_ACPI_MDIO_H

#include <linux/phy.h>

#if IS_ENABLED(CONFIG_ACPI_MDIO)
int __acpi_mdiobus_register(struct mii_bus *mdio, struct fwnode_handle *fwnode,
			    struct module *owner);

static inline int
acpi_mdiobus_register(struct mii_bus *mdio, struct fwnode_handle *handle)
{
	return __acpi_mdiobus_register(mdio, handle, THIS_MODULE);
}
#else 
static inline int
acpi_mdiobus_register(struct mii_bus *mdio, struct fwnode_handle *fwnode)
{
	

	return mdiobus_register(mdio);
}
#endif

#endif 
