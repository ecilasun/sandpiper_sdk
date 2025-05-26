/* SPDX-License-Identifier: GPL-2.0
 *
 *  compress_driver.h - compress offload driver definations
 *
 *  Copyright (C) 2011 Intel Corporation
 *  Authors:	Vinod Koul <vinod.koul@linux.intel.com>
 *		Pierre-Louis Bossart <pierre-louis.bossart@linux.intel.com>
 */

#ifndef __COMPRESS_DRIVER_H
#define __COMPRESS_DRIVER_H

#include <linux/types.h>
#include <linux/sched.h>
#include <sound/core.h>
#include <sound/compress_offload.h>
#include <sound/asound.h>
#include <sound/pcm.h>

struct snd_compr_ops;


struct snd_compr_runtime {
	snd_pcm_state_t state;
	struct snd_compr_ops *ops;
	void *buffer;
	u64 buffer_size;
	u32 fragment_size;
	u32 fragments;
	u64 total_bytes_available;
	u64 total_bytes_transferred;
	wait_queue_head_t sleep;
	void *private_data;

	unsigned char *dma_area;
	dma_addr_t dma_addr;
	size_t dma_bytes;
	struct snd_dma_buffer *dma_buffer_p;
};


struct snd_compr_stream {
	const char *name;
	struct snd_compr_ops *ops;
	struct snd_compr_runtime *runtime;
	struct snd_compr *device;
	struct delayed_work error_work;
	enum snd_compr_direction direction;
	bool metadata_set;
	bool next_track;
	bool partial_drain;
	bool pause_in_draining;
	void *private_data;
	struct snd_dma_buffer dma_buffer;
};


struct snd_compr_ops {
	int (*open)(struct snd_compr_stream *stream);
	int (*free)(struct snd_compr_stream *stream);
	int (*set_params)(struct snd_compr_stream *stream,
			struct snd_compr_params *params);
	int (*get_params)(struct snd_compr_stream *stream,
			struct snd_codec *params);
	int (*set_metadata)(struct snd_compr_stream *stream,
			struct snd_compr_metadata *metadata);
	int (*get_metadata)(struct snd_compr_stream *stream,
			struct snd_compr_metadata *metadata);
	int (*trigger)(struct snd_compr_stream *stream, int cmd);
	int (*pointer)(struct snd_compr_stream *stream,
			struct snd_compr_tstamp *tstamp);
	int (*copy)(struct snd_compr_stream *stream, char __user *buf,
		       size_t count);
	int (*mmap)(struct snd_compr_stream *stream,
			struct vm_area_struct *vma);
	int (*ack)(struct snd_compr_stream *stream, size_t bytes);
	int (*get_caps) (struct snd_compr_stream *stream,
			struct snd_compr_caps *caps);
	int (*get_codec_caps) (struct snd_compr_stream *stream,
			struct snd_compr_codec_caps *codec);
};


struct snd_compr {
	const char *name;
	struct device *dev;
	struct snd_compr_ops *ops;
	void *private_data;
	struct snd_card *card;
	unsigned int direction;
	struct mutex lock;
	int device;
	bool use_pause_in_draining;
#ifdef CONFIG_SND_VERBOSE_PROCFS
	
	char id[64];
	struct snd_info_entry *proc_root;
	struct snd_info_entry *proc_info_entry;
#endif
};


int snd_compress_new(struct snd_card *card, int device,
			int type, const char *id, struct snd_compr *compr);


static inline void snd_compr_use_pause_in_draining(struct snd_compr_stream *substream)
{
	substream->device->use_pause_in_draining = true;
}


static inline void snd_compr_fragment_elapsed(struct snd_compr_stream *stream)
{
	wake_up(&stream->runtime->sleep);
}

static inline void snd_compr_drain_notify(struct snd_compr_stream *stream)
{
	if (snd_BUG_ON(!stream))
		return;

	
	if (stream->partial_drain) {
		stream->runtime->state = SNDRV_PCM_STATE_RUNNING;
		stream->partial_drain = false; 
	} else {
		stream->runtime->state = SNDRV_PCM_STATE_SETUP;
	}

	wake_up(&stream->runtime->sleep);
}


static inline void
snd_compr_set_runtime_buffer(struct snd_compr_stream *stream,
			     struct snd_dma_buffer *bufp)
{
	struct snd_compr_runtime *runtime = stream->runtime;

	if (bufp) {
		runtime->dma_buffer_p = bufp;
		runtime->dma_area = bufp->area;
		runtime->dma_addr = bufp->addr;
		runtime->dma_bytes = bufp->bytes;
	} else {
		runtime->dma_buffer_p = NULL;
		runtime->dma_area = NULL;
		runtime->dma_addr = 0;
		runtime->dma_bytes = 0;
	}
}

int snd_compr_malloc_pages(struct snd_compr_stream *stream, size_t size);
int snd_compr_free_pages(struct snd_compr_stream *stream);

int snd_compr_stop_error(struct snd_compr_stream *stream,
			 snd_pcm_state_t state);

#endif
