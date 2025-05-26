/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SCSI_SCSI_TCQ_H
#define _SCSI_SCSI_TCQ_H

#include <linux/blkdev.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>

#define SCSI_NO_TAG	(-1)    


#ifdef CONFIG_BLOCK

static inline struct scsi_cmnd *scsi_host_find_tag(struct Scsi_Host *shost,
		int tag)
{
	struct request *req = NULL;
	u16 hwq;

	if (tag == SCSI_NO_TAG)
		return NULL;

	hwq = blk_mq_unique_tag_to_hwq(tag);
	if (hwq < shost->tag_set.nr_hw_queues) {
		req = blk_mq_tag_to_rq(shost->tag_set.tags[hwq],
					blk_mq_unique_tag_to_tag(tag));
	}

	if (!req || !blk_mq_request_started(req))
		return NULL;
	return blk_mq_rq_to_pdu(req);
}

#endif 
#endif 
