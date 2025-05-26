/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __IPC_NAMESPACE_H__
#define __IPC_NAMESPACE_H__

#include <linux/err.h>
#include <linux/idr.h>
#include <linux/rwsem.h>
#include <linux/notifier.h>
#include <linux/nsproxy.h>
#include <linux/ns_common.h>
#include <linux/refcount.h>
#include <linux/rhashtable-types.h>
#include <linux/sysctl.h>
#include <linux/percpu_counter.h>

struct user_namespace;

struct ipc_ids {
	int in_use;
	unsigned short seq;
	struct rw_semaphore rwsem;
	struct idr ipcs_idr;
	int max_idx;
	int last_idx;	
#ifdef CONFIG_CHECKPOINT_RESTORE
	int next_id;
#endif
	struct rhashtable key_ht;
};

struct ipc_namespace {
	struct ipc_ids	ids[3];

	int		sem_ctls[4];
	int		used_sems;

	unsigned int	msg_ctlmax;
	unsigned int	msg_ctlmnb;
	unsigned int	msg_ctlmni;
	struct percpu_counter percpu_msg_bytes;
	struct percpu_counter percpu_msg_hdrs;

	size_t		shm_ctlmax;
	size_t		shm_ctlall;
	unsigned long	shm_tot;
	int		shm_ctlmni;
	
	int		shm_rmid_forced;

	struct notifier_block ipcns_nb;

	
	struct vfsmount	*mq_mnt;

	
	unsigned int    mq_queues_count;

	
	unsigned int    mq_queues_max;   
	unsigned int    mq_msg_max;      
	unsigned int    mq_msgsize_max;  
	unsigned int    mq_msg_default;
	unsigned int    mq_msgsize_default;

	struct ctl_table_set	mq_set;
	struct ctl_table_header	*mq_sysctls;

	struct ctl_table_set	ipc_set;
	struct ctl_table_header	*ipc_sysctls;

	
	struct user_namespace *user_ns;
	struct ucounts *ucounts;

	struct llist_node mnt_llist;

	struct ns_common ns;
} __randomize_layout;

extern struct ipc_namespace init_ipc_ns;
extern spinlock_t mq_lock;

#ifdef CONFIG_SYSVIPC
extern void shm_destroy_orphaned(struct ipc_namespace *ns);
#else 
static inline void shm_destroy_orphaned(struct ipc_namespace *ns) {}
#endif 

#ifdef CONFIG_POSIX_MQUEUE
extern int mq_init_ns(struct ipc_namespace *ns);

#define DFLT_QUEUESMAX		      256
#define MIN_MSGMAX			1
#define DFLT_MSG		       10U
#define DFLT_MSGMAX		       10
#define HARD_MSGMAX		    65536
#define MIN_MSGSIZEMAX		      128
#define DFLT_MSGSIZE		     8192U
#define DFLT_MSGSIZEMAX		     8192
#define HARD_MSGSIZEMAX	    (16*1024*1024)
#else
static inline int mq_init_ns(struct ipc_namespace *ns) { return 0; }
#endif

#if defined(CONFIG_IPC_NS)
extern struct ipc_namespace *copy_ipcs(unsigned long flags,
	struct user_namespace *user_ns, struct ipc_namespace *ns);

static inline struct ipc_namespace *get_ipc_ns(struct ipc_namespace *ns)
{
	if (ns)
		refcount_inc(&ns->ns.count);
	return ns;
}

static inline struct ipc_namespace *get_ipc_ns_not_zero(struct ipc_namespace *ns)
{
	if (ns) {
		if (refcount_inc_not_zero(&ns->ns.count))
			return ns;
	}

	return NULL;
}

extern void put_ipc_ns(struct ipc_namespace *ns);
#else
static inline struct ipc_namespace *copy_ipcs(unsigned long flags,
	struct user_namespace *user_ns, struct ipc_namespace *ns)
{
	if (flags & CLONE_NEWIPC)
		return ERR_PTR(-EINVAL);

	return ns;
}

static inline struct ipc_namespace *get_ipc_ns(struct ipc_namespace *ns)
{
	return ns;
}

static inline struct ipc_namespace *get_ipc_ns_not_zero(struct ipc_namespace *ns)
{
	return ns;
}

static inline void put_ipc_ns(struct ipc_namespace *ns)
{
}
#endif

#ifdef CONFIG_POSIX_MQUEUE_SYSCTL

void retire_mq_sysctls(struct ipc_namespace *ns);
bool setup_mq_sysctls(struct ipc_namespace *ns);

#else 

static inline void retire_mq_sysctls(struct ipc_namespace *ns)
{
}

static inline bool setup_mq_sysctls(struct ipc_namespace *ns)
{
	return true;
}

#endif 

#ifdef CONFIG_SYSVIPC_SYSCTL

bool setup_ipc_sysctls(struct ipc_namespace *ns);
void retire_ipc_sysctls(struct ipc_namespace *ns);

#else 

static inline void retire_ipc_sysctls(struct ipc_namespace *ns)
{
}

static inline bool setup_ipc_sysctls(struct ipc_namespace *ns)
{
	return true;
}

#endif 
#endif
