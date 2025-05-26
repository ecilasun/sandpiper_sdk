/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _UFSHCD_H
#define _UFSHCD_H

#include <linux/bitfield.h>
#include <linux/blk-crypto-profile.h>
#include <linux/blk-mq.h>
#include <linux/devfreq.h>
#include <linux/msi.h>
#include <linux/pm_runtime.h>
#include <linux/dma-direction.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
#include <ufs/unipro.h>
#include <ufs/ufs.h>
#include <ufs/ufs_quirks.h>
#include <ufs/ufshci.h>

#define UFSHCD "ufshcd"

struct ufs_hba;

enum dev_cmd_type {
	DEV_CMD_TYPE_NOP		= 0x0,
	DEV_CMD_TYPE_QUERY		= 0x1,
	DEV_CMD_TYPE_RPMB		= 0x2,
};

enum ufs_event_type {
	
	UFS_EVT_PA_ERR = 0,
	UFS_EVT_DL_ERR,
	UFS_EVT_NL_ERR,
	UFS_EVT_TL_ERR,
	UFS_EVT_DME_ERR,

	
	UFS_EVT_AUTO_HIBERN8_ERR,
	UFS_EVT_FATAL_ERR,
	UFS_EVT_LINK_STARTUP_FAIL,
	UFS_EVT_RESUME_ERR,
	UFS_EVT_SUSPEND_ERR,
	UFS_EVT_WL_SUSP_ERR,
	UFS_EVT_WL_RES_ERR,

	
	UFS_EVT_DEV_RESET,
	UFS_EVT_HOST_RESET,
	UFS_EVT_ABORT,

	UFS_EVT_CNT,
};


struct uic_command {
	u32 command;
	u32 argument1;
	u32 argument2;
	u32 argument3;
	int cmd_active;
	struct completion done;
};


enum ufs_pm_op {
	UFS_RUNTIME_PM,
	UFS_SYSTEM_PM,
	UFS_SHUTDOWN_PM,
};


enum uic_link_state {
	UIC_LINK_OFF_STATE	= 0, 
	UIC_LINK_ACTIVE_STATE	= 1, 
	UIC_LINK_HIBERN8_STATE	= 2, 
	UIC_LINK_BROKEN_STATE	= 3, 
};

#define ufshcd_is_link_off(hba) ((hba)->uic_link_state == UIC_LINK_OFF_STATE)
#define ufshcd_is_link_active(hba) ((hba)->uic_link_state == \
				    UIC_LINK_ACTIVE_STATE)
#define ufshcd_is_link_hibern8(hba) ((hba)->uic_link_state == \
				    UIC_LINK_HIBERN8_STATE)
#define ufshcd_is_link_broken(hba) ((hba)->uic_link_state == \
				   UIC_LINK_BROKEN_STATE)
#define ufshcd_set_link_off(hba) ((hba)->uic_link_state = UIC_LINK_OFF_STATE)
#define ufshcd_set_link_active(hba) ((hba)->uic_link_state = \
				    UIC_LINK_ACTIVE_STATE)
#define ufshcd_set_link_hibern8(hba) ((hba)->uic_link_state = \
				    UIC_LINK_HIBERN8_STATE)
#define ufshcd_set_link_broken(hba) ((hba)->uic_link_state = \
				    UIC_LINK_BROKEN_STATE)

#define ufshcd_set_ufs_dev_active(h) \
	((h)->curr_dev_pwr_mode = UFS_ACTIVE_PWR_MODE)
#define ufshcd_set_ufs_dev_sleep(h) \
	((h)->curr_dev_pwr_mode = UFS_SLEEP_PWR_MODE)
#define ufshcd_set_ufs_dev_poweroff(h) \
	((h)->curr_dev_pwr_mode = UFS_POWERDOWN_PWR_MODE)
#define ufshcd_set_ufs_dev_deepsleep(h) \
	((h)->curr_dev_pwr_mode = UFS_DEEPSLEEP_PWR_MODE)
#define ufshcd_is_ufs_dev_active(h) \
	((h)->curr_dev_pwr_mode == UFS_ACTIVE_PWR_MODE)
#define ufshcd_is_ufs_dev_sleep(h) \
	((h)->curr_dev_pwr_mode == UFS_SLEEP_PWR_MODE)
