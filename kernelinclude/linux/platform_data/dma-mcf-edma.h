/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_PLATFORM_DATA_MCF_EDMA_H__
#define __LINUX_PLATFORM_DATA_MCF_EDMA_H__

struct dma_slave_map;

bool mcf_edma_filter_fn(struct dma_chan *chan, void *param);

#define MCF_EDMA_FILTER_PARAM(ch)	((void *)ch)


struct mcf_edma_platform_data {
	int dma_channels;
	const struct dma_slave_map *slave_map;
	int slavecnt;
};

#endif 
