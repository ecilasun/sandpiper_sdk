#ifndef IO_URING_TYPES_H
#define IO_URING_TYPES_H

#include <linux/blkdev.h>
#include <linux/task_work.h>
#include <linux/bitmap.h>
#include <linux/llist.h>
#include <uapi/linux/io_uring.h>

struct io_wq_work_node {
	struct io_wq_work_node *next;
};

struct io_wq_work_list {
	struct io_wq_work_node *first;
	struct io_wq_work_node *last;
};

struct io_wq_work {
	struct io_wq_work_node list;
	unsigned flags;
	
	int cancel_seq;
};

struct io_fixed_file {
	
	unsigned long file_ptr;
};

struct io_file_table {
	struct io_fixed_file *files;
	unsigned long *bitmap;
	unsigned int alloc_hint;
};

struct io_hash_bucket {
	spinlock_t		lock;
	struct hlist_head	list;
} ____cacheline_aligned_in_smp;

struct io_hash_table {
	struct io_hash_bucket	*hbs;
	unsigned		hash_bits;
};


#define IO_RINGFD_REG_MAX 16

struct io_uring_task {
	
	int				cached_refs;
	const struct io_ring_ctx 	*last;
	struct io_wq			*io_wq;
	struct file			*registered_rings[IO_RINGFD_REG_MAX];

	struct xarray			xa;
	struct wait_queue_head		wait;
	atomic_t			in_cancel;
	atomic_t			inflight_tracked;
	struct percpu_counter		inflight;

	struct { 
		struct llist_head	task_list;
		struct callback_head	task_work;
	} ____cacheline_aligned_in_smp;
};

struct io_uring {
	u32 head;
	u32 tail;
};


struct io_rings {
	
	struct io_uring		sq, cq;
	
	u32			sq_ring_mask, cq_ring_mask;
	
	u32			sq_ring_entries, cq_ring_entries;
	
	u32			sq_dropped;
	
	atomic_t		sq_flags;
	
	u32			cq_flags;
	
	u32			cq_overflow;
	
	struct io_uring_cqe	cqes[] ____cacheline_aligned_in_smp;
};

struct io_restriction {
	DECLARE_BITMAP(register_op, IORING_REGISTER_LAST);
	DECLARE_BITMAP(sqe_op, IORING_OP_LAST);
	u8 sqe_flags_allowed;
	u8 sqe_flags_required;
	bool registered;
};

struct io_submit_link {
	struct io_kiocb		*head;
	struct io_kiocb		*last;
};

struct io_submit_state {
	
	struct io_wq_work_node	free_list;
	
	struct io_wq_work_list	compl_reqs;
	struct io_submit_link	link;

	bool			plug_started;
	bool			need_plug;
	unsigned short		submit_nr;
	unsigned int		cqes_count;
	struct blk_plug		plug;
};

struct io_ev_fd {
	struct eventfd_ctx	*cq_ev_fd;
	unsigned int		eventfd_async: 1;
	struct rcu_head		rcu;
	atomic_t		refs;
	atomic_t		ops;
};

struct io_alloc_cache {
	struct io_wq_work_node	list;
	unsigned int		nr_cached;
	unsigned int		max_cached;
	size_t			elem_size;
};

struct io_ring_ctx {
	
	struct {
		unsigned int		flags;
		unsigned int		drain_next: 1;
		unsigned int		restricted: 1;
		unsigned int		off_timeout_used: 1;
		unsigned int		drain_active: 1;
		unsigned int		has_evfd: 1;
		
		unsigned int		task_complete: 1;
		unsigned int		lockless_cq: 1;
		unsigned int		syscall_iopoll: 1;
		unsigned int		poll_activated: 1;
		unsigned int		drain_disabled: 1;
		unsigned int		compat: 1;

		struct task_struct	*submitter_task;
		struct io_rings		*rings;
		struct percpu_ref	refs;

		enum task_work_notify_mode	notify_method;
	} ____cacheline_aligned_in_smp;

	
	struct {
		struct mutex		uring_lock;

		
		u32			*sq_array;
		struct io_uring_sqe	*sq_sqes;
		unsigned		cached_sq_head;
		unsigned		sq_entries;

		
		struct io_rsrc_node	*rsrc_node;
		atomic_t		cancel_seq;
		struct io_file_table	file_table;
		unsigned		nr_user_files;
		unsigned		nr_user_bufs;
		struct io_mapped_ubuf	**user_bufs;

		struct io_submit_state	submit_state;

		struct xarray		io_bl_xa;

		struct io_hash_table	cancel_table_locked;
		struct io_alloc_cache	apoll_cache;
		struct io_alloc_cache	netmsg_cache;

		
		struct io_wq_work_list	iopoll_list;
		bool			poll_multi_queue;
	} ____cacheline_aligned_in_smp;

	struct {
		
		struct io_uring_cqe	*cqe_cached;
		struct io_uring_cqe	*cqe_sentinel;

