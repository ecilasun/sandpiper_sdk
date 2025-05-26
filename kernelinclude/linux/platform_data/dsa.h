/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DSA_PDATA_H
#define __DSA_PDATA_H

struct device;
struct net_device;

#define DSA_MAX_SWITCHES	4
#define DSA_MAX_PORTS		12
#define DSA_RTABLE_NONE		-1

struct dsa_chip_data {
	
	struct device	*host_dev;
	int		sw_addr;

	
	struct device	*netdev[DSA_MAX_PORTS];

	
	int		eeprom_len;

	
	struct device_node *of_node;

	
	char		*port_names[DSA_MAX_PORTS];
	struct device_node *port_dn[DSA_MAX_PORTS];

	
	s8		rtable[DSA_MAX_SWITCHES];
};

struct dsa_platform_data {
	
	struct device	*netdev;
	struct net_device *of_netdev;

	
	int		nr_chips;
	struct dsa_chip_data	*chip;
};


#endif 
