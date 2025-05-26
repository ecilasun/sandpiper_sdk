/* SPDX-License-Identifier: (GPL-2.0 OR MIT)
 * Microsemi Ocelot Switch driver
 * Copyright (c) 2019 Microsemi Corporation
 */

#ifndef _OCELOT_VCAP_H_
#define _OCELOT_VCAP_H_

#include <soc/mscc/ocelot.h>


#define OCELOT_VCAP_ES0_TAG_8021Q_RXVLAN(ocelot, port, upstream) ((upstream) << 16 | (port))
#define OCELOT_VCAP_IS1_TAG_8021Q_TXVLAN(ocelot, port)		(port)
#define OCELOT_VCAP_IS2_TAG_8021Q_TXVLAN(ocelot, port)		(port)
#define OCELOT_VCAP_IS2_MRP_REDIRECT(ocelot, port)		((ocelot)->num_phys_ports + (port))
#define OCELOT_VCAP_IS2_MRP_TRAP(ocelot)			((ocelot)->num_phys_ports * 2)
#define OCELOT_VCAP_IS2_L2_PTP_TRAP(ocelot)			((ocelot)->num_phys_ports * 2 + 1)
#define OCELOT_VCAP_IS2_IPV4_GEN_PTP_TRAP(ocelot)		((ocelot)->num_phys_ports * 2 + 2)
#define OCELOT_VCAP_IS2_IPV4_EV_PTP_TRAP(ocelot)		((ocelot)->num_phys_ports * 2 + 3)
#define OCELOT_VCAP_IS2_IPV6_GEN_PTP_TRAP(ocelot)		((ocelot)->num_phys_ports * 2 + 4)
#define OCELOT_VCAP_IS2_IPV6_EV_PTP_TRAP(ocelot)		((ocelot)->num_phys_ports * 2 + 5)



enum {
	VCAP_ES0,
	VCAP_IS1,
	VCAP_IS2,
	__VCAP_COUNT,
};

#define OCELOT_NUM_VCAP_BLOCKS		__VCAP_COUNT

struct vcap_props {
	u16 tg_width; 
	u16 sw_count; 
	u16 entry_count; 
	u16 entry_words; 
	u16 entry_width; 
	u16 action_count; 
	u16 action_words; 
	u16 action_width; 
	u16 action_type_width; 
	struct {
		u16 width; 
		u16 count; 
	} action_table[2];
	u16 counter_words; 
	u16 counter_width; 

	enum ocelot_target		target;

	const struct vcap_field		*keys;
	const struct vcap_field		*actions;
};


#define VCAP_TG_NONE 0 
#define VCAP_TG_FULL 1 
#define VCAP_TG_HALF 2 
#define VCAP_TG_QUARTER 3 

#define VCAP_CORE_UPDATE_CTRL_UPDATE_CMD(x)      (((x) << 22) & GENMASK(24, 22))
#define VCAP_CORE_UPDATE_CTRL_UPDATE_CMD_M       GENMASK(24, 22)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_CMD_X(x)    (((x) & GENMASK(24, 22)) >> 22)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_ENTRY_DIS   BIT(21)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_ACTION_DIS  BIT(20)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_CNT_DIS     BIT(19)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_ADDR(x)     (((x) << 3) & GENMASK(18, 3))
#define VCAP_CORE_UPDATE_CTRL_UPDATE_ADDR_M      GENMASK(18, 3)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_ADDR_X(x)   (((x) & GENMASK(18, 3)) >> 3)
#define VCAP_CORE_UPDATE_CTRL_UPDATE_SHOT        BIT(2)
#define VCAP_CORE_UPDATE_CTRL_CLEAR_CACHE        BIT(1)
#define VCAP_CORE_UPDATE_CTRL_MV_TRAFFIC_IGN     BIT(0)

