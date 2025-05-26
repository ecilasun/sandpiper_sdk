/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_H
#define __INCLUDE_SOUND_SOF_H

#include <linux/pci.h>
#include <sound/soc.h>
#include <sound/soc-acpi.h>

struct snd_sof_dsp_ops;
struct snd_sof_dev;


enum sof_fw_state {
	SOF_FW_BOOT_NOT_STARTED = 0,
	SOF_DSPLESS_MODE,
	SOF_FW_BOOT_PREPARE,
	SOF_FW_BOOT_IN_PROGRESS,
	SOF_FW_BOOT_FAILED,
	SOF_FW_BOOT_READY_FAILED,
	SOF_FW_BOOT_READY_OK,
	SOF_FW_BOOT_COMPLETE,
	SOF_FW_CRASHED,
};


enum sof_dsp_power_states {
	SOF_DSP_PM_D0,
	SOF_DSP_PM_D1,
	SOF_DSP_PM_D2,
	SOF_DSP_PM_D3,
};


enum sof_ipc_type {
	SOF_IPC_TYPE_3,
	SOF_IPC_TYPE_4,
	SOF_IPC_TYPE_COUNT
};

#define SOF_IPC		SOF_IPC_TYPE_3
#define SOF_INTEL_IPC4	SOF_IPC_TYPE_4


struct snd_sof_pdata {
	const char *name;
	const char *platform;

	
	unsigned short subsystem_vendor;
	unsigned short subsystem_device;
	bool subsystem_id_set;

	struct device *dev;

	
	void (*sof_probe_complete)(struct device *dev);

	
	const struct sof_dev_desc *desc;

	
	const char *fw_filename_prefix;
	const char *fw_filename;
	const char *tplg_filename_prefix;
	const char *tplg_filename;

	
	const char *fw_lib_prefix;

	
	struct platform_device *pdev_mach;
	const struct snd_soc_acpi_mach *machine;
	const struct snd_sof_of_mach *of_machine;

	void *hw_pdata;

	enum sof_ipc_type ipc_type;
};


struct sof_dev_desc {
	
	struct snd_soc_acpi_mach *machines;
	struct snd_sof_of_mach *of_machines;

	
	struct snd_soc_acpi_mach *alt_machines;

	bool use_acpi_target_states;

	
	
	int resindex_lpe_base;
	int resindex_pcicfg_base;
	int resindex_imr_base;
	int irqindex_host_ipc;

	
	int ipc_timeout;
	int boot_timeout;

	
	const void *chip_info;

	
	const char *nocodec_tplg_filename;

	
	unsigned int ipc_supported_mask;
	enum sof_ipc_type ipc_default;

	
	bool dspless_mode_supported;

	
	const char *default_fw_path[SOF_IPC_TYPE_COUNT];
	const char *default_lib_path[SOF_IPC_TYPE_COUNT];
	const char *default_tplg_path[SOF_IPC_TYPE_COUNT];

	
	const char *default_fw_filename[SOF_IPC_TYPE_COUNT];

	struct snd_sof_dsp_ops *ops;
	int (*ops_init)(struct snd_sof_dev *sdev);
	void (*ops_free)(struct snd_sof_dev *sdev);
};

int sof_dai_get_mclk(struct snd_soc_pcm_runtime *rtd);
int sof_dai_get_bclk(struct snd_soc_pcm_runtime *rtd);

#endif
