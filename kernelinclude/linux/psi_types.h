/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PSI_TYPES_H
#define _LINUX_PSI_TYPES_H

#include <linux/kthread.h>
#include <linux/seqlock.h>
#include <linux/types.h>
#include <linux/kref.h>
#include <linux/wait.h>

#ifdef CONFIG_PSI


enum psi_task_count {
	NR_IOWAIT,
	NR_MEMSTALL,
	NR_RUNNING,
	
	NR_MEMSTALL_RUNNING,
	NR_PSI_TASK_COUNTS = 4,
};


#define TSK_IOWAIT	(1 << NR_IOWAIT)
#define TSK_MEMSTALL	(1 << NR_MEMSTALL)
#define TSK_RUNNING	(1 << NR_RUNNING)
#define TSK_MEMSTALL_RUNNING	(1 << NR_MEMSTALL_RUNNING)


#define TSK_ONCPU	(1 << NR_PSI_TASK_COUNTS)


enum psi_res {
	PSI_IO,
	PSI_MEM,
	PSI_CPU,
#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	PSI_IRQ,
#endif
	NR_PSI_RESOURCES,
};


enum psi_states {
	PSI_IO_SOME,
	PSI_IO_FULL,
	PSI_MEM_SOME,
	PSI_MEM_FULL,
	PSI_CPU_SOME,
	PSI_CPU_FULL,
#ifdef CONFIG_IRQ_TIME_ACCOUNTING
	PSI_IRQ_FULL,
#endif
	
	PSI_NONIDLE,
	NR_PSI_STATES,
};


#define PSI_ONCPU	(1 << NR_PSI_STATES)


#define PSI_STATE_RESCHEDULE	(1 << (NR_PSI_STATES + 1))

enum psi_aggregators {
	PSI_AVGS = 0,
	PSI_POLL,
	NR_PSI_AGGREGATORS,
};

struct psi_group_cpu {
	

	
	seqcount_t seq ____cacheline_aligned_in_smp;

	
	unsigned int tasks[NR_PSI_TASK_COUNTS];

	
	u32 state_mask;

	
	u32 times[NR_PSI_STATES];

	
	u64 state_start;

	

	
	u32 times_prev[NR_PSI_AGGREGATORS][NR_PSI_STATES]
			____cacheline_aligned_in_smp;
};


struct psi_window {
	
	u64 size;

	
	u64 start_time;

	
	u64 start_value;

	
	u64 prev_growth;
};

struct psi_trigger {
	
	enum psi_states state;

	
	u64 threshold;

	
	struct list_head node;

	
	struct psi_group *group;

	
	wait_queue_head_t event_wait;

	
	struct kernfs_open_file *of;

	
	int event;

	
	struct psi_window win;

	
	u64 last_event_time;

	
	bool pending_event;

	
	enum psi_aggregators aggregator;
};

struct psi_group {
	struct psi_group *parent;
	bool enabled;

	
	struct mutex avgs_lock;

	
	struct psi_group_cpu __percpu *pcpu;

	
	u64 avg_total[NR_PSI_STATES - 1];
	u64 avg_last_update;
	u64 avg_next_update;

	
	struct delayed_work avgs_work;

	
	struct list_head avg_triggers;
	u32 avg_nr_triggers[NR_PSI_STATES - 1];

	
	u64 total[NR_PSI_AGGREGATORS][NR_PSI_STATES - 1];
	unsigned long avg[NR_PSI_STATES - 1][3];

	
	struct task_struct __rcu *rtpoll_task;
	struct timer_list rtpoll_timer;
	wait_queue_head_t rtpoll_wait;
	atomic_t rtpoll_wakeup;
	atomic_t rtpoll_scheduled;

	
	struct mutex rtpoll_trigger_lock;

	
	struct list_head rtpoll_triggers;
	u32 rtpoll_nr_triggers[NR_PSI_STATES - 1];
	u32 rtpoll_states;
	u64 rtpoll_min_period;

	
	u64 rtpoll_total[NR_PSI_STATES - 1];
	u64 rtpoll_next_update;
	u64 rtpoll_until;
};

#else 

#define NR_PSI_RESOURCES	0

struct psi_group { };

#endif 

#endif 
