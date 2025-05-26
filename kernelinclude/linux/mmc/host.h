/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LINUX_MMC_HOST_H
#define LINUX_MMC_HOST_H

#include <linux/sched.h>
#include <linux/device.h>
#include <linux/fault-inject.h>

#include <linux/mmc/core.h>
#include <linux/mmc/card.h>
#include <linux/mmc/pm.h>
#include <linux/dma-direction.h>
#include <linux/blk-crypto-profile.h>

struct mmc_ios {
	unsigned int	clock;			
	unsigned short	vdd;
	unsigned int	power_delay_ms;		



	unsigned char	bus_mode;		

#define MMC_BUSMODE_OPENDRAIN	1
#define MMC_BUSMODE_PUSHPULL	2

	unsigned char	chip_select;		

#define MMC_CS_DONTCARE		0
#define MMC_CS_HIGH		1
#define MMC_CS_LOW		2

	unsigned char	power_mode;		

#define MMC_POWER_OFF		0
#define MMC_POWER_UP		1
#define MMC_POWER_ON		2
#define MMC_POWER_UNDEFINED	3

	unsigned char	bus_width;		

#define MMC_BUS_WIDTH_1		0
#define MMC_BUS_WIDTH_4		2
#define MMC_BUS_WIDTH_8		3

	unsigned char	timing;			

#define MMC_TIMING_LEGACY	0
#define MMC_TIMING_MMC_HS	1
#define MMC_TIMING_SD_HS	2
#define MMC_TIMING_UHS_SDR12	3
#define MMC_TIMING_UHS_SDR25	4
#define MMC_TIMING_UHS_SDR50	5
#define MMC_TIMING_UHS_SDR104	6
#define MMC_TIMING_UHS_DDR50	7
#define MMC_TIMING_MMC_DDR52	8
#define MMC_TIMING_MMC_HS200	9
#define MMC_TIMING_MMC_HS400	10
#define MMC_TIMING_SD_EXP	11
#define MMC_TIMING_SD_EXP_1_2V	12

	unsigned char	signal_voltage;		

#define MMC_SIGNAL_VOLTAGE_330	0
#define MMC_SIGNAL_VOLTAGE_180	1
#define MMC_SIGNAL_VOLTAGE_120	2

	unsigned char	drv_type;		

#define MMC_SET_DRIVER_TYPE_B	0
#define MMC_SET_DRIVER_TYPE_A	1
#define MMC_SET_DRIVER_TYPE_C	2
#define MMC_SET_DRIVER_TYPE_D	3

	bool enhanced_strobe;			
};

struct mmc_clk_phase {
	bool valid;
	u16 in_deg;
	u16 out_deg;
};

#define MMC_NUM_CLK_PHASES (MMC_TIMING_MMC_HS400 + 1)
struct mmc_clk_phase_map {
	struct mmc_clk_phase phase[MMC_NUM_CLK_PHASES];
};

struct mmc_host;

enum mmc_err_stat {
	MMC_ERR_CMD_TIMEOUT,
	MMC_ERR_CMD_CRC,
	MMC_ERR_DAT_TIMEOUT,
	MMC_ERR_DAT_CRC,
	MMC_ERR_AUTO_CMD,
	MMC_ERR_ADMA,
	MMC_ERR_TUNING,
	MMC_ERR_CMDQ_RED,
	MMC_ERR_CMDQ_GCE,
	MMC_ERR_CMDQ_ICCE,
	MMC_ERR_REQ_TIMEOUT,
	MMC_ERR_CMDQ_REQ_TIMEOUT,
	MMC_ERR_ICE_CFG,
	MMC_ERR_CTRL_TIMEOUT,
	MMC_ERR_UNEXPECTED_IRQ,
	MMC_ERR_MAX,
};

struct mmc_host_ops {
	
	void	(*post_req)(struct mmc_host *host, struct mmc_request *req,
			    int err);
	void	(*pre_req)(struct mmc_host *host, struct mmc_request *req);
	void	(*request)(struct mmc_host *host, struct mmc_request *req);
	
	int	(*request_atomic)(struct mmc_host *host,
				  struct mmc_request *req);

	

	
	void	(*set_ios)(struct mmc_host *host, struct mmc_ios *ios);

	
	int	(*get_ro)(struct mmc_host *host);

	
	int	(*get_cd)(struct mmc_host *host);

