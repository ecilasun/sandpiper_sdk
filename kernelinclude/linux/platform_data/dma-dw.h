/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _PLATFORM_DATA_DMA_DW_H
#define _PLATFORM_DATA_DMA_DW_H

#include <linux/bits.h>
#include <linux/types.h>

#define DW_DMA_MAX_NR_MASTERS	4
#define DW_DMA_MAX_NR_CHANNELS	8
#define DW_DMA_MIN_BURST	1
#define DW_DMA_MAX_BURST	256

struct device;


struct dw_dma_slave {
	struct device		*dma_dev;
	u8			src_id;
	u8			dst_id;
	u8			m_master;
	u8			p_master;
	u8			channels;
	bool			hs_polarity;
};


struct dw_dma_platform_data {
	u32		nr_masters;
	u32		nr_channels;
#define CHAN_ALLOCATION_ASCENDING	0	
#define CHAN_ALLOCATION_DESCENDING	1	
	u32		chan_allocation_order;
#define CHAN_PRIORITY_ASCENDING		0	
#define CHAN_PRIORITY_DESCENDING	1	
	u32		chan_priority;
	u32		block_size;
	u32		data_width[DW_DMA_MAX_NR_MASTERS];
	u32		multi_block[DW_DMA_MAX_NR_CHANNELS];
	u32		max_burst[DW_DMA_MAX_NR_CHANNELS];
#define CHAN_PROTCTL_PRIVILEGED		BIT(0)
#define CHAN_PROTCTL_BUFFERABLE		BIT(1)
#define CHAN_PROTCTL_CACHEABLE		BIT(2)
#define CHAN_PROTCTL_MASK		GENMASK(2, 0)
	u32		protctl;
#define DW_DMA_QUIRK_XBAR_PRESENT	BIT(0)
	u32		quirks;
};

#endif 
