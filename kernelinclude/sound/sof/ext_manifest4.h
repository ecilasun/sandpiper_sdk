/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause) */




#ifndef __SOF_FIRMWARE_EXT_MANIFEST4_H__
#define __SOF_FIRMWARE_EXT_MANIFEST4_H__

#include <linux/uuid.h>


#define SOF_EXT_MAN4_MAGIC_NUMBER	0x31454124

#define MAX_MODULE_NAME_LEN		8
#define MAX_FW_BINARY_NAME		8
#define DEFAULT_HASH_SHA256_LEN		32
#define SOF_MAN4_FW_HDR_OFFSET		0x2000
#define SOF_MAN4_FW_HDR_OFFSET_CAVS_1_5	0x284



struct sof_ext_manifest4_hdr {
	uint32_t id;
	uint32_t len; 
	uint16_t version_major; 
	uint16_t version_minor;
	uint32_t num_module_entries;
} __packed;

struct sof_man4_fw_binary_header {
	
	uint32_t id;
	uint32_t len; 
	uint8_t name[MAX_FW_BINARY_NAME];
	uint32_t preload_page_count; 
	uint32_t fw_image_flags;
	uint32_t feature_mask;
	uint16_t major_version; 
	uint16_t minor_version;
	uint16_t hotfix_version;
	uint16_t build_version;
	uint32_t num_module_entries;

	
	uint32_t hw_buf_base_addr;
	uint32_t hw_buf_length;
	uint32_t load_offset; 
} __packed;

struct sof_man4_segment_desc {
	uint32_t flags;
	uint32_t v_base_addr;
	uint32_t file_offset;
} __packed;

struct sof_man4_module {
	uint32_t id;
	uint8_t name[MAX_MODULE_NAME_LEN];
	guid_t uuid;
	uint32_t type;
	uint8_t hash[DEFAULT_HASH_SHA256_LEN];
	uint32_t entry_point;
	uint16_t cfg_offset;
	uint16_t cfg_count;
	uint32_t affinity_mask;
	uint16_t instance_max_count;
	uint16_t instance_stack_size;
	struct sof_man4_segment_desc	segments[3];
} __packed;

struct sof_man4_module_config {
	uint32_t par[4];	
	uint32_t is_bytes;	
	uint32_t cps;		
	uint32_t ibs;		
	uint32_t obs;		
	uint32_t module_flags;	
	uint32_t cpc;		
	uint32_t obls;		
} __packed;

#endif 
