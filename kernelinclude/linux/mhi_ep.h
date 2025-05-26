/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _MHI_EP_H_
#define _MHI_EP_H_

#include <linux/dma-direction.h>
#include <linux/mhi.h>

#define MHI_EP_DEFAULT_MTU 0x8000


struct mhi_ep_channel_config {
	char *name;
	u32 num;
	u32 num_elements;
	enum dma_data_direction dir;
};


struct mhi_ep_cntrl_config {
	u32 mhi_version;
	u32 max_channels;
	u32 num_channels;
	const struct mhi_ep_channel_config *ch_cfg;
};


struct mhi_ep_db_info {
	u32 mask;
	u32 status;
};


struct mhi_ep_buf_info {
	void *dev_addr;
	u64 host_addr;
	size_t size;
};


struct mhi_ep_cntrl {
	struct device *cntrl_dev;
	struct mhi_ep_device *mhi_dev;
	void __iomem *mmio;

	struct mhi_ep_chan *mhi_chan;
	struct mhi_ep_event *mhi_event;
	struct mhi_ep_cmd *mhi_cmd;
	struct mhi_ep_sm *sm;

	struct mhi_chan_ctxt *ch_ctx_cache;
	struct mhi_event_ctxt *ev_ctx_cache;
	struct mhi_cmd_ctxt *cmd_ctx_cache;
	u64 ch_ctx_host_pa;
	u64 ev_ctx_host_pa;
	u64 cmd_ctx_host_pa;
	phys_addr_t ch_ctx_cache_phys;
	phys_addr_t ev_ctx_cache_phys;
	phys_addr_t cmd_ctx_cache_phys;

	struct mhi_ep_db_info chdb[4];
	struct mutex event_lock;
	struct mutex state_lock;
	spinlock_t list_lock;

	struct list_head st_transition_list;
	struct list_head ch_db_list;

	struct workqueue_struct *wq;
	struct work_struct state_work;
	struct work_struct reset_work;
	struct work_struct cmd_ring_work;
	struct work_struct ch_ring_work;
	struct kmem_cache *ring_item_cache;
	struct kmem_cache *ev_ring_el_cache;
	struct kmem_cache *tre_buf_cache;

	void (*raise_irq)(struct mhi_ep_cntrl *mhi_cntrl, u32 vector);
	int (*alloc_map)(struct mhi_ep_cntrl *mhi_cntrl, u64 pci_addr, phys_addr_t *phys_ptr,
			 void __iomem **virt, size_t size);
	void (*unmap_free)(struct mhi_ep_cntrl *mhi_cntrl, u64 pci_addr, phys_addr_t phys,
			   void __iomem *virt, size_t size);
	int (*read_from_host)(struct mhi_ep_cntrl *mhi_cntrl, struct mhi_ep_buf_info *buf_info);
	int (*write_to_host)(struct mhi_ep_cntrl *mhi_cntrl, struct mhi_ep_buf_info *buf_info);

	enum mhi_state mhi_state;

	u32 max_chan;
	u32 mru;
	u32 event_rings;
	u32 hw_event_rings;
	u32 chdb_offset;
	u32 erdb_offset;
	u32 index;
	int irq;
	bool enabled;
};


struct mhi_ep_device {
	struct device dev;
	struct mhi_ep_cntrl *mhi_cntrl;
	const struct mhi_device_id *id;
	const char *name;
	struct mhi_ep_chan *ul_chan;
	struct mhi_ep_chan *dl_chan;
	enum mhi_device_type dev_type;
};


struct mhi_ep_driver {
	const struct mhi_device_id *id_table;
	struct device_driver driver;
	int (*probe)(struct mhi_ep_device *mhi_ep,
		     const struct mhi_device_id *id);
	void (*remove)(struct mhi_ep_device *mhi_ep);
	void (*ul_xfer_cb)(struct mhi_ep_device *mhi_dev,
			   struct mhi_result *result);
	void (*dl_xfer_cb)(struct mhi_ep_device *mhi_dev,
			   struct mhi_result *result);
};

#define to_mhi_ep_device(dev) container_of(dev, struct mhi_ep_device, dev)
#define to_mhi_ep_driver(drv) container_of(drv, struct mhi_ep_driver, driver)


#define module_mhi_ep_driver(mhi_drv) \
	module_driver(mhi_drv, mhi_ep_driver_register, \
		      mhi_ep_driver_unregister)


#define mhi_ep_driver_register(mhi_drv) \
	__mhi_ep_driver_register(mhi_drv, THIS_MODULE)


int __mhi_ep_driver_register(struct mhi_ep_driver *mhi_drv, struct module *owner);


void mhi_ep_driver_unregister(struct mhi_ep_driver *mhi_drv);


int mhi_ep_register_controller(struct mhi_ep_cntrl *mhi_cntrl,
			       const struct mhi_ep_cntrl_config *config);


void mhi_ep_unregister_controller(struct mhi_ep_cntrl *mhi_cntrl);


int mhi_ep_power_up(struct mhi_ep_cntrl *mhi_cntrl);


void mhi_ep_power_down(struct mhi_ep_cntrl *mhi_cntrl);


bool mhi_ep_queue_is_empty(struct mhi_ep_device *mhi_dev, enum dma_data_direction dir);


int mhi_ep_queue_skb(struct mhi_ep_device *mhi_dev, struct sk_buff *skb);

#endif
