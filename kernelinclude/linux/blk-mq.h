/* SPDX-License-Identifier: GPL-2.0 */
#ifndef BLK_MQ_H
#define BLK_MQ_H

#include <linux/blkdev.h>
#include <linux/sbitmap.h>
#include <linux/lockdep.h>
#include <linux/scatterlist.h>
#include <linux/prefetch.h>
#include <linux/srcu.h>

struct blk_mq_tags;
struct blk_flush_queue;

#define BLKDEV_MIN_RQ	4
#define BLKDEV_DEFAULT_RQ	128

enum rq_end_io_ret {
	RQ_END_IO_NONE,
	RQ_END_IO_FREE,
};

typedef enum rq_end_io_ret (rq_end_io_fn)(struct request *, blk_status_t);


typedef __u32 __bitwise req_flags_t;


#define RQF_STARTED		((__force req_flags_t)(1 << 1))

#define RQF_FLUSH_SEQ		((__force req_flags_t)(1 << 4))

#define RQF_MIXED_MERGE		((__force req_flags_t)(1 << 5))

#define RQF_MQ_INFLIGHT		((__force req_flags_t)(1 << 6))

#define RQF_DONTPREP		((__force req_flags_t)(1 << 7))

#define RQF_SCHED_TAGS		((__force req_flags_t)(1 << 8))

#define RQF_USE_SCHED		((__force req_flags_t)(1 << 9))

#define RQF_FAILED		((__force req_flags_t)(1 << 10))

#define RQF_QUIET		((__force req_flags_t)(1 << 11))

#define RQF_IO_STAT		((__force req_flags_t)(1 << 13))

#define RQF_PM			((__force req_flags_t)(1 << 15))

#define RQF_HASHED		((__force req_flags_t)(1 << 16))

#define RQF_STATS		((__force req_flags_t)(1 << 17))

#define RQF_SPECIAL_PAYLOAD	((__force req_flags_t)(1 << 18))

#define RQF_ZONE_WRITE_LOCKED	((__force req_flags_t)(1 << 19))

#define RQF_TIMED_OUT		((__force req_flags_t)(1 << 21))
#define RQF_RESV		((__force req_flags_t)(1 << 23))


#define RQF_NOMERGE_FLAGS \
	(RQF_STARTED | RQF_FLUSH_SEQ | RQF_SPECIAL_PAYLOAD)

enum mq_rq_state {
	MQ_RQ_IDLE		= 0,
	MQ_RQ_IN_FLIGHT		= 1,
	MQ_RQ_COMPLETE		= 2,
};


struct request {
	struct request_queue *q;
	struct blk_mq_ctx *mq_ctx;
	struct blk_mq_hw_ctx *mq_hctx;

	blk_opf_t cmd_flags;		
	req_flags_t rq_flags;

	int tag;
	int internal_tag;

	unsigned int timeout;

	
	unsigned int __data_len;	
	sector_t __sector;		

	struct bio *bio;
	struct bio *biotail;

	union {
		struct list_head queuelist;
		struct request *rq_next;
	};

	struct block_device *part;
#ifdef CONFIG_BLK_RQ_ALLOC_TIME
	
	u64 alloc_time_ns;
#endif
	
	u64 start_time_ns;
	
	u64 io_start_time_ns;

#ifdef CONFIG_BLK_WBT
	unsigned short wbt_flags;
#endif
	
	unsigned short stats_sectors;

	
	unsigned short nr_phys_segments;

#ifdef CONFIG_BLK_DEV_INTEGRITY
	unsigned short nr_integrity_segments;
#endif

#ifdef CONFIG_BLK_INLINE_ENCRYPTION
	struct bio_crypt_ctx *crypt_ctx;
	struct blk_crypto_keyslot *crypt_keyslot;
#endif

	unsigned short ioprio;

	enum mq_rq_state state;
	atomic_t ref;

	unsigned long deadline;

	
	union {
		struct hlist_node hash;	
		struct llist_node ipi_list;
	};

	
	union {
		struct rb_node rb_node;	
		struct bio_vec special_vec;
	};

	
	struct {
		struct io_cq		*icq;
		void			*priv[2];
	} elv;

