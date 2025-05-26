/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_CGROUP_DEFS_H
#define _LINUX_CGROUP_DEFS_H

#include <linux/limits.h>
#include <linux/list.h>
#include <linux/idr.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <linux/refcount.h>
#include <linux/percpu-refcount.h>
#include <linux/percpu-rwsem.h>
#include <linux/u64_stats_sync.h>
#include <linux/workqueue.h>
#include <linux/bpf-cgroup-defs.h>
#include <linux/psi_types.h>

#ifdef CONFIG_CGROUPS

struct cgroup;
struct cgroup_root;
struct cgroup_subsys;
struct cgroup_taskset;
struct kernfs_node;
struct kernfs_ops;
struct kernfs_open_file;
struct seq_file;
struct poll_table_struct;

#define MAX_CGROUP_TYPE_NAMELEN 32
#define MAX_CGROUP_ROOT_NAMELEN 64
#define MAX_CFTYPE_NAME		64


#define SUBSYS(_x) _x ## _cgrp_id,
enum cgroup_subsys_id {
#include <linux/cgroup_subsys.h>
	CGROUP_SUBSYS_COUNT,
};
#undef SUBSYS


enum {
	CSS_NO_REF	= (1 << 0), 
	CSS_ONLINE	= (1 << 1), 
	CSS_RELEASED	= (1 << 2), 
	CSS_VISIBLE	= (1 << 3), 
	CSS_DYING	= (1 << 4), 
};


enum {
	
	CGRP_NOTIFY_ON_RELEASE,
	
	CGRP_CPUSET_CLONE_CHILDREN,

	
	CGRP_FREEZE,

	
	CGRP_FROZEN,

	
	CGRP_KILL,
};


enum {
	CGRP_ROOT_NOPREFIX	= (1 << 1), 
	CGRP_ROOT_XATTR		= (1 << 2), 

	
	CGRP_ROOT_NS_DELEGATE	= (1 << 3),

	
	CGRP_ROOT_FAVOR_DYNMODS = (1 << 4),

	
	CGRP_ROOT_CPUSET_V2_MODE = (1 << 16),

	
	CGRP_ROOT_MEMORY_LOCAL_EVENTS = (1 << 17),

	
	CGRP_ROOT_MEMORY_RECURSIVE_PROT = (1 << 18),
};


enum {
	CFTYPE_ONLY_ON_ROOT	= (1 << 0),	
	CFTYPE_NOT_ON_ROOT	= (1 << 1),	
	CFTYPE_NS_DELEGATABLE	= (1 << 2),	

	CFTYPE_NO_PREFIX	= (1 << 3),	
	CFTYPE_WORLD_WRITABLE	= (1 << 4),	
	CFTYPE_DEBUG		= (1 << 5),	

	
	__CFTYPE_ONLY_ON_DFL	= (1 << 16),	
	__CFTYPE_NOT_ON_DFL	= (1 << 17),	
	__CFTYPE_ADDED		= (1 << 18),
};


struct cgroup_file {
	
	struct kernfs_node *kn;
	unsigned long notified_at;
	struct timer_list notify_timer;
};


struct cgroup_subsys_state {
	
	struct cgroup *cgroup;

	
	struct cgroup_subsys *ss;

	
	struct percpu_ref refcnt;

	
	struct list_head sibling;
	struct list_head children;

	
	struct list_head rstat_css_node;

	
	int id;

	unsigned int flags;

	
	u64 serial_nr;

	
	atomic_t online_cnt;

	
	struct work_struct destroy_work;
	struct rcu_work destroy_rwork;

	
	struct cgroup_subsys_state *parent;
};


struct css_set {
	
	struct cgroup_subsys_state *subsys[CGROUP_SUBSYS_COUNT];

	
	refcount_t refcount;

	
	struct css_set *dom_cset;

	
	struct cgroup *dfl_cgrp;

	
	int nr_tasks;

	
	struct list_head tasks;
	struct list_head mg_tasks;
	struct list_head dying_tasks;

	
	struct list_head task_iters;

	
	struct list_head e_cset_node[CGROUP_SUBSYS_COUNT];

	
	struct list_head threaded_csets;
	struct list_head threaded_csets_node;

	
	struct hlist_node hlist;

	
	struct list_head cgrp_links;

	
	struct list_head mg_src_preload_node;
	struct list_head mg_dst_preload_node;
	struct list_head mg_node;

	
	struct cgroup *mg_src_cgrp;
	struct cgroup *mg_dst_cgrp;
	struct css_set *mg_dst_cset;

	
	bool dead;

	
	struct rcu_head rcu_head;
};

