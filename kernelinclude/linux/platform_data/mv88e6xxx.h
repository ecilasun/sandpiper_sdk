/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DSA_MV88E6XXX_H
#define __DSA_MV88E6XXX_H

#include <linux/platform_data/dsa.h>

struct dsa_mv88e6xxx_pdata {
	
	struct dsa_chip_data cd;
	const char *compatible;
	unsigned int enabled_ports;
	struct net_device *netdev;
	u32 eeprom_len;
	int irq;
};

#endif
