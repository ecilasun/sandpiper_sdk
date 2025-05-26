/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */


#ifndef DEF_RDMAVT_INCQP_H
#define DEF_RDMAVT_INCQP_H

#include <rdma/rdma_vt.h>
#include <rdma/ib_pack.h>
#include <rdma/ib_verbs.h>
#include <rdma/rdmavt_cq.h>
#include <rdma/rvt-abi.h>

#define RVT_R_WRID_VALID        0
#define RVT_R_REWIND_SGE        1


#define RVT_R_REUSE_SGE 0x01
#define RVT_R_RDMAR_SEQ 0x02
#define RVT_R_RSP_NAK   0x04
#define RVT_R_RSP_SEND  0x08
#define RVT_R_COMM_EST  0x10


#define RVT_KDETH_QP_PREFIX       0x80
#define RVT_KDETH_QP_SUFFIX       0xffff
#define RVT_KDETH_QP_PREFIX_MASK  0x00ff0000
#define RVT_KDETH_QP_PREFIX_SHIFT 16
#define RVT_KDETH_QP_BASE         (u32)(RVT_KDETH_QP_PREFIX << \
					RVT_KDETH_QP_PREFIX_SHIFT)
#define RVT_KDETH_QP_MAX          (u32)(RVT_KDETH_QP_BASE + RVT_KDETH_QP_SUFFIX)


#define RVT_AIP_QP_PREFIX       0x81
#define RVT_AIP_QP_SUFFIX       0xffff
#define RVT_AIP_QP_PREFIX_MASK  0x00ff0000
#define RVT_AIP_QP_PREFIX_SHIFT 16
#define RVT_AIP_QP_BASE         (u32)(RVT_AIP_QP_PREFIX << \
				      RVT_AIP_QP_PREFIX_SHIFT)
#define RVT_AIP_QPN_MAX         BIT(RVT_AIP_QP_PREFIX_SHIFT)
#define RVT_AIP_QP_MAX          (u32)(RVT_AIP_QP_BASE + RVT_AIP_QPN_MAX - 1)


#define RVT_S_SIGNAL_REQ_WR	0x0001
#define RVT_S_BUSY		0x0002
#define RVT_S_TIMER		0x0004
#define RVT_S_RESP_PENDING	0x0008
#define RVT_S_ACK_PENDING	0x0010
#define RVT_S_WAIT_FENCE	0x0020
#define RVT_S_WAIT_RDMAR	0x0040
#define RVT_S_WAIT_RNR		0x0080
#define RVT_S_WAIT_SSN_CREDIT	0x0100
#define RVT_S_WAIT_DMA		0x0200
#define RVT_S_WAIT_PIO		0x0400
#define RVT_S_WAIT_TX		0x0800
#define RVT_S_WAIT_DMA_DESC	0x1000
#define RVT_S_WAIT_KMEM		0x2000
#define RVT_S_WAIT_PSN		0x4000
#define RVT_S_WAIT_ACK		0x8000
#define RVT_S_SEND_ONE		0x10000
#define RVT_S_UNLIMITED_CREDIT	0x20000
#define RVT_S_ECN		0x40000
#define RVT_S_MAX_BIT_MASK	0x800000




#define RVT_S_ANY_WAIT_IO \
	(RVT_S_WAIT_PIO | RVT_S_WAIT_TX | \
	 RVT_S_WAIT_DMA_DESC | RVT_S_WAIT_KMEM)


#define RVT_S_ANY_WAIT_SEND (RVT_S_WAIT_FENCE | RVT_S_WAIT_RDMAR | \
	RVT_S_WAIT_RNR | RVT_S_WAIT_SSN_CREDIT | RVT_S_WAIT_DMA | \
	RVT_S_WAIT_PSN | RVT_S_WAIT_ACK)

#define RVT_S_ANY_WAIT (RVT_S_ANY_WAIT_IO | RVT_S_ANY_WAIT_SEND)


#define RVT_OPCODE_QP_MASK 0xE0


