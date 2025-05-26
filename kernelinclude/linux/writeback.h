/* SPDX-License-Identifier: GPL-2.0 */

#ifndef WRITEBACK_H
#define WRITEBACK_H

#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/flex_proportions.h>
#include <linux/backing-dev-defs.h>
#include <linux/blk_types.h>

struct bio;

DECLARE_PER_CPU(int, dirty_throttle_leaks);


#define DIRTY_SCOPE		8

struct backing_dev_info;


enum writeback_sync_modes {
	WB_SYNC_NONE,	
	WB_SYNC_ALL,	
};


struct writeback_control {
	long nr_to_write;		
	long pages_skipped;		

	
	loff_t range_start;
	loff_t range_end;

	enum writeback_sync_modes sync_mode;

	unsigned for_kupdate:1;		
	unsigned for_background:1;	
	unsigned tagged_writepages:1;	
	unsigned for_reclaim:1;		
	unsigned range_cyclic:1;	
	unsigned for_sync:1;		
	unsigned unpinned_fscache_wb:1;	

	
	unsigned no_cgroup_owner:1;

	
	struct swap_iocb **swap_plug;

#ifdef CONFIG_CGROUP_WRITEBACK
	struct bdi_writeback *wb;	
	struct inode *inode;		

	
	int wb_id;			
	int wb_lcand_id;		
	int wb_tcand_id;		
	size_t wb_bytes;		
	size_t wb_lcand_bytes;		
	size_t wb_tcand_bytes;		
#endif
};

static inline blk_opf_t wbc_to_write_flags(struct writeback_control *wbc)
{
	blk_opf_t flags = 0;

	if (wbc->sync_mode == WB_SYNC_ALL)
		flags |= REQ_SYNC;
	else if (wbc->for_kupdate || wbc->for_background)
		flags |= REQ_BACKGROUND;

	return flags;
}

#ifdef CONFIG_CGROUP_WRITEBACK
#define wbc_blkcg_css(wbc) \
	((wbc)->wb ? (wbc)->wb->blkcg_css : blkcg_root_css)
#else
#define wbc_blkcg_css(wbc)		(blkcg_root_css)
#endif 


struct wb_domain {
	spinlock_t lock;

	
	struct fprop_global completions;
	struct timer_list period_timer;	
	unsigned long period_time;

	
	unsigned long dirty_limit_tstamp;
	unsigned long dirty_limit;
};


static inline void wb_domain_size_changed(struct wb_domain *dom)
{
	spin_lock(&dom->lock);
	dom->dirty_limit_tstamp = jiffies;
	dom->dirty_limit = 0;
	spin_unlock(&dom->lock);
}

	
struct bdi_writeback;
void writeback_inodes_sb(struct super_block *, enum wb_reason reason);
void writeback_inodes_sb_nr(struct super_block *, unsigned long nr,
							enum wb_reason reason);
void try_to_writeback_inodes_sb(struct super_block *sb, enum wb_reason reason);
void sync_inodes_sb(struct super_block *);
void wakeup_flusher_threads(enum wb_reason reason);
void wakeup_flusher_threads_bdi(struct backing_dev_info *bdi,
				enum wb_reason reason);
void inode_wait_for_writeback(struct inode *inode);
void inode_io_list_del(struct inode *inode);


static inline void wait_on_inode(struct inode *inode)
{
	might_sleep();
	wait_on_bit(&inode->i_state, __I_NEW, TASK_UNINTERRUPTIBLE);
}

#ifdef CONFIG_CGROUP_WRITEBACK

#include <linux/cgroup.h>
#include <linux/bio.h>

void __inode_attach_wb(struct inode *inode, struct folio *folio);
void wbc_attach_and_unlock_inode(struct writeback_control *wbc,
				 struct inode *inode)
	__releases(&inode->i_lock);
void wbc_detach_inode(struct writeback_control *wbc);
void wbc_account_cgroup_owner(struct writeback_control *wbc, struct page *page,
			      size_t bytes);
int cgroup_writeback_by_id(u64 bdi_id, int memcg_id,
			   enum wb_reason reason, struct wb_completion *done);
