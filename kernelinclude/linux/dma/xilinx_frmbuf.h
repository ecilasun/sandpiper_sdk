

#ifndef __XILINX_FRMBUF_DMA_H
#define __XILINX_FRMBUF_DMA_H

#include <linux/dmaengine.h>



#define EARLY_CALLBACK			BIT(1)

#define EARLY_CALLBACK_START_DESC	BIT(2)

enum vid_frmwork_type {
	XDMA_DRM = 0,
	XDMA_V4L2,
};


enum operation_mode {
	DEFAULT = 0x0,
	AUTO_RESTART = BIT(7),
};


enum fid_modes {
	FID_MODE_0 = 0,
	FID_MODE_1 = 1,
	FID_MODE_2 = 2,
};

#if IS_ENABLED(CONFIG_XILINX_FRMBUF)

void xilinx_xdma_set_mode(struct dma_chan *chan, enum operation_mode mode);


void xilinx_xdma_drm_config(struct dma_chan *chan, u32 drm_fourcc);


void xilinx_xdma_v4l2_config(struct dma_chan *chan, u32 v4l2_fourcc);


int xilinx_xdma_get_drm_vid_fmts(struct dma_chan *chan, u32 *fmt_cnt,
				 u32 **fmts);


int xilinx_xdma_get_v4l2_vid_fmts(struct dma_chan *chan, u32 *fmt_cnt,
				  u32 **fmts);


int xilinx_xdma_get_fid(struct dma_chan *chan,
			struct dma_async_tx_descriptor *async_tx, u32 *fid);


int xilinx_xdma_set_fid(struct dma_chan *chan,
			struct dma_async_tx_descriptor *async_tx, u32 fid);


int xilinx_xdma_get_fid_err_flag(struct dma_chan *chan,
				 u32 *fid_err_flag);


int xilinx_xdma_get_fid_out(struct dma_chan *chan,
			    u32 *fid_out_val);


int xilinx_xdma_get_earlycb(struct dma_chan *chan,
			    struct dma_async_tx_descriptor *async_tx,
			    u32 *earlycb);


int xilinx_xdma_set_earlycb(struct dma_chan *chan,
			    struct dma_async_tx_descriptor *async_tx,
			    u32 earlycb);

int xilinx_xdma_get_width_align(struct dma_chan *chan, u32 *width_align);

#else
static inline void xilinx_xdma_set_mode(struct dma_chan *chan,
					enum operation_mode mode)
{ }

static inline void xilinx_xdma_drm_config(struct dma_chan *chan, u32 drm_fourcc)
{ }

static inline void xilinx_xdma_v4l2_config(struct dma_chan *chan,
					   u32 v4l2_fourcc)
{ }

static inline int xilinx_xdma_get_drm_vid_fmts(struct dma_chan *chan,
					       u32 *fmt_cnt, u32 **fmts)
{
	return -ENODEV;
}

static inline int xilinx_xdma_get_v4l2_vid_fmts(struct dma_chan *chan,
						u32 *fmt_cnt,u32 **fmts)
{
	return -ENODEV;
}

static inline int xilinx_xdma_get_fid(struct dma_chan *chan,
				      struct dma_async_tx_descriptor *async_tx,
				      u32 *fid)
{
	return -ENODEV;
}

static inline int xilinx_xdma_set_fid(struct dma_chan *chan,
				      struct dma_async_tx_descriptor *async_tx,
				      u32 fid)
{
	return -ENODEV;
}

static inline int xilinx_xdma_get_earlycb(struct dma_chan *chan,
					  struct dma_async_tx_descriptor *atx,
					  u32 *earlycb)
{
	return -ENODEV;
}

static inline int xilinx_xdma_set_earlycb(struct dma_chan *chan,
					  struct dma_async_tx_descriptor *atx,
					  u32 earlycb)
{
	return -ENODEV;
}

static inline int xilinx_xdma_get_width_align(struct dma_chan *chan, u32 *width_align)
{
	return -ENODEV;
}
#endif

#endif 