#define RVT_POST_SEND_OK                0x01
#define RVT_POST_RECV_OK                0x02
#define RVT_PROCESS_RECV_OK             0x04
#define RVT_PROCESS_SEND_OK             0x08
#define RVT_PROCESS_NEXT_SEND_OK        0x10
#define RVT_FLUSH_SEND			0x20
#define RVT_FLUSH_RECV			0x40
#define RVT_PROCESS_OR_FLUSH_SEND \
	(RVT_PROCESS_SEND_OK | RVT_FLUSH_SEND)
#define RVT_SEND_OR_FLUSH_OR_RECV_OK \
	(RVT_PROCESS_SEND_OK | RVT_FLUSH_SEND | RVT_PROCESS_RECV_OK)


#define RVT_SEND_RESERVE_USED           IB_SEND_RESERVED_START
#define RVT_SEND_COMPLETION_ONLY	(IB_SEND_RESERVED_START << 1)


struct rvt_ud_wr {
	struct ib_ud_wr wr;
	struct rdma_ah_attr *attr;
};


struct rvt_swqe {
	union {
		struct ib_send_wr wr;   
		struct rvt_ud_wr ud_wr;
		struct ib_reg_wr reg_wr;
		struct ib_rdma_wr rdma_wr;
		struct ib_atomic_wr atomic_wr;
	};
	u32 psn;                
	u32 lpsn;               
	u32 ssn;                
	u32 length;             
	void *priv;             
	struct rvt_sge sg_list[];
};


struct rvt_krwq {
	spinlock_t p_lock;	
	u32 head;               

	
	spinlock_t c_lock ____cacheline_aligned_in_smp;
	u32 tail;               
	u32 count;		
	struct rvt_rwqe *curr_wq;
	struct rvt_rwqe wq[];
};


static inline struct rvt_ah *rvt_get_swqe_ah(struct rvt_swqe *swqe)
{
	return ibah_to_rvtah(swqe->ud_wr.wr.ah);
}


static inline struct rdma_ah_attr *rvt_get_swqe_ah_attr(struct rvt_swqe *swqe)
{
	return swqe->ud_wr.attr;
}


static inline u32 rvt_get_swqe_remote_qpn(struct rvt_swqe *swqe)
{
	return swqe->ud_wr.wr.remote_qpn;
}


static inline u32 rvt_get_swqe_remote_qkey(struct rvt_swqe *swqe)
{
	return swqe->ud_wr.wr.remote_qkey;
}


static inline u16 rvt_get_swqe_pkey_index(struct rvt_swqe *swqe)
{
	return swqe->ud_wr.wr.pkey_index;
}

struct rvt_rq {
	struct rvt_rwq *wq;
	struct rvt_krwq *kwq;
	u32 size;               
	u8 max_sge;
	
	spinlock_t lock ____cacheline_aligned_in_smp;
};



static inline u32 rvt_get_rq_count(struct rvt_rq *rq, u32 head, u32 tail)
{
	u32 count = head - tail;

	if ((s32)count < 0)
		count += rq->size;
	return count;
}


struct rvt_ack_entry {
	struct rvt_sge rdma_sge;
	u64 atomic_data;
	u32 psn;
	u32 lpsn;
	u8 opcode;
	u8 sent;
	void *priv;
};

#define	RC_QP_SCALING_INTERVAL	5

#define RVT_OPERATION_PRIV        0x00000001
#define RVT_OPERATION_ATOMIC      0x00000002
#define RVT_OPERATION_ATOMIC_SGE  0x00000004
#define RVT_OPERATION_LOCAL       0x00000008
#define RVT_OPERATION_USE_RESERVE 0x00000010
#define RVT_OPERATION_IGN_RNR_CNT 0x00000020

#define RVT_OPERATION_MAX (IB_WR_RESERVED10 + 1)



struct rvt_operation_params {
	size_t length;
	u32 qpt_support;
	u32 flags;
};


struct rvt_qp {
	struct ib_qp ibqp;
	void *priv; 
	