	struct {
		unsigned int		seq;
		rq_end_io_fn		*saved_end_io;
	} flush;

	u64 fifo_time;

	
	rq_end_io_fn *end_io;
	void *end_io_data;
};

static inline enum req_op req_op(const struct request *req)
{
	return req->cmd_flags & REQ_OP_MASK;
}

static inline bool blk_rq_is_passthrough(struct request *rq)
{
	return blk_op_is_passthrough(rq->cmd_flags);
}

static inline unsigned short req_get_ioprio(struct request *req)
{
	return req->ioprio;
}

#define rq_data_dir(rq)		(op_is_write(req_op(rq)) ? WRITE : READ)

#define rq_dma_dir(rq) \
	(op_is_write(req_op(rq)) ? DMA_TO_DEVICE : DMA_FROM_DEVICE)

#define rq_list_add(listptr, rq)	do {		\
	(rq)->rq_next = *(listptr);			\
	*(listptr) = rq;				\
} while (0)

#define rq_list_add_tail(lastpptr, rq)	do {		\
	(rq)->rq_next = NULL;				\
	**(lastpptr) = rq;				\
	*(lastpptr) = &rq->rq_next;			\
} while (0)

#define rq_list_pop(listptr)				\
({							\
	struct request *__req = NULL;			\
	if ((listptr) && *(listptr))	{		\
		__req = *(listptr);			\
		*(listptr) = __req->rq_next;		\
	}						\
	__req;						\
})

#define rq_list_peek(listptr)				\
({							\
	struct request *__req = NULL;			\
	if ((listptr) && *(listptr))			\
		__req = *(listptr);			\
	__req;						\
})

#define rq_list_for_each(listptr, pos)			\
	for (pos = rq_list_peek((listptr)); pos; pos = rq_list_next(pos))

#define rq_list_for_each_safe(listptr, pos, nxt)			\
	for (pos = rq_list_peek((listptr)), nxt = rq_list_next(pos);	\
		pos; pos = nxt, nxt = pos ? rq_list_next(pos) : NULL)

#define rq_list_next(rq)	(rq)->rq_next
#define rq_list_empty(list)	((list) == (struct request *) NULL)


static inline void rq_list_move(struct request **src, struct request **dst,
				struct request *rq, struct request *prev)
{
	if (prev)
		prev->rq_next = rq->rq_next;
	else
		*src = rq->rq_next;
	rq_list_add(dst, rq);
}


enum blk_eh_timer_return {
	BLK_EH_DONE,
	BLK_EH_RESET_TIMER,
};

#define BLK_TAG_ALLOC_FIFO 0 
#define BLK_TAG_ALLOC_RR 1 


struct blk_mq_hw_ctx {
	struct {
		
		spinlock_t		lock;
		
		struct list_head	dispatch;
		 
		unsigned long		state;
	} ____cacheline_aligned_in_smp;

	
	struct delayed_work	run_work;
	
	cpumask_var_t		cpumask;
	
	int			next_cpu;
	
	int			next_cpu_batch;

	
	unsigned long		flags;

	
	void			*sched_data;
	
	struct request_queue	*queue;
	
	struct blk_flush_queue	*fq;

	
	void			*driver_data;

	
	struct sbitmap		ctx_map;

	
	struct blk_mq_ctx	*dispatch_from;
	
	unsigned int		dispatch_busy;

	
	unsigned short		type;
	
	unsigned short		nr_ctx;
	
	struct blk_mq_ctx	**ctxs;

	
	spinlock_t		dispatch_wait_lock;
	
	wait_queue_entry_t	dispatch_wait;

	
	atomic_t		wait_index;

	
	struct blk_mq_tags	*tags;
	
	struct blk_mq_tags	*sched_tags;

	
	unsigned long		run;

	
	unsigned int		numa_node;
	
	unsigned int		queue_num;

	
	atomic_t		nr_active;

	
	struct hlist_node	cpuhp_online;
	
	struct hlist_node	cpuhp_dead;
	
	struct kobject		kobj;

#ifdef CONFIG_BLK_DEBUG_FS
	
	struct dentry		*debugfs_dir;
	
	struct dentry		*sched_debugfs_dir;
#endif

	
	struct list_head	hctx_list;
};