		unsigned		cached_cq_tail;
		unsigned		cq_entries;
		struct io_ev_fd	__rcu	*io_ev_fd;
		unsigned		cq_extra;
	} ____cacheline_aligned_in_smp;

	
	struct {
		struct llist_head	work_llist;
		unsigned long		check_cq;
		atomic_t		cq_wait_nr;
		atomic_t		cq_timeouts;
		struct wait_queue_head	cq_wait;
	} ____cacheline_aligned_in_smp;

	
	struct {
		spinlock_t		timeout_lock;
		struct list_head	timeout_list;
		struct list_head	ltimeout_list;
		unsigned		cq_last_tm_flush;
	} ____cacheline_aligned_in_smp;

	struct io_uring_cqe	completion_cqes[16];

	spinlock_t		completion_lock;

	
	struct io_wq_work_list	locked_free_list;
	unsigned int		locked_free_nr;

	struct list_head	io_buffers_comp;
	struct list_head	cq_overflow_list;
	struct io_hash_table	cancel_table;

	const struct cred	*sq_creds;	
	struct io_sq_data	*sq_data;	

	struct wait_queue_head	sqo_sq_wait;
	struct list_head	sqd_list;

	unsigned int		file_alloc_start;
	unsigned int		file_alloc_end;

	struct xarray		personalities;
	u32			pers_next;

	struct list_head	io_buffers_cache;

	
	struct hlist_head	io_buf_list;

	
	struct wait_queue_head		poll_wq;
	struct io_restriction		restrictions;

	
	struct io_mapped_ubuf		*dummy_ubuf;
	struct io_rsrc_data		*file_data;
	struct io_rsrc_data		*buf_data;

	
	struct list_head		rsrc_ref_list;
	struct io_alloc_cache		rsrc_node_cache;
	struct wait_queue_head		rsrc_quiesce_wq;
	unsigned			rsrc_quiesce;

	struct list_head		io_buffers_pages;

	
	struct io_wq_hash		*hash_map;

	
	struct user_struct		*user;
	struct mm_struct		*mm_account;

	
	struct llist_head		fallback_llist;
	struct delayed_work		fallback_work;
	struct work_struct		exit_work;
	struct list_head		tctx_list;
	struct completion		ref_comp;

	
	u32				iowq_limits[2];
	bool				iowq_limits_set;

	struct callback_head		poll_wq_task_work;
	struct list_head		defer_list;
	unsigned			sq_thread_idle;
	
	unsigned			evfd_last_cq_tail;

	
	unsigned short			n_ring_pages;
	unsigned short			n_sqe_pages;
	struct page			**ring_pages;
	struct page			**sqe_pages;
};

struct io_tw_state {
	
	bool locked;
};

enum {
	REQ_F_FIXED_FILE_BIT	= IOSQE_FIXED_FILE_BIT,
	REQ_F_IO_DRAIN_BIT	= IOSQE_IO_DRAIN_BIT,
	REQ_F_LINK_BIT		= IOSQE_IO_LINK_BIT,
	REQ_F_HARDLINK_BIT	= IOSQE_IO_HARDLINK_BIT,
	REQ_F_FORCE_ASYNC_BIT	= IOSQE_ASYNC_BIT,
	REQ_F_BUFFER_SELECT_BIT	= IOSQE_BUFFER_SELECT_BIT,
	REQ_F_CQE_SKIP_BIT	= IOSQE_CQE_SKIP_SUCCESS_BIT,

	
	REQ_F_FAIL_BIT		= 8,
	REQ_F_INFLIGHT_BIT,
	REQ_F_CUR_POS_BIT,
	REQ_F_NOWAIT_BIT,
	REQ_F_LINK_TIMEOUT_BIT,
	REQ_F_NEED_CLEANUP_BIT,
	REQ_F_POLLED_BIT,
	REQ_F_BUFFER_SELECTED_BIT,
	REQ_F_BUFFER_RING_BIT,
	REQ_F_REISSUE_BIT,
	REQ_F_CREDS_BIT,
	REQ_F_REFCOUNT_BIT,
	REQ_F_ARM_LTIMEOUT_BIT,
	REQ_F_ASYNC_DATA_BIT,
	REQ_F_SKIP_LINK_CQES_BIT,
	REQ_F_SINGLE_POLL_BIT,
	REQ_F_DOUBLE_POLL_BIT,
	REQ_F_PARTIAL_IO_BIT,
	REQ_F_APOLL_MULTISHOT_BIT,
	REQ_F_CLEAR_POLLIN_BIT,
	REQ_F_HASH_LOCKED_BIT,
	
	REQ_F_SUPPORT_NOWAIT_BIT,
	REQ_F_ISREG_BIT,

	
	__REQ_F_LAST_BIT,
};

enum {
	
	REQ_F_FIXED_FILE	= BIT(REQ_F_FIXED_FILE_BIT),
	
	REQ_F_IO_DRAIN		= BIT(REQ_F_IO_DRAIN_BIT),
	
	REQ_F_LINK		= BIT(REQ_F_LINK_BIT),
	
	REQ_F_HARDLINK		= BIT(REQ_F_HARDLINK_BIT),
	
