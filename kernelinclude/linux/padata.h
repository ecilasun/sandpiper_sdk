/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef PADATA_H
#define PADATA_H

#include <linux/refcount.h>
#include <linux/compiler_types.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/kobject.h>

#define PADATA_CPU_SERIAL   0x01
#define PADATA_CPU_PARALLEL 0x02


struct padata_priv {
	struct list_head	list;
	struct parallel_data	*pd;
	int			cb_cpu;
	unsigned int		seq_nr;
	int			info;
	void                    (*parallel)(struct padata_priv *padata);
	void                    (*serial)(struct padata_priv *padata);
};


struct padata_list {
	struct list_head        list;
	spinlock_t              lock;
};


struct padata_serial_queue {
       struct padata_list    serial;
       struct work_struct    work;
       struct parallel_data *pd;
};


struct padata_cpumask {
	cpumask_var_t	pcpu;
	cpumask_var_t	cbcpu;
};


struct parallel_data {
	struct padata_shell		*ps;
	struct padata_list		__percpu *reorder_list;
	struct padata_serial_queue	__percpu *squeue;
	refcount_t			refcnt;
	unsigned int			seq_nr;
	unsigned int			processed;
	int				cpu;
	struct padata_cpumask		cpumask;
	struct work_struct		reorder_work;
	spinlock_t                      ____cacheline_aligned lock;
};


struct padata_shell {
	struct padata_instance		*pinst;
	struct parallel_data __rcu	*pd;
	struct parallel_data		*opd;
	struct list_head		list;
};


struct padata_mt_job {
	void (*thread_fn)(unsigned long start, unsigned long end, void *arg);
	void			*fn_arg;
	unsigned long		start;
	unsigned long		size;
	unsigned long		align;
	unsigned long		min_chunk;
	int			max_threads;
};


struct padata_instance {
	struct hlist_node		cpu_online_node;
	struct hlist_node		cpu_dead_node;
	struct workqueue_struct		*parallel_wq;
	struct workqueue_struct		*serial_wq;
	struct list_head		pslist;
	struct padata_cpumask		cpumask;
	struct kobject                   kobj;
	struct mutex			 lock;
	u8				 flags;
#define	PADATA_INIT	1
#define	PADATA_RESET	2
#define	PADATA_INVALID	4
};

#ifdef CONFIG_PADATA
extern void __init padata_init(void);
#else
static inline void __init padata_init(void) {}
#endif

extern struct padata_instance *padata_alloc(const char *name);
extern void padata_free(struct padata_instance *pinst);
extern struct padata_shell *padata_alloc_shell(struct padata_instance *pinst);
extern void padata_free_shell(struct padata_shell *ps);
extern int padata_do_parallel(struct padata_shell *ps,
			      struct padata_priv *padata, int *cb_cpu);
extern void padata_do_serial(struct padata_priv *padata);
extern void __init padata_do_multithreaded(struct padata_mt_job *job);
extern int padata_set_cpumask(struct padata_instance *pinst, int cpumask_type,
			      cpumask_var_t cpumask);
#endif
