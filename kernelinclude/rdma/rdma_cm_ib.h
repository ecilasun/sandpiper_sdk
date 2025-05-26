/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */


#ifndef RDMA_CM_IB_H
#define RDMA_CM_IB_H

#include <rdma/rdma_cm.h>


int rdma_set_ib_path(struct rdma_cm_id *id,
		     struct sa_path_rec *path_rec);


#define RDMA_UDP_QKEY 0x01234567

#endif 
