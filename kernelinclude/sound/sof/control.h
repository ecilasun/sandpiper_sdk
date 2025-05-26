/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_CONTROL_H__
#define __INCLUDE_SOUND_SOF_CONTROL_H__

#include <uapi/sound/sof/header.h>
#include <sound/sof/header.h>




enum sof_ipc_chmap {
	SOF_CHMAP_UNKNOWN = 0,
	SOF_CHMAP_NA,		
	SOF_CHMAP_MONO,		
	SOF_CHMAP_FL,		
	SOF_CHMAP_FR,		
	SOF_CHMAP_RL,		
	SOF_CHMAP_RR,		
	SOF_CHMAP_FC,		
	SOF_CHMAP_LFE,		
	SOF_CHMAP_SL,		
	SOF_CHMAP_SR,		
	SOF_CHMAP_RC,		
	SOF_CHMAP_FLC,		
	SOF_CHMAP_FRC,		
	SOF_CHMAP_RLC,		
	SOF_CHMAP_RRC,		
	SOF_CHMAP_FLW,		
	SOF_CHMAP_FRW,		
	SOF_CHMAP_FLH,		
	SOF_CHMAP_FCH,		
	SOF_CHMAP_FRH,		
	SOF_CHMAP_TC,		
	SOF_CHMAP_TFL,		
	SOF_CHMAP_TFR,		
	SOF_CHMAP_TFC,		
	SOF_CHMAP_TRL,		
	SOF_CHMAP_TRR,		
	SOF_CHMAP_TRC,		
	SOF_CHMAP_TFLC,		
	SOF_CHMAP_TFRC,		
	SOF_CHMAP_TSL,		
	SOF_CHMAP_TSR,		
	SOF_CHMAP_LLFE,		
	SOF_CHMAP_RLFE,		
	SOF_CHMAP_BC,		
	SOF_CHMAP_BLC,		
	SOF_CHMAP_BRC,		
	SOF_CHMAP_LAST = SOF_CHMAP_BRC,
};


enum sof_ipc_ctrl_type {
	
	SOF_CTRL_TYPE_VALUE_CHAN_GET = 0,
	SOF_CTRL_TYPE_VALUE_CHAN_SET,
	
	SOF_CTRL_TYPE_VALUE_COMP_GET,
	SOF_CTRL_TYPE_VALUE_COMP_SET,
	
	SOF_CTRL_TYPE_DATA_GET,
	SOF_CTRL_TYPE_DATA_SET,
};


enum sof_ipc_ctrl_cmd {
	SOF_CTRL_CMD_VOLUME = 0, 
	SOF_CTRL_CMD_ENUM,	
	SOF_CTRL_CMD_SWITCH,	
	SOF_CTRL_CMD_BINARY,	
};


struct sof_ipc_ctrl_value_chan {
	uint32_t channel;	
	uint32_t value;
} __packed;


struct sof_ipc_ctrl_value_comp {
	uint32_t index;	
	union {
		uint32_t uvalue;
		int32_t svalue;
	};
} __packed;


struct sof_ipc_ctrl_data {
	struct sof_ipc_reply rhdr;
	uint32_t comp_id;

	
	uint32_t type;		
	uint32_t cmd;		
	uint32_t index;		

	
	struct sof_ipc_host_buffer buffer;
	uint32_t num_elems;	
	uint32_t elems_remaining;	

	uint32_t msg_index;	

	
	uint32_t reserved[6];

	
	union {
		
		DECLARE_FLEX_ARRAY(struct sof_ipc_ctrl_value_chan, chanv);
		
		DECLARE_FLEX_ARRAY(struct sof_ipc_ctrl_value_comp, compv);
		
		DECLARE_FLEX_ARRAY(struct sof_abi_hdr, data);
	};
} __packed;


enum sof_ipc_ctrl_event_type {
	SOF_CTRL_EVENT_GENERIC = 0,	
	SOF_CTRL_EVENT_GENERIC_METADATA,	
	SOF_CTRL_EVENT_KD,	
	SOF_CTRL_EVENT_VAD,	
};


struct sof_ipc_comp_event {
	struct sof_ipc_reply rhdr;
	uint16_t src_comp_type;	
	uint32_t src_comp_id;	
	uint32_t event_type;	
	uint32_t num_elems;	

	
	uint32_t reserved[8];

	
	union {
		
		struct sof_abi_hdr data[0];
		
		uint32_t event_value;
	};
} __packed;

#endif
