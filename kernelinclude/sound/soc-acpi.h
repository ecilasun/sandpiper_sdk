/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2013-15, Intel Corporation. All rights reserved.
 */

#ifndef __LINUX_SND_SOC_ACPI_H
#define __LINUX_SND_SOC_ACPI_H

#include <linux/stddef.h>
#include <linux/acpi.h>
#include <linux/mod_devicetable.h>
#include <linux/soundwire/sdw.h>

struct snd_soc_acpi_package_context {
	char *name;           
	int length;           
	struct acpi_buffer *format;
	struct acpi_buffer *state;
	bool data_valid;
};


#define SND_ACPI_I2C_ID_LEN (4 + ACPI_ID_LEN + 3 + 1)

#if IS_ENABLED(CONFIG_ACPI)

struct snd_soc_acpi_mach *
snd_soc_acpi_find_machine(struct snd_soc_acpi_mach *machines);

bool snd_soc_acpi_find_package_from_hid(const u8 hid[ACPI_ID_LEN],
				    struct snd_soc_acpi_package_context *ctx);


struct snd_soc_acpi_mach *snd_soc_acpi_codec_list(void *arg);

#else

static inline struct snd_soc_acpi_mach *
snd_soc_acpi_find_machine(struct snd_soc_acpi_mach *machines)
{
	return NULL;
}

static inline bool
snd_soc_acpi_find_package_from_hid(const u8 hid[ACPI_ID_LEN],
				   struct snd_soc_acpi_package_context *ctx)
{
	return false;
}


static inline struct snd_soc_acpi_mach *snd_soc_acpi_codec_list(void *arg)
{
	return NULL;
}
#endif


struct snd_soc_acpi_mach_params {
	u32 acpi_ipc_irq_index;
	const char *platform;
	u32 codec_mask;
	u32 dmic_num;
	bool common_hdmi_codec_drv;
	u32 link_mask;
	const struct snd_soc_acpi_link_adr *links;
	u32 i2s_link_mask;
	u32 num_dai_drivers;
	struct snd_soc_dai_driver *dai_drivers;
	unsigned short subsystem_vendor;
	unsigned short subsystem_device;
	bool subsystem_id_set;
};


struct snd_soc_acpi_endpoint {
	u8 num;
	u8 aggregated;
	u8 group_position;
	u8 group_id;
};


struct snd_soc_acpi_adr_device {
	const u64 adr;
	const u8 num_endpoints;
	const struct snd_soc_acpi_endpoint *endpoints;
	const char *name_prefix;
};



struct snd_soc_acpi_link_adr {
	const u32 mask;
	const u32 num_adr;
	const struct snd_soc_acpi_adr_device *adr_d;
};


#define SND_SOC_ACPI_TPLG_INTEL_SSP_NUMBER BIT(0)


#define SND_SOC_ACPI_TPLG_INTEL_SSP_MSB BIT(1)


#define SND_SOC_ACPI_TPLG_INTEL_DMIC_NUMBER BIT(2)



struct snd_soc_acpi_mach {
	u8 id[ACPI_ID_LEN];
	const char *uid;
	const struct snd_soc_acpi_codecs *comp_ids;
	const u32 link_mask;
	const struct snd_soc_acpi_link_adr *links;
	const char *drv_name;
	const char *fw_filename;
	const char *tplg_filename;
	const char *board;
	struct snd_soc_acpi_mach * (*machine_quirk)(void *arg);
	const void *quirk_data;
	void *pdata;
	struct snd_soc_acpi_mach_params mach_params;
	const char *sof_tplg_filename;
	const u32 tplg_quirk_mask;
};

#define SND_SOC_ACPI_MAX_CODECS 3


struct snd_soc_acpi_codecs {
	int num_codecs;
	u8 codecs[SND_SOC_ACPI_MAX_CODECS][ACPI_ID_LEN];
};

static inline bool snd_soc_acpi_sof_parent(struct device *dev)
{
	return dev->parent && dev->parent->driver && dev->parent->driver->name &&
		!strncmp(dev->parent->driver->name, "sof-audio-acpi", strlen("sof-audio-acpi"));
}

bool snd_soc_acpi_sdw_link_slaves_found(struct device *dev,
					const struct snd_soc_acpi_link_adr *link,
					struct sdw_extended_slave_id *ids,
					int num_slaves);

#endif
