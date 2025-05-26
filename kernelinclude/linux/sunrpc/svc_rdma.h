/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */


#ifndef SVC_RDMA_H
#define SVC_RDMA_H
#include <linux/llist.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/svcsock.h>
#include <linux/sunrpc/rpc_rdma.h>
#include <linux/sunrpc/rpc_rdma_cid.h>
#include <linux/sunrpc/svc_rdma_pcl.h>

#include <linux/percpu_counter.h>
#include <rdma/ib_verbs.h>
#include <rdma/rdma_cm.h>


enum {
	RPCRDMA_PULLUP_THRESH = RPCRDMA_V1_DEF_INLINE_SIZE >> 1,
	RPCRDMA_DEF_INLINE_THRESH = 4096,
	RPCRDMA_MAX_INLINE_THRESH = 65536
};


extern unsigned int svcrdma_ord;
extern unsigned int svcrdma_max_requests;
extern unsigned int svcrdma_max_bc_requests;
extern unsigned int svcrdma_max_req_size;

extern struct percpu_counter svcrdma_stat_read;
extern struct percpu_counter svcrdma_stat_recv;
extern struct percpu_counter svcrdma_stat_sq_starve;
extern struct percpu_counter svcrdma_stat_write;

struct svcxprt_rdma {
	struct svc_xprt      sc_xprt;		
	struct rdma_cm_id    *sc_cm_id;		
	struct list_head     sc_accept_q;	
	int		     sc_ord;		
	int                  sc_max_send_sges;
	bool		     sc_snd_w_inv;	

	atomic_t             sc_sq_avail;	
	unsigned int	     sc_sq_depth;	
	__be32		     sc_fc_credits;	
	u32		     sc_max_requests;	
	u32		     sc_max_bc_requests;
	int                  sc_max_req_size;	
	u8		     sc_port_num;

	struct ib_pd         *sc_pd;

	spinlock_t	     sc_send_lock;
	struct llist_head    sc_send_ctxts;
	spinlock_t	     sc_rw_ctxt_lock;
	struct llist_head    sc_rw_ctxts;

	u32		     sc_pending_recvs;
	u32		     sc_recv_batch;
	struct list_head     sc_rq_dto_q;
	spinlock_t	     sc_rq_dto_lock;
	struct ib_qp         *sc_qp;
	struct ib_cq         *sc_rq_cq;
	struct ib_cq         *sc_sq_cq;

	spinlock_t	     sc_lock;		

	wait_queue_head_t    sc_send_wait;	
	unsigned long	     sc_flags;
	struct work_struct   sc_work;

	struct llist_head    sc_recv_ctxts;

	atomic_t	     sc_completion_ids;
};

#define RDMAXPRT_CONN_PENDING	3


enum {
	RPCRDMA_LISTEN_BACKLOG	= 10,
	RPCRDMA_MAX_REQUESTS	= 64,
	RPCRDMA_MAX_BC_REQUESTS	= 2,
};

#define RPCSVC_MAXPAYLOAD_RDMA	RPCSVC_MAXPAYLOAD

struct svc_rdma_recv_ctxt {
	struct llist_node	rc_node;
	struct list_head	rc_list;
	struct ib_recv_wr	rc_recv_wr;
	struct ib_cqe		rc_cqe;
	struct rpc_rdma_cid	rc_cid;
	struct ib_sge		rc_recv_sge;
	void			*rc_recv_buf;
	struct xdr_stream	rc_stream;
	u32			rc_byte_len;
	unsigned int		rc_page_count;
	u32			rc_inv_rkey;
	__be32			rc_msgtype;

	struct svc_rdma_pcl	rc_call_pcl;

	struct svc_rdma_pcl	rc_read_pcl;
	struct svc_rdma_chunk	*rc_cur_result_payload;
	struct svc_rdma_pcl	rc_write_pcl;
	struct svc_rdma_pcl	rc_reply_pcl;
};

struct svc_rdma_send_ctxt {
	struct llist_node	sc_node;
	struct rpc_rdma_cid	sc_cid;

	struct ib_send_wr	sc_send_wr;
	struct ib_cqe		sc_cqe;
	struct xdr_buf		sc_hdrbuf;
	struct xdr_stream	sc_stream;
	void			*sc_xprt_buf;
	int			sc_page_count;
	int			sc_cur_sge_no;
	struct page		*sc_pages[RPCSVC_MAXPAGES];
	struct ib_sge		sc_sges[];
};


extern void svc_rdma_handle_bc_reply(struct svc_rqst *rqstp,
				     struct svc_rdma_recv_ctxt *rctxt);


extern void svc_rdma_recv_ctxts_destroy(struct svcxprt_rdma *rdma);
extern bool svc_rdma_post_recvs(struct svcxprt_rdma *rdma);
extern struct svc_rdma_recv_ctxt *
		svc_rdma_recv_ctxt_get(struct svcxprt_rdma *rdma);
extern void svc_rdma_recv_ctxt_put(struct svcxprt_rdma *rdma,
				   struct svc_rdma_recv_ctxt *ctxt);
extern void svc_rdma_flush_recv_queues(struct svcxprt_rdma *rdma);
extern void svc_rdma_release_ctxt(struct svc_xprt *xprt, void *ctxt);
extern int svc_rdma_recvfrom(struct svc_rqst *);


extern void svc_rdma_destroy_rw_ctxts(struct svcxprt_rdma *rdma);
extern int svc_rdma_send_write_chunk(struct svcxprt_rdma *rdma,
				     const struct svc_rdma_chunk *chunk,
				     const struct xdr_buf *xdr);
extern int svc_rdma_send_reply_chunk(struct svcxprt_rdma *rdma,
				     const struct svc_rdma_recv_ctxt *rctxt,
				     const struct xdr_buf *xdr);
extern int svc_rdma_process_read_list(struct svcxprt_rdma *rdma,
				      struct svc_rqst *rqstp,
				      struct svc_rdma_recv_ctxt *head);


extern void svc_rdma_send_ctxts_destroy(struct svcxprt_rdma *rdma);
extern struct svc_rdma_send_ctxt *
		svc_rdma_send_ctxt_get(struct svcxprt_rdma *rdma);
extern void svc_rdma_send_ctxt_put(struct svcxprt_rdma *rdma,
				   struct svc_rdma_send_ctxt *ctxt);
extern int svc_rdma_send(struct svcxprt_rdma *rdma,
			 struct svc_rdma_send_ctxt *ctxt);
extern int svc_rdma_map_reply_msg(struct svcxprt_rdma *rdma,
				  struct svc_rdma_send_ctxt *sctxt,
				  const struct svc_rdma_recv_ctxt *rctxt,
				  const struct xdr_buf *xdr);
extern void svc_rdma_send_error_msg(struct svcxprt_rdma *rdma,
				    struct svc_rdma_send_ctxt *sctxt,
				    struct svc_rdma_recv_ctxt *rctxt,
				    int status);
extern void svc_rdma_wake_send_waiters(struct svcxprt_rdma *rdma, int avail);
extern int svc_rdma_sendto(struct svc_rqst *);
extern int svc_rdma_result_payload(struct svc_rqst *rqstp, unsigned int offset,
				   unsigned int length);


extern struct svc_xprt_class svc_rdma_class;
#ifdef CONFIG_SUNRPC_BACKCHANNEL
extern struct svc_xprt_class svc_rdma_bc_class;
#endif


extern int svc_rdma_init(void);
extern void svc_rdma_cleanup(void);

#endif
