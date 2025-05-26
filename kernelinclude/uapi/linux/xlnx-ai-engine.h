/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */


#ifndef _UAPI_AI_ENGINE_H_
#define _UAPI_AI_ENGINE_H_


#include <linux/ioctl.h>
#include <linux/types.h>

enum aie_reg_op {
	AIE_REG_WRITE,
	AIE_REG_BLOCKWRITE,
	AIE_REG_BLOCKSET,
	AIE_REG_MASKWRITE,
	AIE_REG_MASKPOLL,
	AIE_CONFIG_SHIMDMA_BD,
	AIE_CONFIG_SHIMDMA_DMABUF_BD,
};


enum aie_module_type {
	AIE_MEM_MOD,
	AIE_CORE_MOD,
	AIE_PL_MOD,
	AIE_NOC_MOD,
};


enum aie_rsc_type {
	AIE_RSCTYPE_PERF,
	AIE_RSCTYPE_USEREVENT,
	AIE_RSCTYPE_TRACECONTROL,
	AIE_RSCTYPE_PCEVENT,
	AIE_RSCTYPE_SSSELECT,
	AIE_RSCTYPE_BROADCAST,
	AIE_RSCTYPE_COMBOEVENT,
	AIE_RSCTYPE_GROUPEVENTS,
	AIE_RSCTYPE_MAX
};


enum aie_part_status {
	XAIE_PART_STATUS_IDLE,
	XAIE_PART_STATUS_INUSE,
	XAIE_PART_STATUS_INVALID,
};



#define XAIE_PART_NOT_RST_ON_RELEASE	0x00000001U



#define XAIE_RSC_PATTERN_BLOCK		(1U << 0)


#define XAIE_BROADCAST_ID_ANY		0xFFFFFFFFU


#define XAIE_BROADCAST_ALL		(1U << 0)


struct aie_location {
	__u32 col;
	__u32 row;
};


struct aie_location_byte {
	__u8 row;
	__u8 col;
};


struct aie_range {
	struct aie_location start;
	struct aie_location size;
};


struct aie_mem {
	struct aie_range range;
	__kernel_size_t offset;
	__kernel_size_t size;
	int fd;
};


struct aie_mem_args {
	unsigned int num_mems;
	struct aie_mem *mems;
};


struct aie_reg_args {
	enum aie_reg_op op;
	__u32 mask;
	__u64 offset;
	__u32 val;
	__u64 dataptr;
	__u32 len;
};


struct aie_range_args {
	__u32 partition_id;
	__u32 uid;
	struct aie_range range;
	__u32 status;
};


struct aie_partition_query {
	struct aie_range_args *partitions;
	__u32 partition_cnt;
};

#define AIE_PART_ID_START_COL_SHIFT	0U
#define AIE_PART_ID_NUM_COLS_SHIFT	8U
#define AIE_PART_ID_START_COL_MASK	(0xFFU << AIE_PART_ID_START_COL_SHIFT)
#define AIE_PART_ID_NUM_COLS_MASK	(0xFFU << AIE_PART_ID_NUM_COLS_SHIFT)

#define aie_part_id_get_val(part_id, F) \
	(((part_id) & AIE_PART_ID_##F ##_MASK) >> AIE_PART_ID_##F ##_SHIFT)
#define aie_part_id_get_start_col(part_id) \
	aie_part_id_get_val((part_id), START_COL)
#define aie_part_id_get_num_cols(part_id) \
	aie_part_id_get_val((part_id), NUM_COLS)


struct aie_partition_req {
	__u32 partition_id;
	__u32 uid;
	__u64 meta_data;
	__u32 flag;
};


struct aie_partition_init_args {
	struct aie_location *locs;
	__u32 num_tiles;
	__u32 init_opts;
};


#define AIE_PART_INIT_OPT_COLUMN_RST		(1U << 0)
#define AIE_PART_INIT_OPT_SHIM_RST		(1U << 1)
#define AIE_PART_INIT_OPT_BLOCK_NOCAXIMMERR	(1U << 2)
#define AIE_PART_INIT_OPT_ISOLATE		(1U << 3)
#define AIE_PART_INIT_OPT_ZEROIZEMEM		(1U << 4)
#define AIE_PART_INIT_OPT_DEFAULT		0xFU


struct aie_dma_bd_args {
	__u32 *bd;
	__u64 data_va;
	struct aie_location loc;
	__u32 bd_id;
};


struct aie_dmabuf_bd_args {
	__u32 *bd;
	struct aie_location loc;
	int buf_fd;
	__u32 bd_id;
};


struct aie_tiles_array {
	struct aie_location *locs;
	__u32 num_tiles;
};


struct aie_column_args {
	__u32 start_col;
	__u32 num_cols;
	__u8 enable;
};


struct aie_part_fd {
	struct aie_column_args args;
	__u32 partition_id;
	__u32 uid;
	int fd;
};


struct aie_part_fd_list {
	struct aie_part_fd *list;
	int num_entries;
};


struct aie_txn_inst {
	__u32 num_cmds;
	__u64 cmdsptr;
};


struct aie_rsc_req {
	struct aie_location loc;
	__u32 mod;
	__u32 type;
	__u32 num_rscs;
	__u8 flag;
};


struct aie_rsc {
	struct aie_location_byte loc;
	__u32 mod;
	__u32 type;
	__u32 id;
};


struct aie_rsc_req_rsp {
	struct aie_rsc_req req;
	__u64 rscs;
};


struct aie_rsc_bc_req {
	__u64 rscs;
	__u32 num_rscs;
	__u32 flag;
	__u32 id;
};


#define AIE_RSC_STAT_TYPE_STATIC	0U
#define AIE_RSC_STAT_TYPE_AVAIL		1U
#define AIE_RSC_STAT_TYPE_MAX		2U


struct aie_rsc_user_stat {
	struct aie_location_byte loc;
	__u8 mod;
	__u8 type;
	__u8 num_rscs;
} __attribute__((packed, aligned(4)));


struct aie_rsc_user_stat_array {
	__u64 stats;
	__u32 num_stats;
	__u32 stats_type;
};

#define AIE_IOCTL_BASE 'A'


#define AIE_ENQUIRE_PART_IOCTL		_IOWR(AIE_IOCTL_BASE, 0x1, \
					      struct aie_partition_query)
#define AIE_REQUEST_PART_IOCTL		_IOR(AIE_IOCTL_BASE, 0x2, \
					     struct aie_partition_req)