void cgroup_writeback_umount(void);
bool cleanup_offline_cgwb(struct bdi_writeback *wb);


static inline void inode_attach_wb(struct inode *inode, struct folio *folio)
{
	if (!inode->i_wb)
		__inode_attach_wb(inode, folio);
}


static inline void inode_detach_wb(struct inode *inode)
{
	if (inode->i_wb) {
		WARN_ON_ONCE(!(inode->i_state & I_CLEAR));
		wb_put(inode->i_wb);
		inode->i_wb = NULL;
	}
}


static inline void wbc_attach_fdatawrite_inode(struct writeback_control *wbc,
					       struct inode *inode)
{
	spin_lock(&inode->i_lock);
	inode_attach_wb(inode, NULL);
	wbc_attach_and_unlock_inode(wbc, inode);
}


static inline void wbc_init_bio(struct writeback_control *wbc, struct bio *bio)
{
	
	if (wbc->wb)
		bio_associate_blkg_from_css(bio, wbc->wb->blkcg_css);
}

#else	

static inline void inode_attach_wb(struct inode *inode, struct folio *folio)
{
}

static inline void inode_detach_wb(struct inode *inode)
{
}

static inline void wbc_attach_and_unlock_inode(struct writeback_control *wbc,
					       struct inode *inode)
	__releases(&inode->i_lock)
{
	spin_unlock(&inode->i_lock);
}

static inline void wbc_attach_fdatawrite_inode(struct writeback_control *wbc,
					       struct inode *inode)
{
}

static inline void wbc_detach_inode(struct writeback_control *wbc)
{
}

static inline void wbc_init_bio(struct writeback_control *wbc, struct bio *bio)
{
}

static inline void wbc_account_cgroup_owner(struct writeback_control *wbc,
					    struct page *page, size_t bytes)
{
}

static inline void cgroup_writeback_umount(void)
{
}

#endif	


void laptop_io_completion(struct backing_dev_info *info);
void laptop_sync_completion(void);
void laptop_mode_timer_fn(struct timer_list *t);
bool node_dirty_ok(struct pglist_data *pgdat);
int wb_domain_init(struct wb_domain *dom, gfp_t gfp);
#ifdef CONFIG_CGROUP_WRITEBACK
void wb_domain_exit(struct wb_domain *dom);
#endif

extern struct wb_domain global_wb_domain;


extern unsigned int dirty_writeback_interval;
extern unsigned int dirty_expire_interval;
extern unsigned int dirtytime_expire_interval;
extern int laptop_mode;

int dirtytime_interval_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);

void global_dirty_limits(unsigned long *pbackground, unsigned long *pdirty);
unsigned long wb_calc_thresh(struct bdi_writeback *wb, unsigned long thresh);

void wb_update_bandwidth(struct bdi_writeback *wb);


#define BDP_ASYNC 0x0001

void balance_dirty_pages_ratelimited(struct address_space *mapping);
int balance_dirty_pages_ratelimited_flags(struct address_space *mapping,
		unsigned int flags);

bool wb_over_bg_thresh(struct bdi_writeback *wb);

typedef int (*writepage_t)(struct folio *folio, struct writeback_control *wbc,
				void *data);

void tag_pages_for_writeback(struct address_space *mapping,
			     pgoff_t start, pgoff_t end);
int write_cache_pages(struct address_space *mapping,
		      struct writeback_control *wbc, writepage_t writepage,
		      void *data);
int do_writepages(struct address_space *mapping, struct writeback_control *wbc);
void writeback_set_ratelimit(void);
void tag_pages_for_writeback(struct address_space *mapping,
			     pgoff_t start, pgoff_t end);

bool filemap_dirty_folio(struct address_space *mapping, struct folio *folio);
bool folio_redirty_for_writepage(struct writeback_control *, struct folio *);
bool redirty_page_for_writepage(struct writeback_control *, struct page *);

void sb_mark_inode_writeback(struct inode *inode);
void sb_clear_inode_writeback(struct inode *inode);

#endif		
