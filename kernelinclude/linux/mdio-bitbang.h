/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_MDIO_BITBANG_H
#define __LINUX_MDIO_BITBANG_H

#include <linux/phy.h>

struct module;

struct mdiobb_ctrl;

struct mdiobb_ops {
	struct module *owner;

	
	void (*set_mdc)(struct mdiobb_ctrl *ctrl, int level);

	
	void (*set_mdio_dir)(struct mdiobb_ctrl *ctrl, int output);

	
	void (*set_mdio_data)(struct mdiobb_ctrl *ctrl, int value);

	
	int (*get_mdio_data)(struct mdiobb_ctrl *ctrl);
};

struct mdiobb_ctrl {
	const struct mdiobb_ops *ops;
	unsigned int override_op_c22;
	u8 op_c22_read;
	u8 op_c22_write;
};

int mdiobb_read_c22(struct mii_bus *bus, int phy, int reg);
int mdiobb_write_c22(struct mii_bus *bus, int phy, int reg, u16 val);
int mdiobb_read_c45(struct mii_bus *bus, int devad, int phy, int reg);
int mdiobb_write_c45(struct mii_bus *bus, int devad, int phy, int reg, u16 val);


struct mii_bus *alloc_mdio_bitbang(struct mdiobb_ctrl *ctrl);


void free_mdio_bitbang(struct mii_bus *bus);

#endif
