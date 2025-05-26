/* SPDX-License-Identifier: GPL-2.0 */



#ifndef SUNRPC_SVC_H
#define SUNRPC_SVC_H

#include <linux/in.h>
#include <linux/in6.h>
#include <linux/sunrpc/types.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/auth.h>
#include <linux/sunrpc/svcauth.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/pagevec.h>


struct svc_pool {
	unsigned int		sp_id;	    	
	spinlock_t		sp_lock;	
	struct list_head	sp_sockets;	
	unsigned int		sp_nrthreads;	
	struct list_head	sp_all_threads;	

	
	struct percpu_counter	sp_messages_arrived;
	struct percpu_counter	sp_sockets_queued;
	struct percpu_counter	sp_threads_woken;

	unsigned long		sp_flags;
} ____cacheline_aligned_in_smp;


enum {
	SP_TASK_PENDING,	
	SP_CONGESTED,		
};



struct svc_serv {
	struct svc_program *	sv_program;	
	struct svc_stat *	sv_stats;	
	spinlock_t		sv_lock;
	struct kref		sv_refcnt;
	unsigned int		sv_nrthreads;	
	unsigned int		sv_maxconn;	

	unsigned int		sv_max_payload;	
	unsigned int		sv_max_mesg;	
	unsigned int		sv_xdrsize;	
	struct list_head	sv_permsocks;	
	struct list_head	sv_tempsocks;	
	int			sv_tmpcnt;	
	struct timer_list	sv_temptimer;	

	char *			sv_name;	

	unsigned int		sv_nrpools;	
	struct svc_pool *	sv_pools;	
	int			(*sv_threadfn)(void *data);

#if defined(CONFIG_SUNRPC_BACKCHANNEL)
	struct list_head	sv_cb_list;	
	spinlock_t		sv_cb_lock;	
	wait_queue_head_t	sv_cb_waitq;	
	bool			sv_bc_enabled;	
#endif 
};


static inline struct svc_serv *svc_get(struct svc_serv *serv)
{
	kref_get(&serv->sv_refcnt);
	return serv;
}

void svc_destroy(struct kref *);


static inline void svc_put(struct svc_serv *serv)
{
	kref_put(&serv->sv_refcnt, svc_destroy);
}


#define RPCSVC_MAXPAYLOAD	(1*1024*1024u)
#define RPCSVC_MAXPAYLOAD_TCP	RPCSVC_MAXPAYLOAD
#define RPCSVC_MAXPAYLOAD_UDP	(32*1024u)

extern u32 svc_max_payload(const struct svc_rqst *rqstp);


#define RPCSVC_MAXPAGES		((RPCSVC_MAXPAYLOAD+PAGE_SIZE-1)/PAGE_SIZE \
				+ 2 + 1)


struct svc_rqst {
	struct list_head	rq_all;		
	struct rcu_head		rq_rcu_head;	
	struct svc_xprt *	rq_xprt;	

	struct sockaddr_storage	rq_addr;	
	size_t			rq_addrlen;
	struct sockaddr_storage	rq_daddr;	
	size_t			rq_daddrlen;

	struct svc_serv *	rq_server;	
	struct svc_pool *	rq_pool;	
	const struct svc_procedure *rq_procinfo;
	struct auth_ops *	rq_authop;	
	struct svc_cred		rq_cred;	
	void *			rq_xprt_ctxt;	
	struct svc_deferred_req*rq_deferred;	

	struct xdr_buf		rq_arg;
	struct xdr_stream	rq_arg_stream;
	struct xdr_stream	rq_res_stream;
	struct page		*rq_scratch_page;
	struct xdr_buf		rq_res;
	struct page		*rq_pages[RPCSVC_MAXPAGES + 1];
	struct page *		*rq_respages;	
	struct page *		*rq_next_page; 
	struct page *		*rq_page_end;  

	struct folio_batch	rq_fbatch;
	struct kvec		rq_vec[RPCSVC_MAXPAGES]; 
	struct bio_vec		rq_bvec[RPCSVC_MAXPAGES];

	__be32			rq_xid;		
	u32			rq_prog;	
	u32			rq_vers;	
	u32			rq_proc;	
	u32			rq_prot;	
	int			rq_cachetype;	
	unsigned long		rq_flags;	
	ktime_t			rq_qtime;	

	void *			rq_argp;	
	void *			rq_resp;	
	__be32			*rq_accept_statp;
	void *			rq_auth_data;	
	__be32			rq_auth_stat;	
	int			rq_auth_slack;	
	int			rq_reserved;	
	ktime_t			rq_stime;	

	struct cache_req	rq_chandle;	
	