	void	(*enable_sdio_irq)(struct mmc_host *host, int enable);
	
	void	(*ack_sdio_irq)(struct mmc_host *host);

	
	void	(*init_card)(struct mmc_host *host, struct mmc_card *card);

	int	(*start_signal_voltage_switch)(struct mmc_host *host, struct mmc_ios *ios);

	
	int	(*card_busy)(struct mmc_host *host);

	
	int	(*execute_tuning)(struct mmc_host *host, u32 opcode);

	
	int	(*prepare_hs400_tuning)(struct mmc_host *host, struct mmc_ios *ios);

	
	int	(*execute_hs400_tuning)(struct mmc_host *host, struct mmc_card *card);

	
	int	(*prepare_sd_hs_tuning)(struct mmc_host *host, struct mmc_card *card);

	
	int	(*execute_sd_hs_tuning)(struct mmc_host *host, struct mmc_card *card);

	
	int	(*hs400_prepare_ddr)(struct mmc_host *host);

	
	void	(*hs400_downgrade)(struct mmc_host *host);

	
	void	(*hs400_complete)(struct mmc_host *host);

	
	void	(*hs400_enhanced_strobe)(struct mmc_host *host,
					 struct mmc_ios *ios);
	int	(*select_drive_strength)(struct mmc_card *card,
					 unsigned int max_dtr, int host_drv,
					 int card_drv, int *drv_type);
	
	void	(*card_hw_reset)(struct mmc_host *host);
	void	(*card_event)(struct mmc_host *host);

	
	int	(*multi_io_quirk)(struct mmc_card *card,
				  unsigned int direction, int blk_size);

	
	int	(*init_sd_express)(struct mmc_host *host, struct mmc_ios *ios);
};

struct mmc_cqe_ops {
	
	int	(*cqe_enable)(struct mmc_host *host, struct mmc_card *card);
	
	void	(*cqe_disable)(struct mmc_host *host);
	
	int	(*cqe_request)(struct mmc_host *host, struct mmc_request *mrq);
	
	void	(*cqe_post_req)(struct mmc_host *host, struct mmc_request *mrq);
	
	void	(*cqe_off)(struct mmc_host *host);
	
	int	(*cqe_wait_for_idle)(struct mmc_host *host);
	
	bool	(*cqe_timeout)(struct mmc_host *host, struct mmc_request *mrq,
			       bool *recovery_needed);
	
	void	(*cqe_recovery_start)(struct mmc_host *host);
	
	void	(*cqe_recovery_finish)(struct mmc_host *host);
};

struct mmc_async_req {
	
	struct mmc_request	*mrq;
	
	enum mmc_blk_status (*err_check)(struct mmc_card *, struct mmc_async_req *);
};


struct mmc_slot {
	int cd_irq;
	bool cd_wake_enabled;
	void *handler_priv;
};


struct mmc_context_info {
	bool			is_done_rcv;
	bool			is_new_req;
	bool			is_waiting_last_req;
	wait_queue_head_t	wait;
};

struct regulator;
struct mmc_pwrseq;

struct mmc_supply {
	struct regulator *vmmc;		
	struct regulator *vqmmc;	
};

struct mmc_ctx {
	struct task_struct *task;
};

struct mmc_host {
	struct device		*parent;
	struct device		class_dev;
	int			index;
	const struct mmc_host_ops *ops;
	struct mmc_pwrseq	*pwrseq;
	unsigned int		f_min;
	unsigned int		f_max;
	unsigned int		f_init;
	u32			ocr_avail;
	u32			ocr_avail_sdio;	
	u32			ocr_avail_sd;	
	u32			ocr_avail_mmc;	
	struct wakeup_source	*ws;		
	u32			max_current_330;
	u32			max_current_300;
	u32			max_current_180;

#define MMC_VDD_165_195		0x00000080	
#define MMC_VDD_20_21		0x00000100	
#define MMC_VDD_21_22		0x00000200	
#define MMC_VDD_22_23		0x00000400	
#define MMC_VDD_23_24		0x00000800	
#define MMC_VDD_24_25		0x00001000	
#define MMC_VDD_25_26		0x00002000	
#define MMC_VDD_26_27		0x00004000	
#define MMC_VDD_27_28		0x00008000	
#define MMC_VDD_28_29		0x00010000	
#define MMC_VDD_29_30		0x00020000	
#define MMC_VDD_30_31		0x00040000	
#define MMC_VDD_31_32		0x00080000	
#define MMC_VDD_32_33		0x00100000	
#define MMC_VDD_33_34		0x00200000	
#define MMC_VDD_34_35		0x00400000	
#define MMC_VDD_35_36		0x00800000	