#define VCAP_CORE_MV_CFG_MV_NUM_POS(x)           (((x) << 16) & GENMASK(31, 16))
#define VCAP_CORE_MV_CFG_MV_NUM_POS_M            GENMASK(31, 16)
#define VCAP_CORE_MV_CFG_MV_NUM_POS_X(x)         (((x) & GENMASK(31, 16)) >> 16)
#define VCAP_CORE_MV_CFG_MV_SIZE(x)              ((x) & GENMASK(15, 0))
#define VCAP_CORE_MV_CFG_MV_SIZE_M               GENMASK(15, 0)

#define VCAP_CACHE_ENTRY_DAT_RSZ                 0x4

#define VCAP_CACHE_MASK_DAT_RSZ                  0x4

#define VCAP_CACHE_ACTION_DAT_RSZ                0x4

#define VCAP_CACHE_CNT_DAT_RSZ                   0x4

#define VCAP_STICKY_VCAP_ROW_DELETED_STICKY      BIT(0)

#define TCAM_BIST_CTRL_TCAM_BIST                 BIT(1)
#define TCAM_BIST_CTRL_TCAM_INIT                 BIT(0)

#define TCAM_BIST_CFG_TCAM_BIST_SOE_ENA          BIT(8)
#define TCAM_BIST_CFG_TCAM_HCG_DIS               BIT(7)
#define TCAM_BIST_CFG_TCAM_CG_DIS                BIT(6)
#define TCAM_BIST_CFG_TCAM_BIAS(x)               ((x) & GENMASK(5, 0))
#define TCAM_BIST_CFG_TCAM_BIAS_M                GENMASK(5, 0)

#define TCAM_BIST_STAT_BIST_RT_ERR               BIT(15)
#define TCAM_BIST_STAT_BIST_PENC_ERR             BIT(14)
#define TCAM_BIST_STAT_BIST_COMP_ERR             BIT(13)
#define TCAM_BIST_STAT_BIST_ADDR_ERR             BIT(12)
#define TCAM_BIST_STAT_BIST_BL1E_ERR             BIT(11)
#define TCAM_BIST_STAT_BIST_BL1_ERR              BIT(10)
#define TCAM_BIST_STAT_BIST_BL0E_ERR             BIT(9)
#define TCAM_BIST_STAT_BIST_BL0_ERR              BIT(8)
#define TCAM_BIST_STAT_BIST_PH1_ERR              BIT(7)
#define TCAM_BIST_STAT_BIST_PH0_ERR              BIT(6)
#define TCAM_BIST_STAT_BIST_PV1_ERR              BIT(5)
#define TCAM_BIST_STAT_BIST_PV0_ERR              BIT(4)
#define TCAM_BIST_STAT_BIST_RUN                  BIT(3)
#define TCAM_BIST_STAT_BIST_ERR                  BIT(2)
#define TCAM_BIST_STAT_BIST_BUSY                 BIT(1)
#define TCAM_BIST_STAT_TCAM_RDY                  BIT(0)




#define IS2_TYPE_ETYPE 0
#define IS2_TYPE_LLC 1
#define IS2_TYPE_SNAP 2
#define IS2_TYPE_ARP 3
#define IS2_TYPE_IP_UDP_TCP 4
#define IS2_TYPE_IP_OTHER 5
#define IS2_TYPE_IPV6 6
#define IS2_TYPE_OAM 7
#define IS2_TYPE_SMAC_SIP6 8
#define IS2_TYPE_ANY 100 


#define IS2_TYPE_MASK_IP_ANY 0xe

enum {
	IS2_ACTION_TYPE_NORMAL,
	IS2_ACTION_TYPE_SMAC_SIP,
	IS2_ACTION_TYPE_MAX,
};


#define IS2_ACT_MASK_MODE_NONE 0
#define IS2_ACT_MASK_MODE_FILTER 1
#define IS2_ACT_MASK_MODE_POLICY 2
#define IS2_ACT_MASK_MODE_REDIR 3


