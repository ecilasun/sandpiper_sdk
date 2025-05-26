/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _MXS_DMA_H_
#define _MXS_DMA_H_

#include <linux/dmaengine.h>

#define MXS_DMA_CTRL_WAIT4END	BIT(31)
#define MXS_DMA_CTRL_WAIT4RDY	BIT(30)


static inline struct dma_async_tx_descriptor *mxs_dmaengine_prep_pio(
        struct dma_chan *chan, u32 *pio, unsigned int npio,
        enum dma_transfer_direction dir, unsigned long flags)
{
	return dmaengine_prep_slave_sg(chan, (struct scatterlist *)pio, npio,
				       dir, flags);
}

#endif 
