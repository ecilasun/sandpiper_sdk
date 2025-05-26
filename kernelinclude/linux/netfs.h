/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_NETFS_H
#define _LINUX_NETFS_H

#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/uio.h>

enum netfs_sreq_ref_trace;


#define folio_test_fscache(folio)	folio_test_private_2(folio)
#define PageFsCache(page)		PagePrivate2((page))
#define SetPageFsCache(page)		SetPagePrivate2((page))
#define ClearPageFsCache(page)		ClearPagePrivate2((page))
#define TestSetPageFsCache(page)	TestSetPagePrivate2((page))
#define TestClearPageFsCache(page)	TestClearPagePrivate2((page))


static inline void folio_start_fscache(struct folio *folio)
{
	VM_BUG_ON_FOLIO(folio_test_private_2(folio), folio);
	folio_get(folio);
	folio_set_private_2(folio);
}


static inline void folio_end_fscache(struct folio *folio)
{
	folio_end_private_2(folio);
}


static inline void folio_wait_fscache(struct folio *folio)
{
	folio_wait_private_2(folio);
}


static inline int folio_wait_fscache_killable(struct folio *folio)
{
	return folio_wait_private_2_killable(folio);
}

static inline void set_page_fscache(struct page *page)
{
	folio_start_fscache(page_folio(page));
}

static inline void end_page_fscache(struct page *page)
{
	folio_end_private_2(page_folio(page));
}

static inline void wait_on_page_fscache(struct page *page)
{
	folio_wait_private_2(page_folio(page));
}

static inline int wait_on_page_fscache_killable(struct page *page)
{
	return folio_wait_private_2_killable(page_folio(page));
}

enum netfs_io_source {
	NETFS_FILL_WITH_ZEROES,
	NETFS_DOWNLOAD_FROM_SERVER,
	NETFS_READ_FROM_CACHE,
	NETFS_INVALID_READ,
} __mode(byte);

typedef void (*netfs_io_terminated_t)(void *priv, ssize_t transferred_or_error,
				      bool was_async);


struct netfs_inode {
	struct inode		inode;		
	const struct netfs_request_ops *ops;
#if IS_ENABLED(CONFIG_FSCACHE)
	struct fscache_cookie	*cache;
#endif
	loff_t			remote_i_size;	
};


struct netfs_cache_resources {
	const struct netfs_cache_ops	*ops;
	void				*cache_priv;
	void				*cache_priv2;
	unsigned int			debug_id;	
	unsigned int			inval_counter;	
};


struct netfs_io_subrequest {
	struct netfs_io_request *rreq;		
	struct list_head	rreq_link;	
	loff_t			start;		
	size_t			len;		
	size_t			transferred;	
	refcount_t		ref;
	short			error;		
	unsigned short		debug_index;	
	enum netfs_io_source	source;		
	unsigned long		flags;
#define NETFS_SREQ_COPY_TO_CACHE	0	
#define NETFS_SREQ_CLEAR_TAIL		1	
#define NETFS_SREQ_SHORT_IO		2	
#define NETFS_SREQ_SEEK_DATA_READ	3	
#define NETFS_SREQ_NO_PROGRESS		4	
#define NETFS_SREQ_ONDEMAND		5	
};

enum netfs_io_origin {
	NETFS_READAHEAD,		
	NETFS_READPAGE,			
	NETFS_READ_FOR_WRITE,		
} __mode(byte);


struct netfs_io_request {
	struct work_struct	work;
	struct inode		*inode;		
	struct address_space	*mapping;	
	struct netfs_cache_resources cache_resources;
	struct list_head	subrequests;	
	void			*netfs_priv;	
	unsigned int		debug_id;
	atomic_t		nr_outstanding;	
	atomic_t		nr_copy_ops;	
	size_t			submitted;	
	size_t			len;		
	short			error;		
	enum netfs_io_origin	origin;		
	loff_t			i_size;		
	loff_t			start;		
	pgoff_t			no_unlock_folio; 
	refcount_t		ref;
	unsigned long		flags;
#define NETFS_RREQ_INCOMPLETE_IO	0	
#define NETFS_RREQ_COPY_TO_CACHE	1	
#define NETFS_RREQ_NO_UNLOCK_FOLIO	2	
#define NETFS_RREQ_DONT_UNLOCK_FOLIOS	3	
#define NETFS_RREQ_FAILED		4	
#define NETFS_RREQ_IN_PROGRESS		5	
	const struct netfs_request_ops *netfs_ops;
};