	u32			caps;		

#define MMC_CAP_4_BIT_DATA	(1 << 0)	
#define MMC_CAP_MMC_HIGHSPEED	(1 << 1)	
#define MMC_CAP_SD_HIGHSPEED	(1 << 2)	
#define MMC_CAP_SDIO_IRQ	(1 << 3)	
#define MMC_CAP_SPI		(1 << 4)	
#define MMC_CAP_NEEDS_POLL	(1 << 5)	
#define MMC_CAP_8_BIT_DATA	(1 << 6)	
#define MMC_CAP_AGGRESSIVE_PM	(1 << 7)	
#define MMC_CAP_NONREMOVABLE	(1 << 8)	
#define MMC_CAP_WAIT_WHILE_BUSY	(1 << 9)	
#define MMC_CAP_3_3V_DDR	(1 << 11)	
#define MMC_CAP_1_8V_DDR	(1 << 12)	
#define MMC_CAP_1_2V_DDR	(1 << 13)	
#define MMC_CAP_DDR		(MMC_CAP_3_3V_DDR | MMC_CAP_1_8V_DDR | \
				 MMC_CAP_1_2V_DDR)
#define MMC_CAP_POWER_OFF_CARD	(1 << 14)	
#define MMC_CAP_BUS_WIDTH_TEST	(1 << 15)	
#define MMC_CAP_UHS_SDR12	(1 << 16)	
#define MMC_CAP_UHS_SDR25	(1 << 17)	
#define MMC_CAP_UHS_SDR50	(1 << 18)	
#define MMC_CAP_UHS_SDR104	(1 << 19)	
#define MMC_CAP_UHS_DDR50	(1 << 20)	
#define MMC_CAP_UHS		(MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 | \
				 MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_SDR104 | \
				 MMC_CAP_UHS_DDR50)
#define MMC_CAP_SYNC_RUNTIME_PM	(1 << 21)	
#define MMC_CAP_NEED_RSP_BUSY	(1 << 22)	
#define MMC_CAP_DRIVER_TYPE_A	(1 << 23)	
#define MMC_CAP_DRIVER_TYPE_C	(1 << 24)	
#define MMC_CAP_DRIVER_TYPE_D	(1 << 25)	
#define MMC_CAP_DONE_COMPLETE	(1 << 27)	
#define MMC_CAP_CD_WAKE		(1 << 28)	
#define MMC_CAP_CMD_DURING_TFR	(1 << 29)	
#define MMC_CAP_CMD23		(1 << 30)	
#define MMC_CAP_HW_RESET	(1 << 31)	

	u32			caps2;		

#define MMC_CAP2_BOOTPART_NOACC	(1 << 0)	
#define MMC_CAP2_FULL_PWR_CYCLE	(1 << 2)	
#define MMC_CAP2_FULL_PWR_CYCLE_IN_SUSPEND (1 << 3) 
#define MMC_CAP2_HS200_1_8V_SDR	(1 << 5)        
#define MMC_CAP2_HS200_1_2V_SDR	(1 << 6)        
#define MMC_CAP2_HS200		(MMC_CAP2_HS200_1_8V_SDR | \
				 MMC_CAP2_HS200_1_2V_SDR)
#define MMC_CAP2_SD_EXP		(1 << 7)	
#define MMC_CAP2_SD_EXP_1_2V	(1 << 8)	
#define MMC_CAP2_CD_ACTIVE_HIGH	(1 << 10)	
#define MMC_CAP2_RO_ACTIVE_HIGH	(1 << 11)	
#define MMC_CAP2_NO_PRESCAN_POWERUP (1 << 14)	
#define MMC_CAP2_HS400_1_8V	(1 << 15)	
#define MMC_CAP2_HS400_1_2V	(1 << 16)	
#define MMC_CAP2_HS400		(MMC_CAP2_HS400_1_8V | \
				 MMC_CAP2_HS400_1_2V)
