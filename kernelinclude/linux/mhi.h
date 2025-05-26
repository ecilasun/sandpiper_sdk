/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _MHI_H_
#define _MHI_H_

#include <linux/device.h>
#include <linux/dma-direction.h>
#include <linux/mutex.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

#define MHI_MAX_OEM_PK_HASH_SEGMENTS 16

struct mhi_chan;
struct mhi_event;
struct mhi_ctxt;
struct mhi_cmd;
struct mhi_buf_info;


enum mhi_callback {
	MHI_CB_IDLE,
	MHI_CB_PENDING_DATA,
	MHI_CB_LPM_ENTER,
	MHI_CB_LPM_EXIT,
	MHI_CB_EE_RDDM,
	MHI_CB_EE_MISSION_MODE,
	MHI_CB_SYS_ERROR,
	MHI_CB_FATAL_ERROR,
	MHI_CB_BW_REQ,
};


enum mhi_flags {
	MHI_EOB = BIT(0),
	MHI_EOT = BIT(1),
	MHI_CHAIN = BIT(2),
};


enum mhi_device_type {
	MHI_DEVICE_XFER,
	MHI_DEVICE_CONTROLLER,
};


enum mhi_ch_type {
	MHI_CH_TYPE_INVALID = 0,
	MHI_CH_TYPE_OUTBOUND = DMA_TO_DEVICE,
	MHI_CH_TYPE_INBOUND = DMA_FROM_DEVICE,
	MHI_CH_TYPE_INBOUND_COALESCED = 3,
};


struct image_info {
	struct mhi_buf *mhi_buf;
	
	struct bhi_vec_entry *bhi_vec;
	
	u32 entries;
};


struct mhi_link_info {
	unsigned int target_link_speed;
	unsigned int target_link_width;
};


enum mhi_ee_type {
	MHI_EE_PBL,
	MHI_EE_SBL,
	MHI_EE_AMSS,
	MHI_EE_RDDM,
	MHI_EE_WFW,
	MHI_EE_PTHRU,
	MHI_EE_EDL,
	MHI_EE_FP,
	MHI_EE_MAX_SUPPORTED = MHI_EE_FP,
	MHI_EE_DISABLE_TRANSITION, 
	MHI_EE_NOT_SUPPORTED,
	MHI_EE_MAX,
};


enum mhi_state {
	MHI_STATE_RESET = 0x0,
	MHI_STATE_READY = 0x1,
	MHI_STATE_M0 = 0x2,
	MHI_STATE_M1 = 0x3,
	MHI_STATE_M2 = 0x4,
	MHI_STATE_M3 = 0x5,
	MHI_STATE_M3_FAST = 0x6,
	MHI_STATE_BHI = 0x7,
	MHI_STATE_SYS_ERR = 0xFF,
	MHI_STATE_MAX,
};


enum mhi_ch_ee_mask {
	MHI_CH_EE_PBL = BIT(MHI_EE_PBL),
	MHI_CH_EE_SBL = BIT(MHI_EE_SBL),
	MHI_CH_EE_AMSS = BIT(MHI_EE_AMSS),
	MHI_CH_EE_RDDM = BIT(MHI_EE_RDDM),
	MHI_CH_EE_PTHRU = BIT(MHI_EE_PTHRU),
	MHI_CH_EE_WFW = BIT(MHI_EE_WFW),
	MHI_CH_EE_EDL = BIT(MHI_EE_EDL),
};


enum mhi_er_data_type {
	MHI_ER_DATA,
	MHI_ER_CTRL,
};


enum mhi_db_brst_mode {
	MHI_DB_BRST_DISABLE = 0x2,
	MHI_DB_BRST_ENABLE = 0x3,
};


struct mhi_channel_config {
	char *name;
	u32 num;
	u32 num_elements;
	u32 local_elements;
	u32 event_ring;
	enum dma_data_direction dir;
	enum mhi_ch_type type;
	u32 ee_mask;
	u32 pollcfg;
	enum mhi_db_brst_mode doorbell;
	bool lpm_notify;
	bool offload_channel;
	bool doorbell_mode_switch;
	bool auto_queue;
	bool wake_capable;
};


