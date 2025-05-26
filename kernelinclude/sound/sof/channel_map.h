/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __IPC_CHANNEL_MAP_H__
#define __IPC_CHANNEL_MAP_H__

#include <uapi/sound/sof/header.h>
#include <sound/sof/header.h>


struct sof_ipc_channel_map {
	uint32_t ch_index;
	uint32_t ext_id;
	uint32_t ch_mask;
	uint32_t reserved;
	int32_t ch_coeffs[];
} __packed;


struct sof_ipc_stream_map {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t num_ch_map;
	uint32_t reserved[3];
	struct sof_ipc_channel_map ch_map[];
} __packed;

#endif 
