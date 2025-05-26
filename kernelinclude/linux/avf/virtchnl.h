/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _VIRTCHNL_H_
#define _VIRTCHNL_H_

#include <linux/bitops.h>
#include <linux/bits.h>
#include <linux/overflow.h>
#include <uapi/linux/if_ether.h>






enum virtchnl_status_code {
	VIRTCHNL_STATUS_SUCCESS				= 0,
	VIRTCHNL_STATUS_ERR_PARAM			= -5,
	VIRTCHNL_STATUS_ERR_NO_MEMORY			= -18,
	VIRTCHNL_STATUS_ERR_OPCODE_MISMATCH		= -38,
	VIRTCHNL_STATUS_ERR_CQP_COMPL_ERROR		= -39,
	VIRTCHNL_STATUS_ERR_INVALID_VF_ID		= -40,
	VIRTCHNL_STATUS_ERR_ADMIN_QUEUE_ERROR		= -53,
	VIRTCHNL_STATUS_ERR_NOT_SUPPORTED		= -64,
};


#define VIRTCHNL_ERR_PARAM VIRTCHNL_STATUS_ERR_PARAM
#define VIRTCHNL_STATUS_NOT_SUPPORTED VIRTCHNL_STATUS_ERR_NOT_SUPPORTED

#define VIRTCHNL_LINK_SPEED_2_5GB_SHIFT		0x0
#define VIRTCHNL_LINK_SPEED_100MB_SHIFT		0x1
#define VIRTCHNL_LINK_SPEED_1000MB_SHIFT	0x2
#define VIRTCHNL_LINK_SPEED_10GB_SHIFT		0x3
#define VIRTCHNL_LINK_SPEED_40GB_SHIFT		0x4
#define VIRTCHNL_LINK_SPEED_20GB_SHIFT		0x5
#define VIRTCHNL_LINK_SPEED_25GB_SHIFT		0x6
#define VIRTCHNL_LINK_SPEED_5GB_SHIFT		0x7

enum virtchnl_link_speed {
	VIRTCHNL_LINK_SPEED_UNKNOWN	= 0,
	VIRTCHNL_LINK_SPEED_100MB	= BIT(VIRTCHNL_LINK_SPEED_100MB_SHIFT),
	VIRTCHNL_LINK_SPEED_1GB		= BIT(VIRTCHNL_LINK_SPEED_1000MB_SHIFT),
	VIRTCHNL_LINK_SPEED_10GB	= BIT(VIRTCHNL_LINK_SPEED_10GB_SHIFT),
	VIRTCHNL_LINK_SPEED_40GB	= BIT(VIRTCHNL_LINK_SPEED_40GB_SHIFT),
	VIRTCHNL_LINK_SPEED_20GB	= BIT(VIRTCHNL_LINK_SPEED_20GB_SHIFT),
	VIRTCHNL_LINK_SPEED_25GB	= BIT(VIRTCHNL_LINK_SPEED_25GB_SHIFT),
	VIRTCHNL_LINK_SPEED_2_5GB	= BIT(VIRTCHNL_LINK_SPEED_2_5GB_SHIFT),
	VIRTCHNL_LINK_SPEED_5GB		= BIT(VIRTCHNL_LINK_SPEED_5GB_SHIFT),
};



enum virtchnl_rx_hsplit {
	VIRTCHNL_RX_HSPLIT_NO_SPLIT      = 0,
	VIRTCHNL_RX_HSPLIT_SPLIT_L2      = 1,
	VIRTCHNL_RX_HSPLIT_SPLIT_IP      = 2,
	VIRTCHNL_RX_HSPLIT_SPLIT_TCP_UDP = 4,
	VIRTCHNL_RX_HSPLIT_SPLIT_SCTP    = 8,
};




enum virtchnl_ops {

	VIRTCHNL_OP_UNKNOWN = 0,
	VIRTCHNL_OP_VERSION = 1, 
	VIRTCHNL_OP_RESET_VF = 2,
	VIRTCHNL_OP_GET_VF_RESOURCES = 3,
	VIRTCHNL_OP_CONFIG_TX_QUEUE = 4,
	VIRTCHNL_OP_CONFIG_RX_QUEUE = 5,
	VIRTCHNL_OP_CONFIG_VSI_QUEUES = 6,
	VIRTCHNL_OP_CONFIG_IRQ_MAP = 7,
	VIRTCHNL_OP_ENABLE_QUEUES = 8,
	VIRTCHNL_OP_DISABLE_QUEUES = 9,
	VIRTCHNL_OP_ADD_ETH_ADDR = 10,
	VIRTCHNL_OP_DEL_ETH_ADDR = 11,
	VIRTCHNL_OP_ADD_VLAN = 12,
	VIRTCHNL_OP_DEL_VLAN = 13,
	VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE = 14,
	VIRTCHNL_OP_GET_STATS = 15,
	VIRTCHNL_OP_RSVD = 16,
	VIRTCHNL_OP_EVENT = 17, 
	
