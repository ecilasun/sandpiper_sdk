/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_SURFACE_AGGREGATOR_SERIAL_HUB_H
#define _LINUX_SURFACE_AGGREGATOR_SERIAL_HUB_H

#include <linux/crc-ccitt.h>
#include <linux/kref.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/types.h>





enum ssh_frame_type {
	SSH_FRAME_TYPE_DATA_SEQ = 0x80,
	SSH_FRAME_TYPE_DATA_NSQ = 0x00,
	SSH_FRAME_TYPE_ACK      = 0x40,
	SSH_FRAME_TYPE_NAK      = 0x04,
};


struct ssh_frame {
	u8 type;
	__le16 len;
	u8 seq;
} __packed;

static_assert(sizeof(struct ssh_frame) == 4);


#define SSH_FRAME_MAX_PAYLOAD_SIZE	U16_MAX


enum ssh_payload_type {
	SSH_PLD_TYPE_CMD = 0x80,
};


struct ssh_command {
	u8 type;
	u8 tc;
	u8 tid;
	u8 sid;
	u8 iid;
	__le16 rqid;
	u8 cid;
} __packed;

static_assert(sizeof(struct ssh_command) == 8);


#define SSH_COMMAND_MAX_PAYLOAD_SIZE \
	(SSH_FRAME_MAX_PAYLOAD_SIZE - sizeof(struct ssh_command))


#define SSH_MSG_LEN_BASE	(sizeof(struct ssh_frame) + 3ull * sizeof(u16))


#define SSH_MSG_LEN_CTRL	SSH_MSG_LEN_BASE


#define SSH_MESSAGE_LENGTH(payload_size) (SSH_MSG_LEN_BASE + (payload_size))


#define SSH_COMMAND_MESSAGE_LENGTH(payload_size) \
	SSH_MESSAGE_LENGTH(sizeof(struct ssh_command) + (payload_size))


#define SSH_MSGOFFSET_FRAME(field) \
	(sizeof(u16) + offsetof(struct ssh_frame, field))


#define SSH_MSGOFFSET_COMMAND(field) \
	(2ull * sizeof(u16) + sizeof(struct ssh_frame) \
		+ offsetof(struct ssh_command, field))


#define SSH_MSG_SYN		((u16)0x55aa)


static inline u16 ssh_crc(const u8 *buf, size_t len)
{
	return crc_ccitt_false(0xffff, buf, len);
}


#define SSH_NUM_EVENTS		38


#define SSH_NUM_TARGETS		2


static inline u16 ssh_rqid_next_valid(u16 rqid)
{
	return rqid > 0 ? rqid + 1u : rqid + SSH_NUM_EVENTS + 1u;
}


static inline u16 ssh_rqid_to_event(u16 rqid)
{
	return rqid - 1u;
}


static inline bool ssh_rqid_is_event(u16 rqid)
{
	return ssh_rqid_to_event(rqid) < SSH_NUM_EVENTS;
}


static inline u16 ssh_tc_to_rqid(u8 tc)
{
	return tc;
}


static inline u8 ssh_tid_to_index(u8 tid)
{
	return tid - 1u;
}


static inline bool ssh_tid_is_valid(u8 tid)
{
	return ssh_tid_to_index(tid) < SSH_NUM_TARGETS;
}


struct ssam_span {
	u8    *ptr;
	size_t len;
};


enum ssam_ssh_tid {
	SSAM_SSH_TID_HOST     = 0x00,
	SSAM_SSH_TID_SAM      = 0x01,
	SSAM_SSH_TID_KIP      = 0x02,
	SSAM_SSH_TID_DEBUG    = 0x03,
	SSAM_SSH_TID_SURFLINK = 0x04,
};


enum ssam_ssh_tc {
				  
