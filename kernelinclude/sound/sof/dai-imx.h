/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_DAI_IMX_H__
#define __INCLUDE_SOUND_SOF_DAI_IMX_H__

#include <sound/sof/header.h>


struct sof_ipc_dai_esai_params {
	struct sof_ipc_hdr hdr;

	
	uint16_t reserved1;
	uint16_t mclk_id;
	uint32_t mclk_direction;

	uint32_t mclk_rate;	
	uint32_t fsync_rate;	
	uint32_t bclk_rate;	

	
	uint32_t tdm_slots;
	uint32_t rx_slots;
	uint32_t tx_slots;
	uint16_t tdm_slot_width;
	uint16_t reserved2;	
} __packed;


struct sof_ipc_dai_sai_params {
	struct sof_ipc_hdr hdr;

	
	uint16_t reserved1;
	uint16_t mclk_id;
	uint32_t mclk_direction;

	uint32_t mclk_rate;	
	uint32_t fsync_rate;	
	uint32_t bclk_rate;	

	
	uint32_t tdm_slots;
	uint32_t rx_slots;
	uint32_t tx_slots;
	uint16_t tdm_slot_width;
	uint16_t reserved2;	
} __packed;
#endif
