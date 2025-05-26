/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PIPE_FS_I_H
#define _LINUX_PIPE_FS_I_H

#define PIPE_DEF_BUFFERS	16

#define PIPE_BUF_FLAG_LRU	0x01	
#define PIPE_BUF_FLAG_ATOMIC	0x02	
#define PIPE_BUF_FLAG_GIFT	0x04	
#define PIPE_BUF_FLAG_PACKET	0x08	
#define PIPE_BUF_FLAG_CAN_MERGE	0x10	
#define PIPE_BUF_FLAG_WHOLE	0x20	
#ifdef CONFIG_WATCH_QUEUE
#define PIPE_BUF_FLAG_LOSS	0x40	
#endif


struct pipe_buffer {
	struct page *page;
	unsigned int offset, len;
	const struct pipe_buf_operations *ops;
	unsigned int flags;
	unsigned long private;
};


struct pipe_inode_info {
	struct mutex mutex;
	wait_queue_head_t rd_wait, wr_wait;
	unsigned int head;
	unsigned int tail;
	unsigned int max_usage;
	unsigned int ring_size;
#ifdef CONFIG_WATCH_QUEUE
	bool note_loss;
#endif
	unsigned int nr_accounted;
	unsigned int readers;
	unsigned int writers;
	unsigned int files;
	unsigned int r_counter;
	unsigned int w_counter;
	bool poll_usage;
	struct page *tmp_page;
	struct fasync_struct *fasync_readers;
	struct fasync_struct *fasync_writers;
	struct pipe_buffer *bufs;
	struct user_struct *user;
#ifdef CONFIG_WATCH_QUEUE
	struct watch_queue *watch_queue;
#endif
};


struct pipe_buf_operations {
	
	int (*confirm)(struct pipe_inode_info *, struct pipe_buffer *);

	
	void (*release)(struct pipe_inode_info *, struct pipe_buffer *);

	
	bool (*try_steal)(struct pipe_inode_info *, struct pipe_buffer *);

	
	bool (*get)(struct pipe_inode_info *, struct pipe_buffer *);
};


static inline bool pipe_has_watch_queue(const struct pipe_inode_info *pipe)
{
#ifdef CONFIG_WATCH_QUEUE
	return pipe->watch_queue != NULL;
#else
	return false;
#endif
}


static inline bool pipe_empty(unsigned int head, unsigned int tail)
{
	return head == tail;
}


static inline unsigned int pipe_occupancy(unsigned int head, unsigned int tail)
{
	return head - tail;
}


static inline bool pipe_full(unsigned int head, unsigned int tail,
			     unsigned int limit)
{
	return pipe_occupancy(head, tail) >= limit;
}


static inline struct pipe_buffer *pipe_buf(const struct pipe_inode_info *pipe,
					   unsigned int slot)
{
	return &pipe->bufs[slot & (pipe->ring_size - 1)];
}


static inline struct pipe_buffer *pipe_head_buf(const struct pipe_inode_info *pipe)
{
	return pipe_buf(pipe, pipe->head);
}


static inline __must_check bool pipe_buf_get(struct pipe_inode_info *pipe,
				struct pipe_buffer *buf)
{
	return buf->ops->get(pipe, buf);
}


static inline void pipe_buf_release(struct pipe_inode_info *pipe,
				    struct pipe_buffer *buf)
{
	const struct pipe_buf_operations *ops = buf->ops;

	buf->ops = NULL;
	ops->release(pipe, buf);
}


static inline int pipe_buf_confirm(struct pipe_inode_info *pipe,
				   struct pipe_buffer *buf)
{
	if (!buf->ops->confirm)
		return 0;
	return buf->ops->confirm(pipe, buf);
}


static inline bool pipe_buf_try_steal(struct pipe_inode_info *pipe,
		struct pipe_buffer *buf)
{
	if (!buf->ops->try_steal)
		return false;
	return buf->ops->try_steal(pipe, buf);
}

static inline void pipe_discard_from(struct pipe_inode_info *pipe,
		unsigned int old_head)
{
	unsigned int mask = pipe->ring_size - 1;

	while (pipe->head > old_head)
		pipe_buf_release(pipe, &pipe->bufs[--pipe->head & mask]);
}


#define PIPE_SIZE		PAGE_SIZE


void pipe_lock(struct pipe_inode_info *);
void pipe_unlock(struct pipe_inode_info *);
void pipe_double_lock(struct pipe_inode_info *, struct pipe_inode_info *);


void pipe_wait_readable(struct pipe_inode_info *);
void pipe_wait_writable(struct pipe_inode_info *);

struct pipe_inode_info *alloc_pipe_info(void);
void free_pipe_info(struct pipe_inode_info *);


bool generic_pipe_buf_get(struct pipe_inode_info *, struct pipe_buffer *);
bool generic_pipe_buf_try_steal(struct pipe_inode_info *, struct pipe_buffer *);
void generic_pipe_buf_release(struct pipe_inode_info *, struct pipe_buffer *);

extern const struct pipe_buf_operations nosteal_pipe_buf_ops;

unsigned long account_pipe_buffers(struct user_struct *user,
				   unsigned long old, unsigned long new);
bool too_many_pipe_buffers_soft(unsigned long user_bufs);
bool too_many_pipe_buffers_hard(unsigned long user_bufs);
bool pipe_is_unprivileged_user(void);


int pipe_resize_ring(struct pipe_inode_info *pipe, unsigned int nr_slots);
long pipe_fcntl(struct file *, unsigned int, unsigned int arg);
struct pipe_inode_info *get_pipe_info(struct file *file, bool for_splice);

int create_pipe_files(struct file **, int);
unsigned int round_pipe_size(unsigned int size);

#endif
