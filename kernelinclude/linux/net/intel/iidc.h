/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _IIDC_H_
#define _IIDC_H_

#include <linux/auxiliary_bus.h>
#include <linux/dcbnl.h>
#include <linux/device.h>
#include <linux/if_ether.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

enum iidc_event_type {
	IIDC_EVENT_BEFORE_MTU_CHANGE,
	IIDC_EVENT_AFTER_MTU_CHANGE,
	IIDC_EVENT_BEFORE_TC_CHANGE,
	IIDC_EVENT_AFTER_TC_CHANGE,
	IIDC_EVENT_CRIT_ERR,
	IIDC_EVENT_NBITS		
};

enum iidc_reset_type {
	IIDC_PFR,
	IIDC_CORER,
	IIDC_GLOBR,
};

enum iidc_rdma_protocol {
	IIDC_RDMA_PROTOCOL_IWARP = BIT(0),
	IIDC_RDMA_PROTOCOL_ROCEV2 = BIT(1),
};

#define IIDC_MAX_USER_PRIORITY		8
#define IIDC_MAX_DSCP_MAPPING		64
#define IIDC_DSCP_PFC_MODE		0x1


struct iidc_rdma_qset_params {
	
	u32 teid;	
	u16 qs_handle; 
	u16 vport_id; 
	u8 tc; 
};

struct iidc_qos_info {
	u64 tc_ctx;
	u8 rel_bw;
	u8 prio_type;
	u8 egress_virt_up;
	u8 ingress_virt_up;
};


struct iidc_qos_params {
	struct iidc_qos_info tc_info[IEEE_8021QAZ_MAX_TCS];
	u8 up2tc[IIDC_MAX_USER_PRIORITY];
	u8 vport_relative_bw;
	u8 vport_priority_type;
	u8 num_tc;
	u8 pfc_mode;
	u8 dscp_map[IIDC_MAX_DSCP_MAPPING];
};

struct iidc_event {
	DECLARE_BITMAP(type, IIDC_EVENT_NBITS);
	u32 reg;
};

struct ice_pf;

int ice_add_rdma_qset(struct ice_pf *pf, struct iidc_rdma_qset_params *qset);
int ice_del_rdma_qset(struct ice_pf *pf, struct iidc_rdma_qset_params *qset);
int ice_rdma_request_reset(struct ice_pf *pf, enum iidc_reset_type reset_type);
int ice_rdma_update_vsi_filter(struct ice_pf *pf, u16 vsi_id, bool enable);
void ice_get_qos_params(struct ice_pf *pf, struct iidc_qos_params *qos);



struct iidc_auxiliary_dev {
	struct auxiliary_device adev;
	struct ice_pf *pf;
};


struct iidc_auxiliary_drv {
	struct auxiliary_driver adrv;
	
	void (*event_handler)(struct ice_pf *pf, struct iidc_event *event);
};

#endif 