#define MMC_CAP2_HSX00_1_8V	(MMC_CAP2_HS200_1_8V_SDR | MMC_CAP2_HS400_1_8V)
#define MMC_CAP2_HSX00_1_2V	(MMC_CAP2_HS200_1_2V_SDR | MMC_CAP2_HS400_1_2V)
#define MMC_CAP2_SDIO_IRQ_NOTHREAD (1 << 17)
#define MMC_CAP2_NO_WRITE_PROTECT (1 << 18)	
#define MMC_CAP2_NO_SDIO	(1 << 19)	
#define MMC_CAP2_HS400_ES	(1 << 20)	
#define MMC_CAP2_NO_SD		(1 << 21)	
#define MMC_CAP2_NO_MMC		(1 << 22)	
#define MMC_CAP2_CQE		(1 << 23)	
#define MMC_CAP2_CQE_DCMD	(1 << 24)	
#define MMC_CAP2_AVOID_3_3V	(1 << 25)	
#define MMC_CAP2_MERGE_CAPABLE	(1 << 26)	
#ifdef CONFIG_MMC_CRYPTO
#define MMC_CAP2_CRYPTO		(1 << 27)	
#else
#define MMC_CAP2_CRYPTO		0
#endif
#define MMC_CAP2_ALT_GPT_TEGRA	(1 << 28)	

	int			fixed_drv_type;	

	mmc_pm_flag_t		pm_caps;	

	
	unsigned int		max_seg_size;	
	unsigned short		max_segs;	
	unsigned short		unused;
	unsigned int		max_req_size;	
	unsigned int		max_blk_size;	
	unsigned int		max_blk_count;	
	unsigned int		max_busy_timeout; 

	
	spinlock_t		lock;		

	struct mmc_ios		ios;		

	
	unsigned int		use_spi_crc:1;
	unsigned int		claimed:1;	
	unsigned int		doing_init_tune:1; 
	unsigned int		can_retune:1;	
	unsigned int		doing_retune:1;	
	unsigned int		retune_now:1;	
	unsigned int		retune_paused:1; 
	unsigned int		retune_crc_disable:1; 
	unsigned int		can_dma_map_merge:1; 
	unsigned int		vqmmc_enabled:1; 

	int			rescan_disable;	
	int			rescan_entered;	

	int			need_retune;	
	int			hold_retune;	
	unsigned int		retune_period;	
	struct timer_list	retune_timer;	

	bool			trigger_card_event; 

	struct mmc_card		*card;		

	wait_queue_head_t	wq;
	struct mmc_ctx		*claimer;	
	int			claim_cnt;	
	struct mmc_ctx		default_ctx;	

	struct delayed_work	detect;
	int			detect_change;	
	struct mmc_slot		slot;

	const struct mmc_bus_ops *bus_ops;	

	unsigned int		sdio_irqs;
	struct task_struct	*sdio_irq_thread;
	struct work_struct	sdio_irq_work;
	bool			sdio_irq_pending;
	atomic_t		sdio_irq_thread_abort;

	mmc_pm_flag_t		pm_flags;	

	struct led_trigger	*led;		

#ifdef CONFIG_REGULATOR
	bool			regulator_enabled; 
#endif
	struct mmc_supply	supply;

	struct dentry		*debugfs_root;

	
	struct mmc_request	*ongoing_mrq;

#ifdef CONFIG_FAIL_MMC_REQUEST
	struct fault_attr	fail_mmc_request;
#endif

	unsigned int		actual_clock;	

	unsigned int		slotno;	

	int			dsr_req;	
	u32			dsr;	

	
	const struct mmc_cqe_ops *cqe_ops;
	void			*cqe_private;
	int			cqe_qdepth;
	bool			cqe_enabled;
	bool			cqe_on;

	
#ifdef CONFIG_MMC_CRYPTO
	struct blk_crypto_profile crypto_profile;
#endif

	
	bool			hsq_enabled;

	u32			err_stats[MMC_ERR_MAX];
	unsigned long		private[] ____cacheline_aligned;
};

struct device_node;

struct mmc_host *mmc_alloc_host(int extra, struct device *);
struct mmc_host *devm_mmc_alloc_host(struct device *dev, int extra);
int mmc_add_host(struct mmc_host *);
void mmc_remove_host(struct mmc_host *);
void mmc_free_host(struct mmc_host *);
void mmc_of_parse_clk_phase(struct mmc_host *host,
			    struct mmc_clk_phase_map *map);
int mmc_of_parse(struct mmc_host *host);
int mmc_of_parse_voltage(struct mmc_host *host, u32 *mask);

