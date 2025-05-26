/* SPDX-License-Identifier: GPL-2.0 */


#ifndef RPC_RDMA_CID_H
#define RPC_RDMA_CID_H


struct rpc_rdma_cid {
	u32			ci_queue_id;
	int			ci_completion_id;
};

#endif	