	REQ_F_FORCE_ASYNC	= BIT(REQ_F_FORCE_ASYNC_BIT),
	
	REQ_F_BUFFER_SELECT	= BIT(REQ_F_BUFFER_SELECT_BIT),
	
	REQ_F_CQE_SKIP		= BIT(REQ_F_CQE_SKIP_BIT),

	
	REQ_F_FAIL		= BIT(REQ_F_FAIL_BIT),
	
	REQ_F_INFLIGHT		= BIT(REQ_F_INFLIGHT_BIT),
	
	REQ_F_CUR_POS		= BIT(REQ_F_CUR_POS_BIT),
	
	REQ_F_NOWAIT		= BIT(REQ_F_NOWAIT_BIT),
	
	REQ_F_LINK_TIMEOUT	= BIT(REQ_F_LINK_TIMEOUT_BIT),
	
	REQ_F_NEED_CLEANUP	= BIT(REQ_F_NEED_CLEANUP_BIT),
	
	REQ_F_POLLED		= BIT(REQ_F_POLLED_BIT),
	
	REQ_F_BUFFER_SELECTED	= BIT(REQ_F_BUFFER_SELECTED_BIT),
	
	REQ_F_BUFFER_RING	= BIT(REQ_F_BUFFER_RING_BIT),
	
	REQ_F_REISSUE		= BIT(REQ_F_REISSUE_BIT),
	
	REQ_F_SUPPORT_NOWAIT	= BIT(REQ_F_SUPPORT_NOWAIT_BIT),
	
	REQ_F_ISREG		= BIT(REQ_F_ISREG_BIT),
	
	REQ_F_CREDS		= BIT(REQ_F_CREDS_BIT),
	
	REQ_F_REFCOUNT		= BIT(REQ_F_REFCOUNT_BIT),
	
	REQ_F_ARM_LTIMEOUT	= BIT(REQ_F_ARM_LTIMEOUT_BIT),
	
	REQ_F_ASYNC_DATA	= BIT(REQ_F_ASYNC_DATA_BIT),
	
	REQ_F_SKIP_LINK_CQES	= BIT(REQ_F_SKIP_LINK_CQES_BIT),
	
	REQ_F_SINGLE_POLL	= BIT(REQ_F_SINGLE_POLL_BIT),
	
	REQ_F_DOUBLE_POLL	= BIT(REQ_F_DOUBLE_POLL_BIT),
	
	REQ_F_PARTIAL_IO	= BIT(REQ_F_PARTIAL_IO_BIT),
	
	REQ_F_APOLL_MULTISHOT	= BIT(REQ_F_APOLL_MULTISHOT_BIT),
	
	REQ_F_CLEAR_POLLIN	= BIT(REQ_F_CLEAR_POLLIN_BIT),
	
	REQ_F_HASH_LOCKED	= BIT(REQ_F_HASH_LOCKED_BIT),
};

typedef void (*io_req_tw_func_t)(struct io_kiocb *req, struct io_tw_state *ts);

struct io_task_work {
	struct llist_node		node;
	io_req_tw_func_t		func;
};

struct io_cqe {
	__u64	user_data;
	__s32	res;
	
	union {
		__u32	flags;
		int	fd;
	};
};


struct io_cmd_data {
	struct file		*file;
	
	__u8			data[56];
};

static inline void io_kiocb_cmd_sz_check(size_t cmd_sz)
{
	BUILD_BUG_ON(cmd_sz > sizeof(struct io_cmd_data));
}
#define io_kiocb_to_cmd(req, cmd_type) ( \
	io_kiocb_cmd_sz_check(sizeof(cmd_type)) , \
	((cmd_type *)&(req)->cmd) \
)
#define cmd_to_io_kiocb(ptr)	((struct io_kiocb *) ptr)

struct io_kiocb {
	union {
		
		struct file		*file;
		struct io_cmd_data	cmd;
	};

	u8				opcode;
	
	u8				iopoll_completed;
	
	u16				buf_index;
	unsigned int			flags;

	struct io_cqe			cqe;

	struct io_ring_ctx		*ctx;
	struct task_struct		*task;

	struct io_rsrc_node		*rsrc_node;

	union {
		
		struct io_mapped_ubuf	*imu;

		
		struct io_buffer	*kbuf;

		
		struct io_buffer_list	*buf_list;
	};

	union {
		
		struct io_wq_work_node	comp_list;
		
		__poll_t apoll_events;
	};
	atomic_t			refs;
	atomic_t			poll_refs;
	struct io_task_work		io_task_work;
	unsigned			nr_tw;
	
	struct hlist_node		hash_node;
	
	struct async_poll		*apoll;
	
	void				*async_data;
	
	struct io_kiocb			*link;
	
	const struct cred		*creds;
	struct io_wq_work		work;

	struct {
		u64			extra1;
		u64			extra2;
	} big_cqe;
};

struct io_overflow_cqe {
	struct list_head list;
	struct io_uring_cqe cqe;
};

#endif