	struct rdma_ah_attr remote_ah_attr;
	struct rdma_ah_attr alt_ah_attr;
	struct rvt_qp __rcu *next;           
	struct rvt_swqe *s_wq;  
	struct rvt_mmap_info *ip;

	unsigned long timeout_jiffies;  

	int srate_mbps;		
	pid_t pid;		
	u32 remote_qpn;
	u32 qkey;               
	u32 s_size;             

	u16 pmtu;		
	u8 log_pmtu;		
	u8 state;               
	u8 allowed_ops;		
	u8 qp_access_flags;
	u8 alt_timeout;         
	u8 timeout;             
	u8 s_srate;
	u8 s_mig_state;
	u8 port_num;
	u8 s_pkey_index;        
	u8 s_alt_pkey_index;    
	u8 r_max_rd_atomic;     
	u8 s_max_rd_atomic;     
	u8 s_retry_cnt;         
	u8 s_rnr_retry_cnt;
	u8 r_min_rnr_timer;     
	u8 s_max_sge;           
	u8 s_draining;

	
	atomic_t refcount ____cacheline_aligned_in_smp;
	wait_queue_head_t wait;

	struct rvt_ack_entry *s_ack_queue;
	struct rvt_sge_state s_rdma_read_sge;

	spinlock_t r_lock ____cacheline_aligned_in_smp;      
	u32 r_psn;              
	unsigned long r_aflags;
	u64 r_wr_id;            
	u32 r_ack_psn;          
	u32 r_len;              
	u32 r_rcv_len;          
	u32 r_msn;              

	u8 r_state;             
	u8 r_flags;
	u8 r_head_ack_queue;    
	u8 r_adefered;          

	struct list_head rspwait;       

	struct rvt_sge_state r_sge;     
	struct rvt_rq r_rq;             

	
	spinlock_t s_hlock ____cacheline_aligned_in_smp;
	u32 s_head;             
	u32 s_next_psn;         
	u32 s_avail;            
	u32 s_ssn;              
	atomic_t s_reserved_used; 

	spinlock_t s_lock ____cacheline_aligned_in_smp;
	u32 s_flags;
	struct rvt_sge_state *s_cur_sge;
	struct rvt_swqe *s_wqe;
	struct rvt_sge_state s_sge;     
	struct rvt_mregion *s_rdma_mr;
	u32 s_len;              
	u32 s_rdma_read_len;    
	u32 s_last_psn;         
	u32 s_sending_psn;      
	u32 s_sending_hpsn;     
	u32 s_psn;              
	u32 s_ack_rdma_psn;     
	u32 s_ack_psn;          
	u32 s_tail;             
	u32 s_cur;              
	u32 s_acked;            
	u32 s_last;             
	u32 s_lsn;              
	u32 s_ahgpsn;           
	u16 s_cur_size;         
	u16 s_rdma_ack_cnt;
	u8 s_hdrwords;         
	s8 s_ahgidx;
	u8 s_state;             
	u8 s_ack_state;         
	u8 s_nak_state;         
	u8 r_nak_state;         
	u8 s_retry;             
	u8 s_rnr_retry;         
	u8 s_num_rd_atomic;     
	u8 s_tail_ack_queue;    
	u8 s_acked_ack_queue;   

	struct rvt_sge_state s_ack_rdma_sge;
	struct timer_list s_timer;
	struct hrtimer s_rnr_timer;

	atomic_t local_ops_pending; 

	
	struct rvt_sge *r_sg_list 
		____cacheline_aligned_in_smp;
};

struct rvt_srq {
	struct ib_srq ibsrq;
	struct rvt_rq rq;
	struct rvt_mmap_info *ip;
	
	u32 limit;
};

static inline struct rvt_srq *ibsrq_to_rvtsrq(struct ib_srq *ibsrq)
{
	return container_of(ibsrq, struct rvt_srq, ibsrq);
}

static inline struct rvt_qp *ibqp_to_rvtqp(struct ib_qp *ibqp)
{
	return container_of(ibqp, struct rvt_qp, ibqp);
}