struct cgroup_base_stat {
	struct task_cputime cputime;

#ifdef CONFIG_SCHED_CORE
	u64 forceidle_sum;
#endif
};


struct cgroup_rstat_cpu {
	
	struct u64_stats_sync bsync;
	struct cgroup_base_stat bstat;

	
	struct cgroup_base_stat last_bstat;

	
	struct cgroup_base_stat subtree_bstat;

	
	struct cgroup_base_stat last_subtree_bstat;

	
	struct cgroup *updated_children;	
	struct cgroup *updated_next;		
};

struct cgroup_freezer_state {
	
	bool freeze;

	
	int e_freeze;

	

	
	int nr_frozen_descendants;

	
	int nr_frozen_tasks;
};

struct cgroup {
	
	struct cgroup_subsys_state self;

	unsigned long flags;		

	
	int level;

	
	int max_depth;

	
	int nr_descendants;
	int nr_dying_descendants;
	int max_descendants;

	
	int nr_populated_csets;
	int nr_populated_domain_children;
	int nr_populated_threaded_children;

	int nr_threaded_children;	

	struct kernfs_node *kn;		
	struct cgroup_file procs_file;	
	struct cgroup_file events_file;	

	
	struct cgroup_file psi_files[NR_PSI_RESOURCES];

	
	u16 subtree_control;
	u16 subtree_ss_mask;
	u16 old_subtree_control;
	u16 old_subtree_ss_mask;

	
	struct cgroup_subsys_state __rcu *subsys[CGROUP_SUBSYS_COUNT];

	struct cgroup_root *root;

	
	struct list_head cset_links;

	
	struct list_head e_csets[CGROUP_SUBSYS_COUNT];

	
	struct cgroup *dom_cgrp;
	struct cgroup *old_dom_cgrp;		

	
	struct cgroup_rstat_cpu __percpu *rstat_cpu;
	struct list_head rstat_css_list;

	
	struct cgroup_base_stat last_bstat;
	struct cgroup_base_stat bstat;
	struct prev_cputime prev_cputime;	

	
	struct list_head pidlists;
	struct mutex pidlist_mutex;

	
	wait_queue_head_t offline_waitq;

	
	struct work_struct release_agent_work;

	
	struct psi_group *psi;

	
	struct cgroup_bpf bpf;

	
	atomic_t congestion_count;

	
	struct cgroup_freezer_state freezer;

#ifdef CONFIG_BPF_SYSCALL
	struct bpf_local_storage __rcu  *bpf_cgrp_storage;
#endif

	
	struct cgroup *ancestors[];
};


struct cgroup_root {
	struct kernfs_root *kf_root;

	
	unsigned int subsys_mask;

	
	int hierarchy_id;

	
	struct cgroup cgrp;

	
	struct cgroup *cgrp_ancestor_storage;

	
	atomic_t nr_cgrps;

	
	struct list_head root_list;

	
	unsigned int flags;

	
	char release_agent_path[PATH_MAX];

	
	char name[MAX_CGROUP_ROOT_NAMELEN];
};


struct cftype {
	
	char name[MAX_CFTYPE_NAME];
	unsigned long private;

	
	size_t max_write_len;

	
	unsigned int flags;

	
	unsigned int file_offset;

	
	struct cgroup_subsys *ss;	
	struct list_head node;		
	struct kernfs_ops *kf_ops;

	int (*open)(struct kernfs_open_file *of);
	void (*release)(struct kernfs_open_file *of);

	
	u64 (*read_u64)(struct cgroup_subsys_state *css, struct cftype *cft);
	
	s64 (*read_s64)(struct cgroup_subsys_state *css, struct cftype *cft);

	
	int (*seq_show)(struct seq_file *sf, void *v);

	
	void *(*seq_start)(struct seq_file *sf, loff_t *ppos);
	void *(*seq_next)(struct seq_file *sf, void *v, loff_t *ppos);
	void (*seq_stop)(struct seq_file *sf, void *v);

	
	int (*write_u64)(struct cgroup_subsys_state *css, struct cftype *cft,
			 u64 val);
	