struct blk_mq_queue_map {
	unsigned int *mq_map;
	unsigned int nr_queues;
	unsigned int queue_offset;
};


enum hctx_type {
	HCTX_TYPE_DEFAULT,
	HCTX_TYPE_READ,
	HCTX_TYPE_POLL,

	HCTX_MAX_TYPES,
};


struct blk_mq_tag_set {
	const struct blk_mq_ops	*ops;
	struct blk_mq_queue_map	map[HCTX_MAX_TYPES];
	unsigned int		nr_maps;
	unsigned int		nr_hw_queues;
	unsigned int		queue_depth;
	unsigned int		reserved_tags;
	unsigned int		cmd_size;
	int			numa_node;
	unsigned int		timeout;
	unsigned int		flags;
	void			*driver_data;

	struct blk_mq_tags	**tags;

	struct blk_mq_tags	*shared_tags;

	struct mutex		tag_list_lock;
	struct list_head	tag_list;
	struct srcu_struct	*srcu;
};


struct blk_mq_queue_data {
	struct request *rq;
	bool last;
};

typedef bool (busy_tag_iter_fn)(struct request *, void *);


struct blk_mq_ops {
	
	blk_status_t (*queue_rq)(struct blk_mq_hw_ctx *,
				 const struct blk_mq_queue_data *);

	
	void (*commit_rqs)(struct blk_mq_hw_ctx *);

	
	void (*queue_rqs)(struct request **rqlist);

	
	int (*get_budget)(struct request_queue *);

	
	void (*put_budget)(struct request_queue *, int);

	
	void (*set_rq_budget_token)(struct request *, int);
	
	int (*get_rq_budget_token)(struct request *);

	
	enum blk_eh_timer_return (*timeout)(struct request *);

	
	int (*poll)(struct blk_mq_hw_ctx *, struct io_comp_batch *);

	
	void (*complete)(struct request *);

	
	int (*init_hctx)(struct blk_mq_hw_ctx *, void *, unsigned int);
	
	void (*exit_hctx)(struct blk_mq_hw_ctx *, unsigned int);

	
	int (*init_request)(struct blk_mq_tag_set *set, struct request *,
			    unsigned int, unsigned int);
	
	void (*exit_request)(struct blk_mq_tag_set *set, struct request *,
			     unsigned int);

	
	void (*cleanup_rq)(struct request *);

	
	bool (*busy)(struct request_queue *);

	
	void (*map_queues)(struct blk_mq_tag_set *set);

#ifdef CONFIG_BLK_DEBUG_FS
	
	void (*show_rq)(struct seq_file *m, struct request *rq);
#endif
};

enum {
	BLK_MQ_F_SHOULD_MERGE	= 1 << 0,
	BLK_MQ_F_TAG_QUEUE_SHARED = 1 << 1,
	
	BLK_MQ_F_STACKING	= 1 << 2,
	BLK_MQ_F_TAG_HCTX_SHARED = 1 << 3,
	BLK_MQ_F_BLOCKING	= 1 << 5,
	
	BLK_MQ_F_NO_SCHED	= 1 << 6,
	
	BLK_MQ_F_NO_SCHED_BY_DEFAULT	= 1 << 7,
	BLK_MQ_F_ALLOC_POLICY_START_BIT = 8,
	BLK_MQ_F_ALLOC_POLICY_BITS = 1,

	BLK_MQ_S_STOPPED	= 0,
	BLK_MQ_S_TAG_ACTIVE	= 1,
	BLK_MQ_S_SCHED_RESTART	= 2,

	
	BLK_MQ_S_INACTIVE	= 3,

	BLK_MQ_MAX_DEPTH	= 10240,

	BLK_MQ_CPU_WORK_BATCH	= 8,
};
#define BLK_MQ_FLAG_TO_ALLOC_POLICY(flags) \
	((flags >> BLK_MQ_F_ALLOC_POLICY_START_BIT) & \
		((1 << BLK_MQ_F_ALLOC_POLICY_BITS) - 1))
#define BLK_ALLOC_POLICY_TO_MQ_FLAG(policy) \
	((policy & ((1 << BLK_MQ_F_ALLOC_POLICY_BITS) - 1)) \
		<< BLK_MQ_F_ALLOC_POLICY_START_BIT)

