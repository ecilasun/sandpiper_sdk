/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOUND_UMP_H
#define __SOUND_UMP_H

#include <sound/rawmidi.h>

struct snd_ump_endpoint;
struct snd_ump_block;
struct snd_ump_ops;
struct ump_cvt_to_ump;
struct snd_seq_ump_ops;

struct snd_ump_endpoint {
	struct snd_rawmidi core;	

	struct snd_ump_endpoint_info info;

	const struct snd_ump_ops *ops;	
	struct snd_rawmidi_substream *substreams[2];	

	void *private_data;
	void (*private_free)(struct snd_ump_endpoint *ump);

	
	u32 stream_wait_for;	
	bool stream_finished;	
	bool parsed;		
	bool no_process_stream;	
	wait_queue_head_t stream_wait;
	struct snd_rawmidi_file stream_rfile;

	struct list_head block_list;	

	
	u32 input_buf[4];
	int input_buf_head;
	int input_pending;

	struct mutex open_mutex;

#if IS_ENABLED(CONFIG_SND_UMP_LEGACY_RAWMIDI)
	spinlock_t legacy_locks[2];
	struct snd_rawmidi *legacy_rmidi;
	struct snd_rawmidi_substream *legacy_substreams[2][SNDRV_UMP_MAX_GROUPS];
	unsigned char legacy_mapping[SNDRV_UMP_MAX_GROUPS];

	
	int legacy_out_opens;
	struct snd_rawmidi_file legacy_out_rfile;
	struct ump_cvt_to_ump *out_cvts;
#endif

#if IS_ENABLED(CONFIG_SND_SEQUENCER)
	struct snd_seq_device *seq_dev;
	const struct snd_seq_ump_ops *seq_ops;
	void *seq_client;
#endif
};


struct snd_ump_ops {
	int (*open)(struct snd_ump_endpoint *ump, int dir);
	void (*close)(struct snd_ump_endpoint *ump, int dir);
	void (*trigger)(struct snd_ump_endpoint *ump, int dir, int up);
	void (*drain)(struct snd_ump_endpoint *ump, int dir);
};


struct snd_seq_ump_ops {
	void (*input_receive)(struct snd_ump_endpoint *ump,
			      const u32 *data, int words);
	int (*notify_fb_change)(struct snd_ump_endpoint *ump,
				struct snd_ump_block *fb);
	int (*switch_protocol)(struct snd_ump_endpoint *ump);
};

struct snd_ump_block {
	struct snd_ump_block_info info;
	struct snd_ump_endpoint *ump;

	void *private_data;
	void (*private_free)(struct snd_ump_block *blk);

	struct list_head list;
};

#define rawmidi_to_ump(rmidi)	container_of(rmidi, struct snd_ump_endpoint, core)

int snd_ump_endpoint_new(struct snd_card *card, char *id, int device,
			 int output, int input,
			 struct snd_ump_endpoint **ump_ret);
int snd_ump_parse_endpoint(struct snd_ump_endpoint *ump);
int snd_ump_block_new(struct snd_ump_endpoint *ump, unsigned int blk,
		      unsigned int direction, unsigned int first_group,
		      unsigned int num_groups, struct snd_ump_block **blk_ret);
int snd_ump_receive(struct snd_ump_endpoint *ump, const u32 *buffer, int count);
int snd_ump_transmit(struct snd_ump_endpoint *ump, u32 *buffer, int count);

#if IS_ENABLED(CONFIG_SND_UMP_LEGACY_RAWMIDI)
int snd_ump_attach_legacy_rawmidi(struct snd_ump_endpoint *ump,
				  char *id, int device);
#else
static inline int snd_ump_attach_legacy_rawmidi(struct snd_ump_endpoint *ump,
						char *id, int device)
{
	return 0;
}
#endif

int snd_ump_receive_ump_val(struct snd_ump_endpoint *ump, u32 val);
int snd_ump_switch_protocol(struct snd_ump_endpoint *ump, unsigned int protocol);




enum {
	UMP_MSG_TYPE_UTILITY			= 0x00,
	UMP_MSG_TYPE_SYSTEM			= 0x01,
	UMP_MSG_TYPE_MIDI1_CHANNEL_VOICE	= 0x02,
	UMP_MSG_TYPE_DATA			= 0x03,
	UMP_MSG_TYPE_MIDI2_CHANNEL_VOICE	= 0x04,
	UMP_MSG_TYPE_EXTENDED_DATA		= 0x05,
	UMP_MSG_TYPE_FLEX_DATA			= 0x0d,
	UMP_MSG_TYPE_STREAM			= 0x0f,
};