#define IS2_ACT_REW_OP_NONE 0
#define IS2_ACT_REW_OP_PTP_ONE 2
#define IS2_ACT_REW_OP_PTP_TWO 3
#define IS2_ACT_REW_OP_SPECIAL 8
#define IS2_ACT_REW_OP_PTP_ORG 9
#define IS2_ACT_REW_OP_PTP_ONE_SUB_DELAY_1 (IS2_ACT_REW_OP_PTP_ONE | (1 << 3))
#define IS2_ACT_REW_OP_PTP_ONE_SUB_DELAY_2 (IS2_ACT_REW_OP_PTP_ONE | (2 << 3))
#define IS2_ACT_REW_OP_PTP_ONE_ADD_DELAY (IS2_ACT_REW_OP_PTP_ONE | (1 << 5))
#define IS2_ACT_REW_OP_PTP_ONE_ADD_SUB BIT(7)

#define VCAP_PORT_WIDTH 4


#define IS2_QKO_IGR_PORT 0
#define IS2_QKL_IGR_PORT VCAP_PORT_WIDTH
#define IS2_QKO_L2_SMAC (IS2_QKO_IGR_PORT + IS2_QKL_IGR_PORT)
#define IS2_QKL_L2_SMAC 48
#define IS2_QKO_L3_IP4_SIP (IS2_QKO_L2_SMAC + IS2_QKL_L2_SMAC)
#define IS2_QKL_L3_IP4_SIP 32

enum vcap_is2_half_key_field {
	
	VCAP_IS2_TYPE,
	VCAP_IS2_HK_FIRST,
	VCAP_IS2_HK_PAG,
	VCAP_IS2_HK_RSV1,
	VCAP_IS2_HK_IGR_PORT_MASK,
	VCAP_IS2_HK_RSV2,
	VCAP_IS2_HK_HOST_MATCH,
	VCAP_IS2_HK_L2_MC,
	VCAP_IS2_HK_L2_BC,
	VCAP_IS2_HK_VLAN_TAGGED,
	VCAP_IS2_HK_VID,
	VCAP_IS2_HK_DEI,
	VCAP_IS2_HK_PCP,
	
	VCAP_IS2_HK_L2_DMAC,
	VCAP_IS2_HK_L2_SMAC,
	
	VCAP_IS2_HK_MAC_ETYPE_ETYPE,
	VCAP_IS2_HK_MAC_ETYPE_L2_PAYLOAD0,
	VCAP_IS2_HK_MAC_ETYPE_L2_PAYLOAD1,
	VCAP_IS2_HK_MAC_ETYPE_L2_PAYLOAD2,
	
	VCAP_IS2_HK_MAC_LLC_DMAC,
	VCAP_IS2_HK_MAC_LLC_SMAC,
	VCAP_IS2_HK_MAC_LLC_L2_LLC,
	
	VCAP_IS2_HK_MAC_SNAP_SMAC,
	VCAP_IS2_HK_MAC_SNAP_DMAC,
	VCAP_IS2_HK_MAC_SNAP_L2_SNAP,
	
	VCAP_IS2_HK_MAC_ARP_SMAC,
	VCAP_IS2_HK_MAC_ARP_ADDR_SPACE_OK,
	VCAP_IS2_HK_MAC_ARP_PROTO_SPACE_OK,
	VCAP_IS2_HK_MAC_ARP_LEN_OK,
	VCAP_IS2_HK_MAC_ARP_TARGET_MATCH,
	VCAP_IS2_HK_MAC_ARP_SENDER_MATCH,
	VCAP_IS2_HK_MAC_ARP_OPCODE_UNKNOWN,
	VCAP_IS2_HK_MAC_ARP_OPCODE,
	VCAP_IS2_HK_MAC_ARP_L3_IP4_DIP,
	VCAP_IS2_HK_MAC_ARP_L3_IP4_SIP,
	VCAP_IS2_HK_MAC_ARP_DIP_EQ_SIP,
	
