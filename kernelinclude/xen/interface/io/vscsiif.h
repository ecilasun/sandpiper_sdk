/* SPDX-License-Identifier: MIT */


#ifndef __XEN__PUBLIC_IO_SCSI_H__
#define __XEN__PUBLIC_IO_SCSI_H__

#include "ring.h"
#include "../grant_table.h"








#define VSCSIIF_ACT_SCSI_CDB		1


#define VSCSIIF_ACT_SCSI_ABORT		2


#define VSCSIIF_ACT_SCSI_RESET		3


#define VSCSIIF_ACT_SCSI_SG_PRESET	4


#define VSCSIIF_SG_TABLESIZE		26


#define VSCSIIF_MAX_COMMAND_SIZE	16
#define VSCSIIF_SENSE_BUFFERSIZE	96
#define VSCSIIF_PAGE_SIZE		4096

struct scsiif_request_segment {
	grant_ref_t gref;
	uint16_t offset;
	uint16_t length;
};

#define VSCSIIF_SG_PER_PAGE	(VSCSIIF_PAGE_SIZE / \
				 sizeof(struct scsiif_request_segment))


struct vscsiif_request {
	uint16_t rqid;		
	uint8_t act;		
	uint8_t cmd_len;	

	uint8_t cmnd[VSCSIIF_MAX_COMMAND_SIZE];	
	uint16_t timeout_per_command;	
	uint16_t channel, id, lun;	
	uint16_t ref_rqid;		
	uint8_t sc_data_direction;	
	uint8_t nr_segments;		

#define VSCSIIF_SG_GRANT	0x80

	struct scsiif_request_segment seg[VSCSIIF_SG_TABLESIZE];
	uint32_t reserved[3];
};


struct vscsiif_response {
	uint16_t rqid;		
	uint8_t padding;
	uint8_t sense_len;
	uint8_t sense_buffer[VSCSIIF_SENSE_BUFFERSIZE];
	int32_t rslt;
	uint32_t residual_len;	
	uint32_t reserved[36];
};


#define XEN_VSCSIIF_RSLT_STATUS(x)  ((x) & 0x00ff)


#define XEN_VSCSIIF_RSLT_HOST(x)    (((x) & 0x00ff0000) >> 16)
#define XEN_VSCSIIF_RSLT_HOST_OK                   0

#define XEN_VSCSIIF_RSLT_HOST_NO_CONNECT           1

#define XEN_VSCSIIF_RSLT_HOST_BUS_BUSY             2

#define XEN_VSCSIIF_RSLT_HOST_TIME_OUT             3

#define XEN_VSCSIIF_RSLT_HOST_BAD_TARGET           4

#define XEN_VSCSIIF_RSLT_HOST_ABORT                5

#define XEN_VSCSIIF_RSLT_HOST_PARITY               6

#define XEN_VSCSIIF_RSLT_HOST_ERROR                7

#define XEN_VSCSIIF_RSLT_HOST_RESET                8

#define XEN_VSCSIIF_RSLT_HOST_BAD_INTR             9

#define XEN_VSCSIIF_RSLT_HOST_PASSTHROUGH         10

#define XEN_VSCSIIF_RSLT_HOST_SOFT_ERROR          11

#define XEN_VSCSIIF_RSLT_HOST_IMM_RETRY           12

#define XEN_VSCSIIF_RSLT_HOST_REQUEUE             13

#define XEN_VSCSIIF_RSLT_HOST_TRANSPORT_DISRUPTED 14

#define XEN_VSCSIIF_RSLT_HOST_TRANSPORT_FAILFAST  15

#define XEN_VSCSIIF_RSLT_HOST_TARGET_FAILURE      16

#define XEN_VSCSIIF_RSLT_HOST_NEXUS_FAILURE       17

#define XEN_VSCSIIF_RSLT_HOST_ALLOC_FAILURE       18

#define XEN_VSCSIIF_RSLT_HOST_MEDIUM_ERROR        19

#define XEN_VSCSIIF_RSLT_HOST_TRANSPORT_MARGINAL  20


#define XEN_VSCSIIF_RSLT_RESET_SUCCESS  0x2002
#define XEN_VSCSIIF_RSLT_RESET_FAILED   0x2003

DEFINE_RING_TYPES(vscsiif, struct vscsiif_request, struct vscsiif_response);


#endif  
