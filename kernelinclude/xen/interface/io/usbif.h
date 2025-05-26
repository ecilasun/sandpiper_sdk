/* SPDX-License-Identifier: MIT */



#ifndef __XEN_PUBLIC_IO_USBIF_H__
#define __XEN_PUBLIC_IO_USBIF_H__

#include "ring.h"
#include "../grant_table.h"



enum xenusb_spec_version {
	XENUSB_VER_UNKNOWN = 0,
	XENUSB_VER_USB11,
	XENUSB_VER_USB20,
	XENUSB_VER_USB30,	
};



#define XENUSB_PIPE_PORT_MASK	0x0000001f
#define XENUSB_PIPE_UNLINK	0x00000020
#define XENUSB_PIPE_DIR		0x00000080
#define XENUSB_PIPE_DEV_MASK	0x0000007f
#define XENUSB_PIPE_DEV_SHIFT	8
#define XENUSB_PIPE_EP_MASK	0x0000000f
#define XENUSB_PIPE_EP_SHIFT	15
#define XENUSB_PIPE_TYPE_MASK	0x00000003
#define XENUSB_PIPE_TYPE_SHIFT	30
#define XENUSB_PIPE_TYPE_ISOC	0
#define XENUSB_PIPE_TYPE_INT	1
#define XENUSB_PIPE_TYPE_CTRL	2
#define XENUSB_PIPE_TYPE_BULK	3

#define xenusb_pipeportnum(pipe)		((pipe) & XENUSB_PIPE_PORT_MASK)
#define xenusb_setportnum_pipe(pipe, portnum)	((pipe) | (portnum))

#define xenusb_pipeunlink(pipe)			((pipe) & XENUSB_PIPE_UNLINK)
#define xenusb_pipesubmit(pipe)			(!xenusb_pipeunlink(pipe))
#define xenusb_setunlink_pipe(pipe)		((pipe) | XENUSB_PIPE_UNLINK)

#define xenusb_pipein(pipe)			((pipe) & XENUSB_PIPE_DIR)
#define xenusb_pipeout(pipe)			(!xenusb_pipein(pipe))

#define xenusb_pipedevice(pipe)			\
	(((pipe) >> XENUSB_PIPE_DEV_SHIFT) & XENUSB_PIPE_DEV_MASK)

#define xenusb_pipeendpoint(pipe)		\
	(((pipe) >> XENUSB_PIPE_EP_SHIFT) & XENUSB_PIPE_EP_MASK)

#define xenusb_pipetype(pipe)			\
	(((pipe) >> XENUSB_PIPE_TYPE_SHIFT) & XENUSB_PIPE_TYPE_MASK)
#define xenusb_pipeisoc(pipe)	(xenusb_pipetype(pipe) == XENUSB_PIPE_TYPE_ISOC)
#define xenusb_pipeint(pipe)	(xenusb_pipetype(pipe) == XENUSB_PIPE_TYPE_INT)
#define xenusb_pipectrl(pipe)	(xenusb_pipetype(pipe) == XENUSB_PIPE_TYPE_CTRL)
#define xenusb_pipebulk(pipe)	(xenusb_pipetype(pipe) == XENUSB_PIPE_TYPE_BULK)

#define XENUSB_MAX_SEGMENTS_PER_REQUEST (16)
#define XENUSB_MAX_PORTNR		31
#define XENUSB_RING_SIZE		4096


struct xenusb_request_segment {
	grant_ref_t gref;
	uint16_t offset;
	uint16_t length;
};

struct xenusb_urb_request {
	uint16_t id;			
	uint16_t nr_buffer_segs;	

	
	uint32_t pipe;
	uint16_t transfer_flags;
#define XENUSB_SHORT_NOT_OK	0x0001
	uint16_t buffer_length;
	union {
		uint8_t ctrl[8];	

		struct {
			uint16_t interval;	
			uint16_t start_frame;	
			uint16_t number_of_packets;	
			uint16_t nr_frame_desc_segs;	
		} isoc;

		struct {
			uint16_t interval;	
			uint16_t pad[3];
		} intr;

		struct {
			uint16_t unlink_id;	
			uint16_t pad[3];
		} unlink;

	} u;

	
	struct xenusb_request_segment seg[XENUSB_MAX_SEGMENTS_PER_REQUEST];
};

struct xenusb_urb_response {
	uint16_t id;		
	uint16_t start_frame;	
	int32_t status;		
#define XENUSB_STATUS_OK	0
#define XENUSB_STATUS_NODEV	(-19)
#define XENUSB_STATUS_INVAL	(-22)
#define XENUSB_STATUS_STALL	(-32)
#define XENUSB_STATUS_IOERROR	(-71)
#define XENUSB_STATUS_BABBLE	(-75)
#define XENUSB_STATUS_SHUTDOWN	(-108)
	int32_t actual_length;	
	int32_t error_count;	
};

DEFINE_RING_TYPES(xenusb_urb, struct xenusb_urb_request, struct xenusb_urb_response);
#define XENUSB_URB_RING_SIZE __CONST_RING_SIZE(xenusb_urb, XENUSB_RING_SIZE)


struct xenusb_conn_request {
	uint16_t id;
};

struct xenusb_conn_response {
	uint16_t id;		
	uint8_t portnum;	
	uint8_t speed;		
#define XENUSB_SPEED_NONE	0
#define XENUSB_SPEED_LOW	1
#define XENUSB_SPEED_FULL	2
#define XENUSB_SPEED_HIGH	3
};

DEFINE_RING_TYPES(xenusb_conn, struct xenusb_conn_request, struct xenusb_conn_response);
#define XENUSB_CONN_RING_SIZE __CONST_RING_SIZE(xenusb_conn, XENUSB_RING_SIZE)

#endif 
