/* SPDX-License-Identifier: MIT */


#ifndef __XEN_PUBLIC_IO_SNDIF_H__
#define __XEN_PUBLIC_IO_SNDIF_H__

#include "ring.h"
#include "../grant_table.h"


#define XENSND_PROTOCOL_VERSION	2


#define XENSND_PCM_FORMAT_S8		0
#define XENSND_PCM_FORMAT_U8		1
#define XENSND_PCM_FORMAT_S16_LE	2
#define XENSND_PCM_FORMAT_S16_BE	3
#define XENSND_PCM_FORMAT_U16_LE	4
#define XENSND_PCM_FORMAT_U16_BE	5
#define XENSND_PCM_FORMAT_S24_LE	6
#define XENSND_PCM_FORMAT_S24_BE	7
#define XENSND_PCM_FORMAT_U24_LE	8
#define XENSND_PCM_FORMAT_U24_BE	9
#define XENSND_PCM_FORMAT_S32_LE	10
#define XENSND_PCM_FORMAT_S32_BE	11
#define XENSND_PCM_FORMAT_U32_LE	12
#define XENSND_PCM_FORMAT_U32_BE	13
#define XENSND_PCM_FORMAT_F32_LE	14 
#define XENSND_PCM_FORMAT_F32_BE	15 
#define XENSND_PCM_FORMAT_F64_LE	16 
#define XENSND_PCM_FORMAT_F64_BE	17 
#define XENSND_PCM_FORMAT_IEC958_SUBFRAME_LE 18
#define XENSND_PCM_FORMAT_IEC958_SUBFRAME_BE 19
#define XENSND_PCM_FORMAT_MU_LAW	20
#define XENSND_PCM_FORMAT_A_LAW		21
#define XENSND_PCM_FORMAT_IMA_ADPCM	22
#define XENSND_PCM_FORMAT_MPEG		23
#define XENSND_PCM_FORMAT_GSM		24


#define XENSND_OP_OPEN			0
#define XENSND_OP_CLOSE			1
#define XENSND_OP_READ			2
#define XENSND_OP_WRITE			3
#define XENSND_OP_SET_VOLUME		4
#define XENSND_OP_GET_VOLUME		5
#define XENSND_OP_MUTE			6
#define XENSND_OP_UNMUTE		7
#define XENSND_OP_TRIGGER		8
#define XENSND_OP_HW_PARAM_QUERY	9

#define XENSND_OP_TRIGGER_START		0
#define XENSND_OP_TRIGGER_PAUSE		1
#define XENSND_OP_TRIGGER_STOP		2
#define XENSND_OP_TRIGGER_RESUME	3


#define XENSND_EVT_CUR_POS		0


#define XENSND_DRIVER_NAME		"vsnd"

#define XENSND_LIST_SEPARATOR		","

#define XENSND_FIELD_BE_VERSIONS	"versions"
#define XENSND_FIELD_FE_VERSION		"version"
#define XENSND_FIELD_VCARD_SHORT_NAME	"short-name"
#define XENSND_FIELD_VCARD_LONG_NAME	"long-name"
#define XENSND_FIELD_RING_REF		"ring-ref"
#define XENSND_FIELD_EVT_CHNL		"event-channel"
#define XENSND_FIELD_EVT_RING_REF	"evt-ring-ref"
#define XENSND_FIELD_EVT_EVT_CHNL	"evt-event-channel"
#define XENSND_FIELD_DEVICE_NAME	"name"
#define XENSND_FIELD_TYPE		"type"
#define XENSND_FIELD_STREAM_UNIQUE_ID	"unique-id"
#define XENSND_FIELD_CHANNELS_MIN	"channels-min"
#define XENSND_FIELD_CHANNELS_MAX	"channels-max"
#define XENSND_FIELD_SAMPLE_RATES	"sample-rates"
#define XENSND_FIELD_SAMPLE_FORMATS	"sample-formats"
#define XENSND_FIELD_BUFFER_SIZE	"buffer-size"


#define XENSND_STREAM_TYPE_PLAYBACK	"p"
#define XENSND_STREAM_TYPE_CAPTURE	"c"

#define XENSND_SAMPLE_RATE_MAX_LEN	11

#define XENSND_SAMPLE_FORMAT_MAX_LEN	24

