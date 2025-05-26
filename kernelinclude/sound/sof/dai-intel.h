/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_DAI_INTEL_H__
#define __INCLUDE_SOUND_SOF_DAI_INTEL_H__

#include <sound/sof/header.h>

 
#define SOF_DAI_INTEL_SSP_QUIRK_TINTE		(1 << 0)
 
#define SOF_DAI_INTEL_SSP_QUIRK_PINTE		(1 << 1)
 
#define SOF_DAI_INTEL_SSP_QUIRK_SMTATF		(1 << 2)
 
#define SOF_DAI_INTEL_SSP_QUIRK_MMRATF		(1 << 3)
 
#define SOF_DAI_INTEL_SSP_QUIRK_PSPSTWFDFD	(1 << 4)
 
#define SOF_DAI_INTEL_SSP_QUIRK_PSPSRWFDFD	(1 << 5)

#define SOF_DAI_INTEL_SSP_QUIRK_LBM		(1 << 6)

 

#define SOF_DAI_INTEL_SSP_FRAME_PULSE_WIDTH_MAX		38
#define SOF_DAI_INTEL_SSP_SLOT_PADDING_MAX		31




#define SOF_DAI_INTEL_SSP_MCLK_0_DISABLE		BIT(0)

#define SOF_DAI_INTEL_SSP_MCLK_1_DISABLE		BIT(1)

#define SOF_DAI_INTEL_SSP_CLKCTRL_MCLK_KA		BIT(2)

#define SOF_DAI_INTEL_SSP_CLKCTRL_BCLK_KA		BIT(3)

#define SOF_DAI_INTEL_SSP_CLKCTRL_FS_KA			BIT(4)

#define SOF_DAI_INTEL_SSP_CLKCTRL_BCLK_IDLE_HIGH	BIT(5)

#define SOF_DAI_INTEL_SSP_CLKCTRL_MCLK_ES               BIT(6)

#define SOF_DAI_INTEL_SSP_CLKCTRL_BCLK_ES               BIT(7)

#define SOF_DAI_INTEL_SSP_CLKCTRL_MCLK_AON		BIT(8)


#define SOF_DAI_INTEL_DMIC_NUM_CTRL			4


struct sof_ipc_dai_ssp_params {
	struct sof_ipc_hdr hdr;
	uint16_t reserved1;
	uint16_t mclk_id;

	uint32_t mclk_rate;	
	uint32_t fsync_rate;	
	uint32_t bclk_rate;	

	
	uint32_t tdm_slots;
	uint32_t rx_slots;
	uint32_t tx_slots;

	
	uint32_t sample_valid_bits;
	uint16_t tdm_slot_width;
	uint16_t reserved2;	

	
	uint32_t mclk_direction;

	uint16_t frame_pulse_width;
	uint16_t tdm_per_slot_padding_flag;
	uint32_t clks_control;
	uint32_t quirks;
	uint32_t bclk_delay;	
} __packed;


struct sof_ipc_dai_hda_params {
	struct sof_ipc_hdr hdr;
	uint32_t link_dma_ch;
	uint32_t rate;
	uint32_t channels;
} __packed;


struct sof_ipc_dai_alh_params {
	struct sof_ipc_hdr hdr;
	uint32_t stream_id;
	uint32_t rate;
	uint32_t channels;

	
	uint32_t reserved[13];
} __packed;




struct sof_ipc_dai_dmic_pdm_ctrl {
	struct sof_ipc_hdr hdr;
	uint16_t id;		

	uint16_t enable_mic_a;	
	uint16_t enable_mic_b;	

	uint16_t polarity_mic_a; 
	uint16_t polarity_mic_b; 

	uint16_t clk_edge;	
	uint16_t skew;		

	uint16_t reserved[3];	
} __packed;


struct sof_ipc_dai_dmic_params {
	struct sof_ipc_hdr hdr;
	uint32_t driver_ipc_version;	

	uint32_t pdmclk_min;	
	uint32_t pdmclk_max;	

	uint32_t fifo_fs;	
	uint32_t reserved_1;	
	uint16_t fifo_bits;	
	uint16_t fifo_bits_b;	

	uint16_t duty_min;	
	uint16_t duty_max;	

	uint32_t num_pdm_active; 
				 

	uint32_t wake_up_time;      
	uint32_t min_clock_on_time; 
	uint32_t unmute_ramp_time;  

	
	uint32_t reserved[5];

	
	struct sof_ipc_dai_dmic_pdm_ctrl pdm[SOF_DAI_INTEL_DMIC_NUM_CTRL];
} __packed;

#endif
