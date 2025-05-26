/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_STREAM_H__
#define __INCLUDE_SOUND_SOF_STREAM_H__

#include <sound/sof/header.h>



#define SOF_IPC_MAX_CHANNELS			8


#define SOF_RATE_8000		(1 <<  0) 
#define SOF_RATE_11025		(1 <<  1) 
#define SOF_RATE_12000		(1 <<  2) 
#define SOF_RATE_16000		(1 <<  3) 
#define SOF_RATE_22050		(1 <<  4) 
#define SOF_RATE_24000		(1 <<  5) 
#define SOF_RATE_32000		(1 <<  6) 
#define SOF_RATE_44100		(1 <<  7) 
#define SOF_RATE_48000		(1 <<  8) 
#define SOF_RATE_64000		(1 <<  9) 
#define SOF_RATE_88200		(1 << 10) 
#define SOF_RATE_96000		(1 << 11) 
#define SOF_RATE_176400		(1 << 12) 
#define SOF_RATE_192000		(1 << 13) 


#define SOF_RATE_CONTINUOUS	(1 << 30)  
#define SOF_RATE_KNOT		(1 << 31)  


#define SOF_PCM_FLAG_XRUN_STOP	(1 << 0) 


enum sof_ipc_frame {
	SOF_IPC_FRAME_S16_LE = 0,
	SOF_IPC_FRAME_S24_4LE,
	SOF_IPC_FRAME_S32_LE,
	SOF_IPC_FRAME_FLOAT,
	
};


enum sof_ipc_buffer_format {
	SOF_IPC_BUFFER_INTERLEAVED,
	SOF_IPC_BUFFER_NONINTERLEAVED,
	
};


enum sof_ipc_stream_direction {
	SOF_IPC_STREAM_PLAYBACK = 0,
	SOF_IPC_STREAM_CAPTURE,
};


struct sof_ipc_host_buffer {
	struct sof_ipc_hdr hdr;
	uint32_t phy_addr;
	uint32_t pages;
	uint32_t size;
	uint32_t reserved[3];
} __packed;

struct sof_ipc_stream_params {
	struct sof_ipc_hdr hdr;
	struct sof_ipc_host_buffer buffer;
	uint32_t direction;	
	uint32_t frame_fmt;	
	uint32_t buffer_fmt;	
	uint32_t rate;
	uint16_t stream_tag;
	uint16_t channels;
	uint16_t sample_valid_bytes;
	uint16_t sample_container_bytes;

	uint32_t host_period_bytes;
	uint16_t no_stream_position; 
	uint8_t cont_update_posn; 
	uint8_t reserved0;
	int16_t ext_data_length; 
	uint8_t reserved[2];
	uint16_t chmap[SOF_IPC_MAX_CHANNELS];	
	uint8_t ext_data[]; 
} __packed;


struct sof_ipc_pcm_params {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t comp_id;
	uint32_t flags;		
	uint32_t reserved[2];
	struct sof_ipc_stream_params params;
}  __packed;


struct sof_ipc_pcm_params_reply {
	struct sof_ipc_reply rhdr;
	uint32_t comp_id;
	uint32_t posn_offset;
} __packed;


struct sof_ipc_stream {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t comp_id;
} __packed;


#define	SOF_TIME_HOST_SYNC	(1 << 0)
#define	SOF_TIME_DAI_SYNC	(1 << 1)
#define	SOF_TIME_WALL_SYNC	(1 << 2)
#define	SOF_TIME_STAMP_SYNC	(1 << 3)


#define	SOF_TIME_HOST_VALID	(1 << 8)
#define	SOF_TIME_DAI_VALID	(1 << 9)
#define	SOF_TIME_WALL_VALID	(1 << 10)
#define	SOF_TIME_STAMP_VALID	(1 << 11)


#define	SOF_TIME_HOST_64	(1 << 16)
#define	SOF_TIME_DAI_64		(1 << 17)
#define	SOF_TIME_WALL_64	(1 << 18)
#define	SOF_TIME_STAMP_64	(1 << 19)

struct sof_ipc_stream_posn {
	struct sof_ipc_reply rhdr;
	uint32_t comp_id;	
	uint32_t flags;		
	uint32_t wallclock_hz;	
	uint32_t timestamp_ns;	
	uint64_t host_posn;	
	uint64_t dai_posn;	
	uint64_t comp_posn;	
	uint64_t wallclock;	
	uint64_t timestamp;	
	uint32_t xrun_comp_id;	
	int32_t xrun_size;	
}  __packed;

#endif