	int (*write_s64)(struct cgroup_subsys_state *css, struct cftype *cft,
			 s64 val);

	
	ssize_t (*write)(struct kernfs_open_file *of,
			 char *buf, size_t nbytes, loff_t off);

	__poll_t (*poll)(struct kernfs_open_file *of,
			 struct poll_table_struct *pt);

#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lock_class_key	lockdep_key;
#endif
};


struct cgroup_subsys {
	struct cgroup_subsys_state *(*css_alloc)(struct cgroup_subsys_state *parent_css);
	int (*css_online)(struct cgroup_subsys_state *css);
	void (*css_offline)(struct cgroup_subsys_state *css);
	void (*css_released)(struct cgroup_subsys_state *css);
	void (*css_free)(struct cgroup_subsys_state *css);
	void (*css_reset)(struct cgroup_subsys_state *css);
	void (*css_rstat_flush)(struct cgroup_subsys_state *css, int cpu);
	int (*css_extra_stat_show)(struct seq_file *seq,
				   struct cgroup_subsys_state *css);
	int (*css_local_stat_show)(struct seq_file *seq,
				   struct cgroup_subsys_state *css);

	int (*can_attach)(struct cgroup_taskset *tset);
	void (*cancel_attach)(struct cgroup_taskset *tset);
	void (*attach)(struct cgroup_taskset *tset);
	void (*post_attach)(void);
	int (*can_fork)(struct task_struct *task,
			struct css_set *cset);
	void (*cancel_fork)(struct task_struct *task, struct css_set *cset);
	void (*fork)(struct task_struct *task);
	void (*exit)(struct task_struct *task);
	void (*release)(struct task_struct *task);
	void (*bind)(struct cgroup_subsys_state *root_css);

	bool early_init:1;

	
	bool implicit_on_dfl:1;

	
	bool threaded:1;

	
	int id;
	const char *name;

	
	const char *legacy_name;

	
	struct cgroup_root *root;

	
	struct idr css_idr;

	
	struct list_head cfts;

	
	struct cftype *dfl_cftypes;	
	struct cftype *legacy_cftypes;	

	
	unsigned int depends_on;
};

extern struct percpu_rw_semaphore cgroup_threadgroup_rwsem;


static inline void cgroup_threadgroup_change_begin(struct task_struct *tsk)
{
	percpu_down_read(&cgroup_threadgroup_rwsem);
}


static inline void cgroup_threadgroup_change_end(struct task_struct *tsk)
{
	percpu_up_read(&cgroup_threadgroup_rwsem);
}

#else	

#define CGROUP_SUBSYS_COUNT 0

static inline void cgroup_threadgroup_change_begin(struct task_struct *tsk)
{
	might_sleep();
}

static inline void cgroup_threadgroup_change_end(struct task_struct *tsk) {}

#endif	

#ifdef CONFIG_SOCK_CGROUP_DATA


struct sock_cgroup_data {
	struct cgroup	*cgroup; 
#ifdef CONFIG_CGROUP_NET_CLASSID
	u32		classid; 
#endif
#ifdef CONFIG_CGROUP_NET_PRIO
	u16		prioidx; 
#endif
};

static inline u16 sock_cgroup_prioidx(const struct sock_cgroup_data *skcd)
{
#ifdef CONFIG_CGROUP_NET_PRIO
	return READ_ONCE(skcd->prioidx);
#else
	return 1;
#endif
}

static inline u32 sock_cgroup_classid(const struct sock_cgroup_data *skcd)
{
#ifdef CONFIG_CGROUP_NET_CLASSID
	return READ_ONCE(skcd->classid);
#else
	return 0;
#endif
}

static inline void sock_cgroup_set_prioidx(struct sock_cgroup_data *skcd,
					   u16 prioidx)
{
#ifdef CONFIG_CGROUP_NET_PRIO
	WRITE_ONCE(skcd->prioidx, prioidx);
#endif
}

static inline void sock_cgroup_set_classid(struct sock_cgroup_data *skcd,
					   u32 classid)
{
#ifdef CONFIG_CGROUP_NET_CLASSID
	WRITE_ONCE(skcd->classid, classid);
#endif
}

#else	

struct sock_cgroup_data {
};

#endif	

#endif	
