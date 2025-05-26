/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_TOPOLOGY_H__
#define __INCLUDE_SOUND_SOF_TOPOLOGY_H__

#include <sound/sof/header.h>




enum sof_comp_type {
	SOF_COMP_NONE = 0,
	SOF_COMP_HOST,
	SOF_COMP_DAI,
	SOF_COMP_SG_HOST,	
	SOF_COMP_SG_DAI,	
	SOF_COMP_VOLUME,
	SOF_COMP_MIXER,
	SOF_COMP_MUX,
	SOF_COMP_SRC,
	SOF_COMP_DEPRECATED0, 
	SOF_COMP_TONE,
	SOF_COMP_DEPRECATED1, 
	SOF_COMP_BUFFER,
	SOF_COMP_EQ_IIR,
	SOF_COMP_EQ_FIR,
	SOF_COMP_KEYWORD_DETECT,
	SOF_COMP_KPB,			
	SOF_COMP_SELECTOR,		
	SOF_COMP_DEMUX,
	SOF_COMP_ASRC,		
	SOF_COMP_DCBLOCK,
	SOF_COMP_SMART_AMP,             
	
	SOF_COMP_FILEREAD = 10000,	
	SOF_COMP_FILEWRITE = 10001,	
};


#define SOF_XRUN_STOP		1	
#define SOF_XRUN_UNDER_ZERO	2	
#define SOF_XRUN_OVER_NULL	4	


struct sof_ipc_comp {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t id;
	enum sof_comp_type type;
	uint32_t pipeline_id;
	uint32_t core;

	
	uint32_t ext_data_length;
} __packed;




#define SOF_MEM_CAPS_RAM			(1 << 0)
#define SOF_MEM_CAPS_ROM			(1 << 1)
#define SOF_MEM_CAPS_EXT			(1 << 2) 
#define SOF_MEM_CAPS_LP			(1 << 3) 
#define SOF_MEM_CAPS_HP			(1 << 4) 
#define SOF_MEM_CAPS_DMA			(1 << 5) 
#define SOF_MEM_CAPS_CACHE			(1 << 6) 
#define SOF_MEM_CAPS_EXEC			(1 << 7) 


#define SOF_BUF_OVERRUN_PERMITTED	BIT(0)


#define SOF_BUF_UNDERRUN_PERMITTED	BIT(1)


struct sof_ipc_buffer {
	struct sof_ipc_comp comp;
	uint32_t size;		
	uint32_t caps;		
	uint32_t flags;		
	uint32_t reserved;	
} __packed;


struct sof_ipc_comp_config {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t periods_sink;	
	uint32_t periods_source;
	uint32_t reserved1;	
	uint32_t frame_fmt;	
	uint32_t xrun_action;

	
	uint32_t reserved[2];
} __packed;


struct sof_ipc_comp_host {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	uint32_t direction;	
	uint32_t no_irq;	
	uint32_t dmac_config; 
}  __packed;


struct sof_ipc_comp_dai {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	uint32_t direction;	
	uint32_t dai_index;	
	uint32_t type;		
	uint32_t reserved;	
}  __packed;


struct sof_ipc_comp_mixer {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
}  __packed;


enum sof_volume_ramp {
	SOF_VOLUME_LINEAR	= 0,
	SOF_VOLUME_LOG,
	SOF_VOLUME_LINEAR_ZC,
	SOF_VOLUME_LOG_ZC,
};


struct sof_ipc_comp_volume {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	uint32_t channels;
	uint32_t min_value;
	uint32_t max_value;
	uint32_t ramp;		
	uint32_t initial_ramp;	
}  __packed;


struct sof_ipc_comp_src {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	
	uint32_t source_rate;	
	uint32_t sink_rate;	
	uint32_t rate_mask;	
} __packed;


struct sof_ipc_comp_asrc {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	
	uint32_t source_rate;		
					
					
	uint32_t sink_rate;		
					
					
	uint32_t asynchronous_mode;	
					
					
	uint32_t operation_mode;	
					
					
					
					
					
					

	
	uint32_t reserved[4];
} __attribute__((packed));


struct sof_ipc_comp_mux {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
} __packed;


struct sof_ipc_comp_tone {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	int32_t sample_rate;
	int32_t frequency;
	int32_t amplitude;
	int32_t freq_mult;
	int32_t ampl_mult;
	int32_t length;
	int32_t period;
	int32_t repeats;
	int32_t ramp_step;
} __packed;


enum sof_ipc_process_type {
	SOF_PROCESS_NONE = 0,		
	SOF_PROCESS_EQFIR,		
	SOF_PROCESS_EQIIR,		
	SOF_PROCESS_KEYWORD_DETECT,	
	SOF_PROCESS_KPB,		
	SOF_PROCESS_CHAN_SELECTOR,	
	SOF_PROCESS_MUX,
	SOF_PROCESS_DEMUX,
	SOF_PROCESS_DCBLOCK,
	SOF_PROCESS_SMART_AMP,	
};


struct sof_ipc_comp_process {
	struct sof_ipc_comp comp;
	struct sof_ipc_comp_config config;
	uint32_t size;	
	uint32_t type;	

	
	uint32_t reserved[7];

	uint8_t data[];
} __packed;


struct sof_ipc_free {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t id;
} __packed;

struct sof_ipc_comp_reply {
	struct sof_ipc_reply rhdr;
	uint32_t id;
	uint32_t offset;
} __packed;




enum sof_ipc_pipe_sched_time_domain {
	SOF_TIME_DOMAIN_DMA = 0,	
	SOF_TIME_DOMAIN_TIMER,		
};


struct sof_ipc_pipe_new {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t comp_id;	
	uint32_t pipeline_id;	
	uint32_t sched_id;	
	uint32_t core;		
	uint32_t period;	
	uint32_t priority;	
	uint32_t period_mips;	
	uint32_t frames_per_sched;
	uint32_t xrun_limit_usecs; 
	uint32_t time_domain;	
}  __packed;


struct sof_ipc_pipe_ready {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t comp_id;
}  __packed;

struct sof_ipc_pipe_free {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t comp_id;
}  __packed;


struct sof_ipc_pipe_comp_connect {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t source_id;
	uint32_t sink_id;
}  __packed;


enum sof_event_types {
	SOF_EVENT_NONE = 0,
	SOF_KEYWORD_DETECT_DAPM_EVENT,
};

#endif
