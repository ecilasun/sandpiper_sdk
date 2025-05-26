/* SPDX-License-Identifier: GPL-2.0 */


#ifndef SUNRPC_SVCSOCK_H
#define SUNRPC_SVCSOCK_H

#include <linux/sunrpc/svc.h>
#include <linux/sunrpc/svc_xprt.h>


struct svc_sock {
	struct svc_xprt		sk_xprt;
	struct socket *		sk_sock;	
	struct sock *		sk_sk;		

	
	void			(*sk_ostate)(struct sock *);
	void			(*sk_odata)(struct sock *);
	void			(*sk_owspace)(struct sock *);

	
	
	__be32			sk_marker;
	
	u32			sk_tcplen;
	
	u32			sk_datalen;

	struct page_frag_cache  sk_frag_cache;

	struct completion	sk_handshake_done;

	struct page *		sk_pages[RPCSVC_MAXPAGES];	
};

static inline u32 svc_sock_reclen(struct svc_sock *svsk)
{
	return be32_to_cpu(svsk->sk_marker) & RPC_FRAGMENT_SIZE_MASK;
}

static inline u32 svc_sock_final_rec(struct svc_sock *svsk)
{
	return be32_to_cpu(svsk->sk_marker) & RPC_LAST_STREAM_FRAGMENT;
}


void		svc_recv(struct svc_rqst *rqstp);
void		svc_send(struct svc_rqst *rqstp);
void		svc_drop(struct svc_rqst *);
void		svc_sock_update_bufs(struct svc_serv *serv);
int		svc_addsock(struct svc_serv *serv, struct net *net,
			    const int fd, char *name_return, const size_t len,
			    const struct cred *cred);
void		svc_init_xprt_sock(void);
void		svc_cleanup_xprt_sock(void);


#define SVC_SOCK_DEFAULTS	(0U)
#define SVC_SOCK_ANONYMOUS	(1U << 0)	
#define SVC_SOCK_TEMPORARY	(1U << 1)	

#endif 
