/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause) */




#ifndef __SOF_FIRMWARE_EXT_MANIFEST_H__
#define __SOF_FIRMWARE_EXT_MANIFEST_H__

#include <linux/bits.h>
#include <linux/compiler.h>
#include <linux/types.h>
#include <sound/sof/info.h>


#define SOF_EXT_MAN_MAGIC_NUMBER	0x6e614d58


#define SOF_EXT_MAN_BUILD_VERSION(MAJOR, MINOR, PATH) ((uint32_t)( \
	((MAJOR) << 24) | \
	((MINOR) << 12) | \
	(PATH)))


#define SOF_EXT_MAN_VERSION_INCOMPATIBLE(host_ver, cli_ver) ( \
	((host_ver) & GENMASK(31, 24)) != \
	((cli_ver) & GENMASK(31, 24)))


#define SOF_EXT_MAN_VERSION		SOF_EXT_MAN_BUILD_VERSION(1, 0, 0)


struct sof_ext_man_header {
	uint32_t magic;		
				
	uint32_t full_size;	
				
	uint32_t header_size;	
				
				
	uint32_t header_version; 
				

	
} __packed;




enum sof_ext_man_elem_type {
	SOF_EXT_MAN_ELEM_FW_VERSION		= 0,
	SOF_EXT_MAN_ELEM_WINDOW			= 1,
	SOF_EXT_MAN_ELEM_CC_VERSION		= 2,
	SOF_EXT_MAN_ELEM_DBG_ABI		= 4,
	SOF_EXT_MAN_ELEM_CONFIG_DATA		= 5, 
	SOF_EXT_MAN_ELEM_PLATFORM_CONFIG_DATA   = 6,
};


struct sof_ext_man_elem_header {
	uint32_t type;		
	uint32_t size;		

	
} __packed;


struct sof_ext_man_fw_version {
	struct sof_ext_man_elem_header hdr;
	
	struct sof_ipc_fw_version version;
	uint32_t flags;
} __packed;


struct sof_ext_man_window {
	struct sof_ext_man_elem_header hdr;
	
	struct sof_ipc_window ipc_window;
} __packed;


struct sof_ext_man_cc_version {
	struct sof_ext_man_elem_header hdr;
	
	struct sof_ipc_cc_version cc_version;
} __packed;

struct ext_man_dbg_abi {
	struct sof_ext_man_elem_header hdr;
	
	struct sof_ipc_user_abi_version dbg_abi;
} __packed;


enum config_elem_type {
	SOF_EXT_MAN_CONFIG_EMPTY		= 0,
	SOF_EXT_MAN_CONFIG_IPC_MSG_SIZE		= 1,
	SOF_EXT_MAN_CONFIG_MEMORY_USAGE_SCAN	= 2, 
};

struct sof_config_elem {
	uint32_t token;
	uint32_t value;
} __packed;


struct sof_ext_man_config_data {
	struct sof_ext_man_elem_header hdr;

	struct sof_config_elem elems[];
} __packed;

#endif 
