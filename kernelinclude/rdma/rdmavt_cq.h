/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */


#ifndef DEF_RDMAVT_INCCQ_H
#define DEF_RDMAVT_INCCQ_H

#include <linux/kthread.h>
#include <rdma/ib_user_verbs.h>
#include <rdma/ib_verbs.h>


#define RVT_CQ_NONE      (IB_CQ_NEXT_COMP + 1)


#define RDMA_READ_UAPI_ATOMIC(member) smp_load_acquire(&(member).val)


#define RDMA_WRITE_UAPI_ATOMIC(member, x) smp_store_release(&(member).val, x)
#include <rdma/rvt-abi.h>


struct rvt_k_cq_wc {
	u32 head;               
	u32 tail;               
	struct ib_wc kqueue[];
};


struct rvt_cq {
	struct ib_cq ibcq;
	struct work_struct comptask;
	spinlock_t lock; 
	u8 notify;
	u8 triggered;
	u8 cq_full;
	int comp_vector_cpu;
	struct rvt_dev_info *rdi;
	struct rvt_cq_wc *queue;
	struct rvt_mmap_info *ip;
	struct rvt_k_cq_wc *kqueue;
};

static inline struct rvt_cq *ibcq_to_rvtcq(struct ib_cq *ibcq)
{
	return container_of(ibcq, struct rvt_cq, ibcq);
}

bool rvt_cq_enter(struct rvt_cq *cq, struct ib_wc *entry, bool solicited);

#endif          
