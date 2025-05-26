/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */


#ifndef __SDW_INTEL_H
#define __SDW_INTEL_H

#include <linux/irqreturn.h>
#include <linux/soundwire/sdw.h>



#define SDW_SHIM_BASE			0x2C000
#define SDW_ALH_BASE			0x2C800
#define SDW_SHIM_BASE_ACE		0x38000
#define SDW_ALH_BASE_ACE		0x24000
#define SDW_LINK_BASE			0x30000
#define SDW_LINK_SIZE			0x10000



#define SDW_SHIM_LCAP			0x0
#define SDW_SHIM_LCAP_LCOUNT_MASK	GENMASK(2, 0)


#define SDW_SHIM_LCTL			0x4

#define SDW_SHIM_LCTL_SPA		BIT(0)
#define SDW_SHIM_LCTL_SPA_MASK		GENMASK(3, 0)
#define SDW_SHIM_LCTL_CPA		BIT(8)
#define SDW_SHIM_LCTL_CPA_MASK		GENMASK(11, 8)


#define SDW_SHIM_SYNC			0xC

#define SDW_SHIM_SYNC_SYNCPRD_VAL_24	(24000 / SDW_CADENCE_GSYNC_KHZ - 1)
#define SDW_SHIM_SYNC_SYNCPRD_VAL_38_4	(38400 / SDW_CADENCE_GSYNC_KHZ - 1)
#define SDW_SHIM_SYNC_SYNCPRD		GENMASK(14, 0)
#define SDW_SHIM_SYNC_SYNCCPU		BIT(15)
#define SDW_SHIM_SYNC_CMDSYNC_MASK	GENMASK(19, 16)
#define SDW_SHIM_SYNC_CMDSYNC		BIT(16)
#define SDW_SHIM_SYNC_SYNCGO		BIT(24)


#define SDW_SHIM_CTLSCAP(x)		(0x010 + 0x60 * (x))
#define SDW_SHIM_CTLS0CM(x)		(0x012 + 0x60 * (x))
#define SDW_SHIM_CTLS1CM(x)		(0x014 + 0x60 * (x))
#define SDW_SHIM_CTLS2CM(x)		(0x016 + 0x60 * (x))
#define SDW_SHIM_CTLS3CM(x)		(0x018 + 0x60 * (x))


#define SDW_SHIM_PCMSCAP(x)		(0x020 + 0x60 * (x))

#define SDW_SHIM_PCMSCAP_ISS		GENMASK(3, 0)
#define SDW_SHIM_PCMSCAP_OSS		GENMASK(7, 4)
#define SDW_SHIM_PCMSCAP_BSS		GENMASK(12, 8)


#define SDW_SHIM_PCMSYCHM(x, y)		(0x022 + (0x60 * (x)) + (0x2 * (y)))


#define SDW_SHIM_PCMSYCHC(x, y)		(0x042 + (0x60 * (x)) + (0x2 * (y)))

#define SDW_SHIM_PCMSYCM_LCHN		GENMASK(3, 0)
#define SDW_SHIM_PCMSYCM_HCHN		GENMASK(7, 4)
#define SDW_SHIM_PCMSYCM_STREAM		GENMASK(13, 8)
#define SDW_SHIM_PCMSYCM_DIR		BIT(15)


#define SDW_SHIM_IOCTL(x)		(0x06C + 0x60 * (x))

#define SDW_SHIM_IOCTL_MIF		BIT(0)
#define SDW_SHIM_IOCTL_CO		BIT(1)
#define SDW_SHIM_IOCTL_COE		BIT(2)
#define SDW_SHIM_IOCTL_DO		BIT(3)
#define SDW_SHIM_IOCTL_DOE		BIT(4)
#define SDW_SHIM_IOCTL_BKE		BIT(5)
#define SDW_SHIM_IOCTL_WPDD		BIT(6)
#define SDW_SHIM_IOCTL_CIBD		BIT(8)
#define SDW_SHIM_IOCTL_DIBD		BIT(9)


#define SDW_SHIM_WAKEEN			0x190

#define SDW_SHIM_WAKEEN_ENABLE		BIT(0)


#define SDW_SHIM_WAKESTS		0x192

#define SDW_SHIM_WAKESTS_STATUS		BIT(0)


#define SDW_SHIM_CTMCTL(x)		(0x06E + 0x60 * (x))

#define SDW_SHIM_CTMCTL_DACTQE		BIT(0)
#define SDW_SHIM_CTMCTL_DODS		BIT(1)
#define SDW_SHIM_CTMCTL_DOAIS		GENMASK(4, 3)


