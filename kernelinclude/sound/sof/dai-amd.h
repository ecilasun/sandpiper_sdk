/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_DAI_AMD_H__
#define __INCLUDE_SOUND_SOF_DAI_AMD_H__

#include <sound/sof/header.h>


struct sof_ipc_dai_acp_params {
	struct sof_ipc_hdr hdr;

	uint32_t fsync_rate;    
	uint32_t tdm_slots;
	uint32_t tdm_mode;
} __packed;


struct sof_ipc_dai_acpdmic_params {
	uint32_t pdm_rate;
	uint32_t pdm_ch;
} __packed;

#endif