	VCAP_IS2_HK_IP4,
	VCAP_IS2_HK_L3_FRAGMENT,
	VCAP_IS2_HK_L3_FRAG_OFS_GT0,
	VCAP_IS2_HK_L3_OPTIONS,
	VCAP_IS2_HK_IP4_L3_TTL_GT0,
	VCAP_IS2_HK_L3_TOS,
	VCAP_IS2_HK_L3_IP4_DIP,
	VCAP_IS2_HK_L3_IP4_SIP,
	VCAP_IS2_HK_DIP_EQ_SIP,
	
	VCAP_IS2_HK_TCP,
	VCAP_IS2_HK_L4_SPORT,
	VCAP_IS2_HK_L4_DPORT,
	VCAP_IS2_HK_L4_RNG,
	VCAP_IS2_HK_L4_SPORT_EQ_DPORT,
	VCAP_IS2_HK_L4_SEQUENCE_EQ0,
	VCAP_IS2_HK_L4_URG,
	VCAP_IS2_HK_L4_ACK,
	VCAP_IS2_HK_L4_PSH,
	VCAP_IS2_HK_L4_RST,
	VCAP_IS2_HK_L4_SYN,
	VCAP_IS2_HK_L4_FIN,
	VCAP_IS2_HK_L4_1588_DOM,
	VCAP_IS2_HK_L4_1588_VER,
	
	VCAP_IS2_HK_IP4_L3_PROTO,
	VCAP_IS2_HK_L3_PAYLOAD,
	
	VCAP_IS2_HK_IP6_L3_TTL_GT0,
	VCAP_IS2_HK_IP6_L3_PROTO,
	VCAP_IS2_HK_L3_IP6_SIP,
	
	VCAP_IS2_HK_OAM_MEL_FLAGS,
	VCAP_IS2_HK_OAM_VER,
	VCAP_IS2_HK_OAM_OPCODE,
	VCAP_IS2_HK_OAM_FLAGS,
	VCAP_IS2_HK_OAM_MEPID,
	VCAP_IS2_HK_OAM_CCM_CNTS_EQ0,
	VCAP_IS2_HK_OAM_IS_Y1731,
};

struct vcap_field {
	int offset;
	int length;
};

enum vcap_is2_action_field {
	VCAP_IS2_ACT_HIT_ME_ONCE,
	VCAP_IS2_ACT_CPU_COPY_ENA,
	VCAP_IS2_ACT_CPU_QU_NUM,
	VCAP_IS2_ACT_MASK_MODE,
	VCAP_IS2_ACT_MIRROR_ENA,
	VCAP_IS2_ACT_LRN_DIS,
	VCAP_IS2_ACT_POLICE_ENA,
	VCAP_IS2_ACT_POLICE_IDX,
	VCAP_IS2_ACT_POLICE_VCAP_ONLY,
	VCAP_IS2_ACT_PORT_MASK,
	VCAP_IS2_ACT_REW_OP,
	VCAP_IS2_ACT_SMAC_REPLACE_ENA,
	VCAP_IS2_ACT_RSV,
	VCAP_IS2_ACT_ACL_ID,
	VCAP_IS2_ACT_HIT_CNT,
};




#define IS1_TYPE_S1_NORMAL 0
#define IS1_TYPE_S1_5TUPLE_IP4 1


#define IS1_TYPE_S1_NORMAL_IP6 0
#define IS1_TYPE_S1_7TUPLE 1
#define IS2_TYPE_S1_5TUPLE_IP6 2

enum {
	IS1_ACTION_TYPE_NORMAL,
	IS1_ACTION_TYPE_MAX,
};