struct netfs_request_ops {
	int (*init_request)(struct netfs_io_request *rreq, struct file *file);
	void (*free_request)(struct netfs_io_request *rreq);
	int (*begin_cache_operation)(struct netfs_io_request *rreq);

	void (*expand_readahead)(struct netfs_io_request *rreq);
	bool (*clamp_length)(struct netfs_io_subrequest *subreq);
	void (*issue_read)(struct netfs_io_subrequest *subreq);
	bool (*is_still_valid)(struct netfs_io_request *rreq);
	int (*check_write_begin)(struct file *file, loff_t pos, unsigned len,
				 struct folio **foliop, void **_fsdata);
	void (*done)(struct netfs_io_request *rreq);
};


enum netfs_read_from_hole {
	NETFS_READ_HOLE_IGNORE,
	NETFS_READ_HOLE_CLEAR,
	NETFS_READ_HOLE_FAIL,
};


struct netfs_cache_ops {
	
	void (*end_operation)(struct netfs_cache_resources *cres);

	
	int (*read)(struct netfs_cache_resources *cres,
		    loff_t start_pos,
		    struct iov_iter *iter,
		    enum netfs_read_from_hole read_hole,
		    netfs_io_terminated_t term_func,
		    void *term_func_priv);

	
	int (*write)(struct netfs_cache_resources *cres,
		     loff_t start_pos,
		     struct iov_iter *iter,
		     netfs_io_terminated_t term_func,
		     void *term_func_priv);

	
	void (*expand_readahead)(struct netfs_cache_resources *cres,
				 loff_t *_start, size_t *_len, loff_t i_size);

	
	enum netfs_io_source (*prepare_read)(struct netfs_io_subrequest *subreq,
					     loff_t i_size);

	
	int (*prepare_write)(struct netfs_cache_resources *cres,
			     loff_t *_start, size_t *_len, loff_t i_size,
			     bool no_space_allocated_yet);

	
	enum netfs_io_source (*prepare_ondemand_read)(struct netfs_cache_resources *cres,
						      loff_t start, size_t *_len,
						      loff_t i_size,
						      unsigned long *_flags, ino_t ino);

	
	int (*query_occupancy)(struct netfs_cache_resources *cres,
			       loff_t start, size_t len, size_t granularity,
			       loff_t *_data_start, size_t *_data_len);
};

struct readahead_control;
void netfs_readahead(struct readahead_control *);
int netfs_read_folio(struct file *, struct folio *);
int netfs_write_begin(struct netfs_inode *, struct file *,
		struct address_space *, loff_t pos, unsigned int len,
		struct folio **, void **fsdata);

void netfs_subreq_terminated(struct netfs_io_subrequest *, ssize_t, bool);
void netfs_get_subrequest(struct netfs_io_subrequest *subreq,
			  enum netfs_sreq_ref_trace what);
void netfs_put_subrequest(struct netfs_io_subrequest *subreq,
			  bool was_async, enum netfs_sreq_ref_trace what);
void netfs_stats_show(struct seq_file *);
ssize_t netfs_extract_user_iter(struct iov_iter *orig, size_t orig_len,
				struct iov_iter *new,
				iov_iter_extraction_t extraction_flags);


static inline struct netfs_inode *netfs_inode(struct inode *inode)
{
	return container_of(inode, struct netfs_inode, inode);
}


static inline void netfs_inode_init(struct netfs_inode *ctx,
				    const struct netfs_request_ops *ops)
{
	ctx->ops = ops;
	ctx->remote_i_size = i_size_read(&ctx->inode);
#if IS_ENABLED(CONFIG_FSCACHE)
	ctx->cache = NULL;
#endif
}


static inline void netfs_resize_file(struct netfs_inode *ctx, loff_t new_i_size)
{
	ctx->remote_i_size = new_i_size;
}


static inline struct fscache_cookie *netfs_i_cookie(struct netfs_inode *ctx)
{
#if IS_ENABLED(CONFIG_FSCACHE)
	return ctx->cache;
#else
	return NULL;
#endif
}

#endif 
