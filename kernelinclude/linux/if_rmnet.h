/* SPDX-License-Identifier: GPL-2.0-only
 * Copyright (c) 2013-2019, 2021 The Linux Foundation. All rights reserved.
 */

#ifndef _LINUX_IF_RMNET_H_
#define _LINUX_IF_RMNET_H_

#include <linux/types.h>

struct rmnet_map_header {
	u8 flags;			
	u8 mux_id;
	__be16 pkt_len;			
}  __aligned(1);


#define MAP_PAD_LEN_MASK		GENMASK(5, 0)
#define MAP_NEXT_HEADER_FLAG		BIT(6)
#define MAP_CMD_FLAG			BIT(7)

struct rmnet_map_dl_csum_trailer {
	u8 reserved1;
	u8 flags;			
	__be16 csum_start_offset;
	__be16 csum_length;
	__sum16 csum_value;
} __aligned(1);


#define MAP_CSUM_DL_VALID_FLAG		BIT(0)

struct rmnet_map_ul_csum_header {
	__be16 csum_start_offset;
	__be16 csum_info;		
} __aligned(1);


#define MAP_CSUM_UL_OFFSET_MASK		GENMASK(13, 0)
#define MAP_CSUM_UL_UDP_FLAG		BIT(14)
#define MAP_CSUM_UL_ENABLED_FLAG	BIT(15)


struct rmnet_map_v5_csum_header {
	u8 header_info;
	u8 csum_info;
	__be16 reserved;
} __aligned(1);



#define MAPV5_HDRINFO_NXT_HDR_FLAG	BIT(0)
#define MAPV5_HDRINFO_HDR_TYPE_FMASK	GENMASK(7, 1)
#define MAPV5_CSUMINFO_VALID_FLAG	BIT(7)

#define RMNET_MAP_HEADER_TYPE_CSUM_OFFLOAD 2
#endif 
