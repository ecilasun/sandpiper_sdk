/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_DM_BUFIO_H
#define _LINUX_DM_BUFIO_H

#include <linux/blkdev.h>
#include <linux/types.h>



struct dm_bufio_client;
struct dm_buffer;


#define DM_BUFIO_CLIENT_NO_SLEEP 0x1


struct dm_bufio_client *
dm_bufio_client_create(struct block_device *bdev, unsigned int block_size,
		       unsigned int reserved_buffers, unsigned int aux_size,
		       void (*alloc_callback)(struct dm_buffer *),
		       void (*write_callback)(struct dm_buffer *),
		       unsigned int flags);


void dm_bufio_client_destroy(struct dm_bufio_client *c);

void dm_bufio_client_reset(struct dm_bufio_client *c);


void dm_bufio_set_sector_offset(struct dm_bufio_client *c, sector_t start);




void *dm_bufio_read(struct dm_bufio_client *c, sector_t block,
		    struct dm_buffer **bp);


void *dm_bufio_get(struct dm_bufio_client *c, sector_t block,
		   struct dm_buffer **bp);


void *dm_bufio_new(struct dm_bufio_client *c, sector_t block,
		   struct dm_buffer **bp);


void dm_bufio_prefetch(struct dm_bufio_client *c,
		       sector_t block, unsigned int n_blocks);


void dm_bufio_release(struct dm_buffer *b);


void dm_bufio_mark_buffer_dirty(struct dm_buffer *b);


void dm_bufio_mark_partial_buffer_dirty(struct dm_buffer *b,
					unsigned int start, unsigned int end);


void dm_bufio_write_dirty_buffers_async(struct dm_bufio_client *c);


int dm_bufio_write_dirty_buffers(struct dm_bufio_client *c);


int dm_bufio_issue_flush(struct dm_bufio_client *c);


int dm_bufio_issue_discard(struct dm_bufio_client *c, sector_t block, sector_t count);


void dm_bufio_forget(struct dm_bufio_client *c, sector_t block);


void dm_bufio_forget_buffers(struct dm_bufio_client *c, sector_t block, sector_t n_blocks);


void dm_bufio_set_minimum_buffers(struct dm_bufio_client *c, unsigned int n);

unsigned int dm_bufio_get_block_size(struct dm_bufio_client *c);
sector_t dm_bufio_get_device_size(struct dm_bufio_client *c);
struct dm_io_client *dm_bufio_get_dm_io_client(struct dm_bufio_client *c);
sector_t dm_bufio_get_block_number(struct dm_buffer *b);
void *dm_bufio_get_block_data(struct dm_buffer *b);
void *dm_bufio_get_aux_data(struct dm_buffer *b);
struct dm_bufio_client *dm_bufio_get_client(struct dm_buffer *b);



#endif