struct mhi_event_config {
	u32 num_elements;
	u32 irq_moderation_ms;
	u32 irq;
	u32 channel;
	u32 priority;
	enum mhi_db_brst_mode mode;
	enum mhi_er_data_type data_type;
	bool hardware_event;
	bool client_managed;
	bool offload_channel;
};


struct mhi_controller_config {
	u32 max_channels;
	u32 timeout_ms;
	u32 buf_len;
	u32 num_channels;
	const struct mhi_channel_config *ch_cfg;
	u32 num_events;
	struct mhi_event_config *event_cfg;
	bool use_bounce_buf;
	bool m2_no_db;
};


struct mhi_controller {
	struct device *cntrl_dev;
	struct mhi_device *mhi_dev;
	struct dentry *debugfs_dentry;
	void __iomem *regs;
	void __iomem *bhi;
	void __iomem *bhie;
	void __iomem *wake_db;

	dma_addr_t iova_start;
	dma_addr_t iova_stop;
	const char *fw_image;
	const u8 *fw_data;
	size_t fw_sz;
	const char *edl_image;
	size_t rddm_size;
	size_t sbl_size;
	size_t seg_len;
	size_t reg_len;
	struct image_info *fbc_image;
	struct image_info *rddm_image;
	struct mhi_chan *mhi_chan;
	struct list_head lpm_chans;
	int *irq;
	u32 max_chan;
	u32 total_ev_rings;
	u32 hw_ev_rings;
	u32 sw_ev_rings;
	u32 nr_irqs;
	u32 family_number;
	u32 device_number;
	u32 major_version;
	u32 minor_version;
	u32 serial_number;
	u32 oem_pk_hash[MHI_MAX_OEM_PK_HASH_SEGMENTS];

	struct mhi_event *mhi_event;
	struct mhi_cmd *mhi_cmd;
	struct mhi_ctxt *mhi_ctxt;

	struct mutex pm_mutex;
	rwlock_t pm_lock;
	u32 timeout_ms;
	u32 pm_state;
	u32 db_access;
	enum mhi_ee_type ee;
	enum mhi_state dev_state;
	atomic_t dev_wake;
	atomic_t pending_pkts;
	u32 M0, M2, M3;
	struct list_head transition_list;
	spinlock_t transition_lock;
	spinlock_t wlock;
	struct mhi_link_info mhi_link_info;
	struct work_struct st_worker;
	struct workqueue_struct *hiprio_wq;
	wait_queue_head_t state_event;

	void (*status_cb)(struct mhi_controller *mhi_cntrl,
			  enum mhi_callback cb);
	void (*wake_get)(struct mhi_controller *mhi_cntrl, bool override);
	void (*wake_put)(struct mhi_controller *mhi_cntrl, bool override);
	void (*wake_toggle)(struct mhi_controller *mhi_cntrl);
	int (*runtime_get)(struct mhi_controller *mhi_cntrl);
	void (*runtime_put)(struct mhi_controller *mhi_cntrl);
	int (*map_single)(struct mhi_controller *mhi_cntrl,
			  struct mhi_buf_info *buf);
	void (*unmap_single)(struct mhi_controller *mhi_cntrl,
			     struct mhi_buf_info *buf);
	int (*read_reg)(struct mhi_controller *mhi_cntrl, void __iomem *addr,
			u32 *out);
	void (*write_reg)(struct mhi_controller *mhi_cntrl, void __iomem *addr,
			  u32 val);
	void (*reset)(struct mhi_controller *mhi_cntrl);

	size_t buffer_len;
	int index;
	bool bounce_buf;
	bool fbc_download;
	bool wake_set;
	unsigned long irq_flags;
	u32 mru;
};


struct mhi_device {
	const struct mhi_device_id *id;
	const char *name;
	struct mhi_controller *mhi_cntrl;
	struct mhi_chan *ul_chan;
	struct mhi_chan *dl_chan;
	struct device dev;
	enum mhi_device_type dev_type;
	int ul_chan_id;
	int dl_chan_id;
	u32 dev_wake;
};


struct mhi_result {
	void *buf_addr;
	size_t bytes_xferd;
	enum dma_data_direction dir;
	int transaction_status;
};


