/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _PDS_CORE_ADMINQ_H_
#define _PDS_CORE_ADMINQ_H_

#define PDSC_ADMINQ_MAX_POLL_INTERVAL	256

enum pds_core_adminq_flags {
	PDS_AQ_FLAG_FASTPOLL	= BIT(1),	
};


enum pds_core_adminq_opcode {
	PDS_AQ_CMD_NOP			= 0,

	
	PDS_AQ_CMD_CLIENT_REG		= 6,
	PDS_AQ_CMD_CLIENT_UNREG		= 7,
	PDS_AQ_CMD_CLIENT_CMD		= 8,

	
	PDS_AQ_CMD_LIF_IDENTIFY		= 20,
	PDS_AQ_CMD_LIF_INIT		= 21,
	PDS_AQ_CMD_LIF_RESET		= 22,
	PDS_AQ_CMD_LIF_GETATTR		= 23,
	PDS_AQ_CMD_LIF_SETATTR		= 24,
	PDS_AQ_CMD_LIF_SETPHC		= 25,

	PDS_AQ_CMD_RX_MODE_SET		= 30,
	PDS_AQ_CMD_RX_FILTER_ADD	= 31,
	PDS_AQ_CMD_RX_FILTER_DEL	= 32,

	
	PDS_AQ_CMD_Q_IDENTIFY		= 39,
	PDS_AQ_CMD_Q_INIT		= 40,
	PDS_AQ_CMD_Q_CONTROL		= 41,

	
	PDS_AQ_CMD_VF_GETATTR		= 60,
	PDS_AQ_CMD_VF_SETATTR		= 61,
};


enum pds_core_notifyq_opcode {
	PDS_EVENT_LINK_CHANGE		= 1,
	PDS_EVENT_RESET			= 2,
	PDS_EVENT_XCVR			= 5,
	PDS_EVENT_CLIENT		= 6,
};

#define PDS_COMP_COLOR_MASK  0x80


struct pds_core_notifyq_event {
	__le64 eid;
	__le16 ecode;
};


struct pds_core_link_change_event {
	__le64 eid;
	__le16 ecode;
	__le16 link_status;
	__le32 link_speed;	
};


struct pds_core_reset_event {
	__le64 eid;
	__le16 ecode;
	u8     reset_code;
	u8     state;
};


struct pds_core_client_event {
	__le64 eid;
	__le16 ecode;
	__le16 client_id;
	u8     client_event[54];
};


struct pds_core_notifyq_cmd {
	__le32 data;	
};


union pds_core_notifyq_comp {
	struct {
		__le64 eid;
		__le16 ecode;
	};
	struct pds_core_notifyq_event     event;
	struct pds_core_link_change_event link_change;
	struct pds_core_reset_event       reset;
	u8     data[64];
};

#define PDS_DEVNAME_LEN		32

struct pds_core_client_reg_cmd {
	u8     opcode;
	u8     rsvd[3];
	char   devname[PDS_DEVNAME_LEN];
	u8     vif_type;
};


struct pds_core_client_reg_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	__le16 client_id;
	u8     rsvd1[9];
	u8     color;
};


struct pds_core_client_unreg_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 client_id;
};


struct pds_core_client_request_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 client_id;
	u8     client_cmd[60];
};

#define PDS_CORE_MAX_FRAGS		16

#define PDS_CORE_QCQ_F_INITED		BIT(0)
#define PDS_CORE_QCQ_F_SG		BIT(1)
#define PDS_CORE_QCQ_F_INTR		BIT(2)
#define PDS_CORE_QCQ_F_TX_STATS		BIT(3)
#define PDS_CORE_QCQ_F_RX_STATS		BIT(4)
#define PDS_CORE_QCQ_F_NOTIFYQ		BIT(5)
#define PDS_CORE_QCQ_F_CMB_RINGS	BIT(6)
#define PDS_CORE_QCQ_F_CORE		BIT(7)

enum pds_core_lif_type {
	PDS_CORE_LIF_TYPE_DEFAULT = 0,
};

#define PDS_CORE_IFNAMSIZ		16


enum pds_core_logical_qtype {
	PDS_CORE_QTYPE_ADMINQ  = 0,
	PDS_CORE_QTYPE_NOTIFYQ = 1,
	PDS_CORE_QTYPE_RXQ     = 2,
	PDS_CORE_QTYPE_TXQ     = 3,
	PDS_CORE_QTYPE_EQ      = 4,

	PDS_CORE_QTYPE_MAX     = 16   
};


