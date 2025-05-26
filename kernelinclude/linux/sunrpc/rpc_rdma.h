/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */


#ifndef _LINUX_SUNRPC_RPC_RDMA_H
#define _LINUX_SUNRPC_RPC_RDMA_H

#include <linux/types.h>
#include <linux/bitops.h>

#define RPCRDMA_VERSION		1
#define rpcrdma_version		cpu_to_be32(RPCRDMA_VERSION)

enum {
	RPCRDMA_V1_DEF_INLINE_SIZE	= 1024,
};


enum {
	rpcrdma_fixed_maxsz	= 4,
	rpcrdma_segment_maxsz	= 4,
	rpcrdma_readseg_maxsz	= 1 + rpcrdma_segment_maxsz,
	rpcrdma_readchunk_maxsz	= 1 + rpcrdma_readseg_maxsz,
};


#define RPCRDMA_HDRLEN_MIN	(sizeof(__be32) * 7)
#define RPCRDMA_HDRLEN_ERR	(sizeof(__be32) * 5)

enum rpcrdma_errcode {
	ERR_VERS = 1,
	ERR_CHUNK = 2
};

enum rpcrdma_proc {
	RDMA_MSG = 0,		
	RDMA_NOMSG = 1,		
	RDMA_MSGP = 2,		
	RDMA_DONE = 3,		
	RDMA_ERROR = 4		
};

#define rdma_msg	cpu_to_be32(RDMA_MSG)
#define rdma_nomsg	cpu_to_be32(RDMA_NOMSG)
#define rdma_msgp	cpu_to_be32(RDMA_MSGP)
#define rdma_done	cpu_to_be32(RDMA_DONE)
#define rdma_error	cpu_to_be32(RDMA_ERROR)

#define err_vers	cpu_to_be32(ERR_VERS)
#define err_chunk	cpu_to_be32(ERR_CHUNK)


struct rpcrdma_connect_private {
	__be32			cp_magic;
	u8			cp_version;
	u8			cp_flags;
	u8			cp_send_size;
	u8			cp_recv_size;
} __packed;

#define rpcrdma_cmp_magic	__cpu_to_be32(0xf6ab0e18)

enum {
	RPCRDMA_CMP_VERSION		= 1,
	RPCRDMA_CMP_F_SND_W_INV_OK	= BIT(0),
};

static inline u8
rpcrdma_encode_buffer_size(unsigned int size)
{
	return (size >> 10) - 1;
}

static inline unsigned int
rpcrdma_decode_buffer_size(u8 val)
{
	return ((unsigned int)val + 1) << 10;
}


static inline __be32 *xdr_encode_rdma_segment(__be32 *p, u32 handle,
					      u32 length, u64 offset)
{
	*p++ = cpu_to_be32(handle);
	*p++ = cpu_to_be32(length);
	return xdr_encode_hyper(p, offset);
}


static inline __be32 *xdr_encode_read_segment(__be32 *p, u32 position,
					      u32 handle, u32 length,
					      u64 offset)
{
	*p++ = cpu_to_be32(position);
	return xdr_encode_rdma_segment(p, handle, length, offset);
}


static inline __be32 *xdr_decode_rdma_segment(__be32 *p, u32 *handle,
					      u32 *length, u64 *offset)
{
	*handle = be32_to_cpup(p++);
	*length = be32_to_cpup(p++);
	return xdr_decode_hyper(p, offset);
}


static inline __be32 *xdr_decode_read_segment(__be32 *p, u32 *position,
					      u32 *handle, u32 *length,
					      u64 *offset)
{
	*position = be32_to_cpup(p++);
	return xdr_decode_rdma_segment(p, handle, length, offset);
}

#endif				