enum vcap_is1_half_key_field {
	VCAP_IS1_HK_TYPE,
	VCAP_IS1_HK_LOOKUP,
	VCAP_IS1_HK_IGR_PORT_MASK,
	VCAP_IS1_HK_RSV,
	VCAP_IS1_HK_OAM_Y1731,
	VCAP_IS1_HK_L2_MC,
	VCAP_IS1_HK_L2_BC,
	VCAP_IS1_HK_IP_MC,
	VCAP_IS1_HK_VLAN_TAGGED,
	VCAP_IS1_HK_VLAN_DBL_TAGGED,
	VCAP_IS1_HK_TPID,
	VCAP_IS1_HK_VID,
	VCAP_IS1_HK_DEI,
	VCAP_IS1_HK_PCP,
	
	VCAP_IS1_HK_L2_SMAC,
	VCAP_IS1_HK_ETYPE_LEN,
	VCAP_IS1_HK_ETYPE,
	VCAP_IS1_HK_IP_SNAP,
	VCAP_IS1_HK_IP4,
	VCAP_IS1_HK_L3_FRAGMENT,
	VCAP_IS1_HK_L3_FRAG_OFS_GT0,
	VCAP_IS1_HK_L3_OPTIONS,
	VCAP_IS1_HK_L3_DSCP,
	VCAP_IS1_HK_L3_IP4_SIP,
	VCAP_IS1_HK_TCP_UDP,
	VCAP_IS1_HK_TCP,
	VCAP_IS1_HK_L4_SPORT,
	VCAP_IS1_HK_L4_RNG,
	
	VCAP_IS1_HK_IP4_INNER_TPID,
	VCAP_IS1_HK_IP4_INNER_VID,
	VCAP_IS1_HK_IP4_INNER_DEI,
	VCAP_IS1_HK_IP4_INNER_PCP,
	VCAP_IS1_HK_IP4_IP4,
	VCAP_IS1_HK_IP4_L3_FRAGMENT,
	VCAP_IS1_HK_IP4_L3_FRAG_OFS_GT0,
	VCAP_IS1_HK_IP4_L3_OPTIONS,
	VCAP_IS1_HK_IP4_L3_DSCP,
	VCAP_IS1_HK_IP4_L3_IP4_DIP,
	VCAP_IS1_HK_IP4_L3_IP4_SIP,
	VCAP_IS1_HK_IP4_L3_PROTO,
	VCAP_IS1_HK_IP4_TCP_UDP,
	VCAP_IS1_HK_IP4_TCP,
	VCAP_IS1_HK_IP4_L4_RNG,
	VCAP_IS1_HK_IP4_IP_PAYLOAD_S1_5TUPLE,
};

enum vcap_is1_action_field {
	VCAP_IS1_ACT_DSCP_ENA,
	VCAP_IS1_ACT_DSCP_VAL,
	VCAP_IS1_ACT_QOS_ENA,
	VCAP_IS1_ACT_QOS_VAL,
	VCAP_IS1_ACT_DP_ENA,
	VCAP_IS1_ACT_DP_VAL,
	VCAP_IS1_ACT_PAG_OVERRIDE_MASK,
	VCAP_IS1_ACT_PAG_VAL,
	VCAP_IS1_ACT_RSV,
	VCAP_IS1_ACT_VID_REPLACE_ENA,
	VCAP_IS1_ACT_VID_ADD_VAL,
	VCAP_IS1_ACT_FID_SEL,
	VCAP_IS1_ACT_FID_VAL,
	VCAP_IS1_ACT_PCP_DEI_ENA,
	VCAP_IS1_ACT_PCP_VAL,
	VCAP_IS1_ACT_DEI_VAL,
	VCAP_IS1_ACT_VLAN_POP_CNT_ENA,
	VCAP_IS1_ACT_VLAN_POP_CNT,
	VCAP_IS1_ACT_CUSTOM_ACE_TYPE_ENA,
	VCAP_IS1_ACT_HIT_STICKY,
};