	struct auth_domain *	rq_client;	
	struct auth_domain *	rq_gssclient;	
	struct task_struct	*rq_task;	
	struct net		*rq_bc_net;	
	void **			rq_lease_breaker; 
};


enum {
	RQ_SECURE,		
	RQ_LOCAL,		
	RQ_USEDEFERRAL,		
	RQ_DROPME,		
	RQ_SPLICE_OK,		
	RQ_VICTIM,		
	RQ_BUSY,		
	RQ_DATA,		
};

#define SVC_NET(rqst) (rqst->rq_xprt ? rqst->rq_xprt->xpt_net : rqst->rq_bc_net)


static inline struct sockaddr_in *svc_addr_in(const struct svc_rqst *rqst)
{
	return (struct sockaddr_in *) &rqst->rq_addr;
}

static inline struct sockaddr_in6 *svc_addr_in6(const struct svc_rqst *rqst)
{
	return (struct sockaddr_in6 *) &rqst->rq_addr;
}

static inline struct sockaddr *svc_addr(const struct svc_rqst *rqst)
{
	return (struct sockaddr *) &rqst->rq_addr;
}

static inline struct sockaddr_in *svc_daddr_in(const struct svc_rqst *rqst)
{
	return (struct sockaddr_in *) &rqst->rq_daddr;
}

static inline struct sockaddr_in6 *svc_daddr_in6(const struct svc_rqst *rqst)
{
	return (struct sockaddr_in6 *) &rqst->rq_daddr;
}

static inline struct sockaddr *svc_daddr(const struct svc_rqst *rqst)
{
	return (struct sockaddr *) &rqst->rq_daddr;
}

struct svc_deferred_req {
	u32			prot;	
	struct svc_xprt		*xprt;
	struct sockaddr_storage	addr;	
	size_t			addrlen;
	struct sockaddr_storage	daddr;	
	size_t			daddrlen;
	void			*xprt_ctxt;
	struct cache_deferred_req handle;
	int			argslen;
	__be32			args[];
};

struct svc_process_info {
	union {
		int  (*dispatch)(struct svc_rqst *rqstp);
		struct {
			unsigned int lovers;
			unsigned int hivers;
		} mismatch;
	};
};


struct svc_program {
	struct svc_program *	pg_next;	
	u32			pg_prog;	
	unsigned int		pg_lovers;	
	unsigned int		pg_hivers;	
	unsigned int		pg_nvers;	
	const struct svc_version **pg_vers;	
	char *			pg_name;	
	char *			pg_class;	
	struct svc_stat *	pg_stats;	
	enum svc_auth_status	(*pg_authenticate)(struct svc_rqst *rqstp);
	__be32			(*pg_init_request)(struct svc_rqst *,
						   const struct svc_program *,
						   struct svc_process_info *);
	int			(*pg_rpcbind_set)(struct net *net,
						  const struct svc_program *,
						  u32 version, int family,
						  unsigned short proto,
						  unsigned short port);
};


struct svc_version {
	u32			vs_vers;	
	u32			vs_nproc;	
	const struct svc_procedure *vs_proc;	
	unsigned long __percpu	*vs_count;	
	u32			vs_xdrsize;	

	
	bool			vs_hidden;

	
	bool			vs_rpcb_optnl;

	
	bool			vs_need_cong_ctrl;

	
	int			(*vs_dispatch)(struct svc_rqst *rqstp);
};


struct svc_procedure {
	
	__be32			(*pc_func)(struct svc_rqst *);
	
	bool			(*pc_decode)(struct svc_rqst *rqstp,
					     struct xdr_stream *xdr);
	
	bool			(*pc_encode)(struct svc_rqst *rqstp,
					     struct xdr_stream *xdr);
	
	void			(*pc_release)(struct svc_rqst *);
	unsigned int		pc_argsize;	
	unsigned int		pc_argzero;	
	unsigned int		pc_ressize;	
	unsigned int		pc_cachetype;	
	unsigned int		pc_xdrressize;	
	const char *		pc_name;	
};


int svc_rpcb_setup(struct svc_serv *serv, struct net *net);
void svc_rpcb_cleanup(struct svc_serv *serv, struct net *net);
int svc_bind(struct svc_serv *serv, struct net *net);
struct svc_serv *svc_create(struct svc_program *, unsigned int,
			    int (*threadfn)(void *data));
struct svc_rqst *svc_rqst_alloc(struct svc_serv *serv,
					struct svc_pool *pool, int node);
bool		   svc_rqst_replace_page(struct svc_rqst *rqstp,
					 struct page *page);
void		   svc_rqst_release_pages(struct svc_rqst *rqstp);
void		   svc_rqst_free(struct svc_rqst *);
void		   svc_exit_thread(struct svc_rqst *);
struct svc_serv *  svc_create_pooled(struct svc_program *, unsigned int,
				     int (*threadfn)(void *data));