#define ufshcd_is_ufs_dev_poweroff(h) \
	((h)->curr_dev_pwr_mode == UFS_POWERDOWN_PWR_MODE)
#define ufshcd_is_ufs_dev_deepsleep(h) \
	((h)->curr_dev_pwr_mode == UFS_DEEPSLEEP_PWR_MODE)


enum ufs_pm_level {
	UFS_PM_LVL_0,
	UFS_PM_LVL_1,
	UFS_PM_LVL_2,
	UFS_PM_LVL_3,
	UFS_PM_LVL_4,
	UFS_PM_LVL_5,
	UFS_PM_LVL_6,
	UFS_PM_LVL_MAX
};

struct ufs_pm_lvl_states {
	enum ufs_dev_pwr_mode dev_state;
	enum uic_link_state link_state;
};


struct ufshcd_lrb {
	struct utp_transfer_req_desc *utr_descriptor_ptr;
	struct utp_upiu_req *ucd_req_ptr;
	struct utp_upiu_rsp *ucd_rsp_ptr;
	struct ufshcd_sg_entry *ucd_prdt_ptr;

	dma_addr_t utrd_dma_addr;
	dma_addr_t ucd_req_dma_addr;
	dma_addr_t ucd_rsp_dma_addr;
	dma_addr_t ucd_prdt_dma_addr;

	struct scsi_cmnd *cmd;
	int scsi_status;

	int command_type;
	int task_tag;
	u8 lun; 
	bool intr_cmd;
	ktime_t issue_time_stamp;
	u64 issue_time_stamp_local_clock;
	ktime_t compl_time_stamp;
	u64 compl_time_stamp_local_clock;
#ifdef CONFIG_SCSI_UFS_CRYPTO
	int crypto_key_slot;
	u64 data_unit_num;
#endif

	bool req_abort_skip;
};


struct ufs_query_req {
	u8 query_func;
	struct utp_upiu_query upiu_req;
};


struct ufs_query_res {
	struct utp_upiu_query upiu_res;
};


struct ufs_query {
	struct ufs_query_req request;
	u8 *descriptor;
	struct ufs_query_res response;
};


struct ufs_dev_cmd {
	enum dev_cmd_type type;
	struct mutex lock;
	struct completion *complete;
	struct ufs_query query;
};


struct ufs_clk_info {
	struct list_head list;
	struct clk *clk;
	const char *name;
	u32 max_freq;
	u32 min_freq;
	u32 curr_freq;
	bool keep_link_active;
	bool enabled;
};

enum ufs_notify_change_status {
	PRE_CHANGE,
	POST_CHANGE,
};

struct ufs_pa_layer_attr {
	u32 gear_rx;
	u32 gear_tx;
	u32 lane_rx;
	u32 lane_tx;
	u32 pwr_rx;
	u32 pwr_tx;
	u32 hs_rate;
};

struct ufs_pwr_mode_info {
	bool is_valid;
	struct ufs_pa_layer_attr info;
};