#define RVT_QPN_MAX                 BIT(24)
#define RVT_QPNMAP_ENTRIES          (RVT_QPN_MAX / PAGE_SIZE / BITS_PER_BYTE)
#define RVT_BITS_PER_PAGE           (PAGE_SIZE * BITS_PER_BYTE)
#define RVT_BITS_PER_PAGE_MASK      (RVT_BITS_PER_PAGE - 1)
#define RVT_QPN_MASK		    IB_QPN_MASK


struct rvt_qpn_map {
	void *page;
};

struct rvt_qpn_table {
	spinlock_t lock; 
	unsigned flags;         
	u32 last;               
	u32 nmaps;              
	u16 limit;
	u8  incr;
	
	struct rvt_qpn_map map[RVT_QPNMAP_ENTRIES];
};

struct rvt_qp_ibdev {
	u32 qp_table_size;
	u32 qp_table_bits;
	struct rvt_qp __rcu **qp_table;
	spinlock_t qpt_lock; 
	struct rvt_qpn_table qpn_table;
};


struct rvt_mcast_qp {
	struct list_head list;
	struct rvt_qp *qp;
};

struct rvt_mcast_addr {
	union ib_gid mgid;
	u16 lid;
};

struct rvt_mcast {
	struct rb_node rb_node;
	struct rvt_mcast_addr mcast_addr;
	struct list_head qp_list;
	wait_queue_head_t wait;
	atomic_t refcount;
	int n_attached;
};


static inline struct rvt_swqe *rvt_get_swqe_ptr(struct rvt_qp *qp,
						unsigned n)
{
	return (struct rvt_swqe *)((char *)qp->s_wq +
				     (sizeof(struct rvt_swqe) +
				      qp->s_max_sge *
				      sizeof(struct rvt_sge)) * n);
}


static inline struct rvt_rwqe *rvt_get_rwqe_ptr(struct rvt_rq *rq, unsigned n)
{
	return (struct rvt_rwqe *)
		((char *)rq->kwq->curr_wq +
		 (sizeof(struct rvt_rwqe) +
		  rq->max_sge * sizeof(struct ib_sge)) * n);
}


static inline bool rvt_is_user_qp(struct rvt_qp *qp)
{
	return !!qp->pid;
}


static inline void rvt_get_qp(struct rvt_qp *qp)
{
	atomic_inc(&qp->refcount);
}


static inline void rvt_put_qp(struct rvt_qp *qp)
{
	if (qp && atomic_dec_and_test(&qp->refcount))
		wake_up(&qp->wait);
}


static inline void rvt_put_swqe(struct rvt_swqe *wqe)
{
	int i;

	for (i = 0; i < wqe->wr.num_sge; i++) {
		struct rvt_sge *sge = &wqe->sg_list[i];

		rvt_put_mr(sge->mr);
	}
}


static inline void rvt_qp_wqe_reserve(
	struct rvt_qp *qp,
	struct rvt_swqe *wqe)
{
	atomic_inc(&qp->s_reserved_used);
}


static inline void rvt_qp_wqe_unreserve(struct rvt_qp *qp, int flags)
{
	if (unlikely(flags & RVT_SEND_RESERVE_USED)) {
		atomic_dec(&qp->s_reserved_used);
		
		smp_mb__after_atomic();
	}
}

extern const enum ib_wc_opcode ib_rvt_wc_opcode[];


static inline int rvt_cmp_msn(u32 a, u32 b)
{
	return (((int)a) - ((int)b)) << 8;
}

__be32 rvt_compute_aeth(struct rvt_qp *qp);

void rvt_get_credit(struct rvt_qp *qp, u32 aeth);

u32 rvt_restart_sge(struct rvt_sge_state *ss, struct rvt_swqe *wqe, u32 len);


static inline u32 rvt_div_round_up_mtu(struct rvt_qp *qp, u32 len)
{
	return (len + qp->pmtu - 1) >> qp->log_pmtu;
}


static inline u32 rvt_div_mtu(struct rvt_qp *qp, u32 len)
{
	return len >> qp->log_pmtu;
}


