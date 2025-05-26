/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_DMA_IMX_H
#define __LINUX_DMA_IMX_H

#include <linux/scatterlist.h>
#include <linux/device.h>
#include <linux/dmaengine.h>


enum sdma_peripheral_type {
	IMX_DMATYPE_SSI,	
	IMX_DMATYPE_SSI_SP,	
	IMX_DMATYPE_MMC,	
	IMX_DMATYPE_SDHC,	
	IMX_DMATYPE_UART,	
	IMX_DMATYPE_UART_SP,	
	IMX_DMATYPE_FIRI,	
	IMX_DMATYPE_CSPI,	
	IMX_DMATYPE_CSPI_SP,	
	IMX_DMATYPE_SIM,	
	IMX_DMATYPE_ATA,	
	IMX_DMATYPE_CCM,	
	IMX_DMATYPE_EXT,	
	IMX_DMATYPE_MSHC,	
	IMX_DMATYPE_MSHC_SP,	
	IMX_DMATYPE_DSP,	
	IMX_DMATYPE_MEMORY,	
	IMX_DMATYPE_FIFO_MEMORY,
	IMX_DMATYPE_SPDIF,	
	IMX_DMATYPE_IPU_MEMORY,	
	IMX_DMATYPE_ASRC,	
	IMX_DMATYPE_ESAI,	
	IMX_DMATYPE_SSI_DUAL,	
	IMX_DMATYPE_ASRC_SP,	
	IMX_DMATYPE_SAI,	
	IMX_DMATYPE_MULTI_SAI,	
	IMX_DMATYPE_HDMI,       
};

enum imx_dma_prio {
	DMA_PRIO_HIGH = 0,
	DMA_PRIO_MEDIUM = 1,
	DMA_PRIO_LOW = 2
};

struct imx_dma_data {
	int dma_request; 
	int dma_request2; 
	enum sdma_peripheral_type peripheral_type;
	int priority;
};

static inline int imx_dma_is_ipu(struct dma_chan *chan)
{
	return !strcmp(dev_name(chan->device->dev), "ipu-core");
}

static inline int imx_dma_is_general_purpose(struct dma_chan *chan)
{
	return !strcmp(chan->device->dev->driver->name, "imx-sdma") ||
		!strcmp(chan->device->dev->driver->name, "imx-dma");
}


struct sdma_peripheral_config {
	int n_fifos_src;
	int n_fifos_dst;
	int stride_fifos_src;
	int stride_fifos_dst;
	int words_per_fifo;
	bool sw_done;
};

#endif 