struct ufs_hba_variant_ops {
	const char *name;
	int	(*init)(struct ufs_hba *);
	void    (*exit)(struct ufs_hba *);
	u32	(*get_ufs_hci_version)(struct ufs_hba *);
	int	(*clk_scale_notify)(struct ufs_hba *, bool,
				    enum ufs_notify_change_status);
	int	(*setup_clocks)(struct ufs_hba *, bool,
				enum ufs_notify_change_status);
	int	(*hce_enable_notify)(struct ufs_hba *,
				     enum ufs_notify_change_status);
	int	(*link_startup_notify)(struct ufs_hba *,
				       enum ufs_notify_change_status);
	int	(*pwr_change_notify)(struct ufs_hba *,
					enum ufs_notify_change_status status,
					struct ufs_pa_layer_attr *,
					struct ufs_pa_layer_attr *);
	void	(*setup_xfer_req)(struct ufs_hba *hba, int tag,
				  bool is_scsi_cmd);
	void	(*setup_task_mgmt)(struct ufs_hba *, int, u8);
	void    (*hibern8_notify)(struct ufs_hba *, enum uic_cmd_dme,
					enum ufs_notify_change_status);
	int	(*apply_dev_quirks)(struct ufs_hba *hba);
	void	(*fixup_dev_quirks)(struct ufs_hba *hba);
	int     (*suspend)(struct ufs_hba *, enum ufs_pm_op,
					enum ufs_notify_change_status);
	int     (*resume)(struct ufs_hba *, enum ufs_pm_op);
	void	(*dbg_register_dump)(struct ufs_hba *hba);
	int	(*phy_initialization)(struct ufs_hba *);
	int	(*device_reset)(struct ufs_hba *hba);
	void	(*config_scaling_param)(struct ufs_hba *hba,
				struct devfreq_dev_profile *profile,
				struct devfreq_simple_ondemand_data *data);
	int	(*program_key)(struct ufs_hba *hba,
			       const union ufs_crypto_cfg_entry *cfg, int slot);
	void	(*event_notify)(struct ufs_hba *hba,
				enum ufs_event_type evt, void *data);
	void	(*reinit_notify)(struct ufs_hba *);
	int	(*mcq_config_resource)(struct ufs_hba *hba);
	int	(*get_hba_mac)(struct ufs_hba *hba);
	int	(*op_runtime_config)(struct ufs_hba *hba);
	int	(*get_outstanding_cqs)(struct ufs_hba *hba,
				       unsigned long *ocqs);
	int	(*config_esi)(struct ufs_hba *hba);
	irqreturn_t	(*isr)(struct ufs_hba *hba, u32 intr_status);
};


enum clk_gating_state {
	CLKS_OFF,
	CLKS_ON,
	REQ_CLKS_OFF,
	REQ_CLKS_ON,
};


struct ufs_clk_gating {
	struct delayed_work gate_work;
	struct work_struct ungate_work;
	enum clk_gating_state state;
	unsigned long delay_ms;
	bool is_suspended;
	struct device_attribute delay_attr;
	struct device_attribute enable_attr;
	bool is_enabled;
	bool is_initialized;
	int active_reqs;
	struct workqueue_struct *clk_gating_workq;
};


struct ufs_clk_scaling {
	int active_reqs;
	unsigned long tot_busy_t;
	ktime_t window_start_t;
	ktime_t busy_start_t;
	struct device_attribute enable_attr;
	struct ufs_pa_layer_attr saved_pwr_info;
	struct workqueue_struct *workq;
	struct work_struct suspend_work;
	struct work_struct resume_work;
	u32 min_gear;
	bool is_enabled;
	bool is_allowed;
	bool is_initialized;
	bool is_busy_started;
	bool is_suspended;
};

#define UFS_EVENT_HIST_LENGTH 8

struct ufs_event_hist {
	int pos;
	u32 val[UFS_EVENT_HIST_LENGTH];
	u64 tstamp[UFS_EVENT_HIST_LENGTH];
	unsigned long long cnt;
};


struct ufs_stats {
	u32 last_intr_status;
	u64 last_intr_ts;

	u32 hibern8_exit_cnt;
	u64 last_hibern8_exit_tstamp;
	struct ufs_event_hist event[UFS_EVT_CNT];
};


enum ufshcd_state {
	UFSHCD_STATE_RESET,
	UFSHCD_STATE_OPERATIONAL,
	UFSHCD_STATE_EH_SCHEDULED_NON_FATAL,
	UFSHCD_STATE_EH_SCHEDULED_FATAL,
	UFSHCD_STATE_ERROR,
};

enum ufshcd_quirks {
	
