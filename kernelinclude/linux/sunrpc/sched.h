/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_SUNRPC_SCHED_H_
#define _LINUX_SUNRPC_SCHED_H_

#include <linux/timer.h>
#include <linux/ktime.h>
#include <linux/sunrpc/types.h>
#include <linux/spinlock.h>
#include <linux/wait_bit.h>
#include <linux/workqueue.h>
#include <linux/sunrpc/xdr.h>


struct rpc_procinfo;
struct rpc_message {
	const struct rpc_procinfo *rpc_proc;	
	void *			rpc_argp;	
	void *			rpc_resp;	
	const struct cred *	rpc_cred;	
};

struct rpc_call_ops;
struct rpc_wait_queue;
struct rpc_wait {
	struct list_head	list;		
	struct list_head	links;		
	struct list_head	timer_list;	
};


struct rpc_task {
	atomic_t		tk_count;	
	int			tk_status;	
	struct list_head	tk_task;	

	
	void			(*tk_callback)(struct rpc_task *);
	void			(*tk_action)(struct rpc_task *);

	unsigned long		tk_timeout;	
	unsigned long		tk_runstate;	

	struct rpc_wait_queue 	*tk_waitqueue;	
	union {
		struct work_struct	tk_work;	
		struct rpc_wait		tk_wait;	
	} u;

	
	struct rpc_message	tk_msg;		
	void *			tk_calldata;	
	const struct rpc_call_ops *tk_ops;	

	struct rpc_clnt *	tk_client;	
	struct rpc_xprt *	tk_xprt;	
	struct rpc_cred *	tk_op_cred;	

	struct rpc_rqst *	tk_rqstp;	

	struct workqueue_struct	*tk_workqueue;	
	ktime_t			tk_start;	

	pid_t			tk_owner;	

	int			tk_rpc_status;	
	unsigned short		tk_flags;	
	unsigned short		tk_timeouts;	

#if IS_ENABLED(CONFIG_SUNRPC_DEBUG) || IS_ENABLED(CONFIG_TRACEPOINTS)
	unsigned short		tk_pid;		
#endif
	unsigned char		tk_priority : 2,
				tk_garb_retry : 2,
				tk_cred_retry : 2;
};

typedef void			(*rpc_action)(struct rpc_task *);

struct rpc_call_ops {
	void (*rpc_call_prepare)(struct rpc_task *, void *);
	void (*rpc_call_done)(struct rpc_task *, void *);
	void (*rpc_count_stats)(struct rpc_task *, void *);
	void (*rpc_release)(void *);
};

struct rpc_task_setup {
	struct rpc_task *task;
	struct rpc_clnt *rpc_client;
	struct rpc_xprt *rpc_xprt;
	struct rpc_cred *rpc_op_cred;	
	const struct rpc_message *rpc_message;
	const struct rpc_call_ops *callback_ops;
	void *callback_data;
	struct workqueue_struct *workqueue;
	unsigned short flags;
	signed char priority;
};


#define RPC_TASK_ASYNC		0x0001		
#define RPC_TASK_SWAPPER	0x0002		
#define RPC_TASK_MOVEABLE	0x0004		
#define RPC_TASK_NULLCREDS	0x0010		
#define RPC_CALL_MAJORSEEN	0x0020		
#define RPC_TASK_DYNAMIC	0x0080		
#define	RPC_TASK_NO_ROUND_ROBIN	0x0100		
#define RPC_TASK_SOFT		0x0200		
#define RPC_TASK_SOFTCONN	0x0400		
#define RPC_TASK_SENT		0x0800		
#define RPC_TASK_TIMEOUT	0x1000		
#define RPC_TASK_NOCONNECT	0x2000		
#define RPC_TASK_NO_RETRANS_TIMEOUT	0x4000		
#define RPC_TASK_CRED_NOREF	0x8000		

#define RPC_IS_ASYNC(t)		((t)->tk_flags & RPC_TASK_ASYNC)
#define RPC_IS_SWAPPER(t)	((t)->tk_flags & RPC_TASK_SWAPPER)
#define RPC_IS_SOFT(t)		((t)->tk_flags & (RPC_TASK_SOFT|RPC_TASK_TIMEOUT))
#define RPC_IS_SOFTCONN(t)	((t)->tk_flags & RPC_TASK_SOFTCONN)
#define RPC_WAS_SENT(t)		((t)->tk_flags & RPC_TASK_SENT)
#define RPC_IS_MOVEABLE(t)	((t)->tk_flags & RPC_TASK_MOVEABLE)

#define RPC_TASK_RUNNING	0
#define RPC_TASK_QUEUED		1
#define RPC_TASK_ACTIVE		2
#define RPC_TASK_NEED_XMIT	3
#define RPC_TASK_NEED_RECV	4
#define RPC_TASK_MSG_PIN_WAIT	5
#define RPC_TASK_SIGNALLED	6

#define rpc_test_and_set_running(t) \
				test_and_set_bit(RPC_TASK_RUNNING, &(t)->tk_runstate)
#define rpc_clear_running(t)	clear_bit(RPC_TASK_RUNNING, &(t)->tk_runstate)

#define RPC_IS_QUEUED(t)	test_bit(RPC_TASK_QUEUED, &(t)->tk_runstate)
#define rpc_set_queued(t)	set_bit(RPC_TASK_QUEUED, &(t)->tk_runstate)
#define rpc_clear_queued(t)	clear_bit(RPC_TASK_QUEUED, &(t)->tk_runstate)

#define RPC_IS_ACTIVATED(t)	test_bit(RPC_TASK_ACTIVE, &(t)->tk_runstate)