	VIRTCHNL_OP_IWARP = 20, 
	VIRTCHNL_OP_RDMA = VIRTCHNL_OP_IWARP,
	VIRTCHNL_OP_CONFIG_IWARP_IRQ_MAP = 21, 
	VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP = VIRTCHNL_OP_CONFIG_IWARP_IRQ_MAP,
	VIRTCHNL_OP_RELEASE_IWARP_IRQ_MAP = 22, 
	VIRTCHNL_OP_RELEASE_RDMA_IRQ_MAP = VIRTCHNL_OP_RELEASE_IWARP_IRQ_MAP,
	VIRTCHNL_OP_CONFIG_RSS_KEY = 23,
	VIRTCHNL_OP_CONFIG_RSS_LUT = 24,
	VIRTCHNL_OP_GET_RSS_HENA_CAPS = 25,
	VIRTCHNL_OP_SET_RSS_HENA = 26,
	VIRTCHNL_OP_ENABLE_VLAN_STRIPPING = 27,
	VIRTCHNL_OP_DISABLE_VLAN_STRIPPING = 28,
	VIRTCHNL_OP_REQUEST_QUEUES = 29,
	VIRTCHNL_OP_ENABLE_CHANNELS = 30,
	VIRTCHNL_OP_DISABLE_CHANNELS = 31,
	VIRTCHNL_OP_ADD_CLOUD_FILTER = 32,
	VIRTCHNL_OP_DEL_CLOUD_FILTER = 33,
	
	VIRTCHNL_OP_GET_SUPPORTED_RXDIDS = 44,
	VIRTCHNL_OP_ADD_RSS_CFG = 45,
	VIRTCHNL_OP_DEL_RSS_CFG = 46,
	VIRTCHNL_OP_ADD_FDIR_FILTER = 47,
	VIRTCHNL_OP_DEL_FDIR_FILTER = 48,
	VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS = 51,
	VIRTCHNL_OP_ADD_VLAN_V2 = 52,
	VIRTCHNL_OP_DEL_VLAN_V2 = 53,
	VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2 = 54,
	VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2 = 55,
	VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2 = 56,
	VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2 = 57,
	VIRTCHNL_OP_MAX,
};


