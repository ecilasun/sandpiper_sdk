/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef __INCLUDE_SOUND_SOF_PM_H__
#define __INCLUDE_SOUND_SOF_PM_H__

#include <sound/sof/header.h>




struct sof_ipc_pm_ctx_elem {
	struct sof_ipc_hdr hdr;
	uint32_t type;
	uint32_t size;
	uint64_t addr;
}  __packed;


struct sof_ipc_pm_ctx {
	struct sof_ipc_cmd_hdr hdr;
	struct sof_ipc_host_buffer buffer;
	uint32_t num_elems;
	uint32_t size;

	
	uint32_t reserved[8];

	struct sof_ipc_pm_ctx_elem elems[];
} __packed;


struct sof_ipc_pm_core_config {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t enable_mask;
} __packed;

struct sof_ipc_pm_gate {
	struct sof_ipc_cmd_hdr hdr;
	uint32_t flags;	

	
	uint32_t reserved[5];
} __packed;

#endif