#define SDW_ALH_STRMZCFG(x)		(0x000 + (0x4 * (x)))
#define SDW_ALH_NUM_STREAMS		64

#define SDW_ALH_STRMZCFG_DMAT_VAL	0x3
#define SDW_ALH_STRMZCFG_DMAT		GENMASK(7, 0)
#define SDW_ALH_STRMZCFG_CHN		GENMASK(19, 16)



#define SDW_SHIM2_GENERIC_BASE(x)	(0x00030000 + 0x8000 * (x))
#define SDW_IP_BASE(x)			(0x00030100 + 0x8000 * (x))
#define SDW_SHIM2_VS_BASE(x)		(0x00036000 + 0x8000 * (x))



#define SDW_SHIM2_LECAP			0x00
#define SDW_SHIM2_LECAP_HDS		BIT(0)		
#define SDW_SHIM2_LECAP_MLC		GENMASK(3, 1)	


#define SDW_SHIM2_PCMSCAP		0x10
#define SDW_SHIM2_PCMSCAP_ISS		GENMASK(3, 0)	
#define SDW_SHIM2_PCMSCAP_OSS		GENMASK(7, 4)	
#define SDW_SHIM2_PCMSCAP_BSS		GENMASK(12, 8)	


#define SDW_SHIM2_PCMSYCHC(y)		(0x14 + (0x4 * (y)))
#define SDW_SHIM2_PCMSYCHC_CS		GENMASK(3, 0)	


#define SDW_SHIM2_PCMSYCHM(y)		(0x16 + (0x4 * (y)))
#define SDW_SHIM2_PCMSYCHM_LCHAN	GENMASK(3, 0)	
#define SDW_SHIM2_PCMSYCHM_HCHAN	GENMASK(7, 4)	
#define SDW_SHIM2_PCMSYCHM_STRM		GENMASK(13, 8)	
#define SDW_SHIM2_PCMSYCHM_DIR		BIT(15)		


#define SDW_SHIM2_INTEL_VS_LVSCTL	0x04
#define SDW_SHIM2_INTEL_VS_LVSCTL_FCG	BIT(26)
#define SDW_SHIM2_INTEL_VS_LVSCTL_MLCS	GENMASK(29, 27)
#define SDW_SHIM2_INTEL_VS_LVSCTL_DCGD	BIT(30)
#define SDW_SHIM2_INTEL_VS_LVSCTL_ICGD	BIT(31)

#define SDW_SHIM2_MLCS_XTAL_CLK		0x0
#define SDW_SHIM2_MLCS_CARDINAL_CLK	0x1
#define SDW_SHIM2_MLCS_AUDIO_PLL_CLK	0x2
#define SDW_SHIM2_MLCS_MCLK_INPUT_CLK	0x3
#define SDW_SHIM2_MLCS_WOV_RING_OSC_CLK 0x4

#define SDW_SHIM2_INTEL_VS_WAKEEN	0x08
#define SDW_SHIM2_INTEL_VS_WAKEEN_PWE	BIT(0)

#define SDW_SHIM2_INTEL_VS_WAKESTS	0x0A
#define SDW_SHIM2_INTEL_VS_WAKEEN_PWS	BIT(0)

#define SDW_SHIM2_INTEL_VS_IOCTL	0x0C
#define SDW_SHIM2_INTEL_VS_IOCTL_MIF	BIT(0)
#define SDW_SHIM2_INTEL_VS_IOCTL_CO	BIT(1)
#define SDW_SHIM2_INTEL_VS_IOCTL_COE	BIT(2)
#define SDW_SHIM2_INTEL_VS_IOCTL_DO	BIT(3)
#define SDW_SHIM2_INTEL_VS_IOCTL_DOE	BIT(4)
#define SDW_SHIM2_INTEL_VS_IOCTL_BKE	BIT(5)
#define SDW_SHIM2_INTEL_VS_IOCTL_WPDD	BIT(6)
#define SDW_SHIM2_INTEL_VS_IOCTL_ODC	BIT(7)
#define SDW_SHIM2_INTEL_VS_IOCTL_CIBD	BIT(8)
#define SDW_SHIM2_INTEL_VS_IOCTL_DIBD	BIT(9)
#define SDW_SHIM2_INTEL_VS_IOCTL_HAMIFD	BIT(10)

#define SDW_SHIM2_INTEL_VS_ACTMCTL	0x0E
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DACTQE	BIT(0)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DODS		BIT(1)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DODSE	BIT(2)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DOAIS	GENMASK(4, 3)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DOAISE	BIT(5)