	UFSHCD_QUIRK_BROKEN_INTR_AGGR			= 1 << 0,

	
	UFSHCD_QUIRK_DELAY_BEFORE_DME_CMDS		= 1 << 1,

	
	UFSHCD_QUIRK_BROKEN_LCC				= 1 << 2,

	
	UFSHCD_QUIRK_BROKEN_PA_RXHSUNTERMCAP		= 1 << 3,

	
	UFSHCD_QUIRK_DME_PEER_ACCESS_AUTO_MODE		= 1 << 4,

	
	UFSHCD_QUIRK_BROKEN_UFS_HCI_VERSION		= 1 << 5,

	
	UFSHCI_QUIRK_BROKEN_REQ_LIST_CLR		= 1 << 6,

	
	UFSHCI_QUIRK_SKIP_RESET_INTR_AGGR		= 1 << 7,

	
	UFSHCI_QUIRK_BROKEN_HCE				= 1 << 8,

	
	UFSHCD_QUIRK_PRDT_BYTE_GRAN			= 1 << 9,

	
	UFSHCD_QUIRK_BROKEN_OCS_FATAL_ERROR		= 1 << 10,

	
	UFSHCD_QUIRK_BROKEN_AUTO_HIBERN8		= 1 << 11,

	
	UFSHCI_QUIRK_SKIP_MANUAL_WB_FLUSH_CTRL		= 1 << 12,

	
	UFSHCD_QUIRK_SKIP_DEF_UNIPRO_TIMEOUT_SETTING = 1 << 13,

	
	UFSHCD_QUIRK_4KB_DMA_ALIGNMENT			= 1 << 14,

	
	UFSHCD_QUIRK_BROKEN_UIC_CMD			= 1 << 15,

	
	UFSHCD_QUIRK_SKIP_PH_CONFIGURATION		= 1 << 16,

	
	UFSHCD_QUIRK_BROKEN_64BIT_ADDRESS		= 1 << 17,

	
	UFSHCD_QUIRK_HIBERN_FASTAUTO			= 1 << 18,

	
	UFSHCD_QUIRK_REINIT_AFTER_MAX_GEAR_SWITCH       = 1 << 19,

	
	UFSHCD_QUIRK_MCQ_BROKEN_INTR			= 1 << 20,

	
	UFSHCD_QUIRK_MCQ_BROKEN_RTC			= 1 << 21,
};

enum ufshcd_caps {
	
	UFSHCD_CAP_CLK_GATING				= 1 << 0,

	
	UFSHCD_CAP_HIBERN8_WITH_CLK_GATING		= 1 << 1,

	
	UFSHCD_CAP_CLK_SCALING				= 1 << 2,

	
	UFSHCD_CAP_AUTO_BKOPS_SUSPEND			= 1 << 3,

	
	UFSHCD_CAP_INTR_AGGR				= 1 << 4,

	
	UFSHCD_CAP_KEEP_AUTO_BKOPS_ENABLED_EXCEPT_SUSPEND = 1 << 5,

	
	UFSHCD_CAP_RPM_AUTOSUSPEND			= 1 << 6,

	
	UFSHCD_CAP_WB_EN				= 1 << 7,

	
	UFSHCD_CAP_CRYPTO				= 1 << 8,

	
	UFSHCD_CAP_AGGR_POWER_COLLAPSE			= 1 << 9,

	
	UFSHCD_CAP_DEEPSLEEP				= 1 << 10,

	
	UFSHCD_CAP_TEMP_NOTIF				= 1 << 11,

	
	UFSHCD_CAP_WB_WITH_CLK_SCALING			= 1 << 12,
};

struct ufs_hba_variant_params {
	struct devfreq_dev_profile devfreq_profile;
	struct devfreq_simple_ondemand_data ondemand_data;
	u16 hba_enable_delay_us;
	u32 wb_flush_threshold;
};

struct ufs_hba_monitor {
	unsigned long chunk_size;

	unsigned long nr_sec_rw[2];
	ktime_t total_busy[2];

	unsigned long nr_req[2];
	
	ktime_t lat_sum[2];
	ktime_t lat_max[2];
	ktime_t lat_min[2];

	u32 nr_queued[2];
	ktime_t busy_start_ts[2];

	ktime_t enabled_ts;
	bool enabled;
};


struct ufshcd_res_info {
	const char *name;
	struct resource *resource;
	void __iomem *base;
};

enum ufshcd_res {
	RES_UFS,
	RES_MCQ,
	RES_MCQ_SQD,
	RES_MCQ_SQIS,
	RES_MCQ_CQD,
	RES_MCQ_CQIS,
	RES_MCQ_VS,
	RES_MAX,
};


struct ufshcd_mcq_opr_info_t {
	unsigned long offset;
	unsigned long stride;
	void __iomem *base;
};

enum ufshcd_mcq_opr {
	OPR_SQD,
	OPR_SQIS,
	OPR_CQD,
	OPR_CQIS,
	OPR_MAX,
};


struct ufs_hba {
	void __iomem *mmio_base;

	
	struct utp_transfer_cmd_desc *ucdl_base_addr;
	struct utp_transfer_req_desc *utrdl_base_addr;
	struct utp_task_req_desc *utmrdl_base_addr;

	
	dma_addr_t ucdl_dma_addr;
	dma_addr_t utrdl_dma_addr;
	dma_addr_t utmrdl_dma_addr;