#define VIRTCHNL_CHECK_STRUCT_LEN(n, X) enum virtchnl_static_assert_enum_##X \
	{ virtchnl_static_assert_##X = (n)/((sizeof(struct X) == (n)) ? 1 : 0) }
#define VIRTCHNL_CHECK_UNION_LEN(n, X) enum virtchnl_static_asset_enum_##X \
	{ virtchnl_static_assert_##X = (n)/((sizeof(union X) == (n)) ? 1 : 0) }




#define VIRTCHNL_VERSION_MAJOR		1
#define VIRTCHNL_VERSION_MINOR		1
#define VIRTCHNL_VERSION_MINOR_NO_VF_CAPS	0

struct virtchnl_version_info {
	u32 major;
	u32 minor;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_version_info);

#define VF_IS_V10(_v) (((_v)->major == 1) && ((_v)->minor == 0))
#define VF_IS_V11(_ver) (((_ver)->major == 1) && ((_ver)->minor == 1))




enum virtchnl_vsi_type {
	VIRTCHNL_VSI_TYPE_INVALID = 0,
	VIRTCHNL_VSI_SRIOV = 6,
};



struct virtchnl_vsi_resource {
	u16 vsi_id;
	u16 num_queue_pairs;

	
	s32 vsi_type;
	u16 qset_handle;
	u8 default_mac_addr[ETH_ALEN];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_vsi_resource);


#define VIRTCHNL_VF_OFFLOAD_L2			BIT(0)
#define VIRTCHNL_VF_OFFLOAD_RDMA		BIT(1)
#define VIRTCHNL_VF_CAP_RDMA			VIRTCHNL_VF_OFFLOAD_RDMA
#define VIRTCHNL_VF_OFFLOAD_RSS_AQ		BIT(3)
#define VIRTCHNL_VF_OFFLOAD_RSS_REG		BIT(4)
#define VIRTCHNL_VF_OFFLOAD_WB_ON_ITR		BIT(5)
#define VIRTCHNL_VF_OFFLOAD_REQ_QUEUES		BIT(6)

#define VIRTCHNL_VF_CAP_ADV_LINK_SPEED		BIT(7)
#define VIRTCHNL_VF_OFFLOAD_VLAN_V2		BIT(15)
#define VIRTCHNL_VF_OFFLOAD_VLAN		BIT(16)
#define VIRTCHNL_VF_OFFLOAD_RX_POLLING		BIT(17)
#define VIRTCHNL_VF_OFFLOAD_RSS_PCTYPE_V2	BIT(18)
#define VIRTCHNL_VF_OFFLOAD_RSS_PF		BIT(19)
#define VIRTCHNL_VF_OFFLOAD_ENCAP		BIT(20)
#define VIRTCHNL_VF_OFFLOAD_ENCAP_CSUM		BIT(21)
#define VIRTCHNL_VF_OFFLOAD_RX_ENCAP_CSUM	BIT(22)
#define VIRTCHNL_VF_OFFLOAD_ADQ			BIT(23)
#define VIRTCHNL_VF_OFFLOAD_USO			BIT(25)
#define VIRTCHNL_VF_OFFLOAD_RX_FLEX_DESC	BIT(26)
#define VIRTCHNL_VF_OFFLOAD_ADV_RSS_PF		BIT(27)
#define VIRTCHNL_VF_OFFLOAD_FDIR_PF		BIT(28)

#define VF_BASE_MODE_OFFLOADS (VIRTCHNL_VF_OFFLOAD_L2 | \
			       VIRTCHNL_VF_OFFLOAD_VLAN | \
			       VIRTCHNL_VF_OFFLOAD_RSS_PF)

struct virtchnl_vf_resource {
	u16 num_vsis;
	u16 num_queue_pairs;
	u16 max_vectors;
	u16 max_mtu;

	u32 vf_cap_flags;
	u32 rss_key_size;
	u32 rss_lut_size;

	struct virtchnl_vsi_resource vsi_res[];
};

VIRTCHNL_CHECK_STRUCT_LEN(20, virtchnl_vf_resource);
#define virtchnl_vf_resource_LEGACY_SIZEOF	36




struct virtchnl_txq_info {
	u16 vsi_id;
	u16 queue_id;
	u16 ring_len;		
	u16 headwb_enabled; 
	u64 dma_ring_addr;
	u64 dma_headwb_addr; 
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_txq_info);




struct virtchnl_rxq_info {
	u16 vsi_id;
	u16 queue_id;
	u32 ring_len;		
	u16 hdr_size;
	u16 splithdr_enabled; 
	u32 databuffer_size;
	u32 max_pkt_size;
	u8 pad0;
	u8 rxdid;
	u8 pad1[2];
	u64 dma_ring_addr;

	
	s32 rx_split_pos;
	u32 pad2;
};

VIRTCHNL_CHECK_STRUCT_LEN(40, virtchnl_rxq_info);


struct virtchnl_queue_pair_info {
	
	struct virtchnl_txq_info txq;
	struct virtchnl_rxq_info rxq;
};

VIRTCHNL_CHECK_STRUCT_LEN(64, virtchnl_queue_pair_info);

struct virtchnl_vsi_queue_config_info {
	u16 vsi_id;
	u16 num_queue_pairs;
	u32 pad;
	struct virtchnl_queue_pair_info qpair[];
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_vsi_queue_config_info);
#define virtchnl_vsi_queue_config_info_LEGACY_SIZEOF	72




struct virtchnl_vf_res_request {
	u16 num_queue_pairs;
};


struct virtchnl_vector_map {
	u16 vsi_id;
	u16 vector_id;
	u16 rxq_map;
	u16 txq_map;
	u16 rxitr_idx;
	u16 txitr_idx;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_vector_map);

struct virtchnl_irq_map_info {
	u16 num_vectors;
	struct virtchnl_vector_map vecmap[];
};

VIRTCHNL_CHECK_STRUCT_LEN(2, virtchnl_irq_map_info);
#define virtchnl_irq_map_info_LEGACY_SIZEOF	14


struct virtchnl_queue_select {
	u16 vsi_id;
	u16 pad;
	u32 rx_queues;
	u32 tx_queues;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_queue_select);










struct virtchnl_ether_addr {
	u8 addr[ETH_ALEN];
	u8 type;
#define VIRTCHNL_ETHER_ADDR_LEGACY	0
#define VIRTCHNL_ETHER_ADDR_PRIMARY	1
#define VIRTCHNL_ETHER_ADDR_EXTRA	2
#define VIRTCHNL_ETHER_ADDR_TYPE_MASK	3 
	u8 pad;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_ether_addr);

struct virtchnl_ether_addr_list {
	u16 vsi_id;
	u16 num_elements;
	struct virtchnl_ether_addr list[];
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_ether_addr_list);
#define virtchnl_ether_addr_list_LEGACY_SIZEOF	12





struct virtchnl_vlan_filter_list {
	u16 vsi_id;
	u16 num_elements;
	u16 vlan_id[];
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_vlan_filter_list);
#define virtchnl_vlan_filter_list_LEGACY_SIZEOF	6


enum virtchnl_vlan_support {
	VIRTCHNL_VLAN_UNSUPPORTED =		0,
	VIRTCHNL_VLAN_ETHERTYPE_8100 =		BIT(0),
	VIRTCHNL_VLAN_ETHERTYPE_88A8 =		BIT(1),
	VIRTCHNL_VLAN_ETHERTYPE_9100 =		BIT(2),
	VIRTCHNL_VLAN_TAG_LOCATION_L2TAG1 =	BIT(8),
	VIRTCHNL_VLAN_TAG_LOCATION_L2TAG2 =	BIT(9),
	VIRTCHNL_VLAN_TAG_LOCATION_L2TAG2_2 =	BIT(10),
	VIRTCHNL_VLAN_PRIO =			BIT(24),
	VIRTCHNL_VLAN_FILTER_MASK =		BIT(28),
	VIRTCHNL_VLAN_ETHERTYPE_AND =		BIT(29),
	VIRTCHNL_VLAN_ETHERTYPE_XOR =		BIT(30),
	VIRTCHNL_VLAN_TOGGLE =			BIT(31),
};


struct virtchnl_vlan_supported_caps {
	u32 outer;
	u32 inner;
};


struct virtchnl_vlan_filtering_caps {
	struct virtchnl_vlan_supported_caps filtering_support;
	u32 ethertype_init;
	u16 max_filters;
	u8 pad[2];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_vlan_filtering_caps);


enum virtchnl_vlan_ethertype_match {
	VIRTCHNL_ETHERTYPE_STRIPPING_MATCHES_INSERTION = 0,
	VIRTCHNL_ETHERTYPE_MATCH_NOT_REQUIRED = 1,
};


struct virtchnl_vlan_offload_caps {
	struct virtchnl_vlan_supported_caps stripping_support;
	struct virtchnl_vlan_supported_caps insertion_support;
	u32 ethertype_init;
	u8 ethertype_match;
	u8 pad[3];
};

VIRTCHNL_CHECK_STRUCT_LEN(24, virtchnl_vlan_offload_caps);


struct virtchnl_vlan_caps {
	struct virtchnl_vlan_filtering_caps filtering;
	struct virtchnl_vlan_offload_caps offloads;
};

VIRTCHNL_CHECK_STRUCT_LEN(40, virtchnl_vlan_caps);

struct virtchnl_vlan {
	u16 tci;	
	u16 tci_mask;	
	u16 tpid;	
	u8 pad[2];
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_vlan);

struct virtchnl_vlan_filter {
	struct virtchnl_vlan inner;
	struct virtchnl_vlan outer;
	u8 pad[16];
};

VIRTCHNL_CHECK_STRUCT_LEN(32, virtchnl_vlan_filter);


struct virtchnl_vlan_filter_list_v2 {
	u16 vport_id;
	u16 num_elements;
	u8 pad[4];
	struct virtchnl_vlan_filter filters[];
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_vlan_filter_list_v2);
#define virtchnl_vlan_filter_list_v2_LEGACY_SIZEOF	40


struct virtchnl_vlan_setting {
	u32 outer_ethertype_setting;
	u32 inner_ethertype_setting;
	u16 vport_id;
	u8 pad[6];
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_vlan_setting);


struct virtchnl_promisc_info {
	u16 vsi_id;
	u16 flags;
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_promisc_info);

#define FLAG_VF_UNICAST_PROMISC	0x00000001
#define FLAG_VF_MULTICAST_PROMISC	0x00000002




struct virtchnl_rss_key {
	u16 vsi_id;
	u16 key_len;
	u8 key[];          
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_rss_key);
#define virtchnl_rss_key_LEGACY_SIZEOF	6

struct virtchnl_rss_lut {
	u16 vsi_id;
	u16 lut_entries;
	u8 lut[];         
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_rss_lut);
#define virtchnl_rss_lut_LEGACY_SIZEOF	6


struct virtchnl_rss_hena {
	u64 hena;
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_rss_hena);


struct virtchnl_channel_info {
	u16 count; 
	u16 offset; 
	u32 pad;
	u64 max_tx_rate;
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_channel_info);

struct virtchnl_tc_info {
	u32	num_tc;
	u32	pad;
	struct virtchnl_channel_info list[];
};

VIRTCHNL_CHECK_STRUCT_LEN(8, virtchnl_tc_info);
#define virtchnl_tc_info_LEGACY_SIZEOF	24



struct virtchnl_l4_spec {
	u8	src_mac[ETH_ALEN];
	u8	dst_mac[ETH_ALEN];
	__be16	vlan_id;
	__be16	pad; 
	__be32	src_ip[4];
	__be32	dst_ip[4];
	__be16	src_port;
	__be16	dst_port;
};

VIRTCHNL_CHECK_STRUCT_LEN(52, virtchnl_l4_spec);

union virtchnl_flow_spec {
	struct	virtchnl_l4_spec tcp_spec;
	u8	buffer[128]; 
};

VIRTCHNL_CHECK_UNION_LEN(128, virtchnl_flow_spec);

enum virtchnl_action {
	
	VIRTCHNL_ACTION_DROP = 0,
	VIRTCHNL_ACTION_TC_REDIRECT,
	VIRTCHNL_ACTION_PASSTHRU,
	VIRTCHNL_ACTION_QUEUE,
	VIRTCHNL_ACTION_Q_REGION,
	VIRTCHNL_ACTION_MARK,
	VIRTCHNL_ACTION_COUNT,
};

enum virtchnl_flow_type {
	
	VIRTCHNL_TCP_V4_FLOW = 0,
	VIRTCHNL_TCP_V6_FLOW,
};

struct virtchnl_filter {
	union	virtchnl_flow_spec data;
	union	virtchnl_flow_spec mask;

	
	s32	flow_type;

	
	s32	action;
	u32	action_meta;
	u8	field_flags;
	u8	pad[3];
};

VIRTCHNL_CHECK_STRUCT_LEN(272, virtchnl_filter);

struct virtchnl_supported_rxdids {
	u64 supported_rxdids;
};


enum virtchnl_event_codes {
	VIRTCHNL_EVENT_UNKNOWN = 0,
	VIRTCHNL_EVENT_LINK_CHANGE,
	VIRTCHNL_EVENT_RESET_IMPENDING,
	VIRTCHNL_EVENT_PF_DRIVER_CLOSE,
};

#define PF_EVENT_SEVERITY_INFO		0
#define PF_EVENT_SEVERITY_CERTAIN_DOOM	255

struct virtchnl_pf_event {
	
	s32 event;
	union {
		
		struct {
			enum virtchnl_link_speed link_speed;
			bool link_status;
			u8 pad[3];
		} link_event;
		struct {
			
			u32 link_speed;
			u8 link_status;
			u8 pad[3];
		} link_event_adv;
	} event_data;

	s32 severity;
};

VIRTCHNL_CHECK_STRUCT_LEN(16, virtchnl_pf_event);


#define VIRTCHNL_RDMA_INVALID_QUEUE_IDX	0xFFFF


struct virtchnl_rdma_qv_info {
	u32 v_idx; 
	u16 ceq_idx; 
	u16 aeq_idx; 
	u8 itr_idx;
	u8 pad[3];
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_rdma_qv_info);

struct virtchnl_rdma_qvlist_info {
	u32 num_vectors;
	struct virtchnl_rdma_qv_info qv_info[];
};

VIRTCHNL_CHECK_STRUCT_LEN(4, virtchnl_rdma_qvlist_info);
#define virtchnl_rdma_qvlist_info_LEGACY_SIZEOF	16


enum virtchnl_vfr_states {
	VIRTCHNL_VFR_INPROGRESS = 0,
	VIRTCHNL_VFR_COMPLETED,
	VIRTCHNL_VFR_VFACTIVE,
};


enum virtchnl_rss_algorithm {
	VIRTCHNL_RSS_ALG_TOEPLITZ_ASYMMETRIC	= 0,
	VIRTCHNL_RSS_ALG_R_ASYMMETRIC		= 1,
	VIRTCHNL_RSS_ALG_TOEPLITZ_SYMMETRIC	= 2,
	VIRTCHNL_RSS_ALG_XOR_SYMMETRIC		= 3,
};

#define VIRTCHNL_MAX_NUM_PROTO_HDRS	32
#define PROTO_HDR_SHIFT			5
#define PROTO_HDR_FIELD_START(proto_hdr_type) ((proto_hdr_type) << PROTO_HDR_SHIFT)
#define PROTO_HDR_FIELD_MASK ((1UL << PROTO_HDR_SHIFT) - 1)


#define VIRTCHNL_ADD_PROTO_HDR_FIELD(hdr, field) \
	((hdr)->field_selector |= BIT((field) & PROTO_HDR_FIELD_MASK))
#define VIRTCHNL_DEL_PROTO_HDR_FIELD(hdr, field) \
	((hdr)->field_selector &= ~BIT((field) & PROTO_HDR_FIELD_MASK))
#define VIRTCHNL_TEST_PROTO_HDR_FIELD(hdr, val) \
	((hdr)->field_selector & BIT((val) & PROTO_HDR_FIELD_MASK))
#define VIRTCHNL_GET_PROTO_HDR_FIELD(hdr)	((hdr)->field_selector)

#define VIRTCHNL_ADD_PROTO_HDR_FIELD_BIT(hdr, hdr_type, field) \
	(VIRTCHNL_ADD_PROTO_HDR_FIELD(hdr, \
		VIRTCHNL_PROTO_HDR_ ## hdr_type ## _ ## field))
#define VIRTCHNL_DEL_PROTO_HDR_FIELD_BIT(hdr, hdr_type, field) \
	(VIRTCHNL_DEL_PROTO_HDR_FIELD(hdr, \
		VIRTCHNL_PROTO_HDR_ ## hdr_type ## _ ## field))

#define VIRTCHNL_SET_PROTO_HDR_TYPE(hdr, hdr_type) \
	((hdr)->type = VIRTCHNL_PROTO_HDR_ ## hdr_type)
#define VIRTCHNL_GET_PROTO_HDR_TYPE(hdr) \
	(((hdr)->type) >> PROTO_HDR_SHIFT)
#define VIRTCHNL_TEST_PROTO_HDR_TYPE(hdr, val) \
	((hdr)->type == ((s32)((val) >> PROTO_HDR_SHIFT)))
#define VIRTCHNL_TEST_PROTO_HDR(hdr, val) \
	(VIRTCHNL_TEST_PROTO_HDR_TYPE((hdr), (val)) && \
	 VIRTCHNL_TEST_PROTO_HDR_FIELD((hdr), (val)))


enum virtchnl_proto_hdr_type {
	VIRTCHNL_PROTO_HDR_NONE,
	VIRTCHNL_PROTO_HDR_ETH,
	VIRTCHNL_PROTO_HDR_S_VLAN,
	VIRTCHNL_PROTO_HDR_C_VLAN,
	VIRTCHNL_PROTO_HDR_IPV4,
	VIRTCHNL_PROTO_HDR_IPV6,
	VIRTCHNL_PROTO_HDR_TCP,
	VIRTCHNL_PROTO_HDR_UDP,
	VIRTCHNL_PROTO_HDR_SCTP,
	VIRTCHNL_PROTO_HDR_GTPU_IP,
	VIRTCHNL_PROTO_HDR_GTPU_EH,
	VIRTCHNL_PROTO_HDR_GTPU_EH_PDU_DWN,
	VIRTCHNL_PROTO_HDR_GTPU_EH_PDU_UP,
	VIRTCHNL_PROTO_HDR_PPPOE,
	VIRTCHNL_PROTO_HDR_L2TPV3,
	VIRTCHNL_PROTO_HDR_ESP,
	VIRTCHNL_PROTO_HDR_AH,
	VIRTCHNL_PROTO_HDR_PFCP,
};


enum virtchnl_proto_hdr_field {
	
	VIRTCHNL_PROTO_HDR_ETH_SRC =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_ETH),
	VIRTCHNL_PROTO_HDR_ETH_DST,
	VIRTCHNL_PROTO_HDR_ETH_ETHERTYPE,
	
	VIRTCHNL_PROTO_HDR_S_VLAN_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_S_VLAN),
	
	VIRTCHNL_PROTO_HDR_C_VLAN_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_C_VLAN),
	
	VIRTCHNL_PROTO_HDR_IPV4_SRC =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_IPV4),
	VIRTCHNL_PROTO_HDR_IPV4_DST,
	VIRTCHNL_PROTO_HDR_IPV4_DSCP,
	VIRTCHNL_PROTO_HDR_IPV4_TTL,
	VIRTCHNL_PROTO_HDR_IPV4_PROT,
	
	VIRTCHNL_PROTO_HDR_IPV6_SRC =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_IPV6),
	VIRTCHNL_PROTO_HDR_IPV6_DST,
	VIRTCHNL_PROTO_HDR_IPV6_TC,
	VIRTCHNL_PROTO_HDR_IPV6_HOP_LIMIT,
	VIRTCHNL_PROTO_HDR_IPV6_PROT,
	
	VIRTCHNL_PROTO_HDR_TCP_SRC_PORT =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_TCP),
	VIRTCHNL_PROTO_HDR_TCP_DST_PORT,
	
	VIRTCHNL_PROTO_HDR_UDP_SRC_PORT =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_UDP),
	VIRTCHNL_PROTO_HDR_UDP_DST_PORT,
	
	VIRTCHNL_PROTO_HDR_SCTP_SRC_PORT =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_SCTP),
	VIRTCHNL_PROTO_HDR_SCTP_DST_PORT,
	
	VIRTCHNL_PROTO_HDR_GTPU_IP_TEID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPU_IP),
	
	VIRTCHNL_PROTO_HDR_GTPU_EH_PDU =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_GTPU_EH),
	VIRTCHNL_PROTO_HDR_GTPU_EH_QFI,
	
	VIRTCHNL_PROTO_HDR_PPPOE_SESS_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_PPPOE),
	
	VIRTCHNL_PROTO_HDR_L2TPV3_SESS_ID =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_L2TPV3),
	
	VIRTCHNL_PROTO_HDR_ESP_SPI =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_ESP),
	
	VIRTCHNL_PROTO_HDR_AH_SPI =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_AH),
	
	VIRTCHNL_PROTO_HDR_PFCP_S_FIELD =
		PROTO_HDR_FIELD_START(VIRTCHNL_PROTO_HDR_PFCP),
	VIRTCHNL_PROTO_HDR_PFCP_SEID,
};