struct mhi_buf {
	void *buf;
	const char *name;
	dma_addr_t dma_addr;
	size_t len;
};


struct mhi_driver {
	const struct mhi_device_id *id_table;
	int (*probe)(struct mhi_device *mhi_dev,
		     const struct mhi_device_id *id);
	void (*remove)(struct mhi_device *mhi_dev);
	void (*ul_xfer_cb)(struct mhi_device *mhi_dev,
			   struct mhi_result *result);
	void (*dl_xfer_cb)(struct mhi_device *mhi_dev,
			   struct mhi_result *result);
	void (*status_cb)(struct mhi_device *mhi_dev, enum mhi_callback mhi_cb);
	struct device_driver driver;
};

#define to_mhi_driver(drv) container_of(drv, struct mhi_driver, driver)
#define to_mhi_device(dev) container_of(dev, struct mhi_device, dev)


struct mhi_controller *mhi_alloc_controller(void);


void mhi_free_controller(struct mhi_controller *mhi_cntrl);


int mhi_register_controller(struct mhi_controller *mhi_cntrl,
			const struct mhi_controller_config *config);


void mhi_unregister_controller(struct mhi_controller *mhi_cntrl);


#define module_mhi_driver(mhi_drv) \
	module_driver(mhi_drv, mhi_driver_register, \
		      mhi_driver_unregister)


#define mhi_driver_register(mhi_drv) \
	__mhi_driver_register(mhi_drv, THIS_MODULE)


int __mhi_driver_register(struct mhi_driver *mhi_drv, struct module *owner);


void mhi_driver_unregister(struct mhi_driver *mhi_drv);


void mhi_set_mhi_state(struct mhi_controller *mhi_cntrl,
		       enum mhi_state state);


void mhi_notify(struct mhi_device *mhi_dev, enum mhi_callback cb_reason);


int mhi_get_free_desc_count(struct mhi_device *mhi_dev,
				enum dma_data_direction dir);


int mhi_prepare_for_power_up(struct mhi_controller *mhi_cntrl);


int mhi_async_power_up(struct mhi_controller *mhi_cntrl);


int mhi_sync_power_up(struct mhi_controller *mhi_cntrl);


void mhi_power_down(struct mhi_controller *mhi_cntrl, bool graceful);


void mhi_unprepare_after_power_down(struct mhi_controller *mhi_cntrl);


int mhi_pm_suspend(struct mhi_controller *mhi_cntrl);


int mhi_pm_resume(struct mhi_controller *mhi_cntrl);


int mhi_pm_resume_force(struct mhi_controller *mhi_cntrl);


int mhi_download_rddm_image(struct mhi_controller *mhi_cntrl, bool in_panic);


int mhi_force_rddm_mode(struct mhi_controller *mhi_cntrl);


enum mhi_ee_type mhi_get_exec_env(struct mhi_controller *mhi_cntrl);


enum mhi_state mhi_get_mhi_state(struct mhi_controller *mhi_cntrl);


void mhi_soc_reset(struct mhi_controller *mhi_cntrl);


void mhi_device_get(struct mhi_device *mhi_dev);


int mhi_device_get_sync(struct mhi_device *mhi_dev);


void mhi_device_put(struct mhi_device *mhi_dev);


int mhi_prepare_for_transfer(struct mhi_device *mhi_dev);


int mhi_prepare_for_transfer_autoqueue(struct mhi_device *mhi_dev);


void mhi_unprepare_from_transfer(struct mhi_device *mhi_dev);


int mhi_queue_dma(struct mhi_device *mhi_dev, enum dma_data_direction dir,
		  struct mhi_buf *mhi_buf, size_t len, enum mhi_flags mflags);


int mhi_queue_buf(struct mhi_device *mhi_dev, enum dma_data_direction dir,
		  void *buf, size_t len, enum mhi_flags mflags);


int mhi_queue_skb(struct mhi_device *mhi_dev, enum dma_data_direction dir,
		  struct sk_buff *skb, size_t len, enum mhi_flags mflags);


bool mhi_queue_is_full(struct mhi_device *mhi_dev, enum dma_data_direction dir);

#endif 