#define BLK_MQ_NO_HCTX_IDX	(-1U)

struct gendisk *__blk_mq_alloc_disk(struct blk_mq_tag_set *set, void *queuedata,
		struct lock_class_key *lkclass);
#define blk_mq_alloc_disk(set, queuedata)				\
({									\
	static struct lock_class_key __key;				\
									\
	__blk_mq_alloc_disk(set, queuedata, &__key);			\
})
struct gendisk *blk_mq_alloc_disk_for_queue(struct request_queue *q,
		struct lock_class_key *lkclass);
struct request_queue *blk_mq_init_queue(struct blk_mq_tag_set *);
int blk_mq_init_allocated_queue(struct blk_mq_tag_set *set,
		struct request_queue *q);
void blk_mq_destroy_queue(struct request_queue *);

int blk_mq_alloc_tag_set(struct blk_mq_tag_set *set);
int blk_mq_alloc_sq_tag_set(struct blk_mq_tag_set *set,
		const struct blk_mq_ops *ops, unsigned int queue_depth,
		unsigned int set_flags);
void blk_mq_free_tag_set(struct blk_mq_tag_set *set);

void blk_mq_free_request(struct request *rq);
int blk_rq_poll(struct request *rq, struct io_comp_batch *iob,
		unsigned int poll_flags);

bool blk_mq_queue_inflight(struct request_queue *q);

enum {
	
	BLK_MQ_REQ_NOWAIT	= (__force blk_mq_req_flags_t)(1 << 0),
	
	BLK_MQ_REQ_RESERVED	= (__force blk_mq_req_flags_t)(1 << 1),
	
	BLK_MQ_REQ_PM		= (__force blk_mq_req_flags_t)(1 << 2),
};

struct request *blk_mq_alloc_request(struct request_queue *q, blk_opf_t opf,
		blk_mq_req_flags_t flags);
struct request *blk_mq_alloc_request_hctx(struct request_queue *q,
		blk_opf_t opf, blk_mq_req_flags_t flags,
		unsigned int hctx_idx);


struct blk_mq_tags {
	unsigned int nr_tags;
	unsigned int nr_reserved_tags;
	unsigned int active_queues;

	struct sbitmap_queue bitmap_tags;
	struct sbitmap_queue breserved_tags;

	struct request **rqs;
	struct request **static_rqs;
	struct list_head page_list;

	
	spinlock_t lock;
};

static inline struct request *blk_mq_tag_to_rq(struct blk_mq_tags *tags,
					       unsigned int tag)
{
	if (tag < tags->nr_tags) {
		prefetch(tags->rqs[tag]);
		return tags->rqs[tag];
	}

	return NULL;
}

enum {
	BLK_MQ_UNIQUE_TAG_BITS = 16,
	BLK_MQ_UNIQUE_TAG_MASK = (1 << BLK_MQ_UNIQUE_TAG_BITS) - 1,
};

u32 blk_mq_unique_tag(struct request *rq);

static inline u16 blk_mq_unique_tag_to_hwq(u32 unique_tag)
{
	return unique_tag >> BLK_MQ_UNIQUE_TAG_BITS;
}

static inline u16 blk_mq_unique_tag_to_tag(u32 unique_tag)
{
	return unique_tag & BLK_MQ_UNIQUE_TAG_MASK;
}


static inline enum mq_rq_state blk_mq_rq_state(struct request *rq)
{
	return READ_ONCE(rq->state);
}

static inline int blk_mq_request_started(struct request *rq)
{
	return blk_mq_rq_state(rq) != MQ_RQ_IDLE;
}

static inline int blk_mq_request_completed(struct request *rq)
{
	return blk_mq_rq_state(rq) == MQ_RQ_COMPLETE;
}


static inline void blk_mq_set_request_complete(struct request *rq)
{
	WRITE_ONCE(rq->state, MQ_RQ_COMPLETE);
}


static inline void blk_mq_complete_request_direct(struct request *rq,
		   void (*complete)(struct request *rq))
{
	WRITE_ONCE(rq->state, MQ_RQ_COMPLETE);
	complete(rq);
}