enum {
	ES0_ACTION_TYPE_NORMAL,
	ES0_ACTION_TYPE_MAX,
};

enum vcap_es0_key_field {
	VCAP_ES0_EGR_PORT,
	VCAP_ES0_IGR_PORT,
	VCAP_ES0_RSV,
	VCAP_ES0_L2_MC,
	VCAP_ES0_L2_BC,
	VCAP_ES0_VID,
	VCAP_ES0_DP,
	VCAP_ES0_PCP,
};

enum vcap_es0_action_field {
	VCAP_ES0_ACT_PUSH_OUTER_TAG,
	VCAP_ES0_ACT_PUSH_INNER_TAG,
	VCAP_ES0_ACT_TAG_A_TPID_SEL,
	VCAP_ES0_ACT_TAG_A_VID_SEL,
	VCAP_ES0_ACT_TAG_A_PCP_SEL,
	VCAP_ES0_ACT_TAG_A_DEI_SEL,
	VCAP_ES0_ACT_TAG_B_TPID_SEL,
	VCAP_ES0_ACT_TAG_B_VID_SEL,
	VCAP_ES0_ACT_TAG_B_PCP_SEL,
	VCAP_ES0_ACT_TAG_B_DEI_SEL,
	VCAP_ES0_ACT_VID_A_VAL,
	VCAP_ES0_ACT_PCP_A_VAL,
	VCAP_ES0_ACT_DEI_A_VAL,
	VCAP_ES0_ACT_VID_B_VAL,
	VCAP_ES0_ACT_PCP_B_VAL,
	VCAP_ES0_ACT_DEI_B_VAL,
	VCAP_ES0_ACT_RSV,
	VCAP_ES0_ACT_HIT_STICKY,
};

struct ocelot_ipv4 {
	u8 addr[4];
};

enum ocelot_vcap_bit {
	OCELOT_VCAP_BIT_ANY,
	OCELOT_VCAP_BIT_0,
	OCELOT_VCAP_BIT_1
};

struct ocelot_vcap_u8 {
	u8 value[1];
	u8 mask[1];
};

struct ocelot_vcap_u16 {
	u8 value[2];
	u8 mask[2];
};

struct ocelot_vcap_u24 {
	u8 value[3];
	u8 mask[3];
};

struct ocelot_vcap_u32 {
	u8 value[4];
	u8 mask[4];
};

struct ocelot_vcap_u40 {
	u8 value[5];
	u8 mask[5];
};

struct ocelot_vcap_u48 {
	u8 value[6];
	u8 mask[6];
};

struct ocelot_vcap_u64 {
	u8 value[8];
	u8 mask[8];
};

struct ocelot_vcap_u128 {
	u8 value[16];
	u8 mask[16];
};

struct ocelot_vcap_vid {
	u16 value;
	u16 mask;
};

struct ocelot_vcap_ipv4 {
	struct ocelot_ipv4 value;
	struct ocelot_ipv4 mask;
};

struct ocelot_vcap_udp_tcp {
	u16 value;
	u16 mask;
};

struct ocelot_vcap_port {
	u8 value;
	u8 mask;
};

enum ocelot_vcap_key_type {
	OCELOT_VCAP_KEY_ANY,
	OCELOT_VCAP_KEY_ETYPE,
	OCELOT_VCAP_KEY_LLC,
	OCELOT_VCAP_KEY_SNAP,
	OCELOT_VCAP_KEY_ARP,
	OCELOT_VCAP_KEY_IPV4,
	OCELOT_VCAP_KEY_IPV6
};

struct ocelot_vcap_key_vlan {
	struct ocelot_vcap_vid vid;    
	struct ocelot_vcap_u8  pcp;    
	enum ocelot_vcap_bit dei;    
	enum ocelot_vcap_bit tagged; 
};