#define RPC_SIGNALLED(t)	test_bit(RPC_TASK_SIGNALLED, &(t)->tk_runstate)


#define RPC_PRIORITY_LOW	(-1)
#define RPC_PRIORITY_NORMAL	(0)
#define RPC_PRIORITY_HIGH	(1)
#define RPC_PRIORITY_PRIVILEGED	(2)
#define RPC_NR_PRIORITY		(1 + RPC_PRIORITY_PRIVILEGED - RPC_PRIORITY_LOW)

struct rpc_timer {
	struct list_head list;
	unsigned long expires;
	struct delayed_work dwork;
};


struct rpc_wait_queue {
	spinlock_t		lock;
	struct list_head	tasks[RPC_NR_PRIORITY];	
	unsigned char		maxpriority;		
	unsigned char		priority;		
	unsigned char		nr;			
	unsigned int		qlen;			
	struct rpc_timer	timer_list;
#if IS_ENABLED(CONFIG_SUNRPC_DEBUG) || IS_ENABLED(CONFIG_TRACEPOINTS)
	const char *		name;
#endif
};


#define RPC_IS_PRIORITY(q)		((q)->maxpriority > 0)


struct rpc_task *rpc_new_task(const struct rpc_task_setup *);
struct rpc_task *rpc_run_task(const struct rpc_task_setup *);
struct rpc_task *rpc_run_bc_task(struct rpc_rqst *req);
void		rpc_put_task(struct rpc_task *);
void		rpc_put_task_async(struct rpc_task *);
bool		rpc_task_set_rpc_status(struct rpc_task *task, int rpc_status);
void		rpc_task_try_cancel(struct rpc_task *task, int error);
void		rpc_signal_task(struct rpc_task *);
void		rpc_exit_task(struct rpc_task *);
void		rpc_exit(struct rpc_task *, int);
void		rpc_release_calldata(const struct rpc_call_ops *, void *);
void		rpc_killall_tasks(struct rpc_clnt *);
unsigned long	rpc_cancel_tasks(struct rpc_clnt *clnt, int error,
				 bool (*fnmatch)(const struct rpc_task *,
						 const void *),
				 const void *data);
void		rpc_execute(struct rpc_task *);
void		rpc_init_priority_wait_queue(struct rpc_wait_queue *, const char *);
void		rpc_init_wait_queue(struct rpc_wait_queue *, const char *);
void		rpc_destroy_wait_queue(struct rpc_wait_queue *);
unsigned long	rpc_task_timeout(const struct rpc_task *task);
void		rpc_sleep_on_timeout(struct rpc_wait_queue *queue,
					struct rpc_task *task,
					rpc_action action,
					unsigned long timeout);
void		rpc_sleep_on(struct rpc_wait_queue *, struct rpc_task *,
					rpc_action action);
void		rpc_sleep_on_priority_timeout(struct rpc_wait_queue *queue,
					struct rpc_task *task,
					unsigned long timeout,
					int priority);
void		rpc_sleep_on_priority(struct rpc_wait_queue *,
					struct rpc_task *,
					int priority);
void		rpc_wake_up_queued_task(struct rpc_wait_queue *,
					struct rpc_task *);
void		rpc_wake_up_queued_task_set_status(struct rpc_wait_queue *,
						   struct rpc_task *,
						   int);
void		rpc_wake_up(struct rpc_wait_queue *);
struct rpc_task *rpc_wake_up_next(struct rpc_wait_queue *);
struct rpc_task *rpc_wake_up_first_on_wq(struct workqueue_struct *wq,
					struct rpc_wait_queue *,
					bool (*)(struct rpc_task *, void *),
					void *);
struct rpc_task *rpc_wake_up_first(struct rpc_wait_queue *,
					bool (*)(struct rpc_task *, void *),
					void *);
void		rpc_wake_up_status(struct rpc_wait_queue *, int);
void		rpc_delay(struct rpc_task *, unsigned long);
int		rpc_malloc(struct rpc_task *);
void		rpc_free(struct rpc_task *);
int		rpciod_up(void);
void		rpciod_down(void);
int		rpc_wait_for_completion_task(struct rpc_task *task);
#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
struct net;
void		rpc_show_tasks(struct net *);
#endif
int		rpc_init_mempool(void);
void		rpc_destroy_mempool(void);
extern struct workqueue_struct *rpciod_workqueue;
extern struct workqueue_struct *xprtiod_workqueue;
void		rpc_prepare_task(struct rpc_task *task);
gfp_t		rpc_task_gfp_mask(void);

#if IS_ENABLED(CONFIG_SUNRPC_DEBUG) || IS_ENABLED(CONFIG_TRACEPOINTS)
static inline const char * rpc_qname(const struct rpc_wait_queue *q)
{
	return ((q && q->name) ? q->name : "unknown");
}

static inline void rpc_assign_waitqueue_name(struct rpc_wait_queue *q,
		const char *name)
{
	q->name = name;
}
#else
static inline void rpc_assign_waitqueue_name(struct rpc_wait_queue *q,
		const char *name)
{
}
#endif

#if IS_ENABLED(CONFIG_SUNRPC_SWAP)
int rpc_clnt_swap_activate(struct rpc_clnt *clnt);
void rpc_clnt_swap_deactivate(struct rpc_clnt *clnt);
#else
static inline int
rpc_clnt_swap_activate(struct rpc_clnt *clnt)
{
	return -EINVAL;
}

static inline void
rpc_clnt_swap_deactivate(struct rpc_clnt *clnt)
{
}
#endif 

#endif 