void blk_mq_start_request(struct request *rq);
void blk_mq_end_request(struct request *rq, blk_status_t error);
void __blk_mq_end_request(struct request *rq, blk_status_t error);
void blk_mq_end_request_batch(struct io_comp_batch *ib);


static inline bool blk_mq_need_time_stamp(struct request *rq)
{
	return (rq->rq_flags & (RQF_IO_STAT | RQF_STATS | RQF_USE_SCHED));
}

static inline bool blk_mq_is_reserved_rq(struct request *rq)
{
	return rq->rq_flags & RQF_RESV;
}


static inline bool blk_mq_add_to_batch(struct request *req,
				       struct io_comp_batch *iob, int ioerror,
				       void (*complete)(struct io_comp_batch *))
{
	
	if (!iob || (req->rq_flags & RQF_SCHED_TAGS) || ioerror ||
			(req->end_io && !blk_rq_is_passthrough(req)))
		return false;

	if (!iob->complete)
		iob->complete = complete;
	else if (iob->complete != complete)
		return false;
	iob->need_ts |= blk_mq_need_time_stamp(req);
	rq_list_add(&iob->req_list, req);
	return true;
}

void blk_mq_requeue_request(struct request *rq, bool kick_requeue_list);
void blk_mq_kick_requeue_list(struct request_queue *q);
void blk_mq_delay_kick_requeue_list(struct request_queue *q, unsigned long msecs);
void blk_mq_complete_request(struct request *rq);
bool blk_mq_complete_request_remote(struct request *rq);
void blk_mq_stop_hw_queue(struct blk_mq_hw_ctx *hctx);
void blk_mq_start_hw_queue(struct blk_mq_hw_ctx *hctx);
void blk_mq_stop_hw_queues(struct request_queue *q);
void blk_mq_start_hw_queues(struct request_queue *q);
void blk_mq_start_stopped_hw_queue(struct blk_mq_hw_ctx *hctx, bool async);
void blk_mq_start_stopped_hw_queues(struct request_queue *q, bool async);
void blk_mq_quiesce_queue(struct request_queue *q);
void blk_mq_wait_quiesce_done(struct blk_mq_tag_set *set);
void blk_mq_quiesce_tagset(struct blk_mq_tag_set *set);
void blk_mq_unquiesce_tagset(struct blk_mq_tag_set *set);
void blk_mq_unquiesce_queue(struct request_queue *q);
void blk_mq_delay_run_hw_queue(struct blk_mq_hw_ctx *hctx, unsigned long msecs);
void blk_mq_run_hw_queue(struct blk_mq_hw_ctx *hctx, bool async);
void blk_mq_run_hw_queues(struct request_queue *q, bool async);
void blk_mq_delay_run_hw_queues(struct request_queue *q, unsigned long msecs);
void blk_mq_tagset_busy_iter(struct blk_mq_tag_set *tagset,
		busy_tag_iter_fn *fn, void *priv);
void blk_mq_tagset_wait_completed_request(struct blk_mq_tag_set *tagset);
void blk_mq_freeze_queue(struct request_queue *q);
void blk_mq_unfreeze_queue(struct request_queue *q);
void blk_freeze_queue_start(struct request_queue *q);
void blk_mq_freeze_queue_wait(struct request_queue *q);
int blk_mq_freeze_queue_wait_timeout(struct request_queue *q,
				     unsigned long timeout);

void blk_mq_map_queues(struct blk_mq_queue_map *qmap);
void blk_mq_update_nr_hw_queues(struct blk_mq_tag_set *set, int nr_hw_queues);

void blk_mq_quiesce_queue_nowait(struct request_queue *q);

unsigned int blk_mq_rq_cpu(struct request *rq);

bool __blk_should_fake_timeout(struct request_queue *q);
static inline bool blk_should_fake_timeout(struct request_queue *q)
{
	if (IS_ENABLED(CONFIG_FAIL_IO_TIMEOUT) &&
	    test_bit(QUEUE_FLAG_FAIL_IO, &q->queue_flags))
		return __blk_should_fake_timeout(q);
	return false;
}


static inline struct request *blk_mq_rq_from_pdu(void *pdu)
{
	return pdu - sizeof(struct request);
}


static inline void *blk_mq_rq_to_pdu(struct request *rq)
{
	return rq + 1;
}