#define AIE_GET_PARTITION_FD_LIST_IOCTL	_IOWR(AIE_IOCTL_BASE, 0x3, \
					     struct aie_part_fd_list)



#define AIE_PARTITION_INIT_IOCTL	_IOW(AIE_IOCTL_BASE, 0x3, \
					     struct aie_partition_init_args)


#define AIE_PARTITION_TEAR_IOCTL	_IO(AIE_IOCTL_BASE, 0x4)


#define AIE_PARTITION_CLR_CONTEXT_IOCTL _IO(AIE_IOCTL_BASE, 0x5)

#define AIE_REG_IOCTL			_IOWR(AIE_IOCTL_BASE, 0x8, \
					      struct aie_reg_args)

#define AIE_GET_MEM_IOCTL		_IOWR(AIE_IOCTL_BASE, 0x9, \
					      struct aie_mem_args)

#define AIE_ATTACH_DMABUF_IOCTL		_IOR(AIE_IOCTL_BASE, 0xa, int)


#define AIE_DETACH_DMABUF_IOCTL		_IOR(AIE_IOCTL_BASE, 0xb, int)


#define AIE_SET_SHIMDMA_BD_IOCTL	_IOW(AIE_IOCTL_BASE, 0xd, \
					     struct aie_dma_bd_args)


#define AIE_REQUEST_TILES_IOCTL		_IOW(AIE_IOCTL_BASE, 0xe, \
					     struct aie_tiles_array)


#define AIE_RELEASE_TILES_IOCTL		_IOW(AIE_IOCTL_BASE, 0xf, \
					     struct aie_tiles_array)


#define AIE_SET_SHIMDMA_DMABUF_BD_IOCTL	_IOW(AIE_IOCTL_BASE, 0x10, \
					     struct aie_dmabuf_bd_args)


#define AIE_TRANSACTION_IOCTL		_IOWR(AIE_IOCTL_BASE, 0x11, \
					     struct aie_txn_inst)


#define AIE_RSC_REQ_IOCTL		_IOW(AIE_IOCTL_BASE, 0x14, \
					     struct aie_rsc_req_rsp)


#define AIE_RSC_REQ_SPECIFIC_IOCTL	_IOW(AIE_IOCTL_BASE, 0x15, \
					     struct aie_rsc)


#define AIE_RSC_RELEASE_IOCTL		_IOW(AIE_IOCTL_BASE, 0x16, \
					     struct aie_rsc)


#define AIE_RSC_FREE_IOCTL		_IOW(AIE_IOCTL_BASE, 0x17, \
					     struct aie_rsc)


#define AIE_RSC_CHECK_AVAIL_IOCTL	_IOW(AIE_IOCTL_BASE, 0x18, \
					     struct aie_rsc_req)


#define AIE_RSC_GET_COMMON_BROADCAST_IOCTL	_IOW(AIE_IOCTL_BASE, 0x19, \
						struct aie_rsc_bc_req)


#define AIE_RSC_GET_STAT_IOCTL		_IOW(AIE_IOCTL_BASE, 0x1a, \
					struct aie_rsc_user_stat_array)


#define AIE_SET_COLUMN_CLOCK_IOCTL	_IOW(AIE_IOCTL_BASE, 0x1b, \
					struct aie_tiles_array)


#define AIE_DMA_MEM_ALLOCATE_IOCTL	_IOW(AIE_IOCTL_BASE, 0x1c, \
					     __kernel_size_t)


#define AIE_DMA_MEM_FREE_IOCTL          _IOW(AIE_IOCTL_BASE, 0x1d, int)


#define AIE_UPDATE_SHIMDMA_DMABUF_BD_ADDR_IOCTL	_IOW(AIE_IOCTL_BASE, 0x1e, \
						struct aie_dmabuf_bd_args)

#endif
