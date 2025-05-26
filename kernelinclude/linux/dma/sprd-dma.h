/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _SPRD_DMA_H_
#define _SPRD_DMA_H_

#define SPRD_DMA_REQ_SHIFT	8
#define SPRD_DMA_TRG_MODE_SHIFT	16
#define SPRD_DMA_CHN_MODE_SHIFT	24
#define SPRD_DMA_FLAGS(chn_mode, trg_mode, req_mode, int_type) \
	((chn_mode) << SPRD_DMA_CHN_MODE_SHIFT | \
	(trg_mode) << SPRD_DMA_TRG_MODE_SHIFT | \
	(req_mode) << SPRD_DMA_REQ_SHIFT | (int_type))




enum sprd_dma_chn_mode {
	SPRD_DMA_CHN_MODE_NONE,
	SPRD_DMA_SRC_CHN0,
	SPRD_DMA_SRC_CHN1,
	SPRD_DMA_DST_CHN0,
	SPRD_DMA_DST_CHN1,
};


enum sprd_dma_trg_mode {
	SPRD_DMA_NO_TRG,
	SPRD_DMA_FRAG_DONE_TRG,
	SPRD_DMA_BLOCK_DONE_TRG,
	SPRD_DMA_TRANS_DONE_TRG,
	SPRD_DMA_LIST_DONE_TRG,
};


enum sprd_dma_req_mode {
	SPRD_DMA_FRAG_REQ,
	SPRD_DMA_BLK_REQ,
	SPRD_DMA_TRANS_REQ,
	SPRD_DMA_LIST_REQ,
};


enum sprd_dma_int_type {
	SPRD_DMA_NO_INT,
	SPRD_DMA_FRAG_INT,
	SPRD_DMA_BLK_INT,
	SPRD_DMA_BLK_FRAG_INT,
	SPRD_DMA_TRANS_INT,
	SPRD_DMA_TRANS_FRAG_INT,
	SPRD_DMA_TRANS_BLK_INT,
	SPRD_DMA_LIST_INT,
	SPRD_DMA_CFGERR_INT,
};


struct sprd_dma_linklist {
	unsigned long virt_addr;
	phys_addr_t phy_addr;
	phys_addr_t wrap_addr;
};

#endif
