/* SPDX-License-Identifier: GPL-2.0 */


#ifndef K3_PSIL_H_
#define K3_PSIL_H_

#include <linux/types.h>

#define K3_PSIL_DST_THREAD_ID_OFFSET 0x8000

struct device;


enum udma_tp_level {
	UDMA_TP_NORMAL = 0,
	UDMA_TP_HIGH,
	UDMA_TP_ULTRAHIGH,
	UDMA_TP_LAST,
};


enum psil_endpoint_type {
	PSIL_EP_NATIVE = 0,
	PSIL_EP_PDMA_XY,
	PSIL_EP_PDMA_MCAN,
	PSIL_EP_PDMA_AASRC,
};


struct psil_endpoint_config {
	enum psil_endpoint_type ep_type;
	enum udma_tp_level channel_tpl;

	unsigned pkt_mode:1;
	unsigned notdpkt:1;
	unsigned needs_epib:1;
	
	unsigned pdma_acc32:1;
	unsigned pdma_burst:1;

	u32 psd_size;
	
	s16 mapped_channel_id;
	
	u16 flow_start;
	u16 flow_num;
	s16 default_flow_id;
};

int psil_set_new_ep_config(struct device *dev, const char *name,
			   struct psil_endpoint_config *ep_config);

#endif 