static inline unsigned long rvt_timeout_to_jiffies(u8 timeout)
{
	if (timeout > 31)
		timeout = 31;

	return usecs_to_jiffies(1U << timeout) * 4096UL / 1000UL;
}


static inline struct rvt_qp *rvt_lookup_qpn(struct rvt_dev_info *rdi,
					    struct rvt_ibport *rvp,
					    u32 qpn) __must_hold(RCU)
{
	struct rvt_qp *qp = NULL;

	if (unlikely(qpn <= 1)) {
		qp = rcu_dereference(rvp->qp[qpn]);
	} else {
		u32 n = hash_32(qpn, rdi->qp_dev->qp_table_bits);

		for (qp = rcu_dereference(rdi->qp_dev->qp_table[n]); qp;
			qp = rcu_dereference(qp->next))
			if (qp->ibqp.qp_num == qpn)
				break;
	}
	return qp;
}


static inline void rvt_mod_retry_timer_ext(struct rvt_qp *qp, u8 shift)
{
	struct ib_qp *ibqp = &qp->ibqp;
	struct rvt_dev_info *rdi = ib_to_rvt(ibqp->device);

	lockdep_assert_held(&qp->s_lock);
	qp->s_flags |= RVT_S_TIMER;
	
	mod_timer(&qp->s_timer, jiffies + rdi->busy_jiffies +
		  (qp->timeout_jiffies << shift));
}

static inline void rvt_mod_retry_timer(struct rvt_qp *qp)
{
	return rvt_mod_retry_timer_ext(qp, 0);
}


static inline void rvt_put_qp_swqe(struct rvt_qp *qp, struct rvt_swqe *wqe)
{
	rvt_put_swqe(wqe);
	if (qp->allowed_ops == IB_OPCODE_UD)
		rdma_destroy_ah_attr(wqe->ud_wr.attr);
}


static inline u32
rvt_qp_swqe_incr(struct rvt_qp *qp, u32 val)
{
	if (++val >= qp->s_size)
		val = 0;
	return val;
}

int rvt_error_qp(struct rvt_qp *qp, enum ib_wc_status err);


static inline void rvt_recv_cq(struct rvt_qp *qp, struct ib_wc *wc,
			       bool solicited)
{
	struct rvt_cq *cq = ibcq_to_rvtcq(qp->ibqp.recv_cq);

	if (unlikely(!rvt_cq_enter(cq, wc, solicited)))
		rvt_error_qp(qp, IB_WC_LOC_QP_OP_ERR);
}


static inline void rvt_send_cq(struct rvt_qp *qp, struct ib_wc *wc,
			       bool solicited)
{
	struct rvt_cq *cq = ibcq_to_rvtcq(qp->ibqp.send_cq);

	if (unlikely(!rvt_cq_enter(cq, wc, solicited)))
		rvt_error_qp(qp, IB_WC_LOC_QP_OP_ERR);
}


static inline u32
rvt_qp_complete_swqe(struct rvt_qp *qp,
		     struct rvt_swqe *wqe,
		     enum ib_wc_opcode opcode,
		     enum ib_wc_status status)
{
	bool need_completion;
	u64 wr_id;
	u32 byte_len, last;
	int flags = wqe->wr.send_flags;

	rvt_qp_wqe_unreserve(qp, flags);
	rvt_put_qp_swqe(qp, wqe);

	need_completion =
		!(flags & RVT_SEND_RESERVE_USED) &&
		(!(qp->s_flags & RVT_S_SIGNAL_REQ_WR) ||
		(flags & IB_SEND_SIGNALED) ||
		status != IB_WC_SUCCESS);
	if (need_completion) {
		wr_id = wqe->wr.wr_id;
		byte_len = wqe->length;
		
	}
	last = rvt_qp_swqe_incr(qp, qp->s_last);
	
	smp_store_release(&qp->s_last, last);
	if (need_completion) {
		struct ib_wc w = {
			.wr_id = wr_id,
			.status = status,
			.opcode = opcode,
			.qp = &qp->ibqp,
			.byte_len = byte_len,
		};
		rvt_send_cq(qp, &w, status != IB_WC_SUCCESS);
	}
	return last;
}