int		   svc_set_num_threads(struct svc_serv *, struct svc_pool *, int);
int		   svc_pool_stats_open(struct svc_serv *serv, struct file *file);
void		   svc_process(struct svc_rqst *rqstp);
int		   bc_svc_process(struct svc_serv *, struct rpc_rqst *,
			struct svc_rqst *);
int		   svc_register(const struct svc_serv *, struct net *, const int,
				const unsigned short, const unsigned short);

void		   svc_wake_up(struct svc_serv *);
void		   svc_reserve(struct svc_rqst *rqstp, int space);
void		   svc_pool_wake_idle_thread(struct svc_pool *pool);
struct svc_pool   *svc_pool_for_cpu(struct svc_serv *serv);
char *		   svc_print_addr(struct svc_rqst *, char *, size_t);
const char *	   svc_proc_name(const struct svc_rqst *rqstp);
int		   svc_encode_result_payload(struct svc_rqst *rqstp,
					     unsigned int offset,
					     unsigned int length);
unsigned int	   svc_fill_write_vector(struct svc_rqst *rqstp,
					 struct xdr_buf *payload);
char		  *svc_fill_symlink_pathname(struct svc_rqst *rqstp,
					     struct kvec *first, void *p,
					     size_t total);
__be32		   svc_generic_init_request(struct svc_rqst *rqstp,
					    const struct svc_program *progp,
					    struct svc_process_info *procinfo);
int		   svc_generic_rpcbind_set(struct net *net,
					   const struct svc_program *progp,
					   u32 version, int family,
					   unsigned short proto,
					   unsigned short port);
int		   svc_rpcbind_set_version(struct net *net,
					   const struct svc_program *progp,
					   u32 version, int family,
					   unsigned short proto,
					   unsigned short port);

#define	RPC_MAX_ADDRBUFLEN	(63U)


static inline void svc_reserve_auth(struct svc_rqst *rqstp, int space)
{
	svc_reserve(rqstp, space + rqstp->rq_auth_slack);
}


static inline void svcxdr_init_decode(struct svc_rqst *rqstp)
{
	struct xdr_stream *xdr = &rqstp->rq_arg_stream;
	struct xdr_buf *buf = &rqstp->rq_arg;
	struct kvec *argv = buf->head;

	WARN_ON(buf->len != buf->head->iov_len + buf->page_len + buf->tail->iov_len);
	buf->len = buf->head->iov_len + buf->page_len + buf->tail->iov_len;

	xdr_init_decode(xdr, buf, argv->iov_base, NULL);
	xdr_set_scratch_page(xdr, rqstp->rq_scratch_page);
}


static inline void svcxdr_init_encode(struct svc_rqst *rqstp)
{
	struct xdr_stream *xdr = &rqstp->rq_res_stream;
	struct xdr_buf *buf = &rqstp->rq_res;
	struct kvec *resv = buf->head;

	xdr_reset_scratch_buffer(xdr);

	xdr->buf = buf;
	xdr->iov = resv;
	xdr->p   = resv->iov_base + resv->iov_len;
	xdr->end = resv->iov_base + PAGE_SIZE;
	buf->len = resv->iov_len;
	xdr->page_ptr = buf->pages - 1;
	buf->buflen = PAGE_SIZE * (rqstp->rq_page_end - buf->pages);
	xdr->rqst = NULL;
}


static inline void svcxdr_encode_opaque_pages(struct svc_rqst *rqstp,
					      struct xdr_stream *xdr,
					      struct page **pages,
					      unsigned int base,
					      unsigned int len)
{
	xdr_write_pages(xdr, pages, base, len);
	xdr->page_ptr = rqstp->rq_next_page - 1;
}


static inline void svcxdr_set_auth_slack(struct svc_rqst *rqstp, int slack)
{
	struct xdr_stream *xdr = &rqstp->rq_res_stream;
	struct xdr_buf *buf = &rqstp->rq_res;
	struct kvec *resv = buf->head;

	rqstp->rq_auth_slack = slack;

	xdr->end -= XDR_QUADLEN(slack);
	buf->buflen -= rqstp->rq_auth_slack;

	WARN_ON(xdr->iov != resv);
	WARN_ON(xdr->p > xdr->end);
}


static inline bool svcxdr_set_accept_stat(struct svc_rqst *rqstp)
{
	struct xdr_stream *xdr = &rqstp->rq_res_stream;

	rqstp->rq_accept_statp = xdr_reserve_space(xdr, XDR_UNIT);
	if (unlikely(!rqstp->rq_accept_statp))
		return false;
	*rqstp->rq_accept_statp = rpc_success;
	return true;
}

#endif 