	struct Scsi_Host *host;
	struct device *dev;
	struct scsi_device *ufs_device_wlun;

#ifdef CONFIG_SCSI_UFS_HWMON
	struct device *hwmon_device;
#endif

	enum ufs_dev_pwr_mode curr_dev_pwr_mode;
	enum uic_link_state uic_link_state;
	
	enum ufs_pm_level rpm_lvl;
	
	enum ufs_pm_level spm_lvl;
	int pm_op_in_progress;

	
	u32 ahit;

	struct ufshcd_lrb *lrb;

	unsigned long outstanding_tasks;
	spinlock_t outstanding_lock;
	unsigned long outstanding_reqs;

	u32 capabilities;
	int nutrs;
	u32 mcq_capabilities;
	int nutmrs;
	u32 reserved_slot;
	u32 ufs_version;
	const struct ufs_hba_variant_ops *vops;
	struct ufs_hba_variant_params *vps;
	void *priv;
#ifdef CONFIG_SCSI_UFS_VARIABLE_SG_ENTRY_SIZE
	size_t sg_entry_size;
#endif
	unsigned int irq;
	bool is_irq_enabled;
	enum ufs_ref_clk_freq dev_ref_clk_freq;

	unsigned int quirks;	

	
	unsigned int dev_quirks;

	struct blk_mq_tag_set tmf_tag_set;
	struct request_queue *tmf_queue;
	struct request **tmf_rqs;

	struct uic_command *active_uic_cmd;
	struct mutex uic_cmd_mutex;
	struct completion *uic_async_done;

	enum ufshcd_state ufshcd_state;
	u32 eh_flags;
	u32 intr_mask;
	u16 ee_ctrl_mask;
	u16 ee_drv_mask;
	u16 ee_usr_mask;
	struct mutex ee_ctrl_mutex;
	bool is_powered;
	bool shutting_down;
	struct semaphore host_sem;

	
	struct workqueue_struct *eh_wq;
	struct work_struct eh_work;
	struct work_struct eeh_work;

	
	u32 errors;
	u32 uic_error;
	u32 saved_err;
	u32 saved_uic_err;
	struct ufs_stats ufs_stats;
	bool force_reset;
	bool force_pmc;
	bool silence_err_logs;

	
	struct ufs_dev_cmd dev_cmd;
	ktime_t last_dme_cmd_tstamp;
	int nop_out_timeout;

	
	struct ufs_dev_info dev_info;
	bool auto_bkops_enabled;
	struct ufs_vreg_info vreg_info;
	struct list_head clk_list_head;

	
	int req_abort_count;

	
	u32 lanes_per_direction;
	struct ufs_pa_layer_attr pwr_info;
	struct ufs_pwr_mode_info max_pwr_info;

	struct ufs_clk_gating clk_gating;
	
	u32 caps;

	struct devfreq *devfreq;
	struct ufs_clk_scaling clk_scaling;
	bool system_suspending;
	bool is_sys_suspended;

	enum bkops_status urgent_bkops_lvl;
	bool is_urgent_bkops_lvl_checked;

	struct mutex wb_mutex;
	struct rw_semaphore clk_scaling_lock;
	atomic_t scsi_block_reqs_cnt;

	struct device		bsg_dev;
	struct request_queue	*bsg_queue;
	struct delayed_work rpm_dev_flush_recheck_work;

	struct ufs_hba_monitor	monitor;

#ifdef CONFIG_SCSI_UFS_CRYPTO
	union ufs_crypto_capabilities crypto_capabilities;
	union ufs_crypto_cap_entry *crypto_cap_array;
	u32 crypto_cfg_register;
	struct blk_crypto_profile crypto_profile;
#endif
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_root;
	struct delayed_work debugfs_ee_work;
	u32 debugfs_ee_rate_limit_ms;
#endif
	u32 luns_avail;
	unsigned int nr_hw_queues;
	unsigned int nr_queues[HCTX_MAX_TYPES];
	bool complete_put;
	bool ext_iid_sup;
	bool scsi_host_added;
	bool mcq_sup;
	bool mcq_enabled;
	struct ufshcd_res_info res[RES_MAX];
	void __iomem *mcq_base;
	struct ufs_hw_queue *uhq;
	struct ufs_hw_queue *dev_cmd_queue;
	struct ufshcd_mcq_opr_info_t mcq_opr[OPR_MAX];
};