#define XENSND_PCM_FORMAT_S8_STR	"s8"
#define XENSND_PCM_FORMAT_U8_STR	"u8"
#define XENSND_PCM_FORMAT_S16_LE_STR	"s16_le"
#define XENSND_PCM_FORMAT_S16_BE_STR	"s16_be"
#define XENSND_PCM_FORMAT_U16_LE_STR	"u16_le"
#define XENSND_PCM_FORMAT_U16_BE_STR	"u16_be"
#define XENSND_PCM_FORMAT_S24_LE_STR	"s24_le"
#define XENSND_PCM_FORMAT_S24_BE_STR	"s24_be"
#define XENSND_PCM_FORMAT_U24_LE_STR	"u24_le"
#define XENSND_PCM_FORMAT_U24_BE_STR	"u24_be"
#define XENSND_PCM_FORMAT_S32_LE_STR	"s32_le"
#define XENSND_PCM_FORMAT_S32_BE_STR	"s32_be"
#define XENSND_PCM_FORMAT_U32_LE_STR	"u32_le"
#define XENSND_PCM_FORMAT_U32_BE_STR	"u32_be"
#define XENSND_PCM_FORMAT_F32_LE_STR	"float_le"
#define XENSND_PCM_FORMAT_F32_BE_STR	"float_be"
#define XENSND_PCM_FORMAT_F64_LE_STR	"float64_le"
#define XENSND_PCM_FORMAT_F64_BE_STR	"float64_be"
#define XENSND_PCM_FORMAT_IEC958_SUBFRAME_LE_STR "iec958_subframe_le"
#define XENSND_PCM_FORMAT_IEC958_SUBFRAME_BE_STR "iec958_subframe_be"
#define XENSND_PCM_FORMAT_MU_LAW_STR	"mu_law"
#define XENSND_PCM_FORMAT_A_LAW_STR	"a_law"
#define XENSND_PCM_FORMAT_IMA_ADPCM_STR	"ima_adpcm"
#define XENSND_PCM_FORMAT_MPEG_STR	"mpeg"
#define XENSND_PCM_FORMAT_GSM_STR	"gsm"




struct xensnd_open_req {
	uint32_t pcm_rate;
	uint8_t pcm_format;
	uint8_t pcm_channels;
	uint16_t reserved;
	uint32_t buffer_sz;
	grant_ref_t gref_directory;
	uint32_t period_sz;
};



struct xensnd_page_directory {
	grant_ref_t gref_dir_next_page;
	grant_ref_t gref[1]; 
};



struct xensnd_rw_req {
	uint32_t offset;
	uint32_t length;
};



struct xensnd_trigger_req {
	uint8_t type;
};



struct xensnd_query_hw_param {
	uint64_t formats;
	struct {
		uint32_t min;
		uint32_t max;
	} rates;
	struct {
		uint32_t min;
		uint32_t max;
	} channels;
	struct {
		uint32_t min;
		uint32_t max;
	} buffer;
	struct {
		uint32_t min;
		uint32_t max;
	} period;
};





struct xensnd_cur_pos_evt {
	uint64_t position;
};

struct xensnd_req {
	uint16_t id;
	uint8_t operation;
	uint8_t reserved[5];
	union {
		struct xensnd_open_req open;
		struct xensnd_rw_req rw;
		struct xensnd_trigger_req trigger;
		struct xensnd_query_hw_param hw_param;
		uint8_t reserved[56];
	} op;
};

struct xensnd_resp {
	uint16_t id;
	uint8_t operation;
	uint8_t reserved;
	int32_t status;
	union {
		struct xensnd_query_hw_param hw_param;
		uint8_t reserved1[56];
	} resp;
};

struct xensnd_evt {
	uint16_t id;
	uint8_t type;
	uint8_t reserved[5];
	union {
		struct xensnd_cur_pos_evt cur_pos;
		uint8_t reserved[56];
	} op;
};

DEFINE_RING_TYPES(xen_sndif, struct xensnd_req, struct xensnd_resp);



struct xensnd_event_page {
	uint32_t in_cons;
	uint32_t in_prod;
	uint8_t reserved[56];
};

#define XENSND_EVENT_PAGE_SIZE XEN_PAGE_SIZE
#define XENSND_IN_RING_OFFS (sizeof(struct xensnd_event_page))
#define XENSND_IN_RING_SIZE (XENSND_EVENT_PAGE_SIZE - XENSND_IN_RING_OFFS)
#define XENSND_IN_RING_LEN (XENSND_IN_RING_SIZE / sizeof(struct xensnd_evt))
#define XENSND_IN_RING(page) \
	((struct xensnd_evt *)((char *)(page) + XENSND_IN_RING_OFFS))
#define XENSND_IN_RING_REF(page, idx) \
	(XENSND_IN_RING((page))[(idx) % XENSND_IN_RING_LEN])

#endif 