struct virtchnl_proto_hdr {
	
	s32 type;
	u32 field_selector; 
	u8 buffer[64];
	
};

VIRTCHNL_CHECK_STRUCT_LEN(72, virtchnl_proto_hdr);

struct virtchnl_proto_hdrs {
	u8 tunnel_level;
	u8 pad[3];
	
	int count; 
	struct virtchnl_proto_hdr proto_hdr[VIRTCHNL_MAX_NUM_PROTO_HDRS];
};

VIRTCHNL_CHECK_STRUCT_LEN(2312, virtchnl_proto_hdrs);

struct virtchnl_rss_cfg {
	struct virtchnl_proto_hdrs proto_hdrs;	   

	
	s32 rss_algorithm;
	u8 reserved[128];                          
};

VIRTCHNL_CHECK_STRUCT_LEN(2444, virtchnl_rss_cfg);


struct virtchnl_filter_action {
	
	s32 type;
	union {
		
		struct {
			u16 index;
			u8 region;
		} queue;
		
		struct {
			
			u8 shared;
			u32 id; 
		} count;
		
		u32 mark_id;
		u8 reserve[32];
	} act_conf;
};

VIRTCHNL_CHECK_STRUCT_LEN(36, virtchnl_filter_action);

#define VIRTCHNL_MAX_NUM_ACTIONS  8

