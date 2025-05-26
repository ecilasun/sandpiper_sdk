/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __NET_AX88796_PLAT_H
#define __NET_AX88796_PLAT_H

#include <linux/types.h>

struct sk_buff;
struct net_device;
struct platform_device;

#define AXFLG_HAS_EEPROM		(1<<0)
#define AXFLG_MAC_FROMDEV		(1<<1)	
#define AXFLG_HAS_93CX6			(1<<2)	
#define AXFLG_MAC_FROMPLATFORM		(1<<3)	

struct ax_plat_data {
	unsigned int	 flags;
	unsigned char	 wordlength;	
	unsigned char	 dcr_val;	
	unsigned char	 rcr_val;	
	unsigned char	 gpoc_val;	
	u32		*reg_offsets;	
	u8		*mac_addr;	

	
	void (*block_output)(struct net_device *dev, int count,
			const unsigned char *buf, int star_page);
	void (*block_input)(struct net_device *dev, int count,
			struct sk_buff *skb, int ring_offset);
	
	int (*check_irq)(struct platform_device *pdev);
};


extern void ax_NS8390_reinit(struct net_device *dev);

#endif 