static inline void *mmc_priv(struct mmc_host *host)
{
	return (void *)host->private;
}

static inline struct mmc_host *mmc_from_priv(void *priv)
{
	return container_of(priv, struct mmc_host, private);
}

#define mmc_host_is_spi(host)	((host)->caps & MMC_CAP_SPI)

#define mmc_dev(x)	((x)->parent)
#define mmc_classdev(x)	(&(x)->class_dev)
#define mmc_hostname(x)	(dev_name(&(x)->class_dev))

void mmc_detect_change(struct mmc_host *, unsigned long delay);
void mmc_request_done(struct mmc_host *, struct mmc_request *);
void mmc_command_done(struct mmc_host *host, struct mmc_request *mrq);

void mmc_cqe_request_done(struct mmc_host *host, struct mmc_request *mrq);


static inline bool sdio_irq_claimed(struct mmc_host *host)
{
	return host->sdio_irqs > 0;
}

static inline void mmc_signal_sdio_irq(struct mmc_host *host)
{
	host->ops->enable_sdio_irq(host, 0);
	host->sdio_irq_pending = true;
	if (host->sdio_irq_thread)
		wake_up_process(host->sdio_irq_thread);
}

void sdio_signal_irq(struct mmc_host *host);

#ifdef CONFIG_REGULATOR
int mmc_regulator_set_ocr(struct mmc_host *mmc,
			struct regulator *supply,
			unsigned short vdd_bit);
int mmc_regulator_set_vqmmc(struct mmc_host *mmc, struct mmc_ios *ios);
#else
static inline int mmc_regulator_set_ocr(struct mmc_host *mmc,
				 struct regulator *supply,
				 unsigned short vdd_bit)
{
	return 0;
}

static inline int mmc_regulator_set_vqmmc(struct mmc_host *mmc,
					  struct mmc_ios *ios)
{
	return -EINVAL;
}
#endif

int mmc_regulator_get_supply(struct mmc_host *mmc);
int mmc_regulator_enable_vqmmc(struct mmc_host *mmc);
void mmc_regulator_disable_vqmmc(struct mmc_host *mmc);

static inline int mmc_card_is_removable(struct mmc_host *host)
{
	return !(host->caps & MMC_CAP_NONREMOVABLE);
}

static inline int mmc_card_keep_power(struct mmc_host *host)
{
	return host->pm_flags & MMC_PM_KEEP_POWER;
}

static inline int mmc_card_wake_sdio_irq(struct mmc_host *host)
{
	return host->pm_flags & MMC_PM_WAKE_SDIO_IRQ;
}


static inline int mmc_card_hs(struct mmc_card *card)
{
	return card->host->ios.timing == MMC_TIMING_SD_HS ||
		card->host->ios.timing == MMC_TIMING_MMC_HS;
}


static inline int mmc_card_uhs(struct mmc_card *card)
{
	return card->host->ios.timing >= MMC_TIMING_UHS_SDR12 &&
		card->host->ios.timing <= MMC_TIMING_UHS_DDR50;
}

void mmc_retune_timer_stop(struct mmc_host *host);

static inline void mmc_retune_needed(struct mmc_host *host)
{
	if (host->can_retune)
		host->need_retune = 1;
}

static inline bool mmc_can_retune(struct mmc_host *host)
{
	return host->can_retune == 1;
}

static inline bool mmc_doing_retune(struct mmc_host *host)
{
	return host->doing_retune == 1;
}

static inline bool mmc_doing_tune(struct mmc_host *host)
{
	return host->doing_retune == 1 || host->doing_init_tune == 1;
}

static inline enum dma_data_direction mmc_get_dma_dir(struct mmc_data *data)
{
	return data->flags & MMC_DATA_WRITE ? DMA_TO_DEVICE : DMA_FROM_DEVICE;
}

static inline void mmc_debugfs_err_stats_inc(struct mmc_host *host,
					     enum mmc_err_stat stat)
{
	host->err_stats[stat] += 1;
}

int mmc_sd_switch(struct mmc_card *card, int mode, int group, u8 value, u8 *resp);
int mmc_send_status(struct mmc_card *card, u32 *status);
int mmc_send_tuning(struct mmc_host *host, u32 opcode, int *cmd_error);
int mmc_send_abort_tuning(struct mmc_host *host, u32 opcode);
int mmc_get_ext_csd(struct mmc_card *card, u8 **new_ext_csd);

#endif 