struct ocelot_vcap_key_etype {
	struct ocelot_vcap_u48 dmac;
	struct ocelot_vcap_u48 smac;
	struct ocelot_vcap_u16 etype;
	struct ocelot_vcap_u16 data; 
};

struct ocelot_vcap_key_llc {
	struct ocelot_vcap_u48 dmac;
	struct ocelot_vcap_u48 smac;

	
	struct ocelot_vcap_u32 llc;
};

struct ocelot_vcap_key_snap {
	struct ocelot_vcap_u48 dmac;
	struct ocelot_vcap_u48 smac;

	
	struct ocelot_vcap_u40 snap;
};

struct ocelot_vcap_key_arp {
	struct ocelot_vcap_u48 smac;
	enum ocelot_vcap_bit arp;	
	enum ocelot_vcap_bit req;	
	enum ocelot_vcap_bit unknown;    
	enum ocelot_vcap_bit smac_match; 
	enum ocelot_vcap_bit dmac_match; 

	
	enum ocelot_vcap_bit length;

	enum ocelot_vcap_bit ip;       
	enum  ocelot_vcap_bit ethernet; 
	struct ocelot_vcap_ipv4 sip;     
	struct ocelot_vcap_ipv4 dip;     
};

struct ocelot_vcap_key_ipv4 {
	enum ocelot_vcap_bit ttl;      
	enum ocelot_vcap_bit fragment; 
	enum ocelot_vcap_bit options;  
	struct ocelot_vcap_u8 ds;
	struct ocelot_vcap_u8 proto;      
	struct ocelot_vcap_ipv4 sip;      
	struct ocelot_vcap_ipv4 dip;      
	struct ocelot_vcap_u48 data;      
	struct ocelot_vcap_udp_tcp sport; 
	struct ocelot_vcap_udp_tcp dport; 
	enum ocelot_vcap_bit tcp_fin;
	enum ocelot_vcap_bit tcp_syn;
	enum ocelot_vcap_bit tcp_rst;
	enum ocelot_vcap_bit tcp_psh;
	enum ocelot_vcap_bit tcp_ack;
	enum ocelot_vcap_bit tcp_urg;
	enum ocelot_vcap_bit sip_eq_dip;     
	enum ocelot_vcap_bit sport_eq_dport; 
	enum ocelot_vcap_bit seq_zero;       
};

struct ocelot_vcap_key_ipv6 {
	struct ocelot_vcap_u8 proto; 
	struct ocelot_vcap_u128 sip; 
	struct ocelot_vcap_u128 dip; 
	enum ocelot_vcap_bit ttl;  
	struct ocelot_vcap_u8 ds;
	struct ocelot_vcap_u48 data; 
	struct ocelot_vcap_udp_tcp sport;
	struct ocelot_vcap_udp_tcp dport;
	enum ocelot_vcap_bit tcp_fin;
	enum ocelot_vcap_bit tcp_syn;
	enum ocelot_vcap_bit tcp_rst;
	enum ocelot_vcap_bit tcp_psh;
	enum ocelot_vcap_bit tcp_ack;
	enum ocelot_vcap_bit tcp_urg;
	enum ocelot_vcap_bit sip_eq_dip;     
	enum ocelot_vcap_bit sport_eq_dport; 
	enum ocelot_vcap_bit seq_zero;       
};

enum ocelot_mask_mode {
	OCELOT_MASK_MODE_NONE,
	OCELOT_MASK_MODE_PERMIT_DENY,
	OCELOT_MASK_MODE_POLICY,
	OCELOT_MASK_MODE_REDIRECT,
};

enum ocelot_es0_vid_sel {
	OCELOT_ES0_VID_PLUS_CLASSIFIED_VID = 0,
	OCELOT_ES0_VID = 1,
};

enum ocelot_es0_pcp_sel {
	OCELOT_CLASSIFIED_PCP = 0,
	OCELOT_ES0_PCP = 1,
};