	SSAM_SSH_TC_SAM  = 0x01,  
	SSAM_SSH_TC_BAT  = 0x02,  
	SSAM_SSH_TC_TMP  = 0x03,  
	SSAM_SSH_TC_PMC  = 0x04,
	SSAM_SSH_TC_FAN  = 0x05,
	SSAM_SSH_TC_PoM  = 0x06,
	SSAM_SSH_TC_DBG  = 0x07,
	SSAM_SSH_TC_KBD  = 0x08,  
	SSAM_SSH_TC_FWU  = 0x09,
	SSAM_SSH_TC_UNI  = 0x0a,
	SSAM_SSH_TC_LPC  = 0x0b,
	SSAM_SSH_TC_TCL  = 0x0c,
	SSAM_SSH_TC_SFL  = 0x0d,
	SSAM_SSH_TC_KIP  = 0x0e,  
	SSAM_SSH_TC_EXT  = 0x0f,
	SSAM_SSH_TC_BLD  = 0x10,
	SSAM_SSH_TC_BAS  = 0x11,  
	SSAM_SSH_TC_SEN  = 0x12,
	SSAM_SSH_TC_SRQ  = 0x13,
	SSAM_SSH_TC_MCU  = 0x14,
	SSAM_SSH_TC_HID  = 0x15,  
	SSAM_SSH_TC_TCH  = 0x16,
	SSAM_SSH_TC_BKL  = 0x17,
	SSAM_SSH_TC_TAM  = 0x18,
	SSAM_SSH_TC_ACC0 = 0x19,
	SSAM_SSH_TC_UFI  = 0x1a,
	SSAM_SSH_TC_USC  = 0x1b,
	SSAM_SSH_TC_PEN  = 0x1c,
	SSAM_SSH_TC_VID  = 0x1d,
	SSAM_SSH_TC_AUD  = 0x1e,
	SSAM_SSH_TC_SMC  = 0x1f,
	SSAM_SSH_TC_KPD  = 0x20,
	SSAM_SSH_TC_REG  = 0x21,  
	SSAM_SSH_TC_SPT  = 0x22,
	SSAM_SSH_TC_SYS  = 0x23,
	SSAM_SSH_TC_ACC1 = 0x24,
	SSAM_SSH_TC_SHB  = 0x25,
	SSAM_SSH_TC_POS  = 0x26,  
};





enum ssh_packet_base_priority {
	SSH_PACKET_PRIORITY_FLUSH = 0,	
	SSH_PACKET_PRIORITY_DATA  = 0,
	SSH_PACKET_PRIORITY_NAK   = 1,
	SSH_PACKET_PRIORITY_ACK   = 2,
};


#define __SSH_PACKET_PRIORITY(base, try) \
	(((base) << 4) | ((try) & 0x0f))