#define queue_for_each_hw_ctx(q, hctx, i)				\
	xa_for_each(&(q)->hctx_table, (i), (hctx))

#define hctx_for_each_ctx(hctx, ctx, i)					\
	for ((i) = 0; (i) < (hctx)->nr_ctx &&				\
	     ({ ctx = (hctx)->ctxs[(i)]; 1; }); (i)++)

static inline void blk_mq_cleanup_rq(struct request *rq)
{
	if (rq->q->mq_ops->cleanup_rq)
		rq->q->mq_ops->cleanup_rq(rq);
}

static inline void blk_rq_bio_prep(struct request *rq, struct bio *bio,
		unsigned int nr_segs)
{
	rq->nr_phys_segments = nr_segs;
	rq->__data_len = bio->bi_iter.bi_size;
	rq->bio = rq->biotail = bio;
	rq->ioprio = bio_prio(bio);
}

void blk_mq_hctx_set_fq_lock_class(struct blk_mq_hw_ctx *hctx,
		struct lock_class_key *key);

static inline bool rq_is_sync(struct request *rq)
{
	return op_is_sync(rq->cmd_flags);
}

void blk_rq_init(struct request_queue *q, struct request *rq);
int blk_rq_prep_clone(struct request *rq, struct request *rq_src,
		struct bio_set *bs, gfp_t gfp_mask,
		int (*bio_ctr)(struct bio *, struct bio *, void *), void *data);
void blk_rq_unprep_clone(struct request *rq);
blk_status_t blk_insert_cloned_request(struct request *rq);

struct rq_map_data {
	struct page **pages;
	unsigned long offset;
	unsigned short page_order;
	unsigned short nr_entries;
	bool null_mapped;
	bool from_user;
};

int blk_rq_map_user(struct request_queue *, struct request *,
		struct rq_map_data *, void __user *, unsigned long, gfp_t);
int blk_rq_map_user_io(struct request *, struct rq_map_data *,
		void __user *, unsigned long, gfp_t, bool, int, bool, int);
int blk_rq_map_user_iov(struct request_queue *, struct request *,
		struct rq_map_data *, const struct iov_iter *, gfp_t);
int blk_rq_unmap_user(struct bio *);
int blk_rq_map_kern(struct request_queue *, struct request *, void *,
		unsigned int, gfp_t);
int blk_rq_append_bio(struct request *rq, struct bio *bio);
void blk_execute_rq_nowait(struct request *rq, bool at_head);
blk_status_t blk_execute_rq(struct request *rq, bool at_head);
bool blk_rq_is_poll(struct request *rq);

struct req_iterator {
	struct bvec_iter iter;
	struct bio *bio;
};

#define __rq_for_each_bio(_bio, rq)	\
	if ((rq->bio))			\
		for (_bio = (rq)->bio; _bio; _bio = _bio->bi_next)

#define rq_for_each_segment(bvl, _rq, _iter)			\
	__rq_for_each_bio(_iter.bio, _rq)			\
		bio_for_each_segment(bvl, _iter.bio, _iter.iter)

#define rq_for_each_bvec(bvl, _rq, _iter)			\
	__rq_for_each_bio(_iter.bio, _rq)			\
		bio_for_each_bvec(bvl, _iter.bio, _iter.iter)

#define rq_iter_last(bvec, _iter)				\
		(_iter.bio->bi_next == NULL &&			\
		 bio_iter_last(bvec, _iter.iter))


static inline sector_t blk_rq_pos(const struct request *rq)
{
	return rq->__sector;
}

static inline unsigned int blk_rq_bytes(const struct request *rq)
{
	return rq->__data_len;
}

static inline int blk_rq_cur_bytes(const struct request *rq)
{
	if (!rq->bio)
		return 0;
	if (!bio_has_data(rq->bio))	
		return rq->bio->bi_iter.bi_size;
	return bio_iovec(rq->bio).bv_len;
}

static inline unsigned int blk_rq_sectors(const struct request *rq)
{
	return blk_rq_bytes(rq) >> SECTOR_SHIFT;
}

static inline unsigned int blk_rq_cur_sectors(const struct request *rq)
{
	return blk_rq_cur_bytes(rq) >> SECTOR_SHIFT;
}