struct ufs_hw_queue {
	void __iomem *mcq_sq_head;
	void __iomem *mcq_sq_tail;
	void __iomem *mcq_cq_head;
	void __iomem *mcq_cq_tail;

	struct utp_transfer_req_desc *sqe_base_addr;
	dma_addr_t sqe_dma_addr;
	struct cq_entry *cqe_base_addr;
	dma_addr_t cqe_dma_addr;
	u32 max_entries;
	u32 id;
	u32 sq_tail_slot;
	spinlock_t sq_lock;
	u32 cq_tail_slot;
	u32 cq_head_slot;
	spinlock_t cq_lock;
	
	struct mutex sq_mutex;
};

static inline bool is_mcq_enabled(struct ufs_hba *hba)
{
	return hba->mcq_enabled;
}

#ifdef CONFIG_SCSI_UFS_VARIABLE_SG_ENTRY_SIZE
static inline size_t ufshcd_sg_entry_size(const struct ufs_hba *hba)
{
	return hba->sg_entry_size;
}

static inline void ufshcd_set_sg_entry_size(struct ufs_hba *hba, size_t sg_entry_size)
{
	WARN_ON_ONCE(sg_entry_size < sizeof(struct ufshcd_sg_entry));
	hba->sg_entry_size = sg_entry_size;
}
#else
static inline size_t ufshcd_sg_entry_size(const struct ufs_hba *hba)
{
	return sizeof(struct ufshcd_sg_entry);
}

#define ufshcd_set_sg_entry_size(hba, sg_entry_size)                   \
	({ (void)(hba); BUILD_BUG_ON(sg_entry_size != sizeof(struct ufshcd_sg_entry)); })
#endif

static inline size_t ufshcd_get_ucd_size(const struct ufs_hba *hba)
{
	return sizeof(struct utp_transfer_cmd_desc) + SG_ALL * ufshcd_sg_entry_size(hba);
}


static inline bool ufshcd_is_clkgating_allowed(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_CLK_GATING;
}
static inline bool ufshcd_can_hibern8_during_gating(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_HIBERN8_WITH_CLK_GATING;
}
static inline int ufshcd_is_clkscaling_supported(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_CLK_SCALING;
}
static inline bool ufshcd_can_autobkops_during_suspend(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_AUTO_BKOPS_SUSPEND;
}
static inline bool ufshcd_is_rpm_autosuspend_allowed(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_RPM_AUTOSUSPEND;
}

static inline bool ufshcd_is_intr_aggr_allowed(struct ufs_hba *hba)
{
	return (hba->caps & UFSHCD_CAP_INTR_AGGR) &&
		!(hba->quirks & UFSHCD_QUIRK_BROKEN_INTR_AGGR);
}

static inline bool ufshcd_can_aggressive_pc(struct ufs_hba *hba)
{
	return !!(ufshcd_is_link_hibern8(hba) &&
		  (hba->caps & UFSHCD_CAP_AGGR_POWER_COLLAPSE));
}

static inline bool ufshcd_is_auto_hibern8_supported(struct ufs_hba *hba)
{
	return (hba->capabilities & MASK_AUTO_HIBERN8_SUPPORT) &&
		!(hba->quirks & UFSHCD_QUIRK_BROKEN_AUTO_HIBERN8);
}

static inline bool ufshcd_is_auto_hibern8_enabled(struct ufs_hba *hba)
{
	return FIELD_GET(UFSHCI_AHIBERN8_TIMER_MASK, hba->ahit);
}

static inline bool ufshcd_is_wb_allowed(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_WB_EN;
}

static inline bool ufshcd_enable_wb_if_scaling_up(struct ufs_hba *hba)
{
	return hba->caps & UFSHCD_CAP_WB_WITH_CLK_SCALING;
}

#define ufsmcq_writel(hba, val, reg)	\
	writel((val), (hba)->mcq_base + (reg))
#define ufsmcq_readl(hba, reg)	\
	readl((hba)->mcq_base + (reg))

