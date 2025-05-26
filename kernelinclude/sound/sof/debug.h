/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_DEBUG_H__
#define __INCLUDE_SOUND_SOF_DEBUG_H__

#include <sound/sof/header.h>


enum sof_ipc_dbg_mem_zone {
	SOF_IPC_MEM_ZONE_SYS		= 0,	
	SOF_IPC_MEM_ZONE_SYS_RUNTIME	= 1,	
	SOF_IPC_MEM_ZONE_RUNTIME	= 2,	
	SOF_IPC_MEM_ZONE_BUFFER		= 3,	
	SOF_IPC_MEM_ZONE_RUNTIME_SHARED	= 4,	
	SOF_IPC_MEM_ZONE_SYS_SHARED	= 5,	
};


struct sof_ipc_dbg_mem_usage_elem {
	uint32_t zone;		
	uint32_t id;		
	uint32_t used;		
	uint32_t free;		
	uint32_t reserved;	
} __packed;


struct sof_ipc_dbg_mem_usage {
	struct sof_ipc_reply rhdr;			
	uint32_t reserved[4];				
	uint32_t num_elems;				
	struct sof_ipc_dbg_mem_usage_elem elems[];	
} __packed;

#endif
