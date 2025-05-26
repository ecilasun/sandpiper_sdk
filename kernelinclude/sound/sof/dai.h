/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_DAI_H__
#define __INCLUDE_SOUND_SOF_DAI_H__

#include <sound/sof/header.h>
#include <sound/sof/dai-intel.h>
#include <sound/sof/dai-imx.h>
#include <sound/sof/dai-amd.h>
#include <sound/sof/dai-mediatek.h>



#define SOF_DAI_FMT_I2S		1 
#define SOF_DAI_FMT_RIGHT_J	2 
#define SOF_DAI_FMT_LEFT_J	3 
#define SOF_DAI_FMT_DSP_A	4 
#define SOF_DAI_FMT_DSP_B	5 
#define SOF_DAI_FMT_PDM		6 

#define SOF_DAI_FMT_CONT	(1 << 4) 
#define SOF_DAI_FMT_GATED	(0 << 4) 

#define SOF_DAI_FMT_NB_NF	(0 << 8) 
#define SOF_DAI_FMT_NB_IF	(2 << 8) 
#define SOF_DAI_FMT_IB_NF	(3 << 8) 
#define SOF_DAI_FMT_IB_IF	(4 << 8) 

#define SOF_DAI_FMT_CBP_CFP	(0 << 12) 
#define SOF_DAI_FMT_CBC_CFP	(2 << 12) 
#define SOF_DAI_FMT_CBP_CFC	(3 << 12) 
#define SOF_DAI_FMT_CBC_CFC	(4 << 12) 


#define SOF_DAI_FMT_CBM_CFM	SOF_DAI_FMT_CBP_CFP
#define SOF_DAI_FMT_CBS_CFM	SOF_DAI_FMT_CBC_CFP
#define SOF_DAI_FMT_CBM_CFS	SOF_DAI_FMT_CBP_CFC
#define SOF_DAI_FMT_CBS_CFS	SOF_DAI_FMT_CBC_CFC

#define SOF_DAI_FMT_FORMAT_MASK		0x000f
#define SOF_DAI_FMT_CLOCK_MASK		0x00f0
#define SOF_DAI_FMT_INV_MASK		0x0f00
#define SOF_DAI_FMT_CLOCK_PROVIDER_MASK	0xf000


#define SOF_DAI_CONFIG_FLAGS_CMD_MASK	0xF
#define SOF_DAI_CONFIG_FLAGS_NONE	0 
#define SOF_DAI_CONFIG_FLAGS_HW_PARAMS	BIT(0) 
#define SOF_DAI_CONFIG_FLAGS_HW_FREE	BIT(1) 

#define SOF_DAI_CONFIG_FLAGS_PAUSE	BIT(2)
#define SOF_DAI_CONFIG_FLAGS_QUIRK_SHIFT 4
#define SOF_DAI_CONFIG_FLAGS_QUIRK_MASK  (0xF << SOF_DAI_CONFIG_FLAGS_QUIRK_SHIFT)

#define SOF_DAI_CONFIG_FLAGS_2_STEP_STOP BIT(0)


enum sof_ipc_dai_type {
	SOF_DAI_INTEL_NONE = 0,		
	SOF_DAI_INTEL_SSP,		
	SOF_DAI_INTEL_DMIC,		
	SOF_DAI_INTEL_HDA,		
	SOF_DAI_INTEL_ALH,		
	SOF_DAI_IMX_SAI,		
	SOF_DAI_IMX_ESAI,		
	SOF_DAI_AMD_BT,			
	SOF_DAI_AMD_SP,			
	SOF_DAI_AMD_DMIC,		
	SOF_DAI_MEDIATEK_AFE,		
	SOF_DAI_AMD_HS,			
	SOF_DAI_AMD_SP_VIRTUAL,		
	SOF_DAI_AMD_HS_VIRTUAL,		
};


struct sof_ipc_dai_config {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t type;		
	uint32_t dai_index;	

	
	uint16_t format;	
	uint8_t group_id;	
	uint8_t flags;		

	
	uint32_t reserved[8];

	
	union {
		struct sof_ipc_dai_ssp_params ssp;
		struct sof_ipc_dai_dmic_params dmic;
		struct sof_ipc_dai_hda_params hda;
		struct sof_ipc_dai_alh_params alh;
		struct sof_ipc_dai_esai_params esai;
		struct sof_ipc_dai_sai_params sai;
		struct sof_ipc_dai_acp_params acpbt;
		struct sof_ipc_dai_acp_params acpsp;
		struct sof_ipc_dai_acpdmic_params acpdmic;
		struct sof_ipc_dai_acp_params acphs;
		struct sof_ipc_dai_mtk_afe_params afe;
	};
} __packed;

struct sof_dai_private_data {
	struct sof_ipc_comp_dai *comp_dai;
	struct sof_ipc_dai_config *dai_config;
};

#endif
