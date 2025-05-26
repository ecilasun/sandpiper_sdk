/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_BACKING_DEV_DEFS_H
#define __LINUX_BACKING_DEV_DEFS_H

#include <linux/list.h>
#include <linux/radix-tree.h>
#include <linux/rbtree.h>
#include <linux/spinlock.h>
#include <linux/percpu_counter.h>
#include <linux/percpu-refcount.h>
#include <linux/flex_proportions.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/kref.h>
#include <linux/refcount.h>

struct page;
struct device;
struct dentry;


enum wb_state {
	WB_registered,		
	WB_writeback_running,	
	WB_has_dirty_io,	
	WB_start_all,		
};

enum wb_stat_item {
	WB_RECLAIMABLE,
	WB_WRITEBACK,
	WB_DIRTIED,
	WB_WRITTEN,
	NR_WB_STAT_ITEMS
};

#define WB_STAT_BATCH (8*(1+ilog2(nr_cpu_ids)))


enum wb_reason {
	WB_REASON_BACKGROUND,
	WB_REASON_VMSCAN,
	WB_REASON_SYNC,
	WB_REASON_PERIODIC,
	WB_REASON_LAPTOP_TIMER,
	WB_REASON_FS_FREE_SPACE,
	
	WB_REASON_FORKER_THREAD,
	WB_REASON_FOREIGN_FLUSH,

	WB_REASON_MAX,
};

struct wb_completion {
	atomic_t		cnt;
	wait_queue_head_t	*waitq;
};

#define __WB_COMPLETION_INIT(_waitq)	\
	(struct wb_completion){ .cnt = ATOMIC_INIT(1), .waitq = (_waitq) }


#define WB_COMPLETION_INIT(bdi)		__WB_COMPLETION_INIT(&(bdi)->wb_waitq)

#define DEFINE_WB_COMPLETION(cmpl, bdi)	\
	struct wb_completion cmpl = WB_COMPLETION_INIT(bdi)


struct bdi_writeback {
	struct backing_dev_info *bdi;	

	unsigned long state;		
	unsigned long last_old_flush;	

	struct list_head b_dirty;	
	struct list_head b_io;		
	struct list_head b_more_io;	
	struct list_head b_dirty_time;	
	spinlock_t list_lock;		

	atomic_t writeback_inodes;	
	struct percpu_counter stat[NR_WB_STAT_ITEMS];

	unsigned long bw_time_stamp;	
	unsigned long dirtied_stamp;
	unsigned long written_stamp;	
	unsigned long write_bandwidth;	
	unsigned long avg_write_bandwidth; 

	
	unsigned long dirty_ratelimit;
	unsigned long balanced_dirty_ratelimit;

	struct fprop_local_percpu completions;
	int dirty_exceeded;
	enum wb_reason start_all_reason;

	spinlock_t work_lock;		
	struct list_head work_list;
	struct delayed_work dwork;	
	struct delayed_work bw_dwork;	

	struct list_head bdi_node;	

#ifdef CONFIG_CGROUP_WRITEBACK
	struct percpu_ref refcnt;	
	struct fprop_local_percpu memcg_completions;
	struct cgroup_subsys_state *memcg_css; 
	struct cgroup_subsys_state *blkcg_css; 
	struct list_head memcg_node;	
	struct list_head blkcg_node;	
	struct list_head b_attached;	
	struct list_head offline_node;	

	union {
		struct work_struct release_work;
		struct rcu_head rcu;
	};
#endif
};

struct backing_dev_info {
	u64 id;
	struct rb_node rb_node; 
	struct list_head bdi_list;
	unsigned long ra_pages;	
	unsigned long io_pages;	

	struct kref refcnt;	
	unsigned int capabilities; 
	unsigned int min_ratio;
	unsigned int max_ratio, max_prop_frac;

	
	atomic_long_t tot_write_bandwidth;
	
	unsigned long last_bdp_sleep;

	struct bdi_writeback wb;  
	struct list_head wb_list; 
#ifdef CONFIG_CGROUP_WRITEBACK
	struct radix_tree_root cgwb_tree; 
	struct mutex cgwb_release_mutex;  
	struct rw_semaphore wb_switch_rwsem; 
#endif
	wait_queue_head_t wb_waitq;

	struct device *dev;
	char dev_name[64];
	struct device *owner;

	struct timer_list laptop_mode_wb_timer;

#ifdef CONFIG_DEBUG_FS
	struct dentry *debug_dir;
#endif
};

struct wb_lock_cookie {
	bool locked;
	unsigned long flags;
};

#ifdef CONFIG_CGROUP_WRITEBACK


static inline bool wb_tryget(struct bdi_writeback *wb)
{
	if (wb != &wb->bdi->wb)
		return percpu_ref_tryget(&wb->refcnt);
	return true;
}


static inline void wb_get(struct bdi_writeback *wb)
{
	if (wb != &wb->bdi->wb)
		percpu_ref_get(&wb->refcnt);
}


static inline void wb_put_many(struct bdi_writeback *wb, unsigned long nr)
{
	if (WARN_ON_ONCE(!wb->bdi)) {
		
		return;
	}

	if (wb != &wb->bdi->wb)
		percpu_ref_put_many(&wb->refcnt, nr);
}


static inline void wb_put(struct bdi_writeback *wb)
{
	wb_put_many(wb, 1);
}


static inline bool wb_dying(struct bdi_writeback *wb)
{
	return percpu_ref_is_dying(&wb->refcnt);
}

#else	

static inline bool wb_tryget(struct bdi_writeback *wb)
{
	return true;
}

static inline void wb_get(struct bdi_writeback *wb)
{
}

static inline void wb_put(struct bdi_writeback *wb)
{
}

static inline void wb_put_many(struct bdi_writeback *wb, unsigned long nr)
{
}

static inline bool wb_dying(struct bdi_writeback *wb)
{
	return false;
}

#endif	

#endif	