struct sdw_intel_stream_params_data {
	struct snd_pcm_substream *substream;
	struct snd_soc_dai *dai;
	struct snd_pcm_hw_params *hw_params;
	int link_id;
	int alh_stream_id;
};


struct sdw_intel_stream_free_data {
	struct snd_pcm_substream *substream;
	struct snd_soc_dai *dai;
	int link_id;
};


struct sdw_intel_ops {
	int (*params_stream)(struct device *dev,
			     struct sdw_intel_stream_params_data *params_data);
	int (*free_stream)(struct device *dev,
			   struct sdw_intel_stream_free_data *free_data);
	int (*trigger)(struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai);
};


struct sdw_intel_acpi_info {
	acpi_handle handle;
	int count;
	u32 link_mask;
};

struct sdw_intel_link_dev;




#define SDW_INTEL_CLK_STOP_NOT_ALLOWED		BIT(0)


#define SDW_INTEL_CLK_STOP_TEARDOWN		BIT(1)


#define SDW_INTEL_CLK_STOP_WAKE_CAPABLE_ONLY	BIT(2)


#define SDW_INTEL_CLK_STOP_BUS_RESET		BIT(3)

struct hdac_bus;


struct sdw_intel_ctx {
	int count;
	void __iomem *mmio_base;
	u32 link_mask;
	int num_slaves;
	acpi_handle handle;
	struct sdw_intel_link_dev **ldev;
	struct sdw_extended_slave_id *ids;
	struct list_head link_list;
	struct mutex shim_lock; 
	u32 shim_mask;
	u32 shim_base;
	u32 alh_base;
};


struct sdw_intel_res {
	const struct sdw_intel_hw_ops *hw_ops;
	int count;
	void __iomem *mmio_base;
	int irq;
	acpi_handle handle;
	struct device *parent;
	const struct sdw_intel_ops *ops;
	struct device *dev;
	u32 link_mask;
	u32 clock_stop_quirks;
	u32 shim_base;
	u32 alh_base;
	bool ext;
	struct hdac_bus *hbus;
	struct mutex *eml_lock;
};


int sdw_intel_acpi_scan(acpi_handle *parent_handle,
			struct sdw_intel_acpi_info *info);

void sdw_intel_process_wakeen_event(struct sdw_intel_ctx *ctx);

struct sdw_intel_ctx *
sdw_intel_probe(struct sdw_intel_res *res);

int sdw_intel_startup(struct sdw_intel_ctx *ctx);

void sdw_intel_exit(struct sdw_intel_ctx *ctx);

irqreturn_t sdw_intel_thread(int irq, void *dev_id);

#define SDW_INTEL_QUIRK_MASK_BUS_DISABLE      BIT(1)

struct sdw_intel;


struct sdw_intel_hw_ops {
	void (*debugfs_init)(struct sdw_intel *sdw);
	void (*debugfs_exit)(struct sdw_intel *sdw);

	int (*register_dai)(struct sdw_intel *sdw);

	void (*check_clock_stop)(struct sdw_intel *sdw);
	int (*start_bus)(struct sdw_intel *sdw);
	int (*start_bus_after_reset)(struct sdw_intel *sdw);
	int (*start_bus_after_clock_stop)(struct sdw_intel *sdw);
	int (*stop_bus)(struct sdw_intel *sdw, bool clock_stop);

	int (*link_power_up)(struct sdw_intel *sdw);
	int (*link_power_down)(struct sdw_intel *sdw);

	int  (*shim_check_wake)(struct sdw_intel *sdw);
	void (*shim_wake)(struct sdw_intel *sdw, bool wake_enable);

	int (*pre_bank_switch)(struct sdw_intel *sdw);
	int (*post_bank_switch)(struct sdw_intel *sdw);

	void (*sync_arm)(struct sdw_intel *sdw);
	int (*sync_go_unlocked)(struct sdw_intel *sdw);
	int (*sync_go)(struct sdw_intel *sdw);
	bool (*sync_check_cmdsync_unlocked)(struct sdw_intel *sdw);

	void (*program_sdi)(struct sdw_intel *sdw, int dev_num);
};

extern const struct sdw_intel_hw_ops sdw_intel_cnl_hw_ops;
extern const struct sdw_intel_hw_ops sdw_intel_lnl_hw_ops;



#define SDW_INTEL_DEV_NUM_IDA_MIN           6

#endif