extern const int  ib_rvt_state_ops[];

struct rvt_dev_info;
int rvt_get_rwqe(struct rvt_qp *qp, bool wr_id_only);
void rvt_comm_est(struct rvt_qp *qp);
void rvt_rc_error(struct rvt_qp *qp, enum ib_wc_status err);
unsigned long rvt_rnr_tbl_to_usec(u32 index);
enum hrtimer_restart rvt_rc_rnr_retry(struct hrtimer *t);
void rvt_add_rnr_timer(struct rvt_qp *qp, u32 aeth);
void rvt_del_timers_sync(struct rvt_qp *qp);
void rvt_stop_rc_timers(struct rvt_qp *qp);
void rvt_add_retry_timer_ext(struct rvt_qp *qp, u8 shift);
static inline void rvt_add_retry_timer(struct rvt_qp *qp)
{
	rvt_add_retry_timer_ext(qp, 0);
}

void rvt_copy_sge(struct rvt_qp *qp, struct rvt_sge_state *ss,
		  void *data, u32 length,
		  bool release, bool copy_last);
void rvt_send_complete(struct rvt_qp *qp, struct rvt_swqe *wqe,
		       enum ib_wc_status status);
void rvt_ruc_loopback(struct rvt_qp *qp);


struct rvt_qp_iter {
	struct rvt_qp *qp;
	
	struct rvt_dev_info *rdi;
	
	void (*cb)(struct rvt_qp *qp, u64 v);
	
	u64 v;
	
	int specials;
	
	int n;
};


static inline u32 ib_cq_tail(struct ib_cq *send_cq)
{
	struct rvt_cq *cq = ibcq_to_rvtcq(send_cq);

	return ibcq_to_rvtcq(send_cq)->ip ?
	       RDMA_READ_UAPI_ATOMIC(cq->queue->tail) :
	       ibcq_to_rvtcq(send_cq)->kqueue->tail;
}


static inline u32 ib_cq_head(struct ib_cq *send_cq)
{
	struct rvt_cq *cq = ibcq_to_rvtcq(send_cq);

	return ibcq_to_rvtcq(send_cq)->ip ?
	       RDMA_READ_UAPI_ATOMIC(cq->queue->head) :
	       ibcq_to_rvtcq(send_cq)->kqueue->head;
}


static inline void rvt_free_rq(struct rvt_rq *rq)
{
	kvfree(rq->kwq);
	rq->kwq = NULL;
	vfree(rq->wq);
	rq->wq = NULL;
}


static inline struct rvt_ibport *rvt_to_iport(struct rvt_qp *qp)
{
	struct rvt_dev_info *rdi = ib_to_rvt(qp->ibqp.device);

	return rdi->ports[qp->port_num - 1];
}


static inline bool rvt_rc_credit_avail(struct rvt_qp *qp, struct rvt_swqe *wqe)
{
	lockdep_assert_held(&qp->s_lock);
	if (!(qp->s_flags & RVT_S_UNLIMITED_CREDIT) &&
	    rvt_cmp_msn(wqe->ssn, qp->s_lsn + 1) > 0) {
		struct rvt_ibport *rvp = rvt_to_iport(qp);

		qp->s_flags |= RVT_S_WAIT_SSN_CREDIT;
		rvp->n_rc_crwaits++;
		return false;
	}
	return true;
}

struct rvt_qp_iter *rvt_qp_iter_init(struct rvt_dev_info *rdi,
				     u64 v,
				     void (*cb)(struct rvt_qp *qp, u64 v));
int rvt_qp_iter_next(struct rvt_qp_iter *iter);
void rvt_qp_iter(struct rvt_dev_info *rdi,
		 u64 v,
		 void (*cb)(struct rvt_qp *qp, u64 v));
void rvt_qp_mr_clean(struct rvt_qp *qp, u32 lkey);
#endif          
