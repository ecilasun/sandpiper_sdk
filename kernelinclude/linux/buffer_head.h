/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_BUFFER_HEAD_H
#define _LINUX_BUFFER_HEAD_H

#include <linux/types.h>
#include <linux/blk_types.h>
#include <linux/fs.h>
#include <linux/linkage.h>
#include <linux/pagemap.h>
#include <linux/wait.h>
#include <linux/atomic.h>

enum bh_state_bits {
	BH_Uptodate,	
	BH_Dirty,	
	BH_Lock,	
	BH_Req,		

	BH_Mapped,	
	BH_New,		
	BH_Async_Read,	
	BH_Async_Write,	
	BH_Delay,	
	BH_Boundary,	
	BH_Write_EIO,	
	BH_Unwritten,	
	BH_Quiet,	
	BH_Meta,	
	BH_Prio,	
	BH_Defer_Completion, 

	BH_PrivateStart,
};

#define MAX_BUF_PER_PAGE (PAGE_SIZE / 512)

struct page;
struct buffer_head;
struct address_space;
typedef void (bh_end_io_t)(struct buffer_head *bh, int uptodate);


struct buffer_head {
	unsigned long b_state;		
	struct buffer_head *b_this_page;
	union {
		struct page *b_page;	
		struct folio *b_folio;	
	};

	sector_t b_blocknr;		
	size_t b_size;			
	char *b_data;			

	struct block_device *b_bdev;
	bh_end_io_t *b_end_io;		
 	void *b_private;		
	struct list_head b_assoc_buffers; 
	struct address_space *b_assoc_map;	
	atomic_t b_count;		
	spinlock_t b_uptodate_lock;	
};


