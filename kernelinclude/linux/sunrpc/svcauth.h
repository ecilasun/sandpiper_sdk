/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_SUNRPC_SVCAUTH_H_
#define _LINUX_SUNRPC_SVCAUTH_H_

#include <linux/string.h>
#include <linux/sunrpc/msg_prot.h>
#include <linux/sunrpc/cache.h>
#include <linux/sunrpc/gss_api.h>
#include <linux/hash.h>
#include <linux/stringhash.h>
#include <linux/cred.h>

struct svc_cred {
	kuid_t			cr_uid;
	kgid_t			cr_gid;
	struct group_info	*cr_group_info;
	u32			cr_flavor; 
	
	char			*cr_raw_principal;
	
	char			*cr_principal;
	char			*cr_targ_princ;
	struct gss_api_mech	*cr_gss_mech;
};

static inline void init_svc_cred(struct svc_cred *cred)
{
	cred->cr_group_info = NULL;
	cred->cr_raw_principal = NULL;
	cred->cr_principal = NULL;
	cred->cr_targ_princ = NULL;
	cred->cr_gss_mech = NULL;
}

static inline void free_svc_cred(struct svc_cred *cred)
{
	if (cred->cr_group_info)
		put_group_info(cred->cr_group_info);
	kfree(cred->cr_raw_principal);
	kfree(cred->cr_principal);
	kfree(cred->cr_targ_princ);
	gss_mech_put(cred->cr_gss_mech);
	init_svc_cred(cred);
}

struct svc_rqst;		
struct in6_addr;


struct auth_domain {
	struct kref		ref;
	struct hlist_node	hash;
	char			*name;
	struct auth_ops		*flavour;
	struct rcu_head		rcu_head;
};

enum svc_auth_status {
	SVC_GARBAGE = 1,
	SVC_SYSERR,
	SVC_VALID,
	SVC_NEGATIVE,
	SVC_OK,
	SVC_DROP,
	SVC_CLOSE,
	SVC_DENIED,
	SVC_PENDING,
	SVC_COMPLETE,
};


struct auth_ops {
	char *	name;
	struct module *owner;
	int	flavour;

	enum svc_auth_status	(*accept)(struct svc_rqst *rqstp);
	int			(*release)(struct svc_rqst *rqstp);
	void			(*domain_release)(struct auth_domain *dom);
	enum svc_auth_status	(*set_client)(struct svc_rqst *rqstp);
};

struct svc_xprt;

extern enum svc_auth_status svc_authenticate(struct svc_rqst *rqstp);
extern int	svc_authorise(struct svc_rqst *rqstp);
extern enum svc_auth_status svc_set_client(struct svc_rqst *rqstp);
extern int	svc_auth_register(rpc_authflavor_t flavor, struct auth_ops *aops);
extern void	svc_auth_unregister(rpc_authflavor_t flavor);

extern struct auth_domain *unix_domain_find(char *name);
extern void auth_domain_put(struct auth_domain *item);
extern struct auth_domain *auth_domain_lookup(char *name, struct auth_domain *new);
extern struct auth_domain *auth_domain_find(char *name);
extern void svcauth_unix_purge(struct net *net);
extern void svcauth_unix_info_release(struct svc_xprt *xpt);
extern enum svc_auth_status svcauth_unix_set_client(struct svc_rqst *rqstp);

extern int unix_gid_cache_create(struct net *net);
extern void unix_gid_cache_destroy(struct net *net);


static inline unsigned long hash_str(char const *name, int bits)
{
	return hashlen_hash(hashlen_string(NULL, name)) >> (32 - bits);
}

static inline unsigned long hash_mem(char const *buf, int length, int bits)
{
	return full_name_hash(NULL, buf, length) >> (32 - bits);
}

#endif 