struct virtchnl_filter_action_set {
	
	int count;
	struct virtchnl_filter_action actions[VIRTCHNL_MAX_NUM_ACTIONS];
};

VIRTCHNL_CHECK_STRUCT_LEN(292, virtchnl_filter_action_set);


struct virtchnl_fdir_rule {
	struct virtchnl_proto_hdrs proto_hdrs;
	struct virtchnl_filter_action_set action_set;
};

VIRTCHNL_CHECK_STRUCT_LEN(2604, virtchnl_fdir_rule);


enum virtchnl_fdir_prgm_status {
	VIRTCHNL_FDIR_SUCCESS = 0,
	VIRTCHNL_FDIR_FAILURE_RULE_NORESOURCE,
	VIRTCHNL_FDIR_FAILURE_RULE_EXIST,
	VIRTCHNL_FDIR_FAILURE_RULE_CONFLICT,
	VIRTCHNL_FDIR_FAILURE_RULE_NONEXIST,
	VIRTCHNL_FDIR_FAILURE_RULE_INVALID,
	VIRTCHNL_FDIR_FAILURE_RULE_TIMEOUT,
	VIRTCHNL_FDIR_FAILURE_QUERY_INVALID,
};


struct virtchnl_fdir_add {
	u16 vsi_id;  
	