enum {
	UMP_SYSEX_STATUS_SINGLE			= 0,
	UMP_SYSEX_STATUS_START			= 1,
	UMP_SYSEX_STATUS_CONTINUE		= 2,
	UMP_SYSEX_STATUS_END			= 3,
};


enum {
	UMP_UTILITY_MSG_STATUS_NOOP		= 0x00,
	UMP_UTILITY_MSG_STATUS_JR_CLOCK		= 0x01,
	UMP_UTILITY_MSG_STATUS_JR_TSTAMP	= 0x02,
	UMP_UTILITY_MSG_STATUS_DCTPQ		= 0x03,
	UMP_UTILITY_MSG_STATUS_DC		= 0x04,
};


enum {
	UMP_STREAM_MSG_STATUS_EP_DISCOVERY	= 0x00,
	UMP_STREAM_MSG_STATUS_EP_INFO		= 0x01,
	UMP_STREAM_MSG_STATUS_DEVICE_INFO	= 0x02,
	UMP_STREAM_MSG_STATUS_EP_NAME		= 0x03,
	UMP_STREAM_MSG_STATUS_PRODUCT_ID	= 0x04,
	UMP_STREAM_MSG_STATUS_STREAM_CFG_REQUEST = 0x05,
	UMP_STREAM_MSG_STATUS_STREAM_CFG	= 0x06,
	UMP_STREAM_MSG_STATUS_FB_DISCOVERY	= 0x10,
	UMP_STREAM_MSG_STATUS_FB_INFO		= 0x11,
	UMP_STREAM_MSG_STATUS_FB_NAME		= 0x12,
	UMP_STREAM_MSG_STATUS_START_CLIP	= 0x20,
	UMP_STREAM_MSG_STATUS_END_CLIP		= 0x21,
};


enum {
	UMP_STREAM_MSG_REQUEST_EP_INFO		= (1U << 0),
	UMP_STREAM_MSG_REQUEST_DEVICE_INFO	= (1U << 1),
	UMP_STREAM_MSG_REQUEST_EP_NAME		= (1U << 2),
	UMP_STREAM_MSG_REQUEST_PRODUCT_ID	= (1U << 3),
	UMP_STREAM_MSG_REQUEST_STREAM_CFG	= (1U << 4),
};


enum {
	UMP_STREAM_MSG_REQUEST_FB_INFO		= (1U << 0),
	UMP_STREAM_MSG_REQUEST_FB_NAME		= (1U << 1),
};


enum {
	UMP_STREAM_MSG_EP_INFO_CAP_TXJR		= (1U << 0), 
	UMP_STREAM_MSG_EP_INFO_CAP_RXJR		= (1U << 1), 
	UMP_STREAM_MSG_EP_INFO_CAP_MIDI1	= (1U << 8), 
	UMP_STREAM_MSG_EP_INFO_CAP_MIDI2	= (1U << 9), 
};


enum {
	UMP_STREAM_MSG_FORMAT_SINGLE		= 0,
	UMP_STREAM_MSG_FORMAT_START		= 1,
	UMP_STREAM_MSG_FORMAT_CONTINUE		= 2,
	UMP_STREAM_MSG_FORMAT_END		= 3,
};



static inline unsigned char ump_message_type(u32 data)
{
	return data >> 28;
}


static inline unsigned char ump_message_group(u32 data)
{
	return (data >> 24) & 0x0f;
}


static inline unsigned char ump_message_status_code(u32 data)
{
	return (data >> 20) & 0x0f;
}


static inline unsigned char ump_message_channel(u32 data)
{
	return (data >> 16) & 0x0f;
}


static inline unsigned char ump_message_status_channel(u32 data)
{
	return (data >> 16) & 0xff;
}


static inline u32 ump_compose(unsigned char type, unsigned char group,
			      unsigned char status, unsigned char channel)
{
	return ((u32)type << 28) | ((u32)group << 24) | ((u32)status << 20) |
		((u32)channel << 16);
}


static inline unsigned char ump_sysex_message_status(u32 data)
{
	return (data >> 20) & 0xf;
}


static inline unsigned char ump_sysex_message_length(u32 data)
{
	return (data >> 16) & 0xf;
}


static inline unsigned char ump_stream_message_format(u32 data)
{
	return (data >> 26) & 0x03;
}

static inline unsigned int ump_stream_message_status(u32 data)
{
	return (data >> 16) & 0x3ff;
}

static inline u32 ump_stream_compose(unsigned char status, unsigned short form)
{
	return (UMP_MSG_TYPE_STREAM << 28) | ((u32)form << 26) |
		((u32)status << 16);
}

#define ump_is_groupless_msg(type) \
	((type) == UMP_MSG_TYPE_UTILITY || (type) == UMP_MSG_TYPE_STREAM)

#endif 
