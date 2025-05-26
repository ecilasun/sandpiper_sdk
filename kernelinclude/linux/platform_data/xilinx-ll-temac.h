/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_XILINX_LL_TEMAC_H
#define __LINUX_XILINX_LL_TEMAC_H

#include <linux/if_ether.h>
#include <linux/phy.h>
#include <linux/spinlock.h>

struct ll_temac_platform_data {
	bool txcsum;		
	bool rxcsum;		
	u8 mac_addr[ETH_ALEN];	
	
	u32 mdio_clk_freq;
	unsigned long long mdio_bus_id; 
	int phy_addr;		
	phy_interface_t phy_interface; 
	bool reg_little_endian;	
	bool dma_little_endian;	
	
	spinlock_t *indirect_lock;
	
	u8 tx_irq_timeout;	
	u8 tx_irq_count;	
	u8 rx_irq_timeout;	
	u8 rx_irq_count;	
};

#endif 