#define ufsmcq_writelx(hba, val, reg)	\
	writel_relaxed((val), (hba)->mcq_base + (reg))
#define ufsmcq_readlx(hba, reg)	\
	readl_relaxed((hba)->mcq_base + (reg))

#define ufshcd_writel(hba, val, reg)	\
	writel((val), (hba)->mmio_base + (reg))
#define ufshcd_readl(hba, reg)	\
	readl((hba)->mmio_base + (reg))


static inline void ufshcd_rmwl(struct ufs_hba *hba, u32 mask, u32 val, u32 reg)
{
	u32 tmp;

	tmp = ufshcd_readl(hba, reg);
	tmp &= ~mask;
	tmp |= (val & mask);
	ufshcd_writel(hba, tmp, reg);
}

int ufshcd_alloc_host(struct device *, struct ufs_hba **);
void ufshcd_dealloc_host(struct ufs_hba *);
int ufshcd_hba_enable(struct ufs_hba *hba);
int ufshcd_init(struct ufs_hba *, void __iomem *, unsigned int);
int ufshcd_link_recovery(struct ufs_hba *hba);
int ufshcd_make_hba_operational(struct ufs_hba *hba);
void ufshcd_remove(struct ufs_hba *);
int ufshcd_uic_hibern8_enter(struct ufs_hba *hba);
int ufshcd_uic_hibern8_exit(struct ufs_hba *hba);
void ufshcd_delay_us(unsigned long us, unsigned long tolerance);
void ufshcd_parse_dev_ref_clk_freq(struct ufs_hba *hba, struct clk *refclk);
void ufshcd_update_evt_hist(struct ufs_hba *hba, u32 id, u32 val);
void ufshcd_hba_stop(struct ufs_hba *hba);
void ufshcd_schedule_eh_work(struct ufs_hba *hba);
void ufshcd_mcq_config_mac(struct ufs_hba *hba, u32 max_active_cmds);
u32 ufshcd_mcq_read_cqis(struct ufs_hba *hba, int i);
void ufshcd_mcq_write_cqis(struct ufs_hba *hba, u32 val, int i);
unsigned long ufshcd_mcq_poll_cqe_lock(struct ufs_hba *hba,
					 struct ufs_hw_queue *hwq);
void ufshcd_mcq_make_queues_operational(struct ufs_hba *hba);
void ufshcd_mcq_enable_esi(struct ufs_hba *hba);
void ufshcd_mcq_config_esi(struct ufs_hba *hba, struct msi_msg *msg);


static inline void ufshcd_set_variant(struct ufs_hba *hba, void *variant)
{
	BUG_ON(!hba);
	hba->priv = variant;
}


static inline void *ufshcd_get_variant(struct ufs_hba *hba)
{
	BUG_ON(!hba);
	return hba->priv;
}

#ifdef CONFIG_PM
extern int ufshcd_runtime_suspend(struct device *dev);
extern int ufshcd_runtime_resume(struct device *dev);
#endif
#ifdef CONFIG_PM_SLEEP
extern int ufshcd_system_suspend(struct device *dev);
extern int ufshcd_system_resume(struct device *dev);
extern int ufshcd_system_freeze(struct device *dev);
extern int ufshcd_system_thaw(struct device *dev);
extern int ufshcd_system_restore(struct device *dev);
#endif

extern int ufshcd_dme_configure_adapt(struct ufs_hba *hba,
				      int agreed_gear,
				      int adapt_val);
extern int ufshcd_dme_set_attr(struct ufs_hba *hba, u32 attr_sel,
			       u8 attr_set, u32 mib_val, u8 peer);
extern int ufshcd_dme_get_attr(struct ufs_hba *hba, u32 attr_sel,
			       u32 *mib_val, u8 peer);
extern int ufshcd_config_pwr_mode(struct ufs_hba *hba,
			struct ufs_pa_layer_attr *desired_pwr_mode);
extern int ufshcd_uic_change_pwr_mode(struct ufs_hba *hba, u8 mode);


#define DME_LOCAL	0
#define DME_PEER	1
#define ATTR_SET_NOR	0	
#define ATTR_SET_ST	1	

static inline int ufshcd_dme_set(struct ufs_hba *hba, u32 attr_sel,
				 u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_NOR,
				   mib_val, DME_LOCAL);
}