#define BUFFER_FNS(bit, name)						\
static __always_inline void set_buffer_##name(struct buffer_head *bh)	\
{									\
	if (!test_bit(BH_##bit, &(bh)->b_state))			\
		set_bit(BH_##bit, &(bh)->b_state);			\
}									\
static __always_inline void clear_buffer_##name(struct buffer_head *bh)	\
{									\
	clear_bit(BH_##bit, &(bh)->b_state);				\
}									\
static __always_inline int buffer_##name(const struct buffer_head *bh)	\
{									\
	return test_bit(BH_##bit, &(bh)->b_state);			\
}


#define TAS_BUFFER_FNS(bit, name)					\
static __always_inline int test_set_buffer_##name(struct buffer_head *bh) \
{									\
	return test_and_set_bit(BH_##bit, &(bh)->b_state);		\
}									\
static __always_inline int test_clear_buffer_##name(struct buffer_head *bh) \
{									\
	return test_and_clear_bit(BH_##bit, &(bh)->b_state);		\
}									\


BUFFER_FNS(Dirty, dirty)
TAS_BUFFER_FNS(Dirty, dirty)
BUFFER_FNS(Lock, locked)
BUFFER_FNS(Req, req)
TAS_BUFFER_FNS(Req, req)
BUFFER_FNS(Mapped, mapped)
BUFFER_FNS(New, new)
BUFFER_FNS(Async_Read, async_read)
BUFFER_FNS(Async_Write, async_write)
BUFFER_FNS(Delay, delay)
BUFFER_FNS(Boundary, boundary)
BUFFER_FNS(Write_EIO, write_io_error)
BUFFER_FNS(Unwritten, unwritten)
BUFFER_FNS(Meta, meta)
BUFFER_FNS(Prio, prio)
BUFFER_FNS(Defer_Completion, defer_completion)

static __always_inline void set_buffer_uptodate(struct buffer_head *bh)
{
	
	if (test_bit(BH_Uptodate, &bh->b_state))
		return;

	
	smp_mb__before_atomic();
	set_bit(BH_Uptodate, &bh->b_state);
}

static __always_inline void clear_buffer_uptodate(struct buffer_head *bh)
{
	clear_bit(BH_Uptodate, &bh->b_state);
}

static __always_inline int buffer_uptodate(const struct buffer_head *bh)
{
	
	return test_bit_acquire(BH_Uptodate, &bh->b_state);
}

static inline unsigned long bh_offset(const struct buffer_head *bh)
{
	return (unsigned long)(bh)->b_data & (page_size(bh->b_page) - 1);
}


#define page_buffers(page)					\
	({							\
		BUG_ON(!PagePrivate(page));			\
		((struct buffer_head *)page_private(page));	\
	})
#define page_has_buffers(page)	PagePrivate(page)
#define folio_buffers(folio)		folio_get_private(folio)

void buffer_check_dirty_writeback(struct folio *folio,
				     bool *dirty, bool *writeback);



void mark_buffer_dirty(struct buffer_head *bh);
void mark_buffer_write_io_error(struct buffer_head *bh);
void touch_buffer(struct buffer_head *bh);
void folio_set_bh(struct buffer_head *bh, struct folio *folio,
		  unsigned long offset);
struct buffer_head *folio_alloc_buffers(struct folio *folio, unsigned long size,
					bool retry);
struct buffer_head *alloc_page_buffers(struct page *page, unsigned long size,
		bool retry);
void create_empty_buffers(struct page *, unsigned long,
			unsigned long b_state);
void folio_create_empty_buffers(struct folio *folio, unsigned long blocksize,
				unsigned long b_state);
void end_buffer_read_sync(struct buffer_head *bh, int uptodate);
void end_buffer_write_sync(struct buffer_head *bh, int uptodate);
void end_buffer_async_write(struct buffer_head *bh, int uptodate);


void mark_buffer_dirty_inode(struct buffer_head *bh, struct inode *inode);
int generic_buffers_fsync_noflush(struct file *file, loff_t start, loff_t end,
				  bool datasync);
int generic_buffers_fsync(struct file *file, loff_t start, loff_t end,
			  bool datasync);
void clean_bdev_aliases(struct block_device *bdev, sector_t block,
			sector_t len);
static inline void clean_bdev_bh_alias(struct buffer_head *bh)
{
	clean_bdev_aliases(bh->b_bdev, bh->b_blocknr, 1);
}

void mark_buffer_async_write(struct buffer_head *bh);
void __wait_on_buffer(struct buffer_head *);
wait_queue_head_t *bh_waitq_head(struct buffer_head *bh);
struct buffer_head *__find_get_block(struct block_device *bdev, sector_t block,
			unsigned size);
struct buffer_head *__getblk_gfp(struct block_device *bdev, sector_t block,
				  unsigned size, gfp_t gfp);
void __brelse(struct buffer_head *);
void __bforget(struct buffer_head *);
void __breadahead(struct block_device *, sector_t block, unsigned int size);
struct buffer_head *__bread_gfp(struct block_device *,
				sector_t block, unsigned size, gfp_t gfp);
struct buffer_head *alloc_buffer_head(gfp_t gfp_flags);
void free_buffer_head(struct buffer_head * bh);
void unlock_buffer(struct buffer_head *bh);
void __lock_buffer(struct buffer_head *bh);
int sync_dirty_buffer(struct buffer_head *bh);
int __sync_dirty_buffer(struct buffer_head *bh, blk_opf_t op_flags);
void write_dirty_buffer(struct buffer_head *bh, blk_opf_t op_flags);
void submit_bh(blk_opf_t, struct buffer_head *);
void write_boundary_block(struct block_device *bdev,
			sector_t bblock, unsigned blocksize);
int bh_uptodate_or_lock(struct buffer_head *bh);
int __bh_read(struct buffer_head *bh, blk_opf_t op_flags, bool wait);
void __bh_read_batch(int nr, struct buffer_head *bhs[],
		     blk_opf_t op_flags, bool force_lock);


void block_invalidate_folio(struct folio *folio, size_t offset, size_t length);
int block_write_full_page(struct page *page, get_block_t *get_block,
				struct writeback_control *wbc);
int __block_write_full_folio(struct inode *inode, struct folio *folio,
			get_block_t *get_block, struct writeback_control *wbc,
			bh_end_io_t *handler);
int block_read_full_folio(struct folio *, get_block_t *);
bool block_is_partially_uptodate(struct folio *, size_t from, size_t count);
int block_write_begin(struct address_space *mapping, loff_t pos, unsigned len,
		struct page **pagep, get_block_t *get_block);
int __block_write_begin(struct page *page, loff_t pos, unsigned len,
		get_block_t *get_block);
int block_write_end(struct file *, struct address_space *,
				loff_t, unsigned, unsigned,
				struct page *, void *);
int generic_write_end(struct file *, struct address_space *,
				loff_t, unsigned, unsigned,
				struct page *, void *);
void folio_zero_new_buffers(struct folio *folio, size_t from, size_t to);
void clean_page_buffers(struct page *page);
int cont_write_begin(struct file *, struct address_space *, loff_t,
			unsigned, struct page **, void **,
			get_block_t *, loff_t *);
int generic_cont_expand_simple(struct inode *inode, loff_t size);
void block_commit_write(struct page *page, unsigned int from, unsigned int to);
int block_page_mkwrite(struct vm_area_struct *vma, struct vm_fault *vmf,
				get_block_t get_block);
sector_t generic_block_bmap(struct address_space *, sector_t, get_block_t *);
int block_truncate_page(struct address_space *, loff_t, get_block_t *);

#ifdef CONFIG_MIGRATION
extern int buffer_migrate_folio(struct address_space *,
		struct folio *dst, struct folio *src, enum migrate_mode);
extern int buffer_migrate_folio_norefs(struct address_space *,
		struct folio *dst, struct folio *src, enum migrate_mode);
#else
#define buffer_migrate_folio NULL
#define buffer_migrate_folio_norefs NULL
#endif



static inline void get_bh(struct buffer_head *bh)
{
        atomic_inc(&bh->b_count);
}

static inline void put_bh(struct buffer_head *bh)
{
        smp_mb__before_atomic();
        atomic_dec(&bh->b_count);
}

static inline void brelse(struct buffer_head *bh)
{
	if (bh)
		__brelse(bh);
}

static inline void bforget(struct buffer_head *bh)
{
	if (bh)
		__bforget(bh);
}

static inline struct buffer_head *
sb_bread(struct super_block *sb, sector_t block)
{
	return __bread_gfp(sb->s_bdev, block, sb->s_blocksize, __GFP_MOVABLE);
}

static inline struct buffer_head *
sb_bread_unmovable(struct super_block *sb, sector_t block)
{
	return __bread_gfp(sb->s_bdev, block, sb->s_blocksize, 0);
}

static inline void
sb_breadahead(struct super_block *sb, sector_t block)
{
	__breadahead(sb->s_bdev, block, sb->s_blocksize);
}

static inline struct buffer_head *
sb_getblk(struct super_block *sb, sector_t block)
{
	return __getblk_gfp(sb->s_bdev, block, sb->s_blocksize, __GFP_MOVABLE);
}


static inline struct buffer_head *
sb_getblk_gfp(struct super_block *sb, sector_t block, gfp_t gfp)
{
	return __getblk_gfp(sb->s_bdev, block, sb->s_blocksize, gfp);
}

static inline struct buffer_head *
sb_find_get_block(struct super_block *sb, sector_t block)
{
	return __find_get_block(sb->s_bdev, block, sb->s_blocksize);
}

static inline void
map_bh(struct buffer_head *bh, struct super_block *sb, sector_t block)
{
	set_buffer_mapped(bh);
	bh->b_bdev = sb->s_bdev;
	bh->b_blocknr = block;
	bh->b_size = sb->s_blocksize;
}

static inline void wait_on_buffer(struct buffer_head *bh)
{
	might_sleep();
	if (buffer_locked(bh))
		__wait_on_buffer(bh);
}

static inline int trylock_buffer(struct buffer_head *bh)
{
	return likely(!test_and_set_bit_lock(BH_Lock, &bh->b_state));
}

static inline void lock_buffer(struct buffer_head *bh)
{
	might_sleep();
	if (!trylock_buffer(bh))
		__lock_buffer(bh);
}

static inline struct buffer_head *getblk_unmovable(struct block_device *bdev,
						   sector_t block,
						   unsigned size)
{
	return __getblk_gfp(bdev, block, size, 0);
}

static inline struct buffer_head *__getblk(struct block_device *bdev,
					   sector_t block,
					   unsigned size)
{
	return __getblk_gfp(bdev, block, size, __GFP_MOVABLE);
}

static inline void bh_readahead(struct buffer_head *bh, blk_opf_t op_flags)
{
	if (!buffer_uptodate(bh) && trylock_buffer(bh)) {
		if (!buffer_uptodate(bh))
			__bh_read(bh, op_flags, false);
		else
			unlock_buffer(bh);
	}
}

static inline void bh_read_nowait(struct buffer_head *bh, blk_opf_t op_flags)
{
	if (!bh_uptodate_or_lock(bh))
		__bh_read(bh, op_flags, false);
}


static inline int bh_read(struct buffer_head *bh, blk_opf_t op_flags)
{
	if (bh_uptodate_or_lock(bh))
		return 1;
	return __bh_read(bh, op_flags, true);
}

static inline void bh_read_batch(int nr, struct buffer_head *bhs[])
{
	__bh_read_batch(nr, bhs, 0, true);
}

static inline void bh_readahead_batch(int nr, struct buffer_head *bhs[],
				      blk_opf_t op_flags)
{
	__bh_read_batch(nr, bhs, op_flags, false);
}


static inline struct buffer_head *
__bread(struct block_device *bdev, sector_t block, unsigned size)
{
	return __bread_gfp(bdev, block, size, __GFP_MOVABLE);
}

bool block_dirty_folio(struct address_space *mapping, struct folio *folio);

#ifdef CONFIG_BUFFER_HEAD

void buffer_init(void);
bool try_to_free_buffers(struct folio *folio);
int inode_has_buffers(struct inode *inode);
void invalidate_inode_buffers(struct inode *inode);
int remove_inode_buffers(struct inode *inode);
int sync_mapping_buffers(struct address_space *mapping);
void invalidate_bh_lrus(void);
void invalidate_bh_lrus_cpu(void);
bool has_bh_in_lru(int cpu, void *dummy);
extern int buffer_heads_over_limit;

#else 

static inline void buffer_init(void) {}
static inline bool try_to_free_buffers(struct folio *folio) { return true; }
static inline int inode_has_buffers(struct inode *inode) { return 0; }
static inline void invalidate_inode_buffers(struct inode *inode) {}
static inline int remove_inode_buffers(struct inode *inode) { return 1; }
static inline int sync_mapping_buffers(struct address_space *mapping) { return 0; }
static inline void invalidate_bh_lrus(void) {}
static inline void invalidate_bh_lrus_cpu(void) {}
static inline bool has_bh_in_lru(int cpu, void *dummy) { return false; }
#define buffer_heads_over_limit 0

#endif 
#endif 