enum ocelot_es0_tag {
	OCELOT_NO_ES0_TAG,
	OCELOT_ES0_TAG,
	OCELOT_FORCE_PORT_TAG,
	OCELOT_FORCE_UNTAG,
};

enum ocelot_tag_tpid_sel {
	OCELOT_TAG_TPID_SEL_8021Q,
	OCELOT_TAG_TPID_SEL_8021AD,
};

struct ocelot_vcap_action {
	union {
		
		struct {
			enum ocelot_es0_tag push_outer_tag;
			enum ocelot_es0_tag push_inner_tag;
			enum ocelot_tag_tpid_sel tag_a_tpid_sel;
			int tag_a_vid_sel;
			int tag_a_pcp_sel;
			u16 vid_a_val;
			u8 pcp_a_val;
			u8 dei_a_val;
			enum ocelot_tag_tpid_sel tag_b_tpid_sel;
			int tag_b_vid_sel;
			int tag_b_pcp_sel;
			u16 vid_b_val;
			u8 pcp_b_val;
			u8 dei_b_val;
		};

		
		struct {
			bool vid_replace_ena;
			u16 vid;
			bool vlan_pop_cnt_ena;
			int vlan_pop_cnt;
			bool pcp_dei_ena;
			u8 pcp;
			u8 dei;
			bool qos_ena;
			u8 qos_val;
			u8 pag_override_mask;
			u8 pag_val;
		};

		
		struct {
			bool cpu_copy_ena;
			u8 cpu_qu_num;
			enum ocelot_mask_mode mask_mode;
			unsigned long port_mask;
			bool police_ena;
			bool mirror_ena;
			struct ocelot_policer pol;
			u32 pol_ix;
		};
	};
};

struct ocelot_vcap_stats {
	u64 bytes;
	u64 pkts;
	u64 used;
};

enum ocelot_vcap_filter_type {
	OCELOT_VCAP_FILTER_DUMMY,
	OCELOT_VCAP_FILTER_PAG,
	OCELOT_VCAP_FILTER_OFFLOAD,
	OCELOT_PSFP_FILTER_OFFLOAD,
};

struct ocelot_vcap_id {
	unsigned long cookie;
	bool tc_offload;
};

struct ocelot_vcap_filter {
	struct list_head list;

	enum ocelot_vcap_filter_type type;
	int block_id;
	int goto_target;
	int lookup;
	u8 pag;
	u16 prio;
	struct ocelot_vcap_id id;

	struct ocelot_vcap_action action;
	struct ocelot_vcap_stats stats;
	
	bool take_ts;
	bool is_trap;
	unsigned long ingress_port_mask;
	
	struct ocelot_vcap_port ingress_port;
	
	struct ocelot_vcap_port egress_port;

	enum ocelot_vcap_bit dmac_mc;
	enum ocelot_vcap_bit dmac_bc;
	struct ocelot_vcap_key_vlan vlan;

	enum ocelot_vcap_key_type key_type;
	union {
		
		struct ocelot_vcap_key_etype etype;
		struct ocelot_vcap_key_llc llc;
		struct ocelot_vcap_key_snap snap;
		struct ocelot_vcap_key_arp arp;
		struct ocelot_vcap_key_ipv4 ipv4;
		struct ocelot_vcap_key_ipv6 ipv6;
	} key;
};

int ocelot_vcap_filter_add(struct ocelot *ocelot,
			   struct ocelot_vcap_filter *rule,
			   struct netlink_ext_ack *extack);
int ocelot_vcap_filter_del(struct ocelot *ocelot,
			   struct ocelot_vcap_filter *rule);
int ocelot_vcap_filter_replace(struct ocelot *ocelot,
			       struct ocelot_vcap_filter *filter);
struct ocelot_vcap_filter *
ocelot_vcap_block_find_filter_by_id(struct ocelot_vcap_block *block,
				    unsigned long cookie, bool tc_offload);

#endif 