static inline unsigned int blk_rq_stats_sectors(const struct request *rq)
{
	return rq->stats_sectors;
}


static inline unsigned int blk_rq_payload_bytes(struct request *rq)
{
	if (rq->rq_flags & RQF_SPECIAL_PAYLOAD)
		return rq->special_vec.bv_len;
	return blk_rq_bytes(rq);
}


static inline struct bio_vec req_bvec(struct request *rq)
{
	if (rq->rq_flags & RQF_SPECIAL_PAYLOAD)
		return rq->special_vec;
	return mp_bvec_iter_bvec(rq->bio->bi_io_vec, rq->bio->bi_iter);
}

static inline unsigned int blk_rq_count_bios(struct request *rq)
{
	unsigned int nr_bios = 0;
	struct bio *bio;

	__rq_for_each_bio(bio, rq)
		nr_bios++;

	return nr_bios;
}

void blk_steal_bios(struct bio_list *list, struct request *rq);


bool blk_update_request(struct request *rq, blk_status_t error,
			       unsigned int nr_bytes);
void blk_abort_request(struct request *);


static inline unsigned short blk_rq_nr_phys_segments(struct request *rq)
{
	if (rq->rq_flags & RQF_SPECIAL_PAYLOAD)
		return 1;
	return rq->nr_phys_segments;
}


static inline unsigned short blk_rq_nr_discard_segments(struct request *rq)
{
	return max_t(unsigned short, rq->nr_phys_segments, 1);
}

int __blk_rq_map_sg(struct request_queue *q, struct request *rq,
		struct scatterlist *sglist, struct scatterlist **last_sg);
static inline int blk_rq_map_sg(struct request_queue *q, struct request *rq,
		struct scatterlist *sglist)
{
	struct scatterlist *last_sg = NULL;

	return __blk_rq_map_sg(q, rq, sglist, &last_sg);
}
void blk_dump_rq_flags(struct request *, char *);

#ifdef CONFIG_BLK_DEV_ZONED
static inline unsigned int blk_rq_zone_no(struct request *rq)
{
	return disk_zone_no(rq->q->disk, blk_rq_pos(rq));
}

static inline unsigned int blk_rq_zone_is_seq(struct request *rq)
{
	return disk_zone_is_seq(rq->q->disk, blk_rq_pos(rq));
}


static inline bool blk_rq_is_seq_zoned_write(struct request *rq)
{
	return op_needs_zoned_write_locking(req_op(rq)) &&
		blk_rq_zone_is_seq(rq);
}

bool blk_req_needs_zone_write_lock(struct request *rq);
bool blk_req_zone_write_trylock(struct request *rq);
void __blk_req_zone_write_lock(struct request *rq);
void __blk_req_zone_write_unlock(struct request *rq);

static inline void blk_req_zone_write_lock(struct request *rq)
{
	if (blk_req_needs_zone_write_lock(rq))
		__blk_req_zone_write_lock(rq);
}

static inline void blk_req_zone_write_unlock(struct request *rq)
{
	if (rq->rq_flags & RQF_ZONE_WRITE_LOCKED)
		__blk_req_zone_write_unlock(rq);
}

static inline bool blk_req_zone_is_write_locked(struct request *rq)
{
	return rq->q->disk->seq_zones_wlock &&
		test_bit(blk_rq_zone_no(rq), rq->q->disk->seq_zones_wlock);
}

static inline bool blk_req_can_dispatch_to_zone(struct request *rq)
{
	if (!blk_req_needs_zone_write_lock(rq))
		return true;
	return !blk_req_zone_is_write_locked(rq);
}
#else 
static inline bool blk_rq_is_seq_zoned_write(struct request *rq)
{
	return false;
}

static inline bool blk_req_needs_zone_write_lock(struct request *rq)
{
	return false;
}

static inline void blk_req_zone_write_lock(struct request *rq)
{
}

static inline void blk_req_zone_write_unlock(struct request *rq)
{
}
static inline bool blk_req_zone_is_write_locked(struct request *rq)
{
	return false;
}

static inline bool blk_req_can_dispatch_to_zone(struct request *rq)
{
	return true;
}
#endif 

#endif 
