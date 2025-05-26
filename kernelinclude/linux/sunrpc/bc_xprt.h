/* SPDX-License-Identifier: GPL-2.0-only */




#ifndef _LINUX_SUNRPC_BC_XPRT_H
#define _LINUX_SUNRPC_BC_XPRT_H

#include <linux/sunrpc/svcsock.h>
#include <linux/sunrpc/xprt.h>
#include <linux/sunrpc/sched.h>

#ifdef CONFIG_SUNRPC_BACKCHANNEL
struct rpc_rqst *xprt_lookup_bc_request(struct rpc_xprt *xprt, __be32 xid);
void xprt_complete_bc_request(struct rpc_rqst *req, uint32_t copied);
void xprt_init_bc_request(struct rpc_rqst *req, struct rpc_task *task);
void xprt_free_bc_request(struct rpc_rqst *req);
int xprt_setup_backchannel(struct rpc_xprt *, unsigned int min_reqs);
void xprt_destroy_backchannel(struct rpc_xprt *, unsigned int max_reqs);


int xprt_setup_bc(struct rpc_xprt *xprt, unsigned int min_reqs);
void xprt_destroy_bc(struct rpc_xprt *xprt, unsigned int max_reqs);
void xprt_free_bc_rqst(struct rpc_rqst *req);
unsigned int xprt_bc_max_slots(struct rpc_xprt *xprt);


static inline bool svc_is_backchannel(const struct svc_rqst *rqstp)
{
	return rqstp->rq_server->sv_bc_enabled;
}

static inline void set_bc_enabled(struct svc_serv *serv)
{
	serv->sv_bc_enabled = true;
}
#else 
static inline int xprt_setup_backchannel(struct rpc_xprt *xprt,
					 unsigned int min_reqs)
{
	return 0;
}

static inline void xprt_destroy_backchannel(struct rpc_xprt *xprt,
					    unsigned int max_reqs)
{
}

static inline bool svc_is_backchannel(const struct svc_rqst *rqstp)
{
	return false;
}

static inline void set_bc_enabled(struct svc_serv *serv)
{
}

static inline void xprt_free_bc_request(struct rpc_rqst *req)
{
}
#endif 
#endif 