union pds_core_lif_config {
	struct {
		u8     state;
		u8     rsvd[3];
		char   name[PDS_CORE_IFNAMSIZ];
		u8     rsvd2[12];
		__le64 features;
		__le32 queue_count[PDS_CORE_QTYPE_MAX];
	} __packed;
	__le32 words[64];
};


struct pds_core_lif_status {
	__le64 eid;
	u8     rsvd[56];
};


struct pds_core_lif_info {
	union pds_core_lif_config config;
	struct pds_core_lif_status status;
};


struct pds_core_lif_identity {
	__le64 features;
	u8     version;
	u8     hw_index;
	u8     rsvd[2];
	__le32 max_nb_sessions;
	u8     rsvd2[120];
	union pds_core_lif_config config;
};


struct pds_core_lif_identify_cmd {
	u8     opcode;
	u8     type;
	__le16 client_id;
	u8     ver;
	u8     rsvd[3];
	__le64 ident_pa;
};


struct pds_core_lif_identify_comp {
	u8     status;
	u8     ver;
	__le16 bytes;
	u8     rsvd[11];
	u8     color;
};


struct pds_core_lif_init_cmd {
	u8     opcode;
	u8     type;
	__le16 client_id;
	__le32 rsvd;
	__le64 info_pa;
};


struct pds_core_lif_init_comp {
	u8 status;
	u8 rsvd;
	__le16 hw_index;
	u8     rsvd1[11];
	u8     color;
};


struct pds_core_lif_reset_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 client_id;
};


enum pds_core_lif_attr {
	PDS_CORE_LIF_ATTR_STATE		= 0,
	PDS_CORE_LIF_ATTR_NAME		= 1,
	PDS_CORE_LIF_ATTR_FEATURES	= 4,
	PDS_CORE_LIF_ATTR_STATS_CTRL	= 6,
};


struct pds_core_lif_setattr_cmd {
	u8     opcode;
	u8     attr;
	__le16 client_id;
	union {
		u8      state;
		char    name[PDS_CORE_IFNAMSIZ];
		__le64  features;
		u8      stats_ctl;
		u8      rsvd[60];
	} __packed;
};


struct pds_core_lif_setattr_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	union {
		__le64  features;
		u8      rsvd2[11];
	} __packed;
	u8     color;
};


struct pds_core_lif_getattr_cmd {
	u8     opcode;
	u8     attr;
	__le16 client_id;
};


struct pds_core_lif_getattr_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	union {
		u8      state;
		__le64  features;
		u8      rsvd2[11];
	} __packed;
	u8     color;
};


struct pds_core_q_identity {
	u8      version;
	u8      supported;
	u8      rsvd[6];
#define PDS_CORE_QIDENT_F_CQ	0x01	
	__le64  features;
	__le16  desc_sz;
	__le16  comp_sz;
	u8      rsvd2[6];
};


struct pds_core_q_identify_cmd {
	u8     opcode;
	u8     type;
	__le16 client_id;
	u8     ver;
	u8     rsvd[3];
	__le64 ident_pa;
};


struct pds_core_q_identify_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	u8     ver;
	u8     rsvd1[10];
	u8     color;
};


struct pds_core_q_init_cmd {
	u8     opcode;
	u8     type;
	__le16 client_id;
	u8     ver;
	u8     rsvd[3];
	__le32 index;
	__le16 pid;
	__le16 intr_index;
	__le16 flags;
#define PDS_CORE_QINIT_F_IRQ	0x01	
#define PDS_CORE_QINIT_F_ENA	0x02	
	u8     cos;
#define PDS_CORE_QSIZE_MIN_LG2	2
#define PDS_CORE_QSIZE_MAX_LG2	12
	u8     ring_size;
	__le64 ring_base;
	__le64 cq_ring_base;
} __packed;


struct pds_core_q_init_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	__le32 hw_index;
	u8     hw_type;
	u8     rsvd2[6];
	u8     color;
};


enum pds_vdpa_cmd_opcode {
	PDS_VDPA_CMD_INIT		= 48,
	PDS_VDPA_CMD_IDENT		= 49,
	PDS_VDPA_CMD_RESET		= 51,
	PDS_VDPA_CMD_VQ_RESET		= 52,
	PDS_VDPA_CMD_VQ_INIT		= 53,
	PDS_VDPA_CMD_STATUS_UPDATE	= 54,
	PDS_VDPA_CMD_SET_FEATURES	= 55,
	PDS_VDPA_CMD_SET_ATTR		= 56,
};


struct pds_vdpa_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
};


struct pds_vdpa_init_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
};