static inline int ufshcd_dme_st_set(struct ufs_hba *hba, u32 attr_sel,
				    u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_ST,
				   mib_val, DME_LOCAL);
}

static inline int ufshcd_dme_peer_set(struct ufs_hba *hba, u32 attr_sel,
				      u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_NOR,
				   mib_val, DME_PEER);
}

static inline int ufshcd_dme_peer_st_set(struct ufs_hba *hba, u32 attr_sel,
					 u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_ST,
				   mib_val, DME_PEER);
}

static inline int ufshcd_dme_get(struct ufs_hba *hba,
				 u32 attr_sel, u32 *mib_val)
{
	return ufshcd_dme_get_attr(hba, attr_sel, mib_val, DME_LOCAL);
}

static inline int ufshcd_dme_peer_get(struct ufs_hba *hba,
				      u32 attr_sel, u32 *mib_val)
{
	return ufshcd_dme_get_attr(hba, attr_sel, mib_val, DME_PEER);
}

static inline bool ufshcd_is_hs_mode(struct ufs_pa_layer_attr *pwr_info)
{
	return (pwr_info->pwr_rx == FAST_MODE ||
		pwr_info->pwr_rx == FASTAUTO_MODE) &&
		(pwr_info->pwr_tx == FAST_MODE ||
		pwr_info->pwr_tx == FASTAUTO_MODE);
}

static inline int ufshcd_disable_host_tx_lcc(struct ufs_hba *hba)
{
	return ufshcd_dme_set(hba, UIC_ARG_MIB(PA_LOCAL_TX_LCC_ENABLE), 0);
}

void ufshcd_auto_hibern8_enable(struct ufs_hba *hba);
void ufshcd_auto_hibern8_update(struct ufs_hba *hba, u32 ahit);
void ufshcd_fixup_dev_quirks(struct ufs_hba *hba,
			     const struct ufs_dev_quirk *fixups);
#define SD_ASCII_STD true
#define SD_RAW false
int ufshcd_read_string_desc(struct ufs_hba *hba, u8 desc_index,
			    u8 **buf, bool ascii);

void ufshcd_hold(struct ufs_hba *hba);
void ufshcd_release(struct ufs_hba *hba);

void ufshcd_clkgate_delay_set(struct device *dev, unsigned long value);

u32 ufshcd_get_local_unipro_ver(struct ufs_hba *hba);

int ufshcd_get_vreg(struct device *dev, struct ufs_vreg *vreg);

int ufshcd_send_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd);

int ufshcd_advanced_rpmb_req_handler(struct ufs_hba *hba, struct utp_upiu_req *req_upiu,
				     struct utp_upiu_req *rsp_upiu, struct ufs_ehs *ehs_req,
				     struct ufs_ehs *ehs_rsp, int sg_cnt,
				     struct scatterlist *sg_list, enum dma_data_direction dir);
int ufshcd_wb_toggle(struct ufs_hba *hba, bool enable);
int ufshcd_wb_toggle_buf_flush(struct ufs_hba *hba, bool enable);
int ufshcd_suspend_prepare(struct device *dev);
int __ufshcd_suspend_prepare(struct device *dev, bool rpm_ok_for_spm);
void ufshcd_resume_complete(struct device *dev);
bool ufshcd_is_hba_active(struct ufs_hba *hba);


static inline int ufshcd_vops_init(struct ufs_hba *hba)
{
	if (hba->vops && hba->vops->init)
		return hba->vops->init(hba);

	return 0;
}

static inline int ufshcd_vops_phy_initialization(struct ufs_hba *hba)
{
	if (hba->vops && hba->vops->phy_initialization)
		return hba->vops->phy_initialization(hba);

	return 0;
}

extern const struct ufs_pm_lvl_states ufs_pm_lvl_states[];

int ufshcd_dump_regs(struct ufs_hba *hba, size_t offset, size_t len,
		     const char *prefix);

int __ufshcd_write_ee_control(struct ufs_hba *hba, u32 ee_ctrl_mask);
int ufshcd_write_ee_control(struct ufs_hba *hba);
int ufshcd_update_ee_control(struct ufs_hba *hba, u16 *mask,
			     const u16 *other_mask, u16 set, u16 clr);

#endif 