#define SSH_PACKET_PRIORITY(base, try) \
	__SSH_PACKET_PRIORITY(SSH_PACKET_PRIORITY_##base, (try))


static inline u8 ssh_packet_priority_get_try(u8 priority)
{
	return priority & 0x0f;
}


static inline u8 ssh_packet_priority_get_base(u8 priority)
{
	return (priority & 0xf0) >> 4;
}

enum ssh_packet_flags {
	
	SSH_PACKET_SF_LOCKED_BIT,
	SSH_PACKET_SF_QUEUED_BIT,
	SSH_PACKET_SF_PENDING_BIT,
	SSH_PACKET_SF_TRANSMITTING_BIT,
	SSH_PACKET_SF_TRANSMITTED_BIT,
	SSH_PACKET_SF_ACKED_BIT,
	SSH_PACKET_SF_CANCELED_BIT,
	SSH_PACKET_SF_COMPLETED_BIT,

	
	SSH_PACKET_TY_FLUSH_BIT,
	SSH_PACKET_TY_SEQUENCED_BIT,
	SSH_PACKET_TY_BLOCKING_BIT,

	
	SSH_PACKET_FLAGS_SF_MASK =
		  BIT(SSH_PACKET_SF_LOCKED_BIT)
		| BIT(SSH_PACKET_SF_QUEUED_BIT)
		| BIT(SSH_PACKET_SF_PENDING_BIT)
		| BIT(SSH_PACKET_SF_TRANSMITTING_BIT)
		| BIT(SSH_PACKET_SF_TRANSMITTED_BIT)
		| BIT(SSH_PACKET_SF_ACKED_BIT)
		| BIT(SSH_PACKET_SF_CANCELED_BIT)
		| BIT(SSH_PACKET_SF_COMPLETED_BIT),

	
	SSH_PACKET_FLAGS_TY_MASK =
		  BIT(SSH_PACKET_TY_FLUSH_BIT)
		| BIT(SSH_PACKET_TY_SEQUENCED_BIT)
		| BIT(SSH_PACKET_TY_BLOCKING_BIT),
};

struct ssh_ptl;
struct ssh_packet;


struct ssh_packet_ops {
	void (*release)(struct ssh_packet *p);
	void (*complete)(struct ssh_packet *p, int status);
};


struct ssh_packet {
	struct ssh_ptl *ptl;
	struct kref refcnt;

	u8 priority;

	struct {
		size_t len;
		u8 *ptr;
	} data;

	unsigned long state;
	ktime_t timestamp;

	struct list_head queue_node;
	struct list_head pending_node;

	const struct ssh_packet_ops *ops;
};

struct ssh_packet *ssh_packet_get(struct ssh_packet *p);
void ssh_packet_put(struct ssh_packet *p);


static inline void ssh_packet_set_data(struct ssh_packet *p, u8 *ptr, size_t len)
{
	p->data.ptr = ptr;
	p->data.len = len;
}




enum ssh_request_flags {
	
	SSH_REQUEST_SF_LOCKED_BIT,
	SSH_REQUEST_SF_QUEUED_BIT,
	SSH_REQUEST_SF_PENDING_BIT,
	SSH_REQUEST_SF_TRANSMITTING_BIT,
	SSH_REQUEST_SF_TRANSMITTED_BIT,
	SSH_REQUEST_SF_RSPRCVD_BIT,
	SSH_REQUEST_SF_CANCELED_BIT,
	SSH_REQUEST_SF_COMPLETED_BIT,

	
	SSH_REQUEST_TY_FLUSH_BIT,
	SSH_REQUEST_TY_HAS_RESPONSE_BIT,

	
	SSH_REQUEST_FLAGS_SF_MASK =
		  BIT(SSH_REQUEST_SF_LOCKED_BIT)
		| BIT(SSH_REQUEST_SF_QUEUED_BIT)
		| BIT(SSH_REQUEST_SF_PENDING_BIT)
		| BIT(SSH_REQUEST_SF_TRANSMITTING_BIT)
		| BIT(SSH_REQUEST_SF_TRANSMITTED_BIT)
		| BIT(SSH_REQUEST_SF_RSPRCVD_BIT)
		| BIT(SSH_REQUEST_SF_CANCELED_BIT)
		| BIT(SSH_REQUEST_SF_COMPLETED_BIT),

	
	SSH_REQUEST_FLAGS_TY_MASK =
		  BIT(SSH_REQUEST_TY_FLUSH_BIT)
		| BIT(SSH_REQUEST_TY_HAS_RESPONSE_BIT),
};

struct ssh_rtl;
struct ssh_request;


struct ssh_request_ops {
	void (*release)(struct ssh_request *rqst);
	void (*complete)(struct ssh_request *rqst,
			 const struct ssh_command *cmd,
			 const struct ssam_span *data, int status);
};


struct ssh_request {
	struct ssh_packet packet;
	struct list_head node;

	unsigned long state;
	ktime_t timestamp;

	const struct ssh_request_ops *ops;
};


static inline struct ssh_request *to_ssh_request(struct ssh_packet *p)
{
	return container_of(p, struct ssh_request, packet);
}


static inline struct ssh_request *ssh_request_get(struct ssh_request *r)
{
	return r ? to_ssh_request(ssh_packet_get(&r->packet)) : NULL;
}


static inline void ssh_request_put(struct ssh_request *r)
{
	if (r)
		ssh_packet_put(&r->packet);
}


static inline void ssh_request_set_data(struct ssh_request *r, u8 *ptr, size_t len)
{
	ssh_packet_set_data(&r->packet, ptr, len);
}

#endif 