struct pds_vdpa_ident {
	__le64 hw_features;
	__le16 max_vqs;
	__le16 max_qlen;
	__le16 min_qlen;
};


struct pds_vdpa_ident_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	__le32 len;
	__le64 ident_pa;
};


struct pds_vdpa_status_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
	u8     status;
};


enum pds_vdpa_attr {
	PDS_VDPA_ATTR_MAC          = 1,
	PDS_VDPA_ATTR_MAX_VQ_PAIRS = 2,
};


struct pds_vdpa_setattr_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
	u8     attr;
	u8     pad[3];
	union {
		u8 mac[6];
		__le16 max_vq_pairs;
	} __packed;
};


struct pds_vdpa_vq_init_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
	__le16 qid;
	__le16 len;
	__le64 desc_addr;
	__le64 avail_addr;
	__le64 used_addr;
	__le16 intr_index;
	__le16 avail_index;
	__le16 used_index;
};


struct pds_vdpa_vq_init_comp {
	u8     status;
	u8     hw_qtype;
	__le16 hw_qindex;
	u8     rsvd[11];
	u8     color;
};


struct pds_vdpa_vq_reset_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
	__le16 qid;
};


struct pds_vdpa_vq_reset_comp {
	u8     status;
	u8     rsvd0;
	__le16 avail_index;
	__le16 used_index;
	u8     rsvd[9];
	u8     color;
};


struct pds_vdpa_set_features_cmd {
	u8     opcode;
	u8     vdpa_index;
	__le16 vf_id;
	__le32 rsvd;
	__le64 features;
};

#define PDS_LM_DEVICE_STATE_LENGTH		65536
#define PDS_LM_CHECK_DEVICE_STATE_LENGTH(X) \
			PDS_CORE_SIZE_CHECK(union, PDS_LM_DEVICE_STATE_LENGTH, X)


enum pds_lm_cmd_opcode {
	PDS_LM_CMD_HOST_VF_STATUS  = 1,

	
	PDS_LM_CMD_STATE_SIZE	   = 16,
	PDS_LM_CMD_SUSPEND         = 18,
	PDS_LM_CMD_SUSPEND_STATUS  = 19,
	PDS_LM_CMD_RESUME          = 20,
	PDS_LM_CMD_SAVE            = 21,
	PDS_LM_CMD_RESTORE         = 22,

	
	PDS_LM_CMD_DIRTY_STATUS    = 32,
	PDS_LM_CMD_DIRTY_ENABLE    = 33,
	PDS_LM_CMD_DIRTY_DISABLE   = 34,
	PDS_LM_CMD_DIRTY_READ_SEQ  = 35,
	PDS_LM_CMD_DIRTY_WRITE_ACK = 36,
};


struct pds_lm_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     rsvd2[56];
};


struct pds_lm_state_size_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
};


struct pds_lm_state_size_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	union {
		__le64 size;
		u8     rsvd2[11];
	} __packed;
	u8     color;
};

enum pds_lm_suspend_resume_type {
	PDS_LM_SUSPEND_RESUME_TYPE_FULL = 0,
	PDS_LM_SUSPEND_RESUME_TYPE_P2P = 1,
};


struct pds_lm_suspend_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     type;
};


struct pds_lm_suspend_status_cmd {
	u8 opcode;
	u8 rsvd;
	__le16 vf_id;
	u8 type;
};


struct pds_lm_resume_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     type;
};


struct pds_lm_sg_elem {
	__le64 addr;
	__le32 len;
	__le16 rsvd[2];
};


struct pds_lm_save_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     rsvd2[4];
	__le64 sgl_addr;
	__le32 num_sge;
} __packed;


struct pds_lm_restore_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     rsvd2[4];
	__le64 sgl_addr;
	__le32 num_sge;
} __packed;


union pds_lm_dev_state {
	__le32 words[PDS_LM_DEVICE_STATE_LENGTH / sizeof(__le32)];
};

enum pds_lm_host_vf_status {
	PDS_LM_STA_NONE = 0,
	PDS_LM_STA_IN_PROGRESS,
	PDS_LM_STA_MAX,
};


struct pds_lm_dirty_region_info {
	__le64 dma_base;
	__le32 page_count;
	u8     page_size_log2;
	u8     rsvd[3];
};


struct pds_lm_dirty_status_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     max_regions;
	u8     rsvd2[3];
	__le64 regions_dma;
} __packed;


enum pds_lm_dirty_bmp_type {
	PDS_LM_DIRTY_BMP_TYPE_NONE     = 0,
	PDS_LM_DIRTY_BMP_TYPE_SEQ_ACK  = 1,
};


