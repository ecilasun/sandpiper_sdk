/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _SCSI_SCSI_H
#define _SCSI_SCSI_H

#include <linux/types.h>
#include <linux/scatterlist.h>
#include <linux/kernel.h>
#include <scsi/scsi_common.h>
#include <scsi/scsi_proto.h>
#include <scsi/scsi_status.h>

struct scsi_cmnd;

enum scsi_timeouts {
	SCSI_DEFAULT_EH_TIMEOUT		= 10 * HZ,
};


#define SCSI_MAX_PROT_SG_SEGMENTS	0xFFFF


#define SCAN_WILD_CARD	~0



struct ccs_modesel_head {
	__u8 _r1;			
	__u8 medium;		
	__u8 _r2;			
	__u8 block_desc_length;	
	__u8 density;		
	__u8 number_blocks_hi;	
	__u8 number_blocks_med;
	__u8 number_blocks_lo;
	__u8 _r3;
	__u8 block_length_hi;	
	__u8 block_length_med;
	__u8 block_length_lo;
};


#define SCSI_W_LUN_BASE 0xc100
#define SCSI_W_LUN_REPORT_LUNS (SCSI_W_LUN_BASE + 1)
#define SCSI_W_LUN_ACCESS_CONTROL (SCSI_W_LUN_BASE + 2)
#define SCSI_W_LUN_TARGET_LOG_PAGE (SCSI_W_LUN_BASE + 3)

static inline int scsi_is_wlun(u64 lun)
{
	return (lun & 0xff00) == SCSI_W_LUN_BASE;
}


static inline int scsi_status_is_check_condition(int status)
{
	if (status < 0)
		return false;
	status &= 0xfe;
	return status == SAM_STAT_CHECK_CONDITION;
}


#define     EXTENDED_MODIFY_DATA_POINTER    0x00
#define     EXTENDED_SDTR                   0x01
#define     EXTENDED_EXTENDED_IDENTIFY      0x02    
#define     EXTENDED_WDTR                   0x03
#define     EXTENDED_PPR                    0x04
#define     EXTENDED_MODIFY_BIDI_DATA_PTR   0x05


enum scsi_disposition {
	NEEDS_RETRY		= 0x2001,
	SUCCESS			= 0x2002,
	FAILED			= 0x2003,
	QUEUED			= 0x2004,
	SOFT_ERROR		= 0x2005,
	ADD_TO_MLQUEUE		= 0x2006,
	TIMEOUT_ERROR		= 0x2007,
	SCSI_RETURN_NOT_HANDLED	= 0x2008,
	FAST_IO_FAIL		= 0x2009,
};


#define SCSI_MLQUEUE_HOST_BUSY   0x1055
#define SCSI_MLQUEUE_DEVICE_BUSY 0x1056
#define SCSI_MLQUEUE_EH_RETRY    0x1057
#define SCSI_MLQUEUE_TARGET_BUSY 0x1058


#define status_byte(result) (result & 0xff)
#define host_byte(result)   (((result) >> 16) & 0xff)

#define sense_class(sense)  (((sense) >> 4) & 0x7)
#define sense_error(sense)  ((sense) & 0xf)
#define sense_valid(sense)  ((sense) & 0x80)


#define FORMAT_UNIT_TIMEOUT		(2 * 60 * 60 * HZ)
#define START_STOP_TIMEOUT		(60 * HZ)
#define MOVE_MEDIUM_TIMEOUT		(5 * 60 * HZ)
#define READ_ELEMENT_STATUS_TIMEOUT	(5 * 60 * HZ)
#define READ_DEFECT_DATA_TIMEOUT	(60 * HZ )


#define IDENTIFY_BASE       0x80
#define IDENTIFY(can_disconnect, lun)   (IDENTIFY_BASE |\
		     ((can_disconnect) ?  0x40 : 0) |\
		     ((lun) & 0x07))



#define SCSI_UNKNOWN    0
#define SCSI_1          1
#define SCSI_1_CCS      2
#define SCSI_2          3
#define SCSI_3          4        
#define SCSI_SPC_2      5
#define SCSI_SPC_3      6
#define SCSI_SPC_4	7
#define SCSI_SPC_5	8
#define SCSI_SPC_6	14


#define SCSI_INQ_PQ_CON         0x00
#define SCSI_INQ_PQ_NOT_CON     0x01
#define SCSI_INQ_PQ_NOT_CAP     0x03





#define SCSI_IOCTL_GET_IDLUN		0x5382




#define SCSI_IOCTL_PROBE_HOST		0x5385


#define SCSI_IOCTL_GET_BUS_NUMBER	0x5386


#define SCSI_IOCTL_GET_PCI		0x5387


static inline bool scsi_status_is_good(int status)
{
	if (status < 0)
		return false;

	if (host_byte(status) == DID_NO_CONNECT)
		return false;

	
	status &= 0xfe;
	return ((status == SAM_STAT_GOOD) ||
		(status == SAM_STAT_CONDITION_MET) ||
		
		(status == SAM_STAT_INTERMEDIATE) ||
		(status == SAM_STAT_INTERMEDIATE_CONDITION_MET) ||
		
		(status == SAM_STAT_COMMAND_TERMINATED));
}

#endif 
