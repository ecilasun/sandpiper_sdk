/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __LINUX_SPI_PXA2XX_SPI_H
#define __LINUX_SPI_PXA2XX_SPI_H

#include <linux/types.h>

#include <linux/pxa2xx_ssp.h>

struct dma_chan;


struct pxa2xx_spi_controller {
	u16 num_chipselect;
	u8 enable_dma;
	u8 dma_burst_size;
	bool is_target;

	
	bool (*dma_filter)(struct dma_chan *chan, void *param);
	void *tx_param;
	void *rx_param;

	
	struct ssp_device ssp;
};


struct pxa2xx_spi_chip {
	u8 tx_threshold;
	u8 tx_hi_threshold;
	u8 rx_threshold;
	u8 dma_burst_size;
	u32 timeout;
};

#if defined(CONFIG_ARCH_PXA) || defined(CONFIG_ARCH_MMP)

#include <linux/clk.h>

extern void pxa2xx_set_spi_info(unsigned id, struct pxa2xx_spi_controller *info);

#endif

#endif	