struct pds_lm_dirty_status_comp {
	u8     status;
	u8     rsvd;
	__le16 comp_index;
	u8     max_regions;
	u8     num_regions;
	u8     bmp_type;
	u8     rsvd2;
	__le32 bmp_type_mask;
	u8     rsvd3[3];
	u8     color;
};


struct pds_lm_dirty_enable_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     bmp_type;
	u8     num_regions;
	u8     rsvd2[2];
	__le64 regions_dma;
} __packed;


struct pds_lm_dirty_disable_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
};


struct pds_lm_dirty_seq_ack_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	__le32 off_bytes;
	__le32 len_bytes;
	__le16 num_sge;
	u8     rsvd2[2];
	__le64 sgl_addr;
} __packed;


struct pds_lm_host_vf_status_cmd {
	u8     opcode;
	u8     rsvd;
	__le16 vf_id;
	u8     status;
};

union pds_core_adminq_cmd {
	u8     opcode;
	u8     bytes[64];

	struct pds_core_client_reg_cmd     client_reg;
	struct pds_core_client_unreg_cmd   client_unreg;
	struct pds_core_client_request_cmd client_request;

	struct pds_core_lif_identify_cmd  lif_ident;
	struct pds_core_lif_init_cmd      lif_init;
	struct pds_core_lif_reset_cmd     lif_reset;
	struct pds_core_lif_setattr_cmd   lif_setattr;
	struct pds_core_lif_getattr_cmd   lif_getattr;

	struct pds_core_q_identify_cmd    q_ident;
	struct pds_core_q_init_cmd        q_init;

	struct pds_vdpa_cmd		  vdpa;
	struct pds_vdpa_init_cmd	  vdpa_init;
	struct pds_vdpa_ident_cmd	  vdpa_ident;
	struct pds_vdpa_status_cmd	  vdpa_status;
	struct pds_vdpa_setattr_cmd	  vdpa_setattr;
	struct pds_vdpa_set_features_cmd  vdpa_set_features;
	struct pds_vdpa_vq_init_cmd	  vdpa_vq_init;
	struct pds_vdpa_vq_reset_cmd	  vdpa_vq_reset;

	struct pds_lm_suspend_cmd	  lm_suspend;
	struct pds_lm_suspend_status_cmd  lm_suspend_status;
	struct pds_lm_resume_cmd	  lm_resume;
	struct pds_lm_state_size_cmd	  lm_state_size;
	struct pds_lm_save_cmd		  lm_save;
	struct pds_lm_restore_cmd	  lm_restore;
	struct pds_lm_host_vf_status_cmd  lm_host_vf_status;
	struct pds_lm_dirty_status_cmd	  lm_dirty_status;
	struct pds_lm_dirty_enable_cmd	  lm_dirty_enable;
	struct pds_lm_dirty_disable_cmd	  lm_dirty_disable;
	struct pds_lm_dirty_seq_ack_cmd	  lm_dirty_seq_ack;
};

union pds_core_adminq_comp {
	struct {
		u8     status;
		u8     rsvd;
		__le16 comp_index;
		u8     rsvd2[11];
		u8     color;
	};
	u32    words[4];

	struct pds_core_client_reg_comp   client_reg;

	struct pds_core_lif_identify_comp lif_ident;
	struct pds_core_lif_init_comp     lif_init;
	struct pds_core_lif_setattr_comp  lif_setattr;
	struct pds_core_lif_getattr_comp  lif_getattr;

	struct pds_core_q_identify_comp   q_ident;
	struct pds_core_q_init_comp       q_init;

	struct pds_vdpa_vq_init_comp	  vdpa_vq_init;
	struct pds_vdpa_vq_reset_comp	  vdpa_vq_reset;

	struct pds_lm_state_size_comp	  lm_state_size;
	struct pds_lm_dirty_status_comp	  lm_dirty_status;
};

#ifndef __CHECKER__
static_assert(sizeof(union pds_core_adminq_cmd) == 64);
static_assert(sizeof(union pds_core_adminq_comp) == 16);
static_assert(sizeof(union pds_core_notifyq_comp) == 64);
#endif 


static inline bool pdsc_color_match(u8 color, bool done_color)
{
	return (!!(color & PDS_COMP_COLOR_MASK)) == done_color;
}

struct pdsc;
int pdsc_adminq_post(struct pdsc *pdsc,
		     union pds_core_adminq_cmd *cmd,
		     union pds_core_adminq_comp *comp,
		     bool fast_poll);

#endif 