	u16 validate_only; 
	u32 flow_id;       
	struct virtchnl_fdir_rule rule_cfg; 

	
	s32 status;
};

VIRTCHNL_CHECK_STRUCT_LEN(2616, virtchnl_fdir_add);


struct virtchnl_fdir_del {
	u16 vsi_id;  
	u16 pad;
	u32 flow_id; 

	
	s32 status;
};

VIRTCHNL_CHECK_STRUCT_LEN(12, virtchnl_fdir_del);

#define __vss_byone(p, member, count, old)				      \
	(struct_size(p, member, count) + (old - 1 - struct_size(p, member, 0)))

#define __vss_byelem(p, member, count, old)				      \
	(struct_size(p, member, count - 1) + (old - struct_size(p, member, 0)))

#define __vss_full(p, member, count, old)				      \
	(struct_size(p, member, count) + (old - struct_size(p, member, 0)))

#define __vss(type, func, p, member, count)		\
	struct type: func(p, member, count, type##_LEGACY_SIZEOF)

#define virtchnl_struct_size(p, m, c)					      \
	_Generic(*p,							      \
		 __vss(virtchnl_vf_resource, __vss_full, p, m, c),	      \
		 __vss(virtchnl_vsi_queue_config_info, __vss_full, p, m, c),  \
		 __vss(virtchnl_irq_map_info, __vss_full, p, m, c),	      \
		 __vss(virtchnl_ether_addr_list, __vss_full, p, m, c),	      \
		 __vss(virtchnl_vlan_filter_list, __vss_full, p, m, c),	      \
		 __vss(virtchnl_vlan_filter_list_v2, __vss_byelem, p, m, c),  \
		 __vss(virtchnl_tc_info, __vss_byelem, p, m, c),	      \
		 __vss(virtchnl_rdma_qvlist_info, __vss_byelem, p, m, c),     \
		 __vss(virtchnl_rss_key, __vss_byone, p, m, c),		      \
		 __vss(virtchnl_rss_lut, __vss_byone, p, m, c))


static inline int
virtchnl_vc_validate_vf_msg(struct virtchnl_version_info *ver, u32 v_opcode,
			    u8 *msg, u16 msglen)
{
	bool err_msg_format = false;
	u32 valid_len = 0;

	
	switch (v_opcode) {
	case VIRTCHNL_OP_VERSION:
		valid_len = sizeof(struct virtchnl_version_info);
		break;
	case VIRTCHNL_OP_RESET_VF:
		break;
	case VIRTCHNL_OP_GET_VF_RESOURCES:
		if (VF_IS_V11(ver))
			valid_len = sizeof(u32);
		break;
	case VIRTCHNL_OP_CONFIG_TX_QUEUE:
		valid_len = sizeof(struct virtchnl_txq_info);
		break;
	case VIRTCHNL_OP_CONFIG_RX_QUEUE:
		valid_len = sizeof(struct virtchnl_rxq_info);
		break;
	case VIRTCHNL_OP_CONFIG_VSI_QUEUES:
		valid_len = virtchnl_vsi_queue_config_info_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_vsi_queue_config_info *vqc =
			    (struct virtchnl_vsi_queue_config_info *)msg;
			valid_len = virtchnl_struct_size(vqc, qpair,
							 vqc->num_queue_pairs);
			if (vqc->num_queue_pairs == 0)
				err_msg_format = true;
		}
		break;
	case VIRTCHNL_OP_CONFIG_IRQ_MAP:
		valid_len = virtchnl_irq_map_info_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_irq_map_info *vimi =
			    (struct virtchnl_irq_map_info *)msg;
			valid_len = virtchnl_struct_size(vimi, vecmap,
							 vimi->num_vectors);
			if (vimi->num_vectors == 0)
				err_msg_format = true;
		}
		break;
	case VIRTCHNL_OP_ENABLE_QUEUES:
	case VIRTCHNL_OP_DISABLE_QUEUES:
		valid_len = sizeof(struct virtchnl_queue_select);
		break;
	case VIRTCHNL_OP_ADD_ETH_ADDR:
	case VIRTCHNL_OP_DEL_ETH_ADDR:
		valid_len = virtchnl_ether_addr_list_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_ether_addr_list *veal =
			    (struct virtchnl_ether_addr_list *)msg;
			valid_len = virtchnl_struct_size(veal, list,
							 veal->num_elements);
			if (veal->num_elements == 0)
				err_msg_format = true;
		}
		break;
	case VIRTCHNL_OP_ADD_VLAN:
	case VIRTCHNL_OP_DEL_VLAN:
		valid_len = virtchnl_vlan_filter_list_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_vlan_filter_list *vfl =
			    (struct virtchnl_vlan_filter_list *)msg;
			valid_len = virtchnl_struct_size(vfl, vlan_id,
							 vfl->num_elements);
			if (vfl->num_elements == 0)
				err_msg_format = true;
		}
		break;
	case VIRTCHNL_OP_CONFIG_PROMISCUOUS_MODE:
		valid_len = sizeof(struct virtchnl_promisc_info);
		break;
	case VIRTCHNL_OP_GET_STATS:
		valid_len = sizeof(struct virtchnl_queue_select);
		break;
	case VIRTCHNL_OP_RDMA:
		
		if (msglen)
			valid_len = msglen;
		else
			err_msg_format = true;
		break;
	case VIRTCHNL_OP_RELEASE_RDMA_IRQ_MAP:
		break;
	case VIRTCHNL_OP_CONFIG_RDMA_IRQ_MAP:
		valid_len = virtchnl_rdma_qvlist_info_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_rdma_qvlist_info *qv =
				(struct virtchnl_rdma_qvlist_info *)msg;

			valid_len = virtchnl_struct_size(qv, qv_info,
							 qv->num_vectors);
		}
		break;
	case VIRTCHNL_OP_CONFIG_RSS_KEY:
		valid_len = virtchnl_rss_key_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_rss_key *vrk =
				(struct virtchnl_rss_key *)msg;
			valid_len = virtchnl_struct_size(vrk, key,
							 vrk->key_len);
		}
		break;
	case VIRTCHNL_OP_CONFIG_RSS_LUT:
		valid_len = virtchnl_rss_lut_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_rss_lut *vrl =
				(struct virtchnl_rss_lut *)msg;
			valid_len = virtchnl_struct_size(vrl, lut,
							 vrl->lut_entries);
		}
		break;
	case VIRTCHNL_OP_GET_RSS_HENA_CAPS:
		break;
	case VIRTCHNL_OP_SET_RSS_HENA:
		valid_len = sizeof(struct virtchnl_rss_hena);
		break;
	case VIRTCHNL_OP_ENABLE_VLAN_STRIPPING:
	case VIRTCHNL_OP_DISABLE_VLAN_STRIPPING:
		break;
	case VIRTCHNL_OP_REQUEST_QUEUES:
		valid_len = sizeof(struct virtchnl_vf_res_request);
		break;
	case VIRTCHNL_OP_ENABLE_CHANNELS:
		valid_len = virtchnl_tc_info_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_tc_info *vti =
				(struct virtchnl_tc_info *)msg;
			valid_len = virtchnl_struct_size(vti, list,
							 vti->num_tc);
			if (vti->num_tc == 0)
				err_msg_format = true;
		}
		break;
	case VIRTCHNL_OP_DISABLE_CHANNELS:
		break;
	case VIRTCHNL_OP_ADD_CLOUD_FILTER:
	case VIRTCHNL_OP_DEL_CLOUD_FILTER:
		valid_len = sizeof(struct virtchnl_filter);
		break;
	case VIRTCHNL_OP_GET_SUPPORTED_RXDIDS:
		break;
	case VIRTCHNL_OP_ADD_RSS_CFG:
	case VIRTCHNL_OP_DEL_RSS_CFG:
		valid_len = sizeof(struct virtchnl_rss_cfg);
		break;
	case VIRTCHNL_OP_ADD_FDIR_FILTER:
		valid_len = sizeof(struct virtchnl_fdir_add);
		break;
	case VIRTCHNL_OP_DEL_FDIR_FILTER:
		valid_len = sizeof(struct virtchnl_fdir_del);
		break;
	case VIRTCHNL_OP_GET_OFFLOAD_VLAN_V2_CAPS:
		break;
	case VIRTCHNL_OP_ADD_VLAN_V2:
	case VIRTCHNL_OP_DEL_VLAN_V2:
		valid_len = virtchnl_vlan_filter_list_v2_LEGACY_SIZEOF;
		if (msglen >= valid_len) {
			struct virtchnl_vlan_filter_list_v2 *vfl =
			    (struct virtchnl_vlan_filter_list_v2 *)msg;

			valid_len = virtchnl_struct_size(vfl, filters,
							 vfl->num_elements);

			if (vfl->num_elements == 0) {
				err_msg_format = true;
				break;
			}
		}
		break;
	case VIRTCHNL_OP_ENABLE_VLAN_STRIPPING_V2:
	case VIRTCHNL_OP_DISABLE_VLAN_STRIPPING_V2:
	case VIRTCHNL_OP_ENABLE_VLAN_INSERTION_V2:
	case VIRTCHNL_OP_DISABLE_VLAN_INSERTION_V2:
		valid_len = sizeof(struct virtchnl_vlan_setting);
		break;
	
	case VIRTCHNL_OP_EVENT:
	case VIRTCHNL_OP_UNKNOWN:
	default:
		return VIRTCHNL_STATUS_ERR_PARAM;
	}
	
	if (err_msg_format || valid_len != msglen)
		return VIRTCHNL_STATUS_ERR_OPCODE_MISMATCH;

	return 0;
}
#endif 
