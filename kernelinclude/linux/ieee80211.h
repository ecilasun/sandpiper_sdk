/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef LINUX_IEEE80211_H
#define LINUX_IEEE80211_H

#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/bitfield.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>



#define FCS_LEN 4

#define IEEE80211_FCTL_VERS		0x0003
#define IEEE80211_FCTL_FTYPE		0x000c
#define IEEE80211_FCTL_STYPE		0x00f0
#define IEEE80211_FCTL_TODS		0x0100
#define IEEE80211_FCTL_FROMDS		0x0200
#define IEEE80211_FCTL_MOREFRAGS	0x0400
#define IEEE80211_FCTL_RETRY		0x0800
#define IEEE80211_FCTL_PM		0x1000
#define IEEE80211_FCTL_MOREDATA		0x2000
#define IEEE80211_FCTL_PROTECTED	0x4000
#define IEEE80211_FCTL_ORDER		0x8000
#define IEEE80211_FCTL_CTL_EXT		0x0f00

#define IEEE80211_SCTL_FRAG		0x000F
#define IEEE80211_SCTL_SEQ		0xFFF0

#define IEEE80211_FTYPE_MGMT		0x0000
#define IEEE80211_FTYPE_CTL		0x0004
#define IEEE80211_FTYPE_DATA		0x0008
#define IEEE80211_FTYPE_EXT		0x000c


#define IEEE80211_STYPE_ASSOC_REQ	0x0000
#define IEEE80211_STYPE_ASSOC_RESP	0x0010
#define IEEE80211_STYPE_REASSOC_REQ	0x0020
#define IEEE80211_STYPE_REASSOC_RESP	0x0030
#define IEEE80211_STYPE_PROBE_REQ	0x0040
#define IEEE80211_STYPE_PROBE_RESP	0x0050
#define IEEE80211_STYPE_BEACON		0x0080
#define IEEE80211_STYPE_ATIM		0x0090
#define IEEE80211_STYPE_DISASSOC	0x00A0
#define IEEE80211_STYPE_AUTH		0x00B0
#define IEEE80211_STYPE_DEAUTH		0x00C0
#define IEEE80211_STYPE_ACTION		0x00D0


#define IEEE80211_STYPE_TRIGGER		0x0020
#define IEEE80211_STYPE_CTL_EXT		0x0060
#define IEEE80211_STYPE_BACK_REQ	0x0080
#define IEEE80211_STYPE_BACK		0x0090
#define IEEE80211_STYPE_PSPOLL		0x00A0
#define IEEE80211_STYPE_RTS		0x00B0
#define IEEE80211_STYPE_CTS		0x00C0
#define IEEE80211_STYPE_ACK		0x00D0
#define IEEE80211_STYPE_CFEND		0x00E0
#define IEEE80211_STYPE_CFENDACK	0x00F0


#define IEEE80211_STYPE_DATA			0x0000
#define IEEE80211_STYPE_DATA_CFACK		0x0010
#define IEEE80211_STYPE_DATA_CFPOLL		0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL		0x0030
#define IEEE80211_STYPE_NULLFUNC		0x0040
#define IEEE80211_STYPE_CFACK			0x0050
#define IEEE80211_STYPE_CFPOLL			0x0060
#define IEEE80211_STYPE_CFACKPOLL		0x0070
#define IEEE80211_STYPE_QOS_DATA		0x0080
#define IEEE80211_STYPE_QOS_DATA_CFACK		0x0090
#define IEEE80211_STYPE_QOS_DATA_CFPOLL		0x00A0
#define IEEE80211_STYPE_QOS_DATA_CFACKPOLL	0x00B0
#define IEEE80211_STYPE_QOS_NULLFUNC		0x00C0
#define IEEE80211_STYPE_QOS_CFACK		0x00D0
#define IEEE80211_STYPE_QOS_CFPOLL		0x00E0
#define IEEE80211_STYPE_QOS_CFACKPOLL		0x00F0


#define IEEE80211_STYPE_DMG_BEACON		0x0000
#define IEEE80211_STYPE_S1G_BEACON		0x0010


#define IEEE80211_S1G_BCN_NEXT_TBTT	0x100


#define IEEE80211_S1G_1MHZ_NDP_BITS	25
#define IEEE80211_S1G_1MHZ_NDP_BYTES	4
#define IEEE80211_S1G_2MHZ_NDP_BITS	37
#define IEEE80211_S1G_2MHZ_NDP_BYTES	5

#define IEEE80211_NDP_FTYPE_CTS			0
#define IEEE80211_NDP_FTYPE_CF_END		0
#define IEEE80211_NDP_FTYPE_PS_POLL		1
#define IEEE80211_NDP_FTYPE_ACK			2
#define IEEE80211_NDP_FTYPE_PS_POLL_ACK		3
#define IEEE80211_NDP_FTYPE_BA			4
#define IEEE80211_NDP_FTYPE_BF_REPORT_POLL	5
#define IEEE80211_NDP_FTYPE_PAGING		6
#define IEEE80211_NDP_FTYPE_PREQ		7

#define SM64(f, v)	((((u64)v) << f##_S) & f)


#define IEEE80211_NDP_FTYPE                    0x0000000000000007
#define IEEE80211_NDP_FTYPE_S                  0x0000000000000000


#define IEEE80211_NDP_1M_PREQ_ANO      0x0000000000000008
#define IEEE80211_NDP_1M_PREQ_ANO_S                     3
#define IEEE80211_NDP_1M_PREQ_CSSID    0x00000000000FFFF0
#define IEEE80211_NDP_1M_PREQ_CSSID_S                   4
#define IEEE80211_NDP_1M_PREQ_RTYPE    0x0000000000100000
#define IEEE80211_NDP_1M_PREQ_RTYPE_S                  20
#define IEEE80211_NDP_1M_PREQ_RSV      0x0000000001E00000
#define IEEE80211_NDP_1M_PREQ_RSV      0x0000000001E00000

#define IEEE80211_NDP_2M_PREQ_ANO      0x0000000000000008
#define IEEE80211_NDP_2M_PREQ_ANO_S                     3
#define IEEE80211_NDP_2M_PREQ_CSSID    0x0000000FFFFFFFF0
#define IEEE80211_NDP_2M_PREQ_CSSID_S                   4
#define IEEE80211_NDP_2M_PREQ_RTYPE    0x0000001000000000
#define IEEE80211_NDP_2M_PREQ_RTYPE_S                  36

#define IEEE80211_ANO_NETTYPE_WILD              15


#define IEEE80211_S1G_BCN_NEXT_TBTT    0x100


#define IEEE80211_CTL_EXT_POLL		0x2000
#define IEEE80211_CTL_EXT_SPR		0x3000
#define IEEE80211_CTL_EXT_GRANT	0x4000
#define IEEE80211_CTL_EXT_DMG_CTS	0x5000
#define IEEE80211_CTL_EXT_DMG_DTS	0x6000
#define IEEE80211_CTL_EXT_SSW		0x8000
#define IEEE80211_CTL_EXT_SSW_FBACK	0x9000
#define IEEE80211_CTL_EXT_SSW_ACK	0xa000


#define IEEE80211_SN_MASK		((IEEE80211_SCTL_SEQ) >> 4)
#define IEEE80211_MAX_SN		IEEE80211_SN_MASK
#define IEEE80211_SN_MODULO		(IEEE80211_MAX_SN + 1)



#define IEEE80211_PV1_FCTL_VERS		0x0003
#define IEEE80211_PV1_FCTL_FTYPE	0x001c
#define IEEE80211_PV1_FCTL_STYPE	0x00e0
#define IEEE80211_PV1_FCTL_TODS		0x0100
#define IEEE80211_PV1_FCTL_MOREFRAGS	0x0200
#define IEEE80211_PV1_FCTL_PM		0x0400
#define IEEE80211_PV1_FCTL_MOREDATA	0x0800
#define IEEE80211_PV1_FCTL_PROTECTED	0x1000
#define IEEE80211_PV1_FCTL_END_SP       0x2000
#define IEEE80211_PV1_FCTL_RELAYED      0x4000
#define IEEE80211_PV1_FCTL_ACK_POLICY   0x8000
#define IEEE80211_PV1_FCTL_CTL_EXT	0x0f00

static inline bool ieee80211_sn_less(u16 sn1, u16 sn2)
{
	return ((sn1 - sn2) & IEEE80211_SN_MASK) > (IEEE80211_SN_MODULO >> 1);
}

static inline u16 ieee80211_sn_add(u16 sn1, u16 sn2)
{
	return (sn1 + sn2) & IEEE80211_SN_MASK;
}

static inline u16 ieee80211_sn_inc(u16 sn)
{
	return ieee80211_sn_add(sn, 1);
}

static inline u16 ieee80211_sn_sub(u16 sn1, u16 sn2)
{
	return (sn1 - sn2) & IEEE80211_SN_MASK;
}

#define IEEE80211_SEQ_TO_SN(seq)	(((seq) & IEEE80211_SCTL_SEQ) >> 4)
#define IEEE80211_SN_TO_SEQ(ssn)	(((ssn) << 4) & IEEE80211_SCTL_SEQ)


#define IEEE80211_MAX_FRAG_THRESHOLD	2352
#define IEEE80211_MAX_RTS_THRESHOLD	2353
#define IEEE80211_MAX_AID		2007
#define IEEE80211_MAX_AID_S1G		8191
#define IEEE80211_MAX_TIM_LEN		251
#define IEEE80211_MAX_MESH_PEERINGS	63

#define IEEE80211_MAX_DATA_LEN		2304

#define IEEE80211_MAX_DATA_LEN_DMG	7920

#define IEEE80211_MAX_FRAME_LEN		2352


#define IEEE80211_MAX_MPDU_LEN_HT_BA		4095


#define IEEE80211_MAX_MPDU_LEN_HT_3839		3839
#define IEEE80211_MAX_MPDU_LEN_HT_7935		7935

#define IEEE80211_MAX_MPDU_LEN_VHT_3895		3895
#define IEEE80211_MAX_MPDU_LEN_VHT_7991		7991
#define IEEE80211_MAX_MPDU_LEN_VHT_11454	11454

#define IEEE80211_MAX_SSID_LEN		32

#define IEEE80211_MAX_MESH_ID_LEN	32

#define IEEE80211_FIRST_TSPEC_TSID	8
#define IEEE80211_NUM_TIDS		16


#define IEEE80211_NUM_UPS		8

#define IEEE80211_NUM_ACS		4

#define IEEE80211_QOS_CTL_LEN		2

#define IEEE80211_QOS_CTL_TAG1D_MASK		0x0007

#define IEEE80211_QOS_CTL_TID_MASK		0x000f

#define IEEE80211_QOS_CTL_EOSP			0x0010

#define IEEE80211_QOS_CTL_ACK_POLICY_NORMAL	0x0000
#define IEEE80211_QOS_CTL_ACK_POLICY_NOACK	0x0020
#define IEEE80211_QOS_CTL_ACK_POLICY_NO_EXPL	0x0040
#define IEEE80211_QOS_CTL_ACK_POLICY_BLOCKACK	0x0060
#define IEEE80211_QOS_CTL_ACK_POLICY_MASK	0x0060

#define IEEE80211_QOS_CTL_A_MSDU_PRESENT	0x0080

#define IEEE80211_QOS_CTL_MESH_CONTROL_PRESENT  0x0100


#define IEEE80211_QOS_CTL_MESH_PS_LEVEL		0x0200

#define IEEE80211_QOS_CTL_RSPI			0x0400


#define IEEE80211_WMM_IE_AP_QOSINFO_UAPSD	(1<<7)
#define IEEE80211_WMM_IE_AP_QOSINFO_PARAM_SET_CNT_MASK	0x0f


#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VO	(1<<0)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VI	(1<<1)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BK	(1<<2)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BE	(1<<3)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_MASK	0x0f


#define IEEE80211_WMM_IE_STA_QOSINFO_SP_ALL	0x00
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_2	0x01
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_4	0x02
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_6	0x03
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_MASK	0x03
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_SHIFT	5

#define IEEE80211_HT_CTL_LEN		4


#define IEEE80211_TRIGGER_TYPE_MASK		0xf
#define IEEE80211_TRIGGER_TYPE_BASIC		0x0
#define IEEE80211_TRIGGER_TYPE_BFRP		0x1
#define IEEE80211_TRIGGER_TYPE_MU_BAR		0x2
#define IEEE80211_TRIGGER_TYPE_MU_RTS		0x3
#define IEEE80211_TRIGGER_TYPE_BSRP		0x4
#define IEEE80211_TRIGGER_TYPE_GCR_MU_BAR	0x5
#define IEEE80211_TRIGGER_TYPE_BQRP		0x6
#define IEEE80211_TRIGGER_TYPE_NFRP		0x7

struct ieee80211_hdr {
	__le16 frame_control;
	__le16 duration_id;
	struct_group(addrs,
		u8 addr1[ETH_ALEN];
		u8 addr2[ETH_ALEN];
		u8 addr3[ETH_ALEN];
	);
	__le16 seq_ctrl;
	u8 addr4[ETH_ALEN];
} __packed __aligned(2);

struct ieee80211_hdr_3addr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctrl;
} __packed __aligned(2);

struct ieee80211_qos_hdr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctrl;
	__le16 qos_ctrl;
} __packed __aligned(2);

struct ieee80211_qos_hdr_4addr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctrl;
	u8 addr4[ETH_ALEN];
	__le16 qos_ctrl;
} __packed __aligned(2);

struct ieee80211_trigger {
	__le16 frame_control;
	__le16 duration;
	u8 ra[ETH_ALEN];
	u8 ta[ETH_ALEN];
	__le64 common_info;
	u8 variable[];
} __packed __aligned(2);


static inline bool ieee80211_has_tods(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_TODS)) != 0;
}


static inline bool ieee80211_has_fromds(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FROMDS)) != 0;
}


static inline bool ieee80211_has_a4(__le16 fc)
{
	__le16 tmp = cpu_to_le16(IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS);
	return (fc & tmp) == tmp;
}


static inline bool ieee80211_has_morefrags(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_MOREFRAGS)) != 0;
}


static inline bool ieee80211_has_retry(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_RETRY)) != 0;
}


static inline bool ieee80211_has_pm(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_PM)) != 0;
}


static inline bool ieee80211_has_moredata(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_MOREDATA)) != 0;
}


static inline bool ieee80211_has_protected(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_PROTECTED)) != 0;
}


static inline bool ieee80211_has_order(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_ORDER)) != 0;
}


static inline bool ieee80211_is_mgmt(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT);
}


static inline bool ieee80211_is_ctl(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL);
}


static inline bool ieee80211_is_data(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA);
}


static inline bool ieee80211_is_ext(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_EXT);
}



static inline bool ieee80211_is_data_qos(__le16 fc)
{
	
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_STYPE_QOS_DATA)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA);
}


static inline bool ieee80211_is_data_present(__le16 fc)
{
	
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | 0x40)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA);
}


static inline bool ieee80211_is_assoc_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_REQ);
}


static inline bool ieee80211_is_assoc_resp(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_RESP);
}


static inline bool ieee80211_is_reassoc_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_REQ);
}


static inline bool ieee80211_is_reassoc_resp(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_RESP);
}


static inline bool ieee80211_is_probe_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ);
}


static inline bool ieee80211_is_probe_resp(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP);
}


static inline bool ieee80211_is_beacon(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);
}


static inline bool ieee80211_is_s1g_beacon(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE |
				 IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_EXT | IEEE80211_STYPE_S1G_BEACON);
}


static inline bool ieee80211_next_tbtt_present(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_EXT | IEEE80211_STYPE_S1G_BEACON) &&
	       fc & cpu_to_le16(IEEE80211_S1G_BCN_NEXT_TBTT);
}


static inline bool ieee80211_is_s1g_short_beacon(__le16 fc)
{
	return ieee80211_is_s1g_beacon(fc) && ieee80211_next_tbtt_present(fc);
}


static inline bool ieee80211_is_atim(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ATIM);
}


static inline bool ieee80211_is_disassoc(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DISASSOC);
}


static inline bool ieee80211_is_auth(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_AUTH);
}


static inline bool ieee80211_is_deauth(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DEAUTH);
}


static inline bool ieee80211_is_action(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION);
}


static inline bool ieee80211_is_back_req(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK_REQ);
}


static inline bool ieee80211_is_back(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK);
}


static inline bool ieee80211_is_pspoll(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_PSPOLL);
}


static inline bool ieee80211_is_rts(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_RTS);
}


static inline bool ieee80211_is_cts(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS);
}


static inline bool ieee80211_is_ack(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_ACK);
}


static inline bool ieee80211_is_cfend(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFEND);
}


static inline bool ieee80211_is_cfendack(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFENDACK);
}


static inline bool ieee80211_is_nullfunc(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_NULLFUNC);
}


static inline bool ieee80211_is_qos_nullfunc(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_NULLFUNC);
}


static inline bool ieee80211_is_trigger(__le16 fc)
{
	return (fc & cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) ==
	       cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_TRIGGER);
}


static inline bool ieee80211_is_any_nullfunc(__le16 fc)
{
	return (ieee80211_is_nullfunc(fc) || ieee80211_is_qos_nullfunc(fc));
}


static inline bool ieee80211_is_first_frag(__le16 seq_ctrl)
{
	return (seq_ctrl & cpu_to_le16(IEEE80211_SCTL_FRAG)) == 0;
}


static inline bool ieee80211_is_frag(struct ieee80211_hdr *hdr)
{
	return ieee80211_has_morefrags(hdr->frame_control) ||
	       hdr->seq_ctrl & cpu_to_le16(IEEE80211_SCTL_FRAG);
}

struct ieee80211s_hdr {
	u8 flags;
	u8 ttl;
	__le32 seqnum;
	u8 eaddr1[ETH_ALEN];
	u8 eaddr2[ETH_ALEN];
} __packed __aligned(2);


#define MESH_FLAGS_AE_A4 	0x1
#define MESH_FLAGS_AE_A5_A6	0x2
#define MESH_FLAGS_AE		0x3
#define MESH_FLAGS_PS_DEEP	0x4


enum ieee80211_preq_flags {
	IEEE80211_PREQ_PROACTIVE_PREP_FLAG	= 1<<2,
};


enum ieee80211_preq_target_flags {
	IEEE80211_PREQ_TO_FLAG	= 1<<0,
	IEEE80211_PREQ_USN_FLAG	= 1<<2,
};


struct ieee80211_quiet_ie {
	u8 count;
	u8 period;
	__le16 duration;
	__le16 offset;
} __packed;


struct ieee80211_msrment_ie {
	u8 token;
	u8 mode;
	u8 type;
	u8 request[];
} __packed;


struct ieee80211_channel_sw_ie {
	u8 mode;
	u8 new_ch_num;
	u8 count;
} __packed;


struct ieee80211_ext_chansw_ie {
	u8 mode;
	u8 new_operating_class;
	u8 new_ch_num;
	u8 count;
} __packed;


struct ieee80211_sec_chan_offs_ie {
	u8 sec_chan_offs;
} __packed;


struct ieee80211_mesh_chansw_params_ie {
	u8 mesh_ttl;
	u8 mesh_flags;
	__le16 mesh_reason;
	__le16 mesh_pre_value;
} __packed;


struct ieee80211_wide_bw_chansw_ie {
	u8 new_channel_width;
	u8 new_center_freq_seg0, new_center_freq_seg1;
} __packed;


struct ieee80211_tim_ie {
	u8 dtim_count;
	u8 dtim_period;
	u8 bitmap_ctrl;
	union {
		u8 required_octet;
		DECLARE_FLEX_ARRAY(u8, virtual_map);
	};
} __packed;


struct ieee80211_meshconf_ie {
	u8 meshconf_psel;
	u8 meshconf_pmetric;
	u8 meshconf_congest;
	u8 meshconf_synch;
	u8 meshconf_auth;
	u8 meshconf_form;
	u8 meshconf_cap;
} __packed;


enum mesh_config_capab_flags {
	IEEE80211_MESHCONF_CAPAB_ACCEPT_PLINKS		= 0x01,
	IEEE80211_MESHCONF_CAPAB_FORWARDING		= 0x08,
	IEEE80211_MESHCONF_CAPAB_TBTT_ADJUSTING		= 0x20,
	IEEE80211_MESHCONF_CAPAB_POWER_SAVE_LEVEL	= 0x40,
};

#define IEEE80211_MESHCONF_FORM_CONNECTED_TO_GATE 0x1


#define WLAN_EID_CHAN_SWITCH_PARAM_TX_RESTRICT BIT(0)
#define WLAN_EID_CHAN_SWITCH_PARAM_INITIATOR BIT(1)
#define WLAN_EID_CHAN_SWITCH_PARAM_REASON BIT(2)


struct ieee80211_rann_ie {
	u8 rann_flags;
	u8 rann_hopcount;
	u8 rann_ttl;
	u8 rann_addr[ETH_ALEN];
	__le32 rann_seq;
	__le32 rann_interval;
	__le32 rann_metric;
} __packed;

enum ieee80211_rann_flags {
	RANN_FLAG_IS_GATE = 1 << 0,
};

enum ieee80211_ht_chanwidth_values {
	IEEE80211_HT_CHANWIDTH_20MHZ = 0,
	IEEE80211_HT_CHANWIDTH_ANY = 1,
};


enum ieee80211_vht_opmode_bits {
	IEEE80211_OPMODE_NOTIF_CHANWIDTH_MASK	= 0x03,
	IEEE80211_OPMODE_NOTIF_CHANWIDTH_20MHZ	= 0,
	IEEE80211_OPMODE_NOTIF_CHANWIDTH_40MHZ	= 1,
	IEEE80211_OPMODE_NOTIF_CHANWIDTH_80MHZ	= 2,
	IEEE80211_OPMODE_NOTIF_CHANWIDTH_160MHZ	= 3,
	IEEE80211_OPMODE_NOTIF_BW_160_80P80	= 0x04,
	IEEE80211_OPMODE_NOTIF_RX_NSS_MASK	= 0x70,
	IEEE80211_OPMODE_NOTIF_RX_NSS_SHIFT	= 4,
	IEEE80211_OPMODE_NOTIF_RX_NSS_TYPE_BF	= 0x80,
};


enum ieee80211_s1g_chanwidth {
	IEEE80211_S1G_CHANWIDTH_1MHZ = 0,
	IEEE80211_S1G_CHANWIDTH_2MHZ = 1,
	IEEE80211_S1G_CHANWIDTH_4MHZ = 3,
	IEEE80211_S1G_CHANWIDTH_8MHZ = 7,
	IEEE80211_S1G_CHANWIDTH_16MHZ = 15,
};

#define WLAN_SA_QUERY_TR_ID_LEN 2
#define WLAN_MEMBERSHIP_LEN 8
#define WLAN_USER_POSITION_LEN 16


struct ieee80211_tpc_report_ie {
	u8 tx_power;
	u8 link_margin;
} __packed;

#define IEEE80211_ADDBA_EXT_FRAG_LEVEL_MASK	GENMASK(2, 1)
#define IEEE80211_ADDBA_EXT_FRAG_LEVEL_SHIFT	1
#define IEEE80211_ADDBA_EXT_NO_FRAG		BIT(0)
#define IEEE80211_ADDBA_EXT_BUF_SIZE_MASK	GENMASK(7, 5)
#define IEEE80211_ADDBA_EXT_BUF_SIZE_SHIFT	10

struct ieee80211_addba_ext_ie {
	u8 data;
} __packed;


struct ieee80211_s1g_bcn_compat_ie {
	__le16 compat_info;
	__le16 beacon_int;
	__le32 tsf_completion;
} __packed;


struct ieee80211_s1g_oper_ie {
	u8 ch_width;
	u8 oper_class;
	u8 primary_ch;
	u8 oper_ch;
	__le16 basic_mcs_nss;
} __packed;


struct ieee80211_aid_response_ie {
	__le16 aid;
	u8 switch_count;
	__le16 response_int;
} __packed;

struct ieee80211_s1g_cap {
	u8 capab_info[10];
	u8 supp_mcs_nss[5];
} __packed;

struct ieee80211_ext {
	__le16 frame_control;
	__le16 duration;
	union {
		struct {
			u8 sa[ETH_ALEN];
			__le32 timestamp;
			u8 change_seq;
			u8 variable[0];
		} __packed s1g_beacon;
		struct {
			u8 sa[ETH_ALEN];
			__le32 timestamp;
			u8 change_seq;
			u8 next_tbtt[3];
			u8 variable[0];
		} __packed s1g_short_beacon;
	} u;
} __packed __aligned(2);

#define IEEE80211_TWT_CONTROL_NDP			BIT(0)
#define IEEE80211_TWT_CONTROL_RESP_MODE			BIT(1)
#define IEEE80211_TWT_CONTROL_NEG_TYPE_BROADCAST	BIT(3)
#define IEEE80211_TWT_CONTROL_RX_DISABLED		BIT(4)
#define IEEE80211_TWT_CONTROL_WAKE_DUR_UNIT		BIT(5)

#define IEEE80211_TWT_REQTYPE_REQUEST			BIT(0)
#define IEEE80211_TWT_REQTYPE_SETUP_CMD			GENMASK(3, 1)
#define IEEE80211_TWT_REQTYPE_TRIGGER			BIT(4)
#define IEEE80211_TWT_REQTYPE_IMPLICIT			BIT(5)
#define IEEE80211_TWT_REQTYPE_FLOWTYPE			BIT(6)
#define IEEE80211_TWT_REQTYPE_FLOWID			GENMASK(9, 7)
#define IEEE80211_TWT_REQTYPE_WAKE_INT_EXP		GENMASK(14, 10)
#define IEEE80211_TWT_REQTYPE_PROTECTION		BIT(15)

enum ieee80211_twt_setup_cmd {
	TWT_SETUP_CMD_REQUEST,
	TWT_SETUP_CMD_SUGGEST,
	TWT_SETUP_CMD_DEMAND,
	TWT_SETUP_CMD_GROUPING,
	TWT_SETUP_CMD_ACCEPT,
	TWT_SETUP_CMD_ALTERNATE,
	TWT_SETUP_CMD_DICTATE,
	TWT_SETUP_CMD_REJECT,
};

struct ieee80211_twt_params {
	__le16 req_type;
	__le64 twt;
	u8 min_twt_dur;
	__le16 mantissa;
	u8 channel;
} __packed;

struct ieee80211_twt_setup {
	u8 dialog_token;
	u8 element_id;
	u8 length;
	u8 control;
	u8 params[];
} __packed;

struct ieee80211_mgmt {
	__le16 frame_control;
	__le16 duration;
	u8 da[ETH_ALEN];
	u8 sa[ETH_ALEN];
	u8 bssid[ETH_ALEN];
	__le16 seq_ctrl;
	union {
		struct {
			__le16 auth_alg;
			__le16 auth_transaction;
			__le16 status_code;
			
			u8 variable[];
		} __packed auth;
		struct {
			__le16 reason_code;
		} __packed deauth;
		struct {
			__le16 capab_info;
			__le16 listen_interval;
			
			u8 variable[];
		} __packed assoc_req;
		struct {
			__le16 capab_info;
			__le16 status_code;
			__le16 aid;
			
			u8 variable[];
		} __packed assoc_resp, reassoc_resp;
		struct {
			__le16 capab_info;
			__le16 status_code;
			u8 variable[];
		} __packed s1g_assoc_resp, s1g_reassoc_resp;
		struct {
			__le16 capab_info;
			__le16 listen_interval;
			u8 current_ap[ETH_ALEN];
			
			u8 variable[];
		} __packed reassoc_req;
		struct {
			__le16 reason_code;
		} __packed disassoc;
		struct {
			__le64 timestamp;
			__le16 beacon_int;
			__le16 capab_info;
			
			u8 variable[];
		} __packed beacon;
		struct {
			
			DECLARE_FLEX_ARRAY(u8, variable);
		} __packed probe_req;
		struct {
			__le64 timestamp;
			__le16 beacon_int;
			__le16 capab_info;
			
			u8 variable[];
		} __packed probe_resp;
		struct {
			u8 category;
			union {
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 status_code;
					u8 variable[];
				} __packed wme_action;
				struct{
					u8 action_code;
					u8 variable[];
				} __packed chan_switch;
				struct{
					u8 action_code;
					struct ieee80211_ext_chansw_ie data;
					u8 variable[];
				} __packed ext_chan_switch;
				struct{
					u8 action_code;
					u8 dialog_token;
					u8 element_id;
					u8 length;
					struct ieee80211_msrment_ie msr_elem;
				} __packed measurement;
				struct{
					u8 action_code;
					u8 dialog_token;
					__le16 capab;
					__le16 timeout;
					__le16 start_seq_num;
					
					u8 variable[];
				} __packed addba_req;
				struct{
					u8 action_code;
					u8 dialog_token;
					__le16 status;
					__le16 capab;
					__le16 timeout;
				} __packed addba_resp;
				struct{
					u8 action_code;
					__le16 params;
					__le16 reason_code;
				} __packed delba;
				struct {
					u8 action_code;
					u8 variable[];
				} __packed self_prot;
				struct{
					u8 action_code;
					u8 variable[];
				} __packed mesh_action;
				struct {
					u8 action;
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} __packed sa_query;
				struct {
					u8 action;
					u8 smps_control;
				} __packed ht_smps;
				struct {
					u8 action_code;
					u8 chanwidth;
				} __packed ht_notify_cw;
				struct {
					u8 action_code;
					u8 dialog_token;
					__le16 capability;
					u8 variable[0];
				} __packed tdls_discover_resp;
				struct {
					u8 action_code;
					u8 operating_mode;
				} __packed vht_opmode_notif;
				struct {
					u8 action_code;
					u8 membership[WLAN_MEMBERSHIP_LEN];
					u8 position[WLAN_USER_POSITION_LEN];
				} __packed vht_group_notif;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 tpc_elem_id;
					u8 tpc_elem_length;
					struct ieee80211_tpc_report_ie tpc;
				} __packed tpc_report;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 follow_up;
					u8 tod[6];
					u8 toa[6];
					__le16 tod_error;
					__le16 toa_error;
					u8 variable[];
				} __packed ftm;
				struct {
					u8 action_code;
					u8 variable[];
				} __packed s1g;
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 follow_up;
					u32 tod;
					u32 toa;
					u8 max_tod_error;
					u8 max_toa_error;
				} __packed wnm_timing_msr;
			} u;
		} __packed action;
		DECLARE_FLEX_ARRAY(u8, body); 
	} u;
} __packed __aligned(2);


#define BSS_MEMBERSHIP_SELECTOR_HT_PHY	127
#define BSS_MEMBERSHIP_SELECTOR_VHT_PHY	126
#define BSS_MEMBERSHIP_SELECTOR_GLK	125
#define BSS_MEMBERSHIP_SELECTOR_EPS	124
#define BSS_MEMBERSHIP_SELECTOR_SAE_H2E 123
#define BSS_MEMBERSHIP_SELECTOR_HE_PHY	122
#define BSS_MEMBERSHIP_SELECTOR_EHT_PHY	121


#define IEEE80211_MIN_ACTION_SIZE offsetof(struct ieee80211_mgmt, u.action.u)



struct ieee80211_mmie {
	u8 element_id;
	u8 length;
	__le16 key_id;
	u8 sequence_number[6];
	u8 mic[8];
} __packed;


struct ieee80211_mmie_16 {
	u8 element_id;
	u8 length;
	__le16 key_id;
	u8 sequence_number[6];
	u8 mic[16];
} __packed;

struct ieee80211_vendor_ie {
	u8 element_id;
	u8 len;
	u8 oui[3];
	u8 oui_type;
} __packed;

struct ieee80211_wmm_ac_param {
	u8 aci_aifsn; 
	u8 cw; 
	__le16 txop_limit;
} __packed;

struct ieee80211_wmm_param_ie {
	u8 element_id; 
	u8 len; 
	
	u8 oui[3]; 
	u8 oui_type; 
	u8 oui_subtype; 
	u8 version; 
	u8 qos_info; 
	u8 reserved; 
	
	struct ieee80211_wmm_ac_param ac[4];
} __packed;


struct ieee80211_rts {
	__le16 frame_control;
	__le16 duration;
	u8 ra[ETH_ALEN];
	u8 ta[ETH_ALEN];
} __packed __aligned(2);

struct ieee80211_cts {
	__le16 frame_control;
	__le16 duration;
	u8 ra[ETH_ALEN];
} __packed __aligned(2);

struct ieee80211_pspoll {
	__le16 frame_control;
	__le16 aid;
	u8 bssid[ETH_ALEN];
	u8 ta[ETH_ALEN];
} __packed __aligned(2);




struct ieee80211_ch_switch_timing {
	__le16 switch_time;
	__le16 switch_timeout;
} __packed;


struct ieee80211_tdls_lnkie {
	u8 ie_type; 
	u8 ie_len;
	u8 bssid[ETH_ALEN];
	u8 init_sta[ETH_ALEN];
	u8 resp_sta[ETH_ALEN];
} __packed;

struct ieee80211_tdls_data {
	u8 da[ETH_ALEN];
	u8 sa[ETH_ALEN];
	__be16 ether_type;
	u8 payload_type;
	u8 category;
	u8 action_code;
	union {
		struct {
			u8 dialog_token;
			__le16 capability;
			u8 variable[0];
		} __packed setup_req;
		struct {
			__le16 status_code;
			u8 dialog_token;
			__le16 capability;
			u8 variable[0];
		} __packed setup_resp;
		struct {
			__le16 status_code;
			u8 dialog_token;
			u8 variable[0];
		} __packed setup_cfm;
		struct {
			__le16 reason_code;
			u8 variable[0];
		} __packed teardown;
		struct {
			u8 dialog_token;
			u8 variable[0];
		} __packed discover_req;
		struct {
			u8 target_channel;
			u8 oper_class;
			u8 variable[0];
		} __packed chan_switch_req;
		struct {
			__le16 status_code;
			u8 variable[0];
		} __packed chan_switch_resp;
	} u;
} __packed;



enum ieee80211_p2p_attr_id {
	IEEE80211_P2P_ATTR_STATUS = 0,
	IEEE80211_P2P_ATTR_MINOR_REASON,
	IEEE80211_P2P_ATTR_CAPABILITY,
	IEEE80211_P2P_ATTR_DEVICE_ID,
	IEEE80211_P2P_ATTR_GO_INTENT,
	IEEE80211_P2P_ATTR_GO_CONFIG_TIMEOUT,
	IEEE80211_P2P_ATTR_LISTEN_CHANNEL,
	IEEE80211_P2P_ATTR_GROUP_BSSID,
	IEEE80211_P2P_ATTR_EXT_LISTEN_TIMING,
	IEEE80211_P2P_ATTR_INTENDED_IFACE_ADDR,
	IEEE80211_P2P_ATTR_MANAGABILITY,
	IEEE80211_P2P_ATTR_CHANNEL_LIST,
	IEEE80211_P2P_ATTR_ABSENCE_NOTICE,
	IEEE80211_P2P_ATTR_DEVICE_INFO,
	IEEE80211_P2P_ATTR_GROUP_INFO,
	IEEE80211_P2P_ATTR_GROUP_ID,
	IEEE80211_P2P_ATTR_INTERFACE,
	IEEE80211_P2P_ATTR_OPER_CHANNEL,
	IEEE80211_P2P_ATTR_INVITE_FLAGS,
	
	IEEE80211_P2P_ATTR_VENDOR_SPECIFIC = 221,

	IEEE80211_P2P_ATTR_MAX
};



#define IEEE80211_P2P_NOA_DESC_MAX	4

struct ieee80211_p2p_noa_desc {
	u8 count;
	__le32 duration;
	__le32 interval;
	__le32 start_time;
} __packed;

struct ieee80211_p2p_noa_attr {
	u8 index;
	u8 oppps_ctwindow;
	struct ieee80211_p2p_noa_desc desc[IEEE80211_P2P_NOA_DESC_MAX];
} __packed;

#define IEEE80211_P2P_OPPPS_ENABLE_BIT		BIT(7)
#define IEEE80211_P2P_OPPPS_CTWINDOW_MASK	0x7F


struct ieee80211_bar {
	__le16 frame_control;
	__le16 duration;
	__u8 ra[ETH_ALEN];
	__u8 ta[ETH_ALEN];
	__le16 control;
	__le16 start_seq_num;
} __packed;


#define IEEE80211_BAR_CTRL_ACK_POLICY_NORMAL	0x0000
#define IEEE80211_BAR_CTRL_MULTI_TID		0x0002
#define IEEE80211_BAR_CTRL_CBMTID_COMPRESSED_BA	0x0004
#define IEEE80211_BAR_CTRL_TID_INFO_MASK	0xf000
#define IEEE80211_BAR_CTRL_TID_INFO_SHIFT	12

#define IEEE80211_HT_MCS_MASK_LEN		10


struct ieee80211_mcs_info {
	u8 rx_mask[IEEE80211_HT_MCS_MASK_LEN];
	__le16 rx_highest;
	u8 tx_params;
	u8 reserved[3];
} __packed;


#define IEEE80211_HT_MCS_RX_HIGHEST_MASK	0x3ff
#define IEEE80211_HT_MCS_TX_DEFINED		0x01
#define IEEE80211_HT_MCS_TX_RX_DIFF		0x02

#define IEEE80211_HT_MCS_TX_MAX_STREAMS_MASK	0x0C
#define IEEE80211_HT_MCS_TX_MAX_STREAMS_SHIFT	2
#define		IEEE80211_HT_MCS_TX_MAX_STREAMS	4
#define IEEE80211_HT_MCS_TX_UNEQUAL_MODULATION	0x10


#define IEEE80211_HT_MCS_UNEQUAL_MODULATION_START 33
#define IEEE80211_HT_MCS_UNEQUAL_MODULATION_START_BYTE \
	(IEEE80211_HT_MCS_UNEQUAL_MODULATION_START / 8)


struct ieee80211_ht_cap {
	__le16 cap_info;
	u8 ampdu_params_info;

	
	struct ieee80211_mcs_info mcs;

	__le16 extended_ht_cap_info;
	__le32 tx_BF_cap_info;
	u8 antenna_selection_info;
} __packed;


#define IEEE80211_HT_CAP_LDPC_CODING		0x0001
#define IEEE80211_HT_CAP_SUP_WIDTH_20_40	0x0002
#define IEEE80211_HT_CAP_SM_PS			0x000C
#define		IEEE80211_HT_CAP_SM_PS_SHIFT	2
#define IEEE80211_HT_CAP_GRN_FLD		0x0010
#define IEEE80211_HT_CAP_SGI_20			0x0020
#define IEEE80211_HT_CAP_SGI_40			0x0040
#define IEEE80211_HT_CAP_TX_STBC		0x0080
#define IEEE80211_HT_CAP_RX_STBC		0x0300
#define		IEEE80211_HT_CAP_RX_STBC_SHIFT	8
#define IEEE80211_HT_CAP_DELAY_BA		0x0400
#define IEEE80211_HT_CAP_MAX_AMSDU		0x0800
#define IEEE80211_HT_CAP_DSSSCCK40		0x1000
#define IEEE80211_HT_CAP_RESERVED		0x2000
#define IEEE80211_HT_CAP_40MHZ_INTOLERANT	0x4000
#define IEEE80211_HT_CAP_LSIG_TXOP_PROT		0x8000


#define IEEE80211_HT_EXT_CAP_PCO		0x0001
#define IEEE80211_HT_EXT_CAP_PCO_TIME		0x0006
#define		IEEE80211_HT_EXT_CAP_PCO_TIME_SHIFT	1
#define IEEE80211_HT_EXT_CAP_MCS_FB		0x0300
#define		IEEE80211_HT_EXT_CAP_MCS_FB_SHIFT	8
#define IEEE80211_HT_EXT_CAP_HTC_SUP		0x0400
#define IEEE80211_HT_EXT_CAP_RD_RESPONDER	0x0800


#define IEEE80211_HT_AMPDU_PARM_FACTOR		0x03
#define IEEE80211_HT_AMPDU_PARM_DENSITY		0x1C
#define		IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT	2


enum ieee80211_max_ampdu_length_exp {
	IEEE80211_HT_MAX_AMPDU_8K = 0,
	IEEE80211_HT_MAX_AMPDU_16K = 1,
	IEEE80211_HT_MAX_AMPDU_32K = 2,
	IEEE80211_HT_MAX_AMPDU_64K = 3
};


enum ieee80211_vht_max_ampdu_length_exp {
	IEEE80211_VHT_MAX_AMPDU_8K = 0,
	IEEE80211_VHT_MAX_AMPDU_16K = 1,
	IEEE80211_VHT_MAX_AMPDU_32K = 2,
	IEEE80211_VHT_MAX_AMPDU_64K = 3,
	IEEE80211_VHT_MAX_AMPDU_128K = 4,
	IEEE80211_VHT_MAX_AMPDU_256K = 5,
	IEEE80211_VHT_MAX_AMPDU_512K = 6,
	IEEE80211_VHT_MAX_AMPDU_1024K = 7
};

#define IEEE80211_HT_MAX_AMPDU_FACTOR 13


enum ieee80211_min_mpdu_spacing {
	IEEE80211_HT_MPDU_DENSITY_NONE = 0,	
	IEEE80211_HT_MPDU_DENSITY_0_25 = 1,	
	IEEE80211_HT_MPDU_DENSITY_0_5 = 2,	
	IEEE80211_HT_MPDU_DENSITY_1 = 3,	
	IEEE80211_HT_MPDU_DENSITY_2 = 4,	
	IEEE80211_HT_MPDU_DENSITY_4 = 5,	
	IEEE80211_HT_MPDU_DENSITY_8 = 6,	
	IEEE80211_HT_MPDU_DENSITY_16 = 7	
};


struct ieee80211_ht_operation {
	u8 primary_chan;
	u8 ht_param;
	__le16 operation_mode;
	__le16 stbc_param;
	u8 basic_set[16];
} __packed;


#define IEEE80211_HT_PARAM_CHA_SEC_OFFSET		0x03
#define		IEEE80211_HT_PARAM_CHA_SEC_NONE		0x00
#define		IEEE80211_HT_PARAM_CHA_SEC_ABOVE	0x01
#define		IEEE80211_HT_PARAM_CHA_SEC_BELOW	0x03
#define IEEE80211_HT_PARAM_CHAN_WIDTH_ANY		0x04
#define IEEE80211_HT_PARAM_RIFS_MODE			0x08


#define IEEE80211_HT_OP_MODE_PROTECTION			0x0003
#define		IEEE80211_HT_OP_MODE_PROTECTION_NONE		0
#define		IEEE80211_HT_OP_MODE_PROTECTION_NONMEMBER	1
#define		IEEE80211_HT_OP_MODE_PROTECTION_20MHZ		2
#define		IEEE80211_HT_OP_MODE_PROTECTION_NONHT_MIXED	3
#define IEEE80211_HT_OP_MODE_NON_GF_STA_PRSNT		0x0004
#define IEEE80211_HT_OP_MODE_NON_HT_STA_PRSNT		0x0010
#define IEEE80211_HT_OP_MODE_CCFS2_SHIFT		5
#define IEEE80211_HT_OP_MODE_CCFS2_MASK			0x1fe0


#define IEEE80211_HT_STBC_PARAM_DUAL_BEACON		0x0040
#define IEEE80211_HT_STBC_PARAM_DUAL_CTS_PROT		0x0080
#define IEEE80211_HT_STBC_PARAM_STBC_BEACON		0x0100
#define IEEE80211_HT_STBC_PARAM_LSIG_TXOP_FULLPROT	0x0200
#define IEEE80211_HT_STBC_PARAM_PCO_ACTIVE		0x0400
#define IEEE80211_HT_STBC_PARAM_PCO_PHASE		0x0800



#define IEEE80211_ADDBA_PARAM_AMSDU_MASK 0x0001
#define IEEE80211_ADDBA_PARAM_POLICY_MASK 0x0002
#define IEEE80211_ADDBA_PARAM_TID_MASK 0x003C
#define IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK 0xFFC0
#define IEEE80211_DELBA_PARAM_TID_MASK 0xF000
#define IEEE80211_DELBA_PARAM_INITIATOR_MASK 0x0800


#define IEEE80211_MIN_AMPDU_BUF		0x8
#define IEEE80211_MAX_AMPDU_BUF_HT	0x40
#define IEEE80211_MAX_AMPDU_BUF_HE	0x100
#define IEEE80211_MAX_AMPDU_BUF_EHT	0x400



#define WLAN_HT_CAP_SM_PS_STATIC	0
#define WLAN_HT_CAP_SM_PS_DYNAMIC	1
#define WLAN_HT_CAP_SM_PS_INVALID	2
#define WLAN_HT_CAP_SM_PS_DISABLED	3


#define WLAN_HT_SMPS_CONTROL_DISABLED	0
#define WLAN_HT_SMPS_CONTROL_STATIC	1
#define WLAN_HT_SMPS_CONTROL_DYNAMIC	3


struct ieee80211_vht_mcs_info {
	__le16 rx_mcs_map;
	__le16 rx_highest;
	__le16 tx_mcs_map;
	__le16 tx_highest;
} __packed;


#define IEEE80211_VHT_MAX_NSTS_TOTAL_SHIFT	13
#define IEEE80211_VHT_MAX_NSTS_TOTAL_MASK	(7 << IEEE80211_VHT_MAX_NSTS_TOTAL_SHIFT)


#define IEEE80211_VHT_EXT_NSS_BW_CAPABLE	(1 << 13)


enum ieee80211_vht_mcs_support {
	IEEE80211_VHT_MCS_SUPPORT_0_7	= 0,
	IEEE80211_VHT_MCS_SUPPORT_0_8	= 1,
	IEEE80211_VHT_MCS_SUPPORT_0_9	= 2,
	IEEE80211_VHT_MCS_NOT_SUPPORTED	= 3,
};


struct ieee80211_vht_cap {
	__le32 vht_cap_info;
	struct ieee80211_vht_mcs_info supp_mcs;
} __packed;


enum ieee80211_vht_chanwidth {
	IEEE80211_VHT_CHANWIDTH_USE_HT		= 0,
	IEEE80211_VHT_CHANWIDTH_80MHZ		= 1,
	IEEE80211_VHT_CHANWIDTH_160MHZ		= 2,
	IEEE80211_VHT_CHANWIDTH_80P80MHZ	= 3,
};


struct ieee80211_vht_operation {
	u8 chan_width;
	u8 center_freq_seg0_idx;
	u8 center_freq_seg1_idx;
	__le16 basic_mcs_set;
} __packed;


struct ieee80211_he_cap_elem {
	u8 mac_cap_info[6];
	u8 phy_cap_info[11];
} __packed;

#define IEEE80211_TX_RX_MCS_NSS_DESC_MAX_LEN	5


enum ieee80211_he_mcs_support {
	IEEE80211_HE_MCS_SUPPORT_0_7	= 0,
	IEEE80211_HE_MCS_SUPPORT_0_9	= 1,
	IEEE80211_HE_MCS_SUPPORT_0_11	= 2,
	IEEE80211_HE_MCS_NOT_SUPPORTED	= 3,
};


struct ieee80211_he_mcs_nss_supp {
	__le16 rx_mcs_80;
	__le16 tx_mcs_80;
	__le16 rx_mcs_160;
	__le16 tx_mcs_160;
	__le16 rx_mcs_80p80;
	__le16 tx_mcs_80p80;
} __packed;


struct ieee80211_he_operation {
	__le32 he_oper_params;
	__le16 he_mcs_nss_set;
	u8 optional[];
} __packed;


struct ieee80211_he_spr {
	u8 he_sr_control;
	u8 optional[];
} __packed;


struct ieee80211_he_mu_edca_param_ac_rec {
	u8 aifsn;
	u8 ecw_min_max;
	u8 mu_edca_timer;
} __packed;


struct ieee80211_mu_edca_param_set {
	u8 mu_qos_info;
	struct ieee80211_he_mu_edca_param_ac_rec ac_be;
	struct ieee80211_he_mu_edca_param_ac_rec ac_bk;
	struct ieee80211_he_mu_edca_param_ac_rec ac_vi;
	struct ieee80211_he_mu_edca_param_ac_rec ac_vo;
} __packed;

#define IEEE80211_EHT_MCS_NSS_RX 0x0f
#define IEEE80211_EHT_MCS_NSS_TX 0xf0


struct ieee80211_eht_mcs_nss_supp_20mhz_only {
	union {
		struct {
			u8 rx_tx_mcs7_max_nss;
			u8 rx_tx_mcs9_max_nss;
			u8 rx_tx_mcs11_max_nss;
			u8 rx_tx_mcs13_max_nss;
		};
		u8 rx_tx_max_nss[4];
	};
};


struct ieee80211_eht_mcs_nss_supp_bw {
	union {
		struct {
			u8 rx_tx_mcs9_max_nss;
			u8 rx_tx_mcs11_max_nss;
			u8 rx_tx_mcs13_max_nss;
		};
		u8 rx_tx_max_nss[3];
	};
};


struct ieee80211_eht_cap_elem_fixed {
	u8 mac_cap_info[2];
	u8 phy_cap_info[9];
} __packed;


struct ieee80211_eht_cap_elem {
	struct ieee80211_eht_cap_elem_fixed fixed;

	
	u8 optional[];
} __packed;

#define IEEE80211_EHT_OPER_INFO_PRESENT	                        0x01
#define IEEE80211_EHT_OPER_DISABLED_SUBCHANNEL_BITMAP_PRESENT	0x02
#define IEEE80211_EHT_OPER_EHT_DEF_PE_DURATION	                0x04
#define IEEE80211_EHT_OPER_GROUP_ADDRESSED_BU_IND_LIMIT         0x08
#define IEEE80211_EHT_OPER_GROUP_ADDRESSED_BU_IND_EXP_MASK      0x30


struct ieee80211_eht_operation {
	u8 params;
	struct ieee80211_eht_mcs_nss_supp_20mhz_only basic_mcs_nss;
	u8 optional[];
} __packed;


struct ieee80211_eht_operation_info {
	u8 control;
	u8 ccfs0;
	u8 ccfs1;
	u8 optional[];
} __packed;


#define IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_3895			0x00000000
#define IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_7991			0x00000001
#define IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_11454			0x00000002
#define IEEE80211_VHT_CAP_MAX_MPDU_MASK				0x00000003
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ		0x00000004
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ	0x00000008
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK			0x0000000C
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_SHIFT			2
#define IEEE80211_VHT_CAP_RXLDPC				0x00000010
#define IEEE80211_VHT_CAP_SHORT_GI_80				0x00000020
#define IEEE80211_VHT_CAP_SHORT_GI_160				0x00000040
#define IEEE80211_VHT_CAP_TXSTBC				0x00000080
#define IEEE80211_VHT_CAP_RXSTBC_1				0x00000100
#define IEEE80211_VHT_CAP_RXSTBC_2				0x00000200
#define IEEE80211_VHT_CAP_RXSTBC_3				0x00000300
#define IEEE80211_VHT_CAP_RXSTBC_4				0x00000400
#define IEEE80211_VHT_CAP_RXSTBC_MASK				0x00000700
#define IEEE80211_VHT_CAP_RXSTBC_SHIFT				8
#define IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE			0x00000800
#define IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE			0x00001000
#define IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT                  13
#define IEEE80211_VHT_CAP_BEAMFORMEE_STS_MASK			\
		(7 << IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT)
#define IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_SHIFT		16
#define IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_MASK		\
		(7 << IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_SHIFT)
#define IEEE80211_VHT_CAP_MU_BEAMFORMER_CAPABLE			0x00080000
#define IEEE80211_VHT_CAP_MU_BEAMFORMEE_CAPABLE			0x00100000
#define IEEE80211_VHT_CAP_VHT_TXOP_PS				0x00200000
#define IEEE80211_VHT_CAP_HTC_VHT				0x00400000
#define IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT	23
#define IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MASK	\
		(7 << IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT)
#define IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_UNSOL_MFB	0x08000000
#define IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB	0x0c000000
#define IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN			0x10000000
#define IEEE80211_VHT_CAP_TX_ANTENNA_PATTERN			0x20000000
#define IEEE80211_VHT_CAP_EXT_NSS_BW_SHIFT			30
#define IEEE80211_VHT_CAP_EXT_NSS_BW_MASK			0xc0000000


int ieee80211_get_vht_max_nss(struct ieee80211_vht_cap *cap,
			      enum ieee80211_vht_chanwidth bw,
			      int mcs, bool ext_nss_bw_capable,
			      unsigned int max_vht_nss);


enum ieee80211_ap_reg_power {
	IEEE80211_REG_UNSET_AP,
	IEEE80211_REG_LPI_AP,
	IEEE80211_REG_SP_AP,
	IEEE80211_REG_VLP_AP,
	IEEE80211_REG_AP_POWER_AFTER_LAST,
	IEEE80211_REG_AP_POWER_MAX =
		IEEE80211_REG_AP_POWER_AFTER_LAST - 1,
};


enum ieee80211_client_reg_power {
	IEEE80211_REG_UNSET_CLIENT,
	IEEE80211_REG_DEFAULT_CLIENT,
	IEEE80211_REG_SUBORDINATE_CLIENT,
	IEEE80211_REG_CLIENT_POWER_AFTER_LAST,
	IEEE80211_REG_CLIENT_POWER_MAX =
		IEEE80211_REG_CLIENT_POWER_AFTER_LAST - 1,
};


#define IEEE80211_HE_MAC_CAP0_HTC_HE				0x01
#define IEEE80211_HE_MAC_CAP0_TWT_REQ				0x02
#define IEEE80211_HE_MAC_CAP0_TWT_RES				0x04
#define IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_NOT_SUPP		0x00
#define IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_LEVEL_1		0x08
#define IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_LEVEL_2		0x10
#define IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_LEVEL_3		0x18
#define IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_MASK			0x18
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_1		0x00
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_2		0x20
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_4		0x40
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_8		0x60
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_16		0x80
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_32		0xa0
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_64		0xc0
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_UNLIMITED	0xe0
#define IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_MASK		0xe0

#define IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_UNLIMITED		0x00
#define IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_128			0x01
#define IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_256			0x02
#define IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_512			0x03
#define IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_MASK		0x03
#define IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_0US		0x00
#define IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_8US		0x04
#define IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_16US		0x08
#define IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_MASK		0x0c
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_1		0x00
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_2		0x10
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_3		0x20
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_4		0x30
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_5		0x40
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_6		0x50
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_7		0x60
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_8		0x70
#define IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_MASK		0x70


#define IEEE80211_HE_MAC_CAP1_LINK_ADAPTATION			0x80

#define IEEE80211_HE_MAC_CAP2_LINK_ADAPTATION			0x01
#define IEEE80211_HE_MAC_CAP2_ALL_ACK				0x02
#define IEEE80211_HE_MAC_CAP2_TRS				0x04
#define IEEE80211_HE_MAC_CAP2_BSR				0x08
#define IEEE80211_HE_MAC_CAP2_BCAST_TWT				0x10
#define IEEE80211_HE_MAC_CAP2_32BIT_BA_BITMAP			0x20
#define IEEE80211_HE_MAC_CAP2_MU_CASCADING			0x40
#define IEEE80211_HE_MAC_CAP2_ACK_EN				0x80

#define IEEE80211_HE_MAC_CAP3_OMI_CONTROL			0x02
#define IEEE80211_HE_MAC_CAP3_OFDMA_RA				0x04


#define IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_0		0x00
#define IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_1		0x08
#define IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_2		0x10
#define IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT_3		0x18
#define IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_MASK		0x18
#define IEEE80211_HE_MAC_CAP3_AMSDU_FRAG			0x20
#define IEEE80211_HE_MAC_CAP3_FLEX_TWT_SCHED			0x40
#define IEEE80211_HE_MAC_CAP3_RX_CTRL_FRAME_TO_MULTIBSS		0x80

#define IEEE80211_HE_MAC_CAP4_BSRP_BQRP_A_MPDU_AGG		0x01
#define IEEE80211_HE_MAC_CAP4_QTP				0x02
#define IEEE80211_HE_MAC_CAP4_BQR				0x04
#define IEEE80211_HE_MAC_CAP4_PSR_RESP				0x08
#define IEEE80211_HE_MAC_CAP4_NDP_FB_REP			0x10
#define IEEE80211_HE_MAC_CAP4_OPS				0x20
#define IEEE80211_HE_MAC_CAP4_AMSDU_IN_AMPDU			0x40

#define IEEE80211_HE_MAC_CAP4_MULTI_TID_AGG_TX_QOS_B39		0x80

#define IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B40		0x01
#define IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B41		0x02
#define IEEE80211_HE_MAC_CAP5_SUBCHAN_SELECTIVE_TRANSMISSION	0x04
#define IEEE80211_HE_MAC_CAP5_UL_2x996_TONE_RU			0x08
#define IEEE80211_HE_MAC_CAP5_OM_CTRL_UL_MU_DATA_DIS_RX		0x10
#define IEEE80211_HE_MAC_CAP5_HE_DYNAMIC_SM_PS			0x20
#define IEEE80211_HE_MAC_CAP5_PUNCTURED_SOUNDING		0x40
#define IEEE80211_HE_MAC_CAP5_HT_VHT_TRIG_FRAME_RX		0x80

#define IEEE80211_HE_VHT_MAX_AMPDU_FACTOR	20
#define IEEE80211_HE_HT_MAX_AMPDU_FACTOR	16
#define IEEE80211_HE_6GHZ_MAX_AMPDU_FACTOR	13


#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G		0x02
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G	0x04
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G		0x08
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G	0x10
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_MASK_ALL		0x1e

#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_RU_MAPPING_IN_2G	0x20
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_RU_MAPPING_IN_5G	0x40
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_MASK			0xfe

#define IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_80MHZ_ONLY_SECOND_20MHZ	0x01
#define IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_80MHZ_ONLY_SECOND_40MHZ	0x02
#define IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_160MHZ_ONLY_SECOND_20MHZ	0x04
#define IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_160MHZ_ONLY_SECOND_40MHZ	0x08
#define IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_MASK			0x0f
#define IEEE80211_HE_PHY_CAP1_DEVICE_CLASS_A				0x10
#define IEEE80211_HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD			0x20
#define IEEE80211_HE_PHY_CAP1_HE_LTF_AND_GI_FOR_HE_PPDUS_0_8US		0x40

#define IEEE80211_HE_PHY_CAP1_MIDAMBLE_RX_TX_MAX_NSTS			0x80

#define IEEE80211_HE_PHY_CAP2_MIDAMBLE_RX_TX_MAX_NSTS			0x01
#define IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US			0x02
#define IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ			0x04
#define IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ			0x08
#define IEEE80211_HE_PHY_CAP2_DOPPLER_TX				0x10
#define IEEE80211_HE_PHY_CAP2_DOPPLER_RX				0x20


#define IEEE80211_HE_PHY_CAP2_UL_MU_FULL_MU_MIMO			0x40
#define IEEE80211_HE_PHY_CAP2_UL_MU_PARTIAL_MU_MIMO			0x80

#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_NO_DCM			0x00
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_BPSK			0x01
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_QPSK			0x02
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_16_QAM			0x03
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK			0x03
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_TX_NSS_1				0x00
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_TX_NSS_2				0x04
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_NO_DCM			0x00
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_BPSK			0x08
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_QPSK			0x10
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_16_QAM			0x18
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK			0x18
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_RX_NSS_1				0x00
#define IEEE80211_HE_PHY_CAP3_DCM_MAX_RX_NSS_2				0x20
#define IEEE80211_HE_PHY_CAP3_RX_PARTIAL_BW_SU_IN_20MHZ_MU		0x40
#define IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER				0x80

#define IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE				0x01
#define IEEE80211_HE_PHY_CAP4_MU_BEAMFORMER				0x02


#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_4		0x0c
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_5		0x10
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_6		0x14
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_7		0x18
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_8		0x1c
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_MASK	0x1c


#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_4		0x60
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_5		0x80
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_6		0xa0
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_7		0xc0
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_8		0xe0
#define IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_MASK	0xe0

#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_1	0x00
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_2	0x01
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_3	0x02
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_4	0x03
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_5	0x04
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_6	0x05
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_7	0x06
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_8	0x07
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_MASK	0x07

#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_1	0x00
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_2	0x08
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_3	0x10
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_4	0x18
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_5	0x20
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_6	0x28
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_7	0x30
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_8	0x38
#define IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_MASK	0x38

#define IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK				0x40
#define IEEE80211_HE_PHY_CAP5_NG16_MU_FEEDBACK				0x80

#define IEEE80211_HE_PHY_CAP6_CODEBOOK_SIZE_42_SU			0x01
#define IEEE80211_HE_PHY_CAP6_CODEBOOK_SIZE_75_MU			0x02
#define IEEE80211_HE_PHY_CAP6_TRIG_SU_BEAMFORMING_FB			0x04
#define IEEE80211_HE_PHY_CAP6_TRIG_MU_BEAMFORMING_PARTIAL_BW_FB		0x08
#define IEEE80211_HE_PHY_CAP6_TRIG_CQI_FB				0x10
#define IEEE80211_HE_PHY_CAP6_PARTIAL_BW_EXT_RANGE			0x20
#define IEEE80211_HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MUMIMO		0x40
#define IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT			0x80

#define IEEE80211_HE_PHY_CAP7_PSR_BASED_SR				0x01
#define IEEE80211_HE_PHY_CAP7_POWER_BOOST_FACTOR_SUPP			0x02
#define IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI		0x04
#define IEEE80211_HE_PHY_CAP7_MAX_NC_1					0x08
#define IEEE80211_HE_PHY_CAP7_MAX_NC_2					0x10
#define IEEE80211_HE_PHY_CAP7_MAX_NC_3					0x18
#define IEEE80211_HE_PHY_CAP7_MAX_NC_4					0x20
#define IEEE80211_HE_PHY_CAP7_MAX_NC_5					0x28
#define IEEE80211_HE_PHY_CAP7_MAX_NC_6					0x30
#define IEEE80211_HE_PHY_CAP7_MAX_NC_7					0x38
#define IEEE80211_HE_PHY_CAP7_MAX_NC_MASK				0x38
#define IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ			0x40
#define IEEE80211_HE_PHY_CAP7_STBC_RX_ABOVE_80MHZ			0x80

#define IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI		0x01
#define IEEE80211_HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_IN_2G		0x02
#define IEEE80211_HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU			0x04
#define IEEE80211_HE_PHY_CAP8_80MHZ_IN_160MHZ_HE_PPDU			0x08
#define IEEE80211_HE_PHY_CAP8_HE_ER_SU_1XLTF_AND_08_US_GI		0x10
#define IEEE80211_HE_PHY_CAP8_MIDAMBLE_RX_TX_2X_AND_1XLTF		0x20
#define IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_242				0x00
#define IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_484				0x40
#define IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_996				0x80
#define IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_2x996				0xc0
#define IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_MASK				0xc0

#define IEEE80211_HE_PHY_CAP9_LONGER_THAN_16_SIGB_OFDM_SYM		0x01
#define IEEE80211_HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK		0x02
#define IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU		0x04
#define IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU		0x08
#define IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_COMP_SIGB	0x10
#define IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_NON_COMP_SIGB	0x20
#define IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_0US			0x0
#define IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_8US			0x1
#define IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_16US			0x2
#define IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_RESERVED		0x3
#define IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_POS			6
#define IEEE80211_HE_PHY_CAP9_NOMINAL_PKT_PADDING_MASK			0xc0

#define IEEE80211_HE_PHY_CAP10_HE_MU_M1RU_MAX_LTF			0x01


#define IEEE80211_TX_RX_MCS_NSS_SUPP_HIGHEST_MCS_POS			(3)
#define IEEE80211_TX_RX_MCS_NSS_SUPP_TX_BITMAP_POS			(6)
#define IEEE80211_TX_RX_MCS_NSS_SUPP_RX_BITMAP_POS			(11)
#define IEEE80211_TX_RX_MCS_NSS_SUPP_TX_BITMAP_MASK			0x07c0
#define IEEE80211_TX_RX_MCS_NSS_SUPP_RX_BITMAP_MASK			0xf800


enum ieee80211_he_highest_mcs_supported_subfield_enc {
	HIGHEST_MCS_SUPPORTED_MCS7 = 0,
	HIGHEST_MCS_SUPPORTED_MCS8,
	HIGHEST_MCS_SUPPORTED_MCS9,
	HIGHEST_MCS_SUPPORTED_MCS10,
	HIGHEST_MCS_SUPPORTED_MCS11,
};


static inline u8
ieee80211_he_mcs_nss_size(const struct ieee80211_he_cap_elem *he_cap)
{
	u8 count = 4;

	if (he_cap->phy_cap_info[0] &
	    IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G)
		count += 4;

	if (he_cap->phy_cap_info[0] &
	    IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G)
		count += 4;

	return count;
}


#define IEEE80211_PPE_THRES_NSS_SUPPORT_2NSS			(1)
#define IEEE80211_PPE_THRES_NSS_POS				(0)
#define IEEE80211_PPE_THRES_NSS_MASK				(7)
#define IEEE80211_PPE_THRES_RU_INDEX_BITMASK_2x966_AND_966_RU	\
	(BIT(5) | BIT(6))
#define IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK		0x78
#define IEEE80211_PPE_THRES_RU_INDEX_BITMASK_POS		(3)
#define IEEE80211_PPE_THRES_INFO_PPET_SIZE			(3)
#define IEEE80211_HE_PPE_THRES_INFO_HEADER_SIZE			(7)


static inline u8
ieee80211_he_ppe_size(u8 ppe_thres_hdr, const u8 *phy_cap_info)
{
	u8 n;

	if ((phy_cap_info[6] &
	     IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) == 0)
		return 0;

	n = hweight8(ppe_thres_hdr &
		     IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK);
	n *= (1 + ((ppe_thres_hdr & IEEE80211_PPE_THRES_NSS_MASK) >>
		   IEEE80211_PPE_THRES_NSS_POS));

	
	n = (n * IEEE80211_PPE_THRES_INFO_PPET_SIZE * 2) + 7;
	n = DIV_ROUND_UP(n, 8);

	return n;
}

static inline bool ieee80211_he_capa_size_ok(const u8 *data, u8 len)
{
	const struct ieee80211_he_cap_elem *he_cap_ie_elem = (const void *)data;
	u8 needed = sizeof(*he_cap_ie_elem);

	if (len < needed)
		return false;

	needed += ieee80211_he_mcs_nss_size(he_cap_ie_elem);
	if (len < needed)
		return false;

	if (he_cap_ie_elem->phy_cap_info[6] &
			IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT) {
		if (len < needed + 1)
			return false;
		needed += ieee80211_he_ppe_size(data[needed],
						he_cap_ie_elem->phy_cap_info);
	}

	return len >= needed;
}


#define IEEE80211_HE_OPERATION_DFLT_PE_DURATION_MASK		0x00000007
#define IEEE80211_HE_OPERATION_TWT_REQUIRED			0x00000008
#define IEEE80211_HE_OPERATION_RTS_THRESHOLD_MASK		0x00003ff0
#define IEEE80211_HE_OPERATION_RTS_THRESHOLD_OFFSET		4
#define IEEE80211_HE_OPERATION_VHT_OPER_INFO			0x00004000
#define IEEE80211_HE_OPERATION_CO_HOSTED_BSS			0x00008000
#define IEEE80211_HE_OPERATION_ER_SU_DISABLE			0x00010000
#define IEEE80211_HE_OPERATION_6GHZ_OP_INFO			0x00020000
#define IEEE80211_HE_OPERATION_BSS_COLOR_MASK			0x3f000000
#define IEEE80211_HE_OPERATION_BSS_COLOR_OFFSET			24
#define IEEE80211_HE_OPERATION_PARTIAL_BSS_COLOR		0x40000000
#define IEEE80211_HE_OPERATION_BSS_COLOR_DISABLED		0x80000000

#define IEEE80211_6GHZ_CTRL_REG_LPI_AP	0
#define IEEE80211_6GHZ_CTRL_REG_SP_AP	1


struct ieee80211_he_6ghz_oper {
	u8 primary;
#define IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH	0x3
#define		IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_20MHZ	0
#define		IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_40MHZ	1
#define		IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_80MHZ	2
#define		IEEE80211_HE_6GHZ_OPER_CTRL_CHANWIDTH_160MHZ	3
#define IEEE80211_HE_6GHZ_OPER_CTRL_DUP_BEACON	0x4
#define IEEE80211_HE_6GHZ_OPER_CTRL_REG_INFO	0x38
	u8 control;
	u8 ccfs0;
	u8 ccfs1;
	u8 minrate;
} __packed;


#define IEEE80211_TPE_MAX_IE_COUNT	8

#define IEEE80211_MAX_NUM_PWR_LEVEL	8

#define IEEE80211_TPE_MAX_POWER_COUNT	8


enum ieee80211_tx_power_intrpt_type {
	IEEE80211_TPE_LOCAL_EIRP,
	IEEE80211_TPE_LOCAL_EIRP_PSD,
	IEEE80211_TPE_REG_CLIENT_EIRP,
	IEEE80211_TPE_REG_CLIENT_EIRP_PSD,
};


struct ieee80211_tx_pwr_env {
	u8 tx_power_info;
	s8 tx_power[IEEE80211_TPE_MAX_POWER_COUNT];
} __packed;

#define IEEE80211_TX_PWR_ENV_INFO_COUNT 0x7
#define IEEE80211_TX_PWR_ENV_INFO_INTERPRET 0x38
#define IEEE80211_TX_PWR_ENV_INFO_CATEGORY 0xC0


static inline u8
ieee80211_he_oper_size(const u8 *he_oper_ie)
{
	const struct ieee80211_he_operation *he_oper = (const void *)he_oper_ie;
	u8 oper_len = sizeof(struct ieee80211_he_operation);
	u32 he_oper_params;

	
	if (!he_oper_ie)
		return 0;

	
	he_oper_params = le32_to_cpu(he_oper->he_oper_params);
	if (he_oper_params & IEEE80211_HE_OPERATION_VHT_OPER_INFO)
		oper_len += 3;
	if (he_oper_params & IEEE80211_HE_OPERATION_CO_HOSTED_BSS)
		oper_len++;
	if (he_oper_params & IEEE80211_HE_OPERATION_6GHZ_OP_INFO)
		oper_len += sizeof(struct ieee80211_he_6ghz_oper);

	
	oper_len++;

	return oper_len;
}


static inline const struct ieee80211_he_6ghz_oper *
ieee80211_he_6ghz_oper(const struct ieee80211_he_operation *he_oper)
{
	const u8 *ret;
	u32 he_oper_params;

	if (!he_oper)
		return NULL;

	ret = (const void *)&he_oper->optional;

	he_oper_params = le32_to_cpu(he_oper->he_oper_params);

	if (!(he_oper_params & IEEE80211_HE_OPERATION_6GHZ_OP_INFO))
		return NULL;
	if (he_oper_params & IEEE80211_HE_OPERATION_VHT_OPER_INFO)
		ret += 3;
	if (he_oper_params & IEEE80211_HE_OPERATION_CO_HOSTED_BSS)
		ret++;

	return (const void *)ret;
}


#define IEEE80211_HE_SPR_PSR_DISALLOWED				BIT(0)
#define IEEE80211_HE_SPR_NON_SRG_OBSS_PD_SR_DISALLOWED		BIT(1)
#define IEEE80211_HE_SPR_NON_SRG_OFFSET_PRESENT			BIT(2)
#define IEEE80211_HE_SPR_SRG_INFORMATION_PRESENT		BIT(3)
#define IEEE80211_HE_SPR_HESIGA_SR_VAL15_ALLOWED		BIT(4)


static inline u8
ieee80211_he_spr_size(const u8 *he_spr_ie)
{
	const struct ieee80211_he_spr *he_spr = (const void *)he_spr_ie;
	u8 spr_len = sizeof(struct ieee80211_he_spr);
	u8 he_spr_params;

	
	if (!he_spr_ie)
		return 0;

	
	he_spr_params = he_spr->he_sr_control;
	if (he_spr_params & IEEE80211_HE_SPR_NON_SRG_OFFSET_PRESENT)
		spr_len++;
	if (he_spr_params & IEEE80211_HE_SPR_SRG_INFORMATION_PRESENT)
		spr_len += 18;

	
	spr_len++;

	return spr_len;
}


#define IEEE80211_S1G_CAPABILITY_LEN	15

#define S1G_CAP0_S1G_LONG	BIT(0)
#define S1G_CAP0_SGI_1MHZ	BIT(1)
#define S1G_CAP0_SGI_2MHZ	BIT(2)
#define S1G_CAP0_SGI_4MHZ	BIT(3)
#define S1G_CAP0_SGI_8MHZ	BIT(4)
#define S1G_CAP0_SGI_16MHZ	BIT(5)
#define S1G_CAP0_SUPP_CH_WIDTH	GENMASK(7, 6)

#define S1G_SUPP_CH_WIDTH_2	0
#define S1G_SUPP_CH_WIDTH_4	1
#define S1G_SUPP_CH_WIDTH_8	2
#define S1G_SUPP_CH_WIDTH_16	3
#define S1G_SUPP_CH_WIDTH_MAX(cap) ((1 << FIELD_GET(S1G_CAP0_SUPP_CH_WIDTH, \
						    cap[0])) << 1)

#define S1G_CAP1_RX_LDPC	BIT(0)
#define S1G_CAP1_TX_STBC	BIT(1)
#define S1G_CAP1_RX_STBC	BIT(2)
#define S1G_CAP1_SU_BFER	BIT(3)
#define S1G_CAP1_SU_BFEE	BIT(4)
#define S1G_CAP1_BFEE_STS	GENMASK(7, 5)

#define S1G_CAP2_SOUNDING_DIMENSIONS	GENMASK(2, 0)
#define S1G_CAP2_MU_BFER		BIT(3)
#define S1G_CAP2_MU_BFEE		BIT(4)
#define S1G_CAP2_PLUS_HTC_VHT		BIT(5)
#define S1G_CAP2_TRAVELING_PILOT	GENMASK(7, 6)

#define S1G_CAP3_RD_RESPONDER		BIT(0)
#define S1G_CAP3_HT_DELAYED_BA		BIT(1)
#define S1G_CAP3_MAX_MPDU_LEN		BIT(2)
#define S1G_CAP3_MAX_AMPDU_LEN_EXP	GENMASK(4, 3)
#define S1G_CAP3_MIN_MPDU_START		GENMASK(7, 5)

#define S1G_CAP4_UPLINK_SYNC	BIT(0)
#define S1G_CAP4_DYNAMIC_AID	BIT(1)
#define S1G_CAP4_BAT		BIT(2)
#define S1G_CAP4_TIME_ADE	BIT(3)
#define S1G_CAP4_NON_TIM	BIT(4)
#define S1G_CAP4_GROUP_AID	BIT(5)
#define S1G_CAP4_STA_TYPE	GENMASK(7, 6)

#define S1G_CAP5_CENT_AUTH_CONTROL	BIT(0)
#define S1G_CAP5_DIST_AUTH_CONTROL	BIT(1)
#define S1G_CAP5_AMSDU			BIT(2)
#define S1G_CAP5_AMPDU			BIT(3)
#define S1G_CAP5_ASYMMETRIC_BA		BIT(4)
#define S1G_CAP5_FLOW_CONTROL		BIT(5)
#define S1G_CAP5_SECTORIZED_BEAM	GENMASK(7, 6)

#define S1G_CAP6_OBSS_MITIGATION	BIT(0)
#define S1G_CAP6_FRAGMENT_BA		BIT(1)
#define S1G_CAP6_NDP_PS_POLL		BIT(2)
#define S1G_CAP6_RAW_OPERATION		BIT(3)
#define S1G_CAP6_PAGE_SLICING		BIT(4)
#define S1G_CAP6_TXOP_SHARING_IMP_ACK	BIT(5)
#define S1G_CAP6_VHT_LINK_ADAPT		GENMASK(7, 6)

#define S1G_CAP7_TACK_AS_PS_POLL		BIT(0)
#define S1G_CAP7_DUP_1MHZ			BIT(1)
#define S1G_CAP7_MCS_NEGOTIATION		BIT(2)
#define S1G_CAP7_1MHZ_CTL_RESPONSE_PREAMBLE	BIT(3)
#define S1G_CAP7_NDP_BFING_REPORT_POLL		BIT(4)
#define S1G_CAP7_UNSOLICITED_DYN_AID		BIT(5)
#define S1G_CAP7_SECTOR_TRAINING_OPERATION	BIT(6)
#define S1G_CAP7_TEMP_PS_MODE_SWITCH		BIT(7)

#define S1G_CAP8_TWT_GROUPING	BIT(0)
#define S1G_CAP8_BDT		BIT(1)
#define S1G_CAP8_COLOR		GENMASK(4, 2)
#define S1G_CAP8_TWT_REQUEST	BIT(5)
#define S1G_CAP8_TWT_RESPOND	BIT(6)
#define S1G_CAP8_PV1_FRAME	BIT(7)

#define S1G_CAP9_LINK_ADAPT_PER_CONTROL_RESPONSE BIT(0)

#define S1G_OPER_CH_WIDTH_PRIMARY_1MHZ	BIT(0)
#define S1G_OPER_CH_WIDTH_OPER		GENMASK(4, 1)


#define IEEE80211_EHT_MAC_CAP0_EPCS_PRIO_ACCESS			0x01
#define IEEE80211_EHT_MAC_CAP0_OM_CONTROL			0x02
#define IEEE80211_EHT_MAC_CAP0_TRIG_TXOP_SHARING_MODE1		0x04
#define IEEE80211_EHT_MAC_CAP0_TRIG_TXOP_SHARING_MODE2		0x08
#define IEEE80211_EHT_MAC_CAP0_RESTRICTED_TWT			0x10
#define IEEE80211_EHT_MAC_CAP0_SCS_TRAFFIC_DESC			0x20
#define IEEE80211_EHT_MAC_CAP0_MAX_MPDU_LEN_MASK		0xc0
#define	IEEE80211_EHT_MAC_CAP0_MAX_MPDU_LEN_3895	        0
#define	IEEE80211_EHT_MAC_CAP0_MAX_MPDU_LEN_7991	        1
#define	IEEE80211_EHT_MAC_CAP0_MAX_MPDU_LEN_11454	        2

#define IEEE80211_EHT_MAC_CAP1_MAX_AMPDU_LEN_MASK		0x01


#define IEEE80211_EHT_PHY_CAP0_320MHZ_IN_6GHZ			0x02
#define IEEE80211_EHT_PHY_CAP0_242_TONE_RU_GT20MHZ		0x04
#define IEEE80211_EHT_PHY_CAP0_NDP_4_EHT_LFT_32_GI		0x08
#define IEEE80211_EHT_PHY_CAP0_PARTIAL_BW_UL_MU_MIMO		0x10
#define IEEE80211_EHT_PHY_CAP0_SU_BEAMFORMER			0x20
#define IEEE80211_EHT_PHY_CAP0_SU_BEAMFORMEE			0x40


#define IEEE80211_EHT_PHY_CAP0_BEAMFORMEE_SS_80MHZ_MASK		0x80
#define IEEE80211_EHT_PHY_CAP1_BEAMFORMEE_SS_80MHZ_MASK		0x03

#define IEEE80211_EHT_PHY_CAP1_BEAMFORMEE_SS_160MHZ_MASK	0x1c
#define IEEE80211_EHT_PHY_CAP1_BEAMFORMEE_SS_320MHZ_MASK	0xe0

#define IEEE80211_EHT_PHY_CAP2_SOUNDING_DIM_80MHZ_MASK		0x07
#define IEEE80211_EHT_PHY_CAP2_SOUNDING_DIM_160MHZ_MASK		0x38


#define IEEE80211_EHT_PHY_CAP2_SOUNDING_DIM_320MHZ_MASK		0xc0
#define IEEE80211_EHT_PHY_CAP3_SOUNDING_DIM_320MHZ_MASK		0x01
#define IEEE80211_EHT_PHY_CAP3_NG_16_SU_FEEDBACK		0x02
#define IEEE80211_EHT_PHY_CAP3_NG_16_MU_FEEDBACK		0x04
#define IEEE80211_EHT_PHY_CAP3_CODEBOOK_4_2_SU_FDBK		0x08
#define IEEE80211_EHT_PHY_CAP3_CODEBOOK_7_5_MU_FDBK		0x10
#define IEEE80211_EHT_PHY_CAP3_TRIG_SU_BF_FDBK			0x20
#define IEEE80211_EHT_PHY_CAP3_TRIG_MU_BF_PART_BW_FDBK		0x40
#define IEEE80211_EHT_PHY_CAP3_TRIG_CQI_FDBK			0x80

#define IEEE80211_EHT_PHY_CAP4_PART_BW_DL_MU_MIMO		0x01
#define IEEE80211_EHT_PHY_CAP4_PSR_SR_SUPP			0x02
#define IEEE80211_EHT_PHY_CAP4_POWER_BOOST_FACT_SUPP		0x04
#define IEEE80211_EHT_PHY_CAP4_EHT_MU_PPDU_4_EHT_LTF_08_GI	0x08
#define IEEE80211_EHT_PHY_CAP4_MAX_NC_MASK			0xf0

#define IEEE80211_EHT_PHY_CAP5_NON_TRIG_CQI_FEEDBACK		0x01
#define IEEE80211_EHT_PHY_CAP5_TX_LESS_242_TONE_RU_SUPP		0x02
#define IEEE80211_EHT_PHY_CAP5_RX_LESS_242_TONE_RU_SUPP		0x04
#define IEEE80211_EHT_PHY_CAP5_PPE_THRESHOLD_PRESENT		0x08
#define IEEE80211_EHT_PHY_CAP5_COMMON_NOMINAL_PKT_PAD_MASK	0x30
#define   IEEE80211_EHT_PHY_CAP5_COMMON_NOMINAL_PKT_PAD_0US	0
#define   IEEE80211_EHT_PHY_CAP5_COMMON_NOMINAL_PKT_PAD_8US	1
#define   IEEE80211_EHT_PHY_CAP5_COMMON_NOMINAL_PKT_PAD_16US	2
#define   IEEE80211_EHT_PHY_CAP5_COMMON_NOMINAL_PKT_PAD_20US	3


#define IEEE80211_EHT_PHY_CAP5_MAX_NUM_SUPP_EHT_LTF_MASK	0xc0
#define IEEE80211_EHT_PHY_CAP5_SUPP_EXTRA_EHT_LTF		0x40
#define IEEE80211_EHT_PHY_CAP6_MAX_NUM_SUPP_EHT_LTF_MASK	0x07

#define IEEE80211_EHT_PHY_CAP6_MCS15_SUPP_MASK			0x78
#define IEEE80211_EHT_PHY_CAP6_EHT_DUP_6GHZ_SUPP		0x80

#define IEEE80211_EHT_PHY_CAP7_20MHZ_STA_RX_NDP_WIDER_BW	0x01
#define IEEE80211_EHT_PHY_CAP7_NON_OFDMA_UL_MU_MIMO_80MHZ	0x02
#define IEEE80211_EHT_PHY_CAP7_NON_OFDMA_UL_MU_MIMO_160MHZ	0x04
#define IEEE80211_EHT_PHY_CAP7_NON_OFDMA_UL_MU_MIMO_320MHZ	0x08
#define IEEE80211_EHT_PHY_CAP7_MU_BEAMFORMER_80MHZ		0x10
#define IEEE80211_EHT_PHY_CAP7_MU_BEAMFORMER_160MHZ		0x20
#define IEEE80211_EHT_PHY_CAP7_MU_BEAMFORMER_320MHZ		0x40
#define IEEE80211_EHT_PHY_CAP7_TB_SOUNDING_FDBK_RATE_LIMIT	0x80

#define IEEE80211_EHT_PHY_CAP8_RX_1024QAM_WIDER_BW_DL_OFDMA	0x01
#define IEEE80211_EHT_PHY_CAP8_RX_4096QAM_WIDER_BW_DL_OFDMA	0x02


#define IEEE80211_EHT_OPER_CHAN_WIDTH		0x7
#define IEEE80211_EHT_OPER_CHAN_WIDTH_20MHZ	0
#define IEEE80211_EHT_OPER_CHAN_WIDTH_40MHZ	1
#define IEEE80211_EHT_OPER_CHAN_WIDTH_80MHZ	2
#define IEEE80211_EHT_OPER_CHAN_WIDTH_160MHZ	3
#define IEEE80211_EHT_OPER_CHAN_WIDTH_320MHZ	4


static inline u8
ieee80211_eht_mcs_nss_size(const struct ieee80211_he_cap_elem *he_cap,
			   const struct ieee80211_eht_cap_elem_fixed *eht_cap,
			   bool from_ap)
{
	u8 count = 0;

	
	if (he_cap->phy_cap_info[0] &
	    IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G)
		return 3;

	
	if (he_cap->phy_cap_info[0] &
	    IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G)
		count += 3;

	if (he_cap->phy_cap_info[0] &
	    IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G)
		count += 3;

	if (eht_cap->phy_cap_info[0] & IEEE80211_EHT_PHY_CAP0_320MHZ_IN_6GHZ)
		count += 3;

	if (count)
		return count;

	return from_ap ? 3 : 4;
}


#define IEEE80211_EHT_PPE_THRES_NSS_POS			0
#define IEEE80211_EHT_PPE_THRES_NSS_MASK		0xf
#define IEEE80211_EHT_PPE_THRES_RU_INDEX_BITMASK_MASK	0x1f0
#define IEEE80211_EHT_PPE_THRES_INFO_PPET_SIZE		3
#define IEEE80211_EHT_PPE_THRES_INFO_HEADER_SIZE	9


static inline u8
ieee80211_eht_ppe_size(u16 ppe_thres_hdr, const u8 *phy_cap_info)
{
	u32 n;

	if (!(phy_cap_info[5] &
	      IEEE80211_EHT_PHY_CAP5_PPE_THRESHOLD_PRESENT))
		return 0;

	n = hweight16(ppe_thres_hdr &
		      IEEE80211_EHT_PPE_THRES_RU_INDEX_BITMASK_MASK);
	n *= 1 + u16_get_bits(ppe_thres_hdr, IEEE80211_EHT_PPE_THRES_NSS_MASK);

	
	n = n * IEEE80211_EHT_PPE_THRES_INFO_PPET_SIZE * 2 +
	    IEEE80211_EHT_PPE_THRES_INFO_HEADER_SIZE;
	return DIV_ROUND_UP(n, 8);
}

static inline bool
ieee80211_eht_capa_size_ok(const u8 *he_capa, const u8 *data, u8 len,
			   bool from_ap)
{
	const struct ieee80211_eht_cap_elem_fixed *elem = (const void *)data;
	u8 needed = sizeof(struct ieee80211_eht_cap_elem_fixed);

	if (len < needed || !he_capa)
		return false;

	needed += ieee80211_eht_mcs_nss_size((const void *)he_capa,
					     (const void *)data,
					     from_ap);
	if (len < needed)
		return false;

	if (elem->phy_cap_info[5] &
			IEEE80211_EHT_PHY_CAP5_PPE_THRESHOLD_PRESENT) {
		u16 ppe_thres_hdr;

		if (len < needed + sizeof(ppe_thres_hdr))
			return false;

		ppe_thres_hdr = get_unaligned_le16(data + needed);
		needed += ieee80211_eht_ppe_size(ppe_thres_hdr,
						 elem->phy_cap_info);
	}

	return len >= needed;
}

static inline bool
ieee80211_eht_oper_size_ok(const u8 *data, u8 len)
{
	const struct ieee80211_eht_operation *elem = (const void *)data;
	u8 needed = sizeof(*elem);

	if (len < needed)
		return false;

	if (elem->params & IEEE80211_EHT_OPER_INFO_PRESENT) {
		needed += 3;

		if (elem->params &
		    IEEE80211_EHT_OPER_DISABLED_SUBCHANNEL_BITMAP_PRESENT)
			needed += 2;
	}

	return len >= needed;
}

#define LISTEN_INT_USF	GENMASK(15, 14)
#define LISTEN_INT_UI	GENMASK(13, 0)

#define IEEE80211_MAX_USF	FIELD_MAX(LISTEN_INT_USF)
#define IEEE80211_MAX_UI	FIELD_MAX(LISTEN_INT_UI)


#define WLAN_AUTH_OPEN 0
#define WLAN_AUTH_SHARED_KEY 1
#define WLAN_AUTH_FT 2
#define WLAN_AUTH_SAE 3
#define WLAN_AUTH_FILS_SK 4
#define WLAN_AUTH_FILS_SK_PFS 5
#define WLAN_AUTH_FILS_PK 6
#define WLAN_AUTH_LEAP 128

#define WLAN_AUTH_CHALLENGE_LEN 128

#define WLAN_CAPABILITY_ESS		(1<<0)
#define WLAN_CAPABILITY_IBSS		(1<<1)


#define WLAN_CAPABILITY_IS_STA_BSS(cap)	\
	(!((cap) & (WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_IBSS)))

#define WLAN_CAPABILITY_CF_POLLABLE	(1<<2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST	(1<<3)
#define WLAN_CAPABILITY_PRIVACY		(1<<4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE	(1<<5)
#define WLAN_CAPABILITY_PBCC		(1<<6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY	(1<<7)


#define WLAN_CAPABILITY_SPECTRUM_MGMT	(1<<8)
#define WLAN_CAPABILITY_QOS		(1<<9)
#define WLAN_CAPABILITY_SHORT_SLOT_TIME	(1<<10)
#define WLAN_CAPABILITY_APSD		(1<<11)
#define WLAN_CAPABILITY_RADIO_MEASURE	(1<<12)
#define WLAN_CAPABILITY_DSSS_OFDM	(1<<13)
#define WLAN_CAPABILITY_DEL_BACK	(1<<14)
#define WLAN_CAPABILITY_IMM_BACK	(1<<15)



#define WLAN_CAPABILITY_DMG_TYPE_MASK		(3<<0)
#define WLAN_CAPABILITY_DMG_TYPE_IBSS		(1<<0) 
#define WLAN_CAPABILITY_DMG_TYPE_PBSS		(2<<0) 
#define WLAN_CAPABILITY_DMG_TYPE_AP		(3<<0) 

#define WLAN_CAPABILITY_DMG_CBAP_ONLY		(1<<2)
#define WLAN_CAPABILITY_DMG_CBAP_SOURCE		(1<<3)
#define WLAN_CAPABILITY_DMG_PRIVACY		(1<<4)
#define WLAN_CAPABILITY_DMG_ECPAC		(1<<5)

#define WLAN_CAPABILITY_DMG_SPECTRUM_MGMT	(1<<8)
#define WLAN_CAPABILITY_DMG_RADIO_MEASURE	(1<<12)


#define IEEE80211_SPCT_MSR_RPRT_MODE_LATE	(1<<0)
#define IEEE80211_SPCT_MSR_RPRT_MODE_INCAPABLE	(1<<1)
#define IEEE80211_SPCT_MSR_RPRT_MODE_REFUSED	(1<<2)

#define IEEE80211_SPCT_MSR_RPRT_TYPE_BASIC	0
#define IEEE80211_SPCT_MSR_RPRT_TYPE_CCA	1
#define IEEE80211_SPCT_MSR_RPRT_TYPE_RPI	2
#define IEEE80211_SPCT_MSR_RPRT_TYPE_LCI	8
#define IEEE80211_SPCT_MSR_RPRT_TYPE_CIVIC	11


#define WLAN_ERP_NON_ERP_PRESENT (1<<0)
#define WLAN_ERP_USE_PROTECTION (1<<1)
#define WLAN_ERP_BARKER_PREAMBLE (1<<2)


enum {
	WLAN_ERP_PREAMBLE_SHORT = 0,
	WLAN_ERP_PREAMBLE_LONG = 1,
};


enum {
	IEEE80211_BANDID_TV_WS = 0, 
	IEEE80211_BANDID_SUB1  = 1, 
	IEEE80211_BANDID_2G    = 2, 
	IEEE80211_BANDID_3G    = 3, 
	IEEE80211_BANDID_5G    = 4, 
	IEEE80211_BANDID_60G   = 5, 
};


enum ieee80211_statuscode {
	WLAN_STATUS_SUCCESS = 0,
	WLAN_STATUS_UNSPECIFIED_FAILURE = 1,
	WLAN_STATUS_CAPS_UNSUPPORTED = 10,
	WLAN_STATUS_REASSOC_NO_ASSOC = 11,
	WLAN_STATUS_ASSOC_DENIED_UNSPEC = 12,
	WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG = 13,
	WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION = 14,
	WLAN_STATUS_CHALLENGE_FAIL = 15,
	WLAN_STATUS_AUTH_TIMEOUT = 16,
	WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA = 17,
	WLAN_STATUS_ASSOC_DENIED_RATES = 18,
	
	WLAN_STATUS_ASSOC_DENIED_NOSHORTPREAMBLE = 19,
	WLAN_STATUS_ASSOC_DENIED_NOPBCC = 20,
	WLAN_STATUS_ASSOC_DENIED_NOAGILITY = 21,
	
	WLAN_STATUS_ASSOC_DENIED_NOSPECTRUM = 22,
	WLAN_STATUS_ASSOC_REJECTED_BAD_POWER = 23,
	WLAN_STATUS_ASSOC_REJECTED_BAD_SUPP_CHAN = 24,
	
	WLAN_STATUS_ASSOC_DENIED_NOSHORTTIME = 25,
	WLAN_STATUS_ASSOC_DENIED_NODSSSOFDM = 26,
	
	WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY = 30,
	WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION = 31,
	
	WLAN_STATUS_INVALID_IE = 40,
	WLAN_STATUS_INVALID_GROUP_CIPHER = 41,
	WLAN_STATUS_INVALID_PAIRWISE_CIPHER = 42,
	WLAN_STATUS_INVALID_AKMP = 43,
	WLAN_STATUS_UNSUPP_RSN_VERSION = 44,
	WLAN_STATUS_INVALID_RSN_IE_CAP = 45,
	WLAN_STATUS_CIPHER_SUITE_REJECTED = 46,
	
	WLAN_STATUS_UNSPECIFIED_QOS = 32,
	WLAN_STATUS_ASSOC_DENIED_NOBANDWIDTH = 33,
	WLAN_STATUS_ASSOC_DENIED_LOWACK = 34,
	WLAN_STATUS_ASSOC_DENIED_UNSUPP_QOS = 35,
	WLAN_STATUS_REQUEST_DECLINED = 37,
	WLAN_STATUS_INVALID_QOS_PARAM = 38,
	WLAN_STATUS_CHANGE_TSPEC = 39,
	WLAN_STATUS_WAIT_TS_DELAY = 47,
	WLAN_STATUS_NO_DIRECT_LINK = 48,
	WLAN_STATUS_STA_NOT_PRESENT = 49,
	WLAN_STATUS_STA_NOT_QSTA = 50,
	
	WLAN_STATUS_ANTI_CLOG_REQUIRED = 76,
	WLAN_STATUS_FCG_NOT_SUPP = 78,
	WLAN_STATUS_STA_NO_TBTT = 78,
	
	WLAN_STATUS_REJECTED_WITH_SUGGESTED_CHANGES = 39,
	WLAN_STATUS_REJECTED_FOR_DELAY_PERIOD = 47,
	WLAN_STATUS_REJECT_WITH_SCHEDULE = 83,
	WLAN_STATUS_PENDING_ADMITTING_FST_SESSION = 86,
	WLAN_STATUS_PERFORMING_FST_NOW = 87,
	WLAN_STATUS_PENDING_GAP_IN_BA_WINDOW = 88,
	WLAN_STATUS_REJECT_U_PID_SETTING = 89,
	WLAN_STATUS_REJECT_DSE_BAND = 96,
	WLAN_STATUS_DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL = 99,
	WLAN_STATUS_DENIED_DUE_TO_SPECTRUM_MANAGEMENT = 103,
	
	WLAN_STATUS_FILS_AUTHENTICATION_FAILURE = 108,
	WLAN_STATUS_UNKNOWN_AUTHENTICATION_SERVER = 109,
	WLAN_STATUS_SAE_HASH_TO_ELEMENT = 126,
	WLAN_STATUS_SAE_PK = 127,
};



enum ieee80211_reasoncode {
	WLAN_REASON_UNSPECIFIED = 1,
	WLAN_REASON_PREV_AUTH_NOT_VALID = 2,
	WLAN_REASON_DEAUTH_LEAVING = 3,
	WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY = 4,
	WLAN_REASON_DISASSOC_AP_BUSY = 5,
	WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA = 6,
	WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA = 7,
	WLAN_REASON_DISASSOC_STA_HAS_LEFT = 8,
	WLAN_REASON_STA_REQ_ASSOC_WITHOUT_AUTH = 9,
	
	WLAN_REASON_DISASSOC_BAD_POWER = 10,
	WLAN_REASON_DISASSOC_BAD_SUPP_CHAN = 11,
	
	WLAN_REASON_INVALID_IE = 13,
	WLAN_REASON_MIC_FAILURE = 14,
	WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT = 15,
	WLAN_REASON_GROUP_KEY_HANDSHAKE_TIMEOUT = 16,
	WLAN_REASON_IE_DIFFERENT = 17,
	WLAN_REASON_INVALID_GROUP_CIPHER = 18,
	WLAN_REASON_INVALID_PAIRWISE_CIPHER = 19,
	WLAN_REASON_INVALID_AKMP = 20,
	WLAN_REASON_UNSUPP_RSN_VERSION = 21,
	WLAN_REASON_INVALID_RSN_IE_CAP = 22,
	WLAN_REASON_IEEE8021X_FAILED = 23,
	WLAN_REASON_CIPHER_SUITE_REJECTED = 24,
	
	WLAN_REASON_TDLS_TEARDOWN_UNREACHABLE = 25,
	WLAN_REASON_TDLS_TEARDOWN_UNSPECIFIED = 26,
	
	WLAN_REASON_DISASSOC_UNSPECIFIED_QOS = 32,
	WLAN_REASON_DISASSOC_QAP_NO_BANDWIDTH = 33,
	WLAN_REASON_DISASSOC_LOW_ACK = 34,
	WLAN_REASON_DISASSOC_QAP_EXCEED_TXOP = 35,
	WLAN_REASON_QSTA_LEAVE_QBSS = 36,
	WLAN_REASON_QSTA_NOT_USE = 37,
	WLAN_REASON_QSTA_REQUIRE_SETUP = 38,
	WLAN_REASON_QSTA_TIMEOUT = 39,
	WLAN_REASON_QSTA_CIPHER_NOT_SUPP = 45,
	
	WLAN_REASON_MESH_PEER_CANCELED = 52,
	WLAN_REASON_MESH_MAX_PEERS = 53,
	WLAN_REASON_MESH_CONFIG = 54,
	WLAN_REASON_MESH_CLOSE = 55,
	WLAN_REASON_MESH_MAX_RETRIES = 56,
	WLAN_REASON_MESH_CONFIRM_TIMEOUT = 57,
	WLAN_REASON_MESH_INVALID_GTK = 58,
	WLAN_REASON_MESH_INCONSISTENT_PARAM = 59,
	WLAN_REASON_MESH_INVALID_SECURITY = 60,
	WLAN_REASON_MESH_PATH_ERROR = 61,
	WLAN_REASON_MESH_PATH_NOFORWARD = 62,
	WLAN_REASON_MESH_PATH_DEST_UNREACHABLE = 63,
	WLAN_REASON_MAC_EXISTS_IN_MBSS = 64,
	WLAN_REASON_MESH_CHAN_REGULATORY = 65,
	WLAN_REASON_MESH_CHAN = 66,
};



enum ieee80211_eid {
	WLAN_EID_SSID = 0,
	WLAN_EID_SUPP_RATES = 1,
	WLAN_EID_FH_PARAMS = 2, 
	WLAN_EID_DS_PARAMS = 3,
	WLAN_EID_CF_PARAMS = 4,
	WLAN_EID_TIM = 5,
	WLAN_EID_IBSS_PARAMS = 6,
	WLAN_EID_COUNTRY = 7,
	
	WLAN_EID_REQUEST = 10,
	WLAN_EID_QBSS_LOAD = 11,
	WLAN_EID_EDCA_PARAM_SET = 12,
	WLAN_EID_TSPEC = 13,
	WLAN_EID_TCLAS = 14,
	WLAN_EID_SCHEDULE = 15,
	WLAN_EID_CHALLENGE = 16,
	
	WLAN_EID_PWR_CONSTRAINT = 32,
	WLAN_EID_PWR_CAPABILITY = 33,
	WLAN_EID_TPC_REQUEST = 34,
	WLAN_EID_TPC_REPORT = 35,
	WLAN_EID_SUPPORTED_CHANNELS = 36,
	WLAN_EID_CHANNEL_SWITCH = 37,
	WLAN_EID_MEASURE_REQUEST = 38,
	WLAN_EID_MEASURE_REPORT = 39,
	WLAN_EID_QUIET = 40,
	WLAN_EID_IBSS_DFS = 41,
	WLAN_EID_ERP_INFO = 42,
	WLAN_EID_TS_DELAY = 43,
	WLAN_EID_TCLAS_PROCESSING = 44,
	WLAN_EID_HT_CAPABILITY = 45,
	WLAN_EID_QOS_CAPA = 46,
	
	WLAN_EID_RSN = 48,
	WLAN_EID_802_15_COEX = 49,
	WLAN_EID_EXT_SUPP_RATES = 50,
	WLAN_EID_AP_CHAN_REPORT = 51,
	WLAN_EID_NEIGHBOR_REPORT = 52,
	WLAN_EID_RCPI = 53,
	WLAN_EID_MOBILITY_DOMAIN = 54,
	WLAN_EID_FAST_BSS_TRANSITION = 55,
	WLAN_EID_TIMEOUT_INTERVAL = 56,
	WLAN_EID_RIC_DATA = 57,
	WLAN_EID_DSE_REGISTERED_LOCATION = 58,
	WLAN_EID_SUPPORTED_REGULATORY_CLASSES = 59,
	WLAN_EID_EXT_CHANSWITCH_ANN = 60,
	WLAN_EID_HT_OPERATION = 61,
	WLAN_EID_SECONDARY_CHANNEL_OFFSET = 62,
	WLAN_EID_BSS_AVG_ACCESS_DELAY = 63,
	WLAN_EID_ANTENNA_INFO = 64,
	WLAN_EID_RSNI = 65,
	WLAN_EID_MEASUREMENT_PILOT_TX_INFO = 66,
	WLAN_EID_BSS_AVAILABLE_CAPACITY = 67,
	WLAN_EID_BSS_AC_ACCESS_DELAY = 68,
	WLAN_EID_TIME_ADVERTISEMENT = 69,
	WLAN_EID_RRM_ENABLED_CAPABILITIES = 70,
	WLAN_EID_MULTIPLE_BSSID = 71,
	WLAN_EID_BSS_COEX_2040 = 72,
	WLAN_EID_BSS_INTOLERANT_CHL_REPORT = 73,
	WLAN_EID_OVERLAP_BSS_SCAN_PARAM = 74,
	WLAN_EID_RIC_DESCRIPTOR = 75,
	WLAN_EID_MMIE = 76,
	WLAN_EID_ASSOC_COMEBACK_TIME = 77,
	WLAN_EID_EVENT_REQUEST = 78,
	WLAN_EID_EVENT_REPORT = 79,
	WLAN_EID_DIAGNOSTIC_REQUEST = 80,
	WLAN_EID_DIAGNOSTIC_REPORT = 81,
	WLAN_EID_LOCATION_PARAMS = 82,
	WLAN_EID_NON_TX_BSSID_CAP =  83,
	WLAN_EID_SSID_LIST = 84,
	WLAN_EID_MULTI_BSSID_IDX = 85,
	WLAN_EID_FMS_DESCRIPTOR = 86,
	WLAN_EID_FMS_REQUEST = 87,
	WLAN_EID_FMS_RESPONSE = 88,
	WLAN_EID_QOS_TRAFFIC_CAPA = 89,
	WLAN_EID_BSS_MAX_IDLE_PERIOD = 90,
	WLAN_EID_TSF_REQUEST = 91,
	WLAN_EID_TSF_RESPOSNE = 92,
	WLAN_EID_WNM_SLEEP_MODE = 93,
	WLAN_EID_TIM_BCAST_REQ = 94,
	WLAN_EID_TIM_BCAST_RESP = 95,
	WLAN_EID_COLL_IF_REPORT = 96,
	WLAN_EID_CHANNEL_USAGE = 97,
	WLAN_EID_TIME_ZONE = 98,
	WLAN_EID_DMS_REQUEST = 99,
	WLAN_EID_DMS_RESPONSE = 100,
	WLAN_EID_LINK_ID = 101,
	WLAN_EID_WAKEUP_SCHEDUL = 102,
	
	WLAN_EID_CHAN_SWITCH_TIMING = 104,
	WLAN_EID_PTI_CONTROL = 105,
	WLAN_EID_PU_BUFFER_STATUS = 106,
	WLAN_EID_INTERWORKING = 107,
	WLAN_EID_ADVERTISEMENT_PROTOCOL = 108,
	WLAN_EID_EXPEDITED_BW_REQ = 109,
	WLAN_EID_QOS_MAP_SET = 110,
	WLAN_EID_ROAMING_CONSORTIUM = 111,
	WLAN_EID_EMERGENCY_ALERT = 112,
	WLAN_EID_MESH_CONFIG = 113,
	WLAN_EID_MESH_ID = 114,
	WLAN_EID_LINK_METRIC_REPORT = 115,
	WLAN_EID_CONGESTION_NOTIFICATION = 116,
	WLAN_EID_PEER_MGMT = 117,
	WLAN_EID_CHAN_SWITCH_PARAM = 118,
	WLAN_EID_MESH_AWAKE_WINDOW = 119,
	WLAN_EID_BEACON_TIMING = 120,
	WLAN_EID_MCCAOP_SETUP_REQ = 121,
	WLAN_EID_MCCAOP_SETUP_RESP = 122,
	WLAN_EID_MCCAOP_ADVERT = 123,
	WLAN_EID_MCCAOP_TEARDOWN = 124,
	WLAN_EID_GANN = 125,
	WLAN_EID_RANN = 126,
	WLAN_EID_EXT_CAPABILITY = 127,
	
	WLAN_EID_PREQ = 130,
	WLAN_EID_PREP = 131,
	WLAN_EID_PERR = 132,
	
	WLAN_EID_PXU = 137,
	WLAN_EID_PXUC = 138,
	WLAN_EID_AUTH_MESH_PEER_EXCH = 139,
	WLAN_EID_MIC = 140,
	WLAN_EID_DESTINATION_URI = 141,
	WLAN_EID_UAPSD_COEX = 142,
	WLAN_EID_WAKEUP_SCHEDULE = 143,
	WLAN_EID_EXT_SCHEDULE = 144,
	WLAN_EID_STA_AVAILABILITY = 145,
	WLAN_EID_DMG_TSPEC = 146,
	WLAN_EID_DMG_AT = 147,
	WLAN_EID_DMG_CAP = 148,
	
	WLAN_EID_CISCO_VENDOR_SPECIFIC = 150,
	WLAN_EID_DMG_OPERATION = 151,
	WLAN_EID_DMG_BSS_PARAM_CHANGE = 152,
	WLAN_EID_DMG_BEAM_REFINEMENT = 153,
	WLAN_EID_CHANNEL_MEASURE_FEEDBACK = 154,
	
	WLAN_EID_AWAKE_WINDOW = 157,
	WLAN_EID_MULTI_BAND = 158,
	WLAN_EID_ADDBA_EXT = 159,
	WLAN_EID_NEXT_PCP_LIST = 160,
	WLAN_EID_PCP_HANDOVER = 161,
	WLAN_EID_DMG_LINK_MARGIN = 162,
	WLAN_EID_SWITCHING_STREAM = 163,
	WLAN_EID_SESSION_TRANSITION = 164,
	WLAN_EID_DYN_TONE_PAIRING_REPORT = 165,
	WLAN_EID_CLUSTER_REPORT = 166,
	WLAN_EID_RELAY_CAP = 167,
	WLAN_EID_RELAY_XFER_PARAM_SET = 168,
	WLAN_EID_BEAM_LINK_MAINT = 169,
	WLAN_EID_MULTIPLE_MAC_ADDR = 170,
	WLAN_EID_U_PID = 171,
	WLAN_EID_DMG_LINK_ADAPT_ACK = 172,
	
	WLAN_EID_MCCAOP_ADV_OVERVIEW = 174,
	WLAN_EID_QUIET_PERIOD_REQ = 175,
	
	WLAN_EID_QUIET_PERIOD_RESP = 177,
	
	
	WLAN_EID_EPAC_POLICY = 182,
	WLAN_EID_CLISTER_TIME_OFF = 183,
	WLAN_EID_INTER_AC_PRIO = 184,
	WLAN_EID_SCS_DESCRIPTOR = 185,
	WLAN_EID_QLOAD_REPORT = 186,
	WLAN_EID_HCCA_TXOP_UPDATE_COUNT = 187,
	WLAN_EID_HL_STREAM_ID = 188,
	WLAN_EID_GCR_GROUP_ADDR = 189,
	WLAN_EID_ANTENNA_SECTOR_ID_PATTERN = 190,
	WLAN_EID_VHT_CAPABILITY = 191,
	WLAN_EID_VHT_OPERATION = 192,
	WLAN_EID_EXTENDED_BSS_LOAD = 193,
	WLAN_EID_WIDE_BW_CHANNEL_SWITCH = 194,
	WLAN_EID_TX_POWER_ENVELOPE = 195,
	WLAN_EID_CHANNEL_SWITCH_WRAPPER = 196,
	WLAN_EID_AID = 197,
	WLAN_EID_QUIET_CHANNEL = 198,
	WLAN_EID_OPMODE_NOTIF = 199,

	WLAN_EID_REDUCED_NEIGHBOR_REPORT = 201,

	WLAN_EID_AID_REQUEST = 210,
	WLAN_EID_AID_RESPONSE = 211,
	WLAN_EID_S1G_BCN_COMPAT = 213,
	WLAN_EID_S1G_SHORT_BCN_INTERVAL = 214,
	WLAN_EID_S1G_TWT = 216,
	WLAN_EID_S1G_CAPABILITIES = 217,
	WLAN_EID_VENDOR_SPECIFIC = 221,
	WLAN_EID_QOS_PARAMETER = 222,
	WLAN_EID_S1G_OPERATION = 232,
	WLAN_EID_CAG_NUMBER = 237,
	WLAN_EID_AP_CSN = 239,
	WLAN_EID_FILS_INDICATION = 240,
	WLAN_EID_DILS = 241,
	WLAN_EID_FRAGMENT = 242,
	WLAN_EID_RSNX = 244,
	WLAN_EID_EXTENSION = 255
};


enum ieee80211_eid_ext {
	WLAN_EID_EXT_ASSOC_DELAY_INFO = 1,
	WLAN_EID_EXT_FILS_REQ_PARAMS = 2,
	WLAN_EID_EXT_FILS_KEY_CONFIRM = 3,
	WLAN_EID_EXT_FILS_SESSION = 4,
	WLAN_EID_EXT_FILS_HLP_CONTAINER = 5,
	WLAN_EID_EXT_FILS_IP_ADDR_ASSIGN = 6,
	WLAN_EID_EXT_KEY_DELIVERY = 7,
	WLAN_EID_EXT_FILS_WRAPPED_DATA = 8,
	WLAN_EID_EXT_FILS_PUBLIC_KEY = 12,
	WLAN_EID_EXT_FILS_NONCE = 13,
	WLAN_EID_EXT_FUTURE_CHAN_GUIDANCE = 14,
	WLAN_EID_EXT_HE_CAPABILITY = 35,
	WLAN_EID_EXT_HE_OPERATION = 36,
	WLAN_EID_EXT_UORA = 37,
	WLAN_EID_EXT_HE_MU_EDCA = 38,
	WLAN_EID_EXT_HE_SPR = 39,
	WLAN_EID_EXT_NDP_FEEDBACK_REPORT_PARAMSET = 41,
	WLAN_EID_EXT_BSS_COLOR_CHG_ANN = 42,
	WLAN_EID_EXT_QUIET_TIME_PERIOD_SETUP = 43,
	WLAN_EID_EXT_ESS_REPORT = 45,
	WLAN_EID_EXT_OPS = 46,
	WLAN_EID_EXT_HE_BSS_LOAD = 47,
	WLAN_EID_EXT_MAX_CHANNEL_SWITCH_TIME = 52,
	WLAN_EID_EXT_MULTIPLE_BSSID_CONFIGURATION = 55,
	WLAN_EID_EXT_NON_INHERITANCE = 56,
	WLAN_EID_EXT_KNOWN_BSSID = 57,
	WLAN_EID_EXT_SHORT_SSID_LIST = 58,
	WLAN_EID_EXT_HE_6GHZ_CAPA = 59,
	WLAN_EID_EXT_UL_MU_POWER_CAPA = 60,
	WLAN_EID_EXT_EHT_OPERATION = 106,
	WLAN_EID_EXT_EHT_MULTI_LINK = 107,
	WLAN_EID_EXT_EHT_CAPABILITY = 108,
};


enum ieee80211_category {
	WLAN_CATEGORY_SPECTRUM_MGMT = 0,
	WLAN_CATEGORY_QOS = 1,
	WLAN_CATEGORY_DLS = 2,
	WLAN_CATEGORY_BACK = 3,
	WLAN_CATEGORY_PUBLIC = 4,
	WLAN_CATEGORY_RADIO_MEASUREMENT = 5,
	WLAN_CATEGORY_FAST_BBS_TRANSITION = 6,
	WLAN_CATEGORY_HT = 7,
	WLAN_CATEGORY_SA_QUERY = 8,
	WLAN_CATEGORY_PROTECTED_DUAL_OF_ACTION = 9,
	WLAN_CATEGORY_WNM = 10,
	WLAN_CATEGORY_WNM_UNPROTECTED = 11,
	WLAN_CATEGORY_TDLS = 12,
	WLAN_CATEGORY_MESH_ACTION = 13,
	WLAN_CATEGORY_MULTIHOP_ACTION = 14,
	WLAN_CATEGORY_SELF_PROTECTED = 15,
	WLAN_CATEGORY_DMG = 16,
	WLAN_CATEGORY_WMM = 17,
	WLAN_CATEGORY_FST = 18,
	WLAN_CATEGORY_UNPROT_DMG = 20,
	WLAN_CATEGORY_VHT = 21,
	WLAN_CATEGORY_S1G = 22,
	WLAN_CATEGORY_VENDOR_SPECIFIC_PROTECTED = 126,
	WLAN_CATEGORY_VENDOR_SPECIFIC = 127,
};


enum ieee80211_spectrum_mgmt_actioncode {
	WLAN_ACTION_SPCT_MSR_REQ = 0,
	WLAN_ACTION_SPCT_MSR_RPRT = 1,
	WLAN_ACTION_SPCT_TPC_REQ = 2,
	WLAN_ACTION_SPCT_TPC_RPRT = 3,
	WLAN_ACTION_SPCT_CHL_SWITCH = 4,
};


enum ieee80211_ht_actioncode {
	WLAN_HT_ACTION_NOTIFY_CHANWIDTH = 0,
	WLAN_HT_ACTION_SMPS = 1,
	WLAN_HT_ACTION_PSMP = 2,
	WLAN_HT_ACTION_PCO_PHASE = 3,
	WLAN_HT_ACTION_CSI = 4,
	WLAN_HT_ACTION_NONCOMPRESSED_BF = 5,
	WLAN_HT_ACTION_COMPRESSED_BF = 6,
	WLAN_HT_ACTION_ASEL_IDX_FEEDBACK = 7,
};


enum ieee80211_vht_actioncode {
	WLAN_VHT_ACTION_COMPRESSED_BF = 0,
	WLAN_VHT_ACTION_GROUPID_MGMT = 1,
	WLAN_VHT_ACTION_OPMODE_NOTIF = 2,
};


enum ieee80211_self_protected_actioncode {
	WLAN_SP_RESERVED = 0,
	WLAN_SP_MESH_PEERING_OPEN = 1,
	WLAN_SP_MESH_PEERING_CONFIRM = 2,
	WLAN_SP_MESH_PEERING_CLOSE = 3,
	WLAN_SP_MGK_INFORM = 4,
	WLAN_SP_MGK_ACK = 5,
};


enum ieee80211_mesh_actioncode {
	WLAN_MESH_ACTION_LINK_METRIC_REPORT,
	WLAN_MESH_ACTION_HWMP_PATH_SELECTION,
	WLAN_MESH_ACTION_GATE_ANNOUNCEMENT,
	WLAN_MESH_ACTION_CONGESTION_CONTROL_NOTIFICATION,
	WLAN_MESH_ACTION_MCCA_SETUP_REQUEST,
	WLAN_MESH_ACTION_MCCA_SETUP_REPLY,
	WLAN_MESH_ACTION_MCCA_ADVERTISEMENT_REQUEST,
	WLAN_MESH_ACTION_MCCA_ADVERTISEMENT,
	WLAN_MESH_ACTION_MCCA_TEARDOWN,
	WLAN_MESH_ACTION_TBTT_ADJUSTMENT_REQUEST,
	WLAN_MESH_ACTION_TBTT_ADJUSTMENT_RESPONSE,
};


enum ieee80211_unprotected_wnm_actioncode {
	WLAN_UNPROTECTED_WNM_ACTION_TIM = 0,
	WLAN_UNPROTECTED_WNM_ACTION_TIMING_MEASUREMENT_RESPONSE = 1,
};


enum ieee80211_key_len {
	WLAN_KEY_LEN_WEP40 = 5,
	WLAN_KEY_LEN_WEP104 = 13,
	WLAN_KEY_LEN_CCMP = 16,
	WLAN_KEY_LEN_CCMP_256 = 32,
	WLAN_KEY_LEN_TKIP = 32,
	WLAN_KEY_LEN_AES_CMAC = 16,
	WLAN_KEY_LEN_SMS4 = 32,
	WLAN_KEY_LEN_GCMP = 16,
	WLAN_KEY_LEN_GCMP_256 = 32,
	WLAN_KEY_LEN_BIP_CMAC_256 = 32,
	WLAN_KEY_LEN_BIP_GMAC_128 = 16,
	WLAN_KEY_LEN_BIP_GMAC_256 = 32,
};

enum ieee80211_s1g_actioncode {
	WLAN_S1G_AID_SWITCH_REQUEST,
	WLAN_S1G_AID_SWITCH_RESPONSE,
	WLAN_S1G_SYNC_CONTROL,
	WLAN_S1G_STA_INFO_ANNOUNCE,
	WLAN_S1G_EDCA_PARAM_SET,
	WLAN_S1G_EL_OPERATION,
	WLAN_S1G_TWT_SETUP,
	WLAN_S1G_TWT_TEARDOWN,
	WLAN_S1G_SECT_GROUP_ID_LIST,
	WLAN_S1G_SECT_ID_FEEDBACK,
	WLAN_S1G_TWT_INFORMATION = 11,
};

#define IEEE80211_WEP_IV_LEN		4
#define IEEE80211_WEP_ICV_LEN		4
#define IEEE80211_CCMP_HDR_LEN		8
#define IEEE80211_CCMP_MIC_LEN		8
#define IEEE80211_CCMP_PN_LEN		6
#define IEEE80211_CCMP_256_HDR_LEN	8
#define IEEE80211_CCMP_256_MIC_LEN	16
#define IEEE80211_CCMP_256_PN_LEN	6
#define IEEE80211_TKIP_IV_LEN		8
#define IEEE80211_TKIP_ICV_LEN		4
#define IEEE80211_CMAC_PN_LEN		6
#define IEEE80211_GMAC_PN_LEN		6
#define IEEE80211_GCMP_HDR_LEN		8
#define IEEE80211_GCMP_MIC_LEN		16
#define IEEE80211_GCMP_PN_LEN		6

#define FILS_NONCE_LEN			16
#define FILS_MAX_KEK_LEN		64

#define FILS_ERP_MAX_USERNAME_LEN	16
#define FILS_ERP_MAX_REALM_LEN		253
#define FILS_ERP_MAX_RRK_LEN		64

#define PMK_MAX_LEN			64
#define SAE_PASSWORD_MAX_LEN		128


enum ieee80211_pub_actioncode {
	WLAN_PUB_ACTION_20_40_BSS_COEX = 0,
	WLAN_PUB_ACTION_DSE_ENABLEMENT = 1,
	WLAN_PUB_ACTION_DSE_DEENABLEMENT = 2,
	WLAN_PUB_ACTION_DSE_REG_LOC_ANN = 3,
	WLAN_PUB_ACTION_EXT_CHANSW_ANN = 4,
	WLAN_PUB_ACTION_DSE_MSMT_REQ = 5,
	WLAN_PUB_ACTION_DSE_MSMT_RESP = 6,
	WLAN_PUB_ACTION_MSMT_PILOT = 7,
	WLAN_PUB_ACTION_DSE_PC = 8,
	WLAN_PUB_ACTION_VENDOR_SPECIFIC = 9,
	WLAN_PUB_ACTION_GAS_INITIAL_REQ = 10,
	WLAN_PUB_ACTION_GAS_INITIAL_RESP = 11,
	WLAN_PUB_ACTION_GAS_COMEBACK_REQ = 12,
	WLAN_PUB_ACTION_GAS_COMEBACK_RESP = 13,
	WLAN_PUB_ACTION_TDLS_DISCOVER_RES = 14,
	WLAN_PUB_ACTION_LOC_TRACK_NOTI = 15,
	WLAN_PUB_ACTION_QAB_REQUEST_FRAME = 16,
	WLAN_PUB_ACTION_QAB_RESPONSE_FRAME = 17,
	WLAN_PUB_ACTION_QMF_POLICY = 18,
	WLAN_PUB_ACTION_QMF_POLICY_CHANGE = 19,
	WLAN_PUB_ACTION_QLOAD_REQUEST = 20,
	WLAN_PUB_ACTION_QLOAD_REPORT = 21,
	WLAN_PUB_ACTION_HCCA_TXOP_ADVERT = 22,
	WLAN_PUB_ACTION_HCCA_TXOP_RESPONSE = 23,
	WLAN_PUB_ACTION_PUBLIC_KEY = 24,
	WLAN_PUB_ACTION_CHANNEL_AVAIL_QUERY = 25,
	WLAN_PUB_ACTION_CHANNEL_SCHEDULE_MGMT = 26,
	WLAN_PUB_ACTION_CONTACT_VERI_SIGNAL = 27,
	WLAN_PUB_ACTION_GDD_ENABLEMENT_REQ = 28,
	WLAN_PUB_ACTION_GDD_ENABLEMENT_RESP = 29,
	WLAN_PUB_ACTION_NETWORK_CHANNEL_CONTROL = 30,
	WLAN_PUB_ACTION_WHITE_SPACE_MAP_ANN = 31,
	WLAN_PUB_ACTION_FTM_REQUEST = 32,
	WLAN_PUB_ACTION_FTM_RESPONSE = 33,
	WLAN_PUB_ACTION_FILS_DISCOVERY = 34,
};


enum ieee80211_tdls_actioncode {
	WLAN_TDLS_SETUP_REQUEST = 0,
	WLAN_TDLS_SETUP_RESPONSE = 1,
	WLAN_TDLS_SETUP_CONFIRM = 2,
	WLAN_TDLS_TEARDOWN = 3,
	WLAN_TDLS_PEER_TRAFFIC_INDICATION = 4,
	WLAN_TDLS_CHANNEL_SWITCH_REQUEST = 5,
	WLAN_TDLS_CHANNEL_SWITCH_RESPONSE = 6,
	WLAN_TDLS_PEER_PSM_REQUEST = 7,
	WLAN_TDLS_PEER_PSM_RESPONSE = 8,
	WLAN_TDLS_PEER_TRAFFIC_RESPONSE = 9,
	WLAN_TDLS_DISCOVERY_REQUEST = 10,
};


#define WLAN_EXT_CAPA1_EXT_CHANNEL_SWITCHING	BIT(2)


#define WLAN_EXT_CAPA3_MULTI_BSSID_SUPPORT	BIT(6)


#define WLAN_EXT_CAPA3_TIMING_MEASUREMENT_SUPPORT	BIT(7)


#define WLAN_EXT_CAPA4_TDLS_BUFFER_STA		BIT(4)
#define WLAN_EXT_CAPA4_TDLS_PEER_PSM		BIT(5)
#define WLAN_EXT_CAPA4_TDLS_CHAN_SWITCH		BIT(6)


#define WLAN_EXT_CAPA4_INTERWORKING_ENABLED	BIT(7)


#define WLAN_EXT_CAPA5_TDLS_ENABLED	BIT(5)
#define WLAN_EXT_CAPA5_TDLS_PROHIBITED	BIT(6)
#define WLAN_EXT_CAPA5_TDLS_CH_SW_PROHIBITED	BIT(7)

#define WLAN_EXT_CAPA8_TDLS_WIDE_BW_ENABLED	BIT(5)
#define WLAN_EXT_CAPA8_OPMODE_NOTIF	BIT(6)


#define WLAN_EXT_CAPA8_MAX_MSDU_IN_AMSDU_LSB	BIT(7)
#define WLAN_EXT_CAPA9_MAX_MSDU_IN_AMSDU_MSB	BIT(0)


#define WLAN_EXT_CAPA9_FTM_INITIATOR	BIT(7)


#define WLAN_EXT_CAPA10_TWT_REQUESTER_SUPPORT	BIT(5)
#define WLAN_EXT_CAPA10_TWT_RESPONDER_SUPPORT	BIT(6)


#define WLAN_EXT_CAPA10_OBSS_NARROW_BW_RU_TOLERANCE_SUPPORT BIT(7)


#define WLAN_EXT_CAPA11_EMA_SUPPORT	BIT(3)


#define WLAN_TDLS_SNAP_RFTYPE	0x2


#define WLAN_BSS_COEX_INFORMATION_REQUEST	BIT(0)


enum ieee80211_mesh_sync_method {
	IEEE80211_SYNC_METHOD_NEIGHBOR_OFFSET = 1,
	IEEE80211_SYNC_METHOD_VENDOR = 255,
};


enum ieee80211_mesh_path_protocol {
	IEEE80211_PATH_PROTOCOL_HWMP = 1,
	IEEE80211_PATH_PROTOCOL_VENDOR = 255,
};


enum ieee80211_mesh_path_metric {
	IEEE80211_PATH_METRIC_AIRTIME = 1,
	IEEE80211_PATH_METRIC_VENDOR = 255,
};


enum ieee80211_root_mode_identifier {
	IEEE80211_ROOTMODE_NO_ROOT = 0,
	IEEE80211_ROOTMODE_ROOT = 1,
	IEEE80211_PROACTIVE_PREQ_NO_PREP = 2,
	IEEE80211_PROACTIVE_PREQ_WITH_PREP = 3,
	IEEE80211_PROACTIVE_RANN = 4,
};




#define IEEE80211_COUNTRY_IE_MIN_LEN	6


#define IEEE80211_COUNTRY_STRING_LEN	3


#define IEEE80211_COUNTRY_EXTENSION_ID 201


struct ieee80211_country_ie_triplet {
	union {
		struct {
			u8 first_channel;
			u8 num_channels;
			s8 max_power;
		} __packed chans;
		struct {
			u8 reg_extension_id;
			u8 reg_class;
			u8 coverage_class;
		} __packed ext;
	};
} __packed;

enum ieee80211_timeout_interval_type {
	WLAN_TIMEOUT_REASSOC_DEADLINE = 1 ,
	WLAN_TIMEOUT_KEY_LIFETIME = 2 ,
	WLAN_TIMEOUT_ASSOC_COMEBACK = 3 ,
};


struct ieee80211_timeout_interval_ie {
	u8 type;
	__le32 value;
} __packed;


enum ieee80211_idle_options {
	WLAN_IDLE_OPTIONS_PROTECTED_KEEP_ALIVE = BIT(0),
};


struct ieee80211_bss_max_idle_period_ie {
	__le16 max_idle_period;
	u8 idle_options;
} __packed;


enum ieee80211_back_actioncode {
	WLAN_ACTION_ADDBA_REQ = 0,
	WLAN_ACTION_ADDBA_RESP = 1,
	WLAN_ACTION_DELBA = 2,
};


enum ieee80211_back_parties {
	WLAN_BACK_RECIPIENT = 0,
	WLAN_BACK_INITIATOR = 1,
};


enum ieee80211_sa_query_action {
	WLAN_ACTION_SA_QUERY_REQUEST = 0,
	WLAN_ACTION_SA_QUERY_RESPONSE = 1,
};


struct ieee80211_bssid_index {
	u8 bssid_index;
	u8 dtim_period;
	u8 dtim_count;
};


struct ieee80211_multiple_bssid_configuration {
	u8 bssid_count;
	u8 profile_periodicity;
};

#define SUITE(oui, id)	(((oui) << 8) | (id))


#define WLAN_CIPHER_SUITE_USE_GROUP	SUITE(0x000FAC, 0)
#define WLAN_CIPHER_SUITE_WEP40		SUITE(0x000FAC, 1)
#define WLAN_CIPHER_SUITE_TKIP		SUITE(0x000FAC, 2)

#define WLAN_CIPHER_SUITE_CCMP		SUITE(0x000FAC, 4)
#define WLAN_CIPHER_SUITE_WEP104	SUITE(0x000FAC, 5)
#define WLAN_CIPHER_SUITE_AES_CMAC	SUITE(0x000FAC, 6)
#define WLAN_CIPHER_SUITE_GCMP		SUITE(0x000FAC, 8)
#define WLAN_CIPHER_SUITE_GCMP_256	SUITE(0x000FAC, 9)
#define WLAN_CIPHER_SUITE_CCMP_256	SUITE(0x000FAC, 10)
#define WLAN_CIPHER_SUITE_BIP_GMAC_128	SUITE(0x000FAC, 11)
#define WLAN_CIPHER_SUITE_BIP_GMAC_256	SUITE(0x000FAC, 12)
#define WLAN_CIPHER_SUITE_BIP_CMAC_256	SUITE(0x000FAC, 13)

#define WLAN_CIPHER_SUITE_SMS4		SUITE(0x001472, 1)


#define WLAN_AKM_SUITE_8021X			SUITE(0x000FAC, 1)
#define WLAN_AKM_SUITE_PSK			SUITE(0x000FAC, 2)
#define WLAN_AKM_SUITE_FT_8021X			SUITE(0x000FAC, 3)
#define WLAN_AKM_SUITE_FT_PSK			SUITE(0x000FAC, 4)
#define WLAN_AKM_SUITE_8021X_SHA256		SUITE(0x000FAC, 5)
#define WLAN_AKM_SUITE_PSK_SHA256		SUITE(0x000FAC, 6)
#define WLAN_AKM_SUITE_TDLS			SUITE(0x000FAC, 7)
#define WLAN_AKM_SUITE_SAE			SUITE(0x000FAC, 8)
#define WLAN_AKM_SUITE_FT_OVER_SAE		SUITE(0x000FAC, 9)
#define WLAN_AKM_SUITE_AP_PEER_KEY		SUITE(0x000FAC, 10)
#define WLAN_AKM_SUITE_8021X_SUITE_B		SUITE(0x000FAC, 11)
#define WLAN_AKM_SUITE_8021X_SUITE_B_192	SUITE(0x000FAC, 12)
#define WLAN_AKM_SUITE_FT_8021X_SHA384		SUITE(0x000FAC, 13)
#define WLAN_AKM_SUITE_FILS_SHA256		SUITE(0x000FAC, 14)
#define WLAN_AKM_SUITE_FILS_SHA384		SUITE(0x000FAC, 15)
#define WLAN_AKM_SUITE_FT_FILS_SHA256		SUITE(0x000FAC, 16)
#define WLAN_AKM_SUITE_FT_FILS_SHA384		SUITE(0x000FAC, 17)
#define WLAN_AKM_SUITE_OWE			SUITE(0x000FAC, 18)
#define WLAN_AKM_SUITE_FT_PSK_SHA384		SUITE(0x000FAC, 19)
#define WLAN_AKM_SUITE_PSK_SHA384		SUITE(0x000FAC, 20)

#define WLAN_AKM_SUITE_WFA_DPP			SUITE(WLAN_OUI_WFA, 2)

#define WLAN_MAX_KEY_LEN		32

#define WLAN_PMK_NAME_LEN		16
#define WLAN_PMKID_LEN			16
#define WLAN_PMK_LEN_EAP_LEAP		16
#define WLAN_PMK_LEN			32
#define WLAN_PMK_LEN_SUITE_B_192	48

#define WLAN_OUI_WFA			0x506f9a
#define WLAN_OUI_TYPE_WFA_P2P		9
#define WLAN_OUI_TYPE_WFA_DPP		0x1A
#define WLAN_OUI_MICROSOFT		0x0050f2
#define WLAN_OUI_TYPE_MICROSOFT_WPA	1
#define WLAN_OUI_TYPE_MICROSOFT_WMM	2
#define WLAN_OUI_TYPE_MICROSOFT_WPS	4
#define WLAN_OUI_TYPE_MICROSOFT_TPC	8


#define IEEE80211_WMM_IE_TSPEC_TID_MASK		0x0F
#define IEEE80211_WMM_IE_TSPEC_TID_SHIFT	1

enum ieee80211_tspec_status_code {
	IEEE80211_TSPEC_STATUS_ADMISS_ACCEPTED = 0,
	IEEE80211_TSPEC_STATUS_ADDTS_INVAL_PARAMS = 0x1,
};

struct ieee80211_tspec_ie {
	u8 element_id;
	u8 len;
	u8 oui[3];
	u8 oui_type;
	u8 oui_subtype;
	u8 version;
	__le16 tsinfo;
	u8 tsinfo_resvd;
	__le16 nominal_msdu;
	__le16 max_msdu;
	__le32 min_service_int;
	__le32 max_service_int;
	__le32 inactivity_int;
	__le32 suspension_int;
	__le32 service_start_time;
	__le32 min_data_rate;
	__le32 mean_data_rate;
	__le32 peak_data_rate;
	__le32 max_burst_size;
	__le32 delay_bound;
	__le32 min_phy_rate;
	__le16 sba;
	__le16 medium_time;
} __packed;

struct ieee80211_he_6ghz_capa {
	
	__le16 capa;
} __packed;



#define IEEE80211_HE_6GHZ_CAP_MIN_MPDU_START	0x0007

#define IEEE80211_HE_6GHZ_CAP_MAX_AMPDU_LEN_EXP	0x0038

#define IEEE80211_HE_6GHZ_CAP_MAX_MPDU_LEN	0x00c0

#define IEEE80211_HE_6GHZ_CAP_SM_PS		0x0600
#define IEEE80211_HE_6GHZ_CAP_RD_RESPONDER	0x0800
#define IEEE80211_HE_6GHZ_CAP_RX_ANTPAT_CONS	0x1000
#define IEEE80211_HE_6GHZ_CAP_TX_ANTPAT_CONS	0x2000


static inline u8 *ieee80211_get_qos_ctl(struct ieee80211_hdr *hdr)
{
	union {
		struct ieee80211_qos_hdr	addr3;
		struct ieee80211_qos_hdr_4addr	addr4;
	} *qos;

	qos = (void *)hdr;
	if (ieee80211_has_a4(qos->addr3.frame_control))
		return (u8 *)&qos->addr4.qos_ctrl;
	else
		return (u8 *)&qos->addr3.qos_ctrl;
}


static inline u8 ieee80211_get_tid(struct ieee80211_hdr *hdr)
{
	u8 *qc = ieee80211_get_qos_ctl(hdr);

	return qc[0] & IEEE80211_QOS_CTL_TID_MASK;
}


static inline u8 *ieee80211_get_SA(struct ieee80211_hdr *hdr)
{
	if (ieee80211_has_a4(hdr->frame_control))
		return hdr->addr4;
	if (ieee80211_has_fromds(hdr->frame_control))
		return hdr->addr3;
	return hdr->addr2;
}


static inline u8 *ieee80211_get_DA(struct ieee80211_hdr *hdr)
{
	if (ieee80211_has_tods(hdr->frame_control))
		return hdr->addr3;
	else
		return hdr->addr1;
}


static inline bool ieee80211_is_bufferable_mmpdu(struct sk_buff *skb)
{
	struct ieee80211_mgmt *mgmt = (void *)skb->data;
	__le16 fc = mgmt->frame_control;

	
	if (!ieee80211_is_mgmt(fc))
		return false;

	if (ieee80211_is_disassoc(fc) || ieee80211_is_deauth(fc))
		return true;

	if (!ieee80211_is_action(fc))
		return false;

	if (skb->len < offsetofend(typeof(*mgmt), u.action.u.ftm.action_code))
		return true;

	

	if (mgmt->u.action.category != WLAN_CATEGORY_PUBLIC &&
	    mgmt->u.action.category != WLAN_CATEGORY_PROTECTED_DUAL_OF_ACTION)
		return true;

	if (mgmt->u.action.u.ftm.action_code == WLAN_PUB_ACTION_FTM_REQUEST ||
	    mgmt->u.action.u.ftm.action_code == WLAN_PUB_ACTION_FTM_RESPONSE)
		return false;

	return true;
}


static inline bool _ieee80211_is_robust_mgmt_frame(struct ieee80211_hdr *hdr)
{
	if (ieee80211_is_disassoc(hdr->frame_control) ||
	    ieee80211_is_deauth(hdr->frame_control))
		return true;

	if (ieee80211_is_action(hdr->frame_control)) {
		u8 *category;

		
		if (ieee80211_has_protected(hdr->frame_control))
			return true;
		category = ((u8 *) hdr) + 24;
		return *category != WLAN_CATEGORY_PUBLIC &&
			*category != WLAN_CATEGORY_HT &&
			*category != WLAN_CATEGORY_WNM_UNPROTECTED &&
			*category != WLAN_CATEGORY_SELF_PROTECTED &&
			*category != WLAN_CATEGORY_UNPROT_DMG &&
			*category != WLAN_CATEGORY_VHT &&
			*category != WLAN_CATEGORY_S1G &&
			*category != WLAN_CATEGORY_VENDOR_SPECIFIC;
	}

	return false;
}


static inline bool ieee80211_is_robust_mgmt_frame(struct sk_buff *skb)
{
	if (skb->len < IEEE80211_MIN_ACTION_SIZE)
		return false;
	return _ieee80211_is_robust_mgmt_frame((void *)skb->data);
}


static inline bool ieee80211_is_public_action(struct ieee80211_hdr *hdr,
					      size_t len)
{
	struct ieee80211_mgmt *mgmt = (void *)hdr;

	if (len < IEEE80211_MIN_ACTION_SIZE)
		return false;
	if (!ieee80211_is_action(hdr->frame_control))
		return false;
	return mgmt->u.action.category == WLAN_CATEGORY_PUBLIC;
}


static inline bool
ieee80211_is_protected_dual_of_public_action(struct sk_buff *skb)
{
	u8 action;

	if (!ieee80211_is_public_action((void *)skb->data, skb->len) ||
	    skb->len < IEEE80211_MIN_ACTION_SIZE + 1)
		return false;

	action = *(u8 *)(skb->data + IEEE80211_MIN_ACTION_SIZE);

	return action != WLAN_PUB_ACTION_20_40_BSS_COEX &&
		action != WLAN_PUB_ACTION_DSE_REG_LOC_ANN &&
		action != WLAN_PUB_ACTION_MSMT_PILOT &&
		action != WLAN_PUB_ACTION_TDLS_DISCOVER_RES &&
		action != WLAN_PUB_ACTION_LOC_TRACK_NOTI &&
		action != WLAN_PUB_ACTION_FTM_REQUEST &&
		action != WLAN_PUB_ACTION_FTM_RESPONSE &&
		action != WLAN_PUB_ACTION_FILS_DISCOVERY &&
		action != WLAN_PUB_ACTION_VENDOR_SPECIFIC;
}


static inline bool _ieee80211_is_group_privacy_action(struct ieee80211_hdr *hdr)
{
	struct ieee80211_mgmt *mgmt = (void *)hdr;

	if (!ieee80211_is_action(hdr->frame_control) ||
	    !is_multicast_ether_addr(hdr->addr1))
		return false;

	return mgmt->u.action.category == WLAN_CATEGORY_MESH_ACTION ||
	       mgmt->u.action.category == WLAN_CATEGORY_MULTIHOP_ACTION;
}


static inline bool ieee80211_is_group_privacy_action(struct sk_buff *skb)
{
	if (skb->len < IEEE80211_MIN_ACTION_SIZE)
		return false;
	return _ieee80211_is_group_privacy_action((void *)skb->data);
}


static inline unsigned long ieee80211_tu_to_usec(unsigned long tu)
{
	return 1024 * tu;
}


static inline bool ieee80211_check_tim(const struct ieee80211_tim_ie *tim,
				       u8 tim_len, u16 aid)
{
	u8 mask;
	u8 index, indexn1, indexn2;

	if (unlikely(!tim || tim_len < sizeof(*tim)))
		return false;

	aid &= 0x3fff;
	index = aid / 8;
	mask  = 1 << (aid & 7);

	indexn1 = tim->bitmap_ctrl & 0xfe;
	indexn2 = tim_len + indexn1 - 4;

	if (index < indexn1 || index > indexn2)
		return false;

	index -= indexn1;

	return !!(tim->virtual_map[index] & mask);
}


static inline int ieee80211_get_tdls_action(struct sk_buff *skb, u32 hdr_size)
{
	if (!skb_is_nonlinear(skb) &&
	    skb->len > (skb_network_offset(skb) + 2)) {
		
		const u8 *tdls_data = skb_network_header(skb) - 2;

		if (get_unaligned_be16(tdls_data) == ETH_P_TDLS &&
		    tdls_data[2] == WLAN_TDLS_SNAP_RFTYPE &&
		    tdls_data[3] == WLAN_CATEGORY_TDLS)
			return tdls_data[4];
	}

	return -1;
}


#define TU_TO_JIFFIES(x)	(usecs_to_jiffies((x) * 1024))
#define TU_TO_EXP_TIME(x)	(jiffies + TU_TO_JIFFIES(x))


#define MHZ_TO_KHZ(freq) ((freq) * 1000)
#define KHZ_TO_MHZ(freq) ((freq) / 1000)
#define PR_KHZ(f) KHZ_TO_MHZ(f), f % 1000
#define KHZ_F "%d.%03d"


#define DBI_TO_MBI(gain) ((gain) * 100)
#define MBI_TO_DBI(gain) ((gain) / 100)
#define DBM_TO_MBM(gain) ((gain) * 100)
#define MBM_TO_DBM(gain) ((gain) / 100)


static inline bool ieee80211_action_contains_tpc(struct sk_buff *skb)
{
	struct ieee80211_mgmt *mgmt = (void *)skb->data;

	if (!ieee80211_is_action(mgmt->frame_control))
		return false;

	if (skb->len < IEEE80211_MIN_ACTION_SIZE +
		       sizeof(mgmt->u.action.u.tpc_report))
		return false;

	
	if (mgmt->u.action.category != WLAN_CATEGORY_SPECTRUM_MGMT &&
	    mgmt->u.action.category != WLAN_CATEGORY_RADIO_MEASUREMENT)
		return false;

	
	if (mgmt->u.action.u.tpc_report.action_code !=
	    WLAN_ACTION_SPCT_TPC_RPRT)
		return false;

	if (mgmt->u.action.u.tpc_report.tpc_elem_id != WLAN_EID_TPC_REPORT ||
	    mgmt->u.action.u.tpc_report.tpc_elem_length !=
	    sizeof(struct ieee80211_tpc_report_ie))
		return false;

	return true;
}

static inline bool ieee80211_is_timing_measurement(struct sk_buff *skb)
{
	struct ieee80211_mgmt *mgmt = (void *)skb->data;

	if (skb->len < IEEE80211_MIN_ACTION_SIZE)
		return false;

	if (!ieee80211_is_action(mgmt->frame_control))
		return false;

	if (mgmt->u.action.category == WLAN_CATEGORY_WNM_UNPROTECTED &&
	    mgmt->u.action.u.wnm_timing_msr.action_code ==
		WLAN_UNPROTECTED_WNM_ACTION_TIMING_MEASUREMENT_RESPONSE &&
	    skb->len >= offsetofend(typeof(*mgmt), u.action.u.wnm_timing_msr))
		return true;

	return false;
}

static inline bool ieee80211_is_ftm(struct sk_buff *skb)
{
	struct ieee80211_mgmt *mgmt = (void *)skb->data;

	if (!ieee80211_is_public_action((void *)mgmt, skb->len))
		return false;

	if (mgmt->u.action.u.ftm.action_code ==
		WLAN_PUB_ACTION_FTM_RESPONSE &&
	    skb->len >= offsetofend(typeof(*mgmt), u.action.u.ftm))
		return true;

	return false;
}

struct element {
	u8 id;
	u8 datalen;
	u8 data[];
} __packed;


#define for_each_element(_elem, _data, _datalen)			\
	for (_elem = (const struct element *)(_data);			\
	     (const u8 *)(_data) + (_datalen) - (const u8 *)_elem >=	\
		(int)sizeof(*_elem) &&					\
	     (const u8 *)(_data) + (_datalen) - (const u8 *)_elem >=	\
		(int)sizeof(*_elem) + _elem->datalen;			\
	     _elem = (const struct element *)(_elem->data + _elem->datalen))

#define for_each_element_id(element, _id, data, datalen)		\
	for_each_element(element, data, datalen)			\
		if (element->id == (_id))

#define for_each_element_extid(element, extid, _data, _datalen)		\
	for_each_element(element, _data, _datalen)			\
		if (element->id == WLAN_EID_EXTENSION &&		\
		    element->datalen > 0 &&				\
		    element->data[0] == (extid))

#define for_each_subelement(sub, element)				\
	for_each_element(sub, (element)->data, (element)->datalen)

#define for_each_subelement_id(sub, id, element)			\
	for_each_element_id(sub, id, (element)->data, (element)->datalen)

#define for_each_subelement_extid(sub, extid, element)			\
	for_each_element_extid(sub, extid, (element)->data, (element)->datalen)


static inline bool for_each_element_completed(const struct element *element,
					      const void *data, size_t datalen)
{
	return (const u8 *)element == (const u8 *)data + datalen;
}


#define WLAN_RSNX_CAPA_PROTECTED_TWT BIT(4)
#define WLAN_RSNX_CAPA_SAE_H2E BIT(5)


#define IEEE80211_AP_INFO_TBTT_HDR_TYPE				0x03
#define IEEE80211_AP_INFO_TBTT_HDR_FILTERED			0x04
#define IEEE80211_AP_INFO_TBTT_HDR_COLOC			0x08
#define IEEE80211_AP_INFO_TBTT_HDR_COUNT			0xF0
#define IEEE80211_TBTT_INFO_TYPE_TBTT				0
#define IEEE80211_TBTT_INFO_TYPE_MLD				1

#define IEEE80211_RNR_TBTT_PARAMS_OCT_RECOMMENDED		0x01
#define IEEE80211_RNR_TBTT_PARAMS_SAME_SSID			0x02
#define IEEE80211_RNR_TBTT_PARAMS_MULTI_BSSID			0x04
#define IEEE80211_RNR_TBTT_PARAMS_TRANSMITTED_BSSID		0x08
#define IEEE80211_RNR_TBTT_PARAMS_COLOC_ESS			0x10
#define IEEE80211_RNR_TBTT_PARAMS_PROBE_ACTIVE			0x20
#define IEEE80211_RNR_TBTT_PARAMS_COLOC_AP			0x40

#define IEEE80211_RNR_TBTT_PARAMS_PSD_NO_LIMIT			127
#define IEEE80211_RNR_TBTT_PARAMS_PSD_RESERVED			-128

struct ieee80211_neighbor_ap_info {
	u8 tbtt_info_hdr;
	u8 tbtt_info_len;
	u8 op_class;
	u8 channel;
} __packed;

enum ieee80211_range_params_max_total_ltf {
	IEEE80211_RANGE_PARAMS_MAX_TOTAL_LTF_4 = 0,
	IEEE80211_RANGE_PARAMS_MAX_TOTAL_LTF_8,
	IEEE80211_RANGE_PARAMS_MAX_TOTAL_LTF_16,
	IEEE80211_RANGE_PARAMS_MAX_TOTAL_LTF_UNSPECIFIED,
};


struct ieee80211_rnr_mld_params {
	u8 mld_id;
	__le16 params;
} __packed;

#define IEEE80211_RNR_MLD_PARAMS_LINK_ID			0x000F
#define IEEE80211_RNR_MLD_PARAMS_BSS_CHANGE_COUNT		0x0FF0
#define IEEE80211_RNR_MLD_PARAMS_UPDATES_INCLUDED		0x1000
#define IEEE80211_RNR_MLD_PARAMS_DISABLED_LINK			0x2000


struct ieee80211_tbtt_info_7_8_9 {
	u8 tbtt_offset;
	u8 bssid[ETH_ALEN];

	
	u8 bss_params;
	s8 psd_20;
} __packed;


struct ieee80211_tbtt_info_ge_11 {
	u8 tbtt_offset;
	u8 bssid[ETH_ALEN];
	__le32 short_ssid;

	
	u8 bss_params;
	s8 psd_20;
	struct ieee80211_rnr_mld_params mld_params;
} __packed;


#define IEEE80211_MLD_MAX_NUM_LINKS	15

#define IEEE80211_ML_CONTROL_TYPE			0x0007
#define IEEE80211_ML_CONTROL_TYPE_BASIC			0
#define IEEE80211_ML_CONTROL_TYPE_PREQ			1
#define IEEE80211_ML_CONTROL_TYPE_RECONF		2
#define IEEE80211_ML_CONTROL_TYPE_TDLS			3
#define IEEE80211_ML_CONTROL_TYPE_PRIO_ACCESS		4
#define IEEE80211_ML_CONTROL_PRESENCE_MASK		0xfff0

struct ieee80211_multi_link_elem {
	__le16 control;
	u8 variable[];
} __packed;

#define IEEE80211_MLC_BASIC_PRES_LINK_ID		0x0010
#define IEEE80211_MLC_BASIC_PRES_BSS_PARAM_CH_CNT	0x0020
#define IEEE80211_MLC_BASIC_PRES_MED_SYNC_DELAY		0x0040
#define IEEE80211_MLC_BASIC_PRES_EML_CAPA		0x0080
#define IEEE80211_MLC_BASIC_PRES_MLD_CAPA_OP		0x0100
#define IEEE80211_MLC_BASIC_PRES_MLD_ID			0x0200

#define IEEE80211_MED_SYNC_DELAY_DURATION		0x00ff
#define IEEE80211_MED_SYNC_DELAY_SYNC_OFDM_ED_THRESH	0x0f00
#define IEEE80211_MED_SYNC_DELAY_SYNC_MAX_NUM_TXOPS	0xf000


#define IEEE80211_MED_SYNC_DELAY_DEFAULT		0x10ac

#define IEEE80211_EML_CAP_EMLSR_SUPP			0x0001
#define IEEE80211_EML_CAP_EMLSR_PADDING_DELAY		0x000e
#define  IEEE80211_EML_CAP_EMLSR_PADDING_DELAY_0US		0
#define  IEEE80211_EML_CAP_EMLSR_PADDING_DELAY_32US		1
#define  IEEE80211_EML_CAP_EMLSR_PADDING_DELAY_64US		2
#define  IEEE80211_EML_CAP_EMLSR_PADDING_DELAY_128US		3
#define  IEEE80211_EML_CAP_EMLSR_PADDING_DELAY_256US		4
#define IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY	0x0070
#define  IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY_0US		0
#define  IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY_16US		1
#define  IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY_32US		2
#define  IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY_64US		3
#define  IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY_128US		4
#define  IEEE80211_EML_CAP_EMLSR_TRANSITION_DELAY_256US		5
#define IEEE80211_EML_CAP_EMLMR_SUPPORT			0x0080
#define IEEE80211_EML_CAP_EMLMR_DELAY			0x0700
#define  IEEE80211_EML_CAP_EMLMR_DELAY_0US			0
#define  IEEE80211_EML_CAP_EMLMR_DELAY_32US			1
#define  IEEE80211_EML_CAP_EMLMR_DELAY_64US			2
#define  IEEE80211_EML_CAP_EMLMR_DELAY_128US			3
#define  IEEE80211_EML_CAP_EMLMR_DELAY_256US			4
#define IEEE80211_EML_CAP_TRANSITION_TIMEOUT		0x7800
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_0			0
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_128US		1
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_256US		2
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_512US		3
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_1TU		4
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_2TU		5
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_4TU		6
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_8TU		7
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_16TU		8
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_32TU		9
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_64TU		10
#define  IEEE80211_EML_CAP_TRANSITION_TIMEOUT_128TU		11

#define IEEE80211_MLD_CAP_OP_MAX_SIMUL_LINKS		0x000f
#define IEEE80211_MLD_CAP_OP_SRS_SUPPORT		0x0010
#define IEEE80211_MLD_CAP_OP_TID_TO_LINK_MAP_NEG_SUPP	0x0060
#define IEEE80211_MLD_CAP_OP_FREQ_SEP_TYPE_IND		0x0f80
#define IEEE80211_MLD_CAP_OP_AAR_SUPPORT		0x1000

struct ieee80211_mle_basic_common_info {
	u8 len;
	u8 mld_mac_addr[ETH_ALEN];
	u8 variable[];
} __packed;

#define IEEE80211_MLC_PREQ_PRES_MLD_ID			0x0010

struct ieee80211_mle_preq_common_info {
	u8 len;
	u8 variable[];
} __packed;

#define IEEE80211_MLC_RECONF_PRES_MLD_MAC_ADDR		0x0010



struct ieee80211_mle_tdls_common_info {
	u8 len;
	u8 ap_mld_mac_addr[ETH_ALEN];
} __packed;

#define IEEE80211_MLC_PRIO_ACCESS_PRES_AP_MLD_MAC_ADDR	0x0010




static inline u8 ieee80211_mle_common_size(const u8 *data)
{
	const struct ieee80211_multi_link_elem *mle = (const void *)data;
	u16 control = le16_to_cpu(mle->control);
	u8 common = 0;

	switch (u16_get_bits(control, IEEE80211_ML_CONTROL_TYPE)) {
	case IEEE80211_ML_CONTROL_TYPE_BASIC:
	case IEEE80211_ML_CONTROL_TYPE_PREQ:
	case IEEE80211_ML_CONTROL_TYPE_TDLS:
	case IEEE80211_ML_CONTROL_TYPE_RECONF:
		
		break;
	case IEEE80211_ML_CONTROL_TYPE_PRIO_ACCESS:
		if (control & IEEE80211_MLC_PRIO_ACCESS_PRES_AP_MLD_MAC_ADDR)
			common += ETH_ALEN;
		return common;
	default:
		WARN_ON(1);
		return 0;
	}

	return sizeof(*mle) + common + mle->variable[0];
}


static inline u8
ieee80211_mle_get_bss_param_ch_cnt(const struct ieee80211_multi_link_elem *mle)
{
	u16 control = le16_to_cpu(mle->control);
	const u8 *common = mle->variable;

	
	common += sizeof(struct ieee80211_mle_basic_common_info);

	if (!(control & IEEE80211_MLC_BASIC_PRES_BSS_PARAM_CH_CNT))
		return 0;

	if (control & IEEE80211_MLC_BASIC_PRES_LINK_ID)
		common += 1;

	return *common;
}


static inline u16 ieee80211_mle_get_eml_med_sync_delay(const u8 *data)
{
	const struct ieee80211_multi_link_elem *mle = (const void *)data;
	u16 control = le16_to_cpu(mle->control);
	const u8 *common = mle->variable;

	
	common += sizeof(struct ieee80211_mle_basic_common_info);

	if (!(control & IEEE80211_MLC_BASIC_PRES_MED_SYNC_DELAY))
		return IEEE80211_MED_SYNC_DELAY_DEFAULT;

	if (control & IEEE80211_MLC_BASIC_PRES_LINK_ID)
		common += 1;
	if (control & IEEE80211_MLC_BASIC_PRES_BSS_PARAM_CH_CNT)
		common += 1;

	return get_unaligned_le16(common);
}


static inline u16 ieee80211_mle_get_eml_cap(const u8 *data)
{
	const struct ieee80211_multi_link_elem *mle = (const void *)data;
	u16 control = le16_to_cpu(mle->control);
	const u8 *common = mle->variable;

	
	common += sizeof(struct ieee80211_mle_basic_common_info);

	if (!(control & IEEE80211_MLC_BASIC_PRES_EML_CAPA))
		return 0;

	if (control & IEEE80211_MLC_BASIC_PRES_LINK_ID)
		common += 1;
	if (control & IEEE80211_MLC_BASIC_PRES_BSS_PARAM_CH_CNT)
		common += 1;
	if (control & IEEE80211_MLC_BASIC_PRES_MED_SYNC_DELAY)
		common += 2;

	return get_unaligned_le16(common);
}


static inline bool ieee80211_mle_size_ok(const u8 *data, size_t len)
{
	const struct ieee80211_multi_link_elem *mle = (const void *)data;
	u8 fixed = sizeof(*mle);
	u8 common = 0;
	bool check_common_len = false;
	u16 control;

	if (!data || len < fixed)
		return false;

	control = le16_to_cpu(mle->control);

	switch (u16_get_bits(control, IEEE80211_ML_CONTROL_TYPE)) {
	case IEEE80211_ML_CONTROL_TYPE_BASIC:
		common += sizeof(struct ieee80211_mle_basic_common_info);
		check_common_len = true;
		if (control & IEEE80211_MLC_BASIC_PRES_LINK_ID)
			common += 1;
		if (control & IEEE80211_MLC_BASIC_PRES_BSS_PARAM_CH_CNT)
			common += 1;
		if (control & IEEE80211_MLC_BASIC_PRES_MED_SYNC_DELAY)
			common += 2;
		if (control & IEEE80211_MLC_BASIC_PRES_EML_CAPA)
			common += 2;
		if (control & IEEE80211_MLC_BASIC_PRES_MLD_CAPA_OP)
			common += 2;
		if (control & IEEE80211_MLC_BASIC_PRES_MLD_ID)
			common += 1;
		break;
	case IEEE80211_ML_CONTROL_TYPE_PREQ:
		common += sizeof(struct ieee80211_mle_preq_common_info);
		if (control & IEEE80211_MLC_PREQ_PRES_MLD_ID)
			common += 1;
		check_common_len = true;
		break;
	case IEEE80211_ML_CONTROL_TYPE_RECONF:
		if (control & IEEE80211_MLC_RECONF_PRES_MLD_MAC_ADDR)
			common += ETH_ALEN;
		break;
	case IEEE80211_ML_CONTROL_TYPE_TDLS:
		common += sizeof(struct ieee80211_mle_tdls_common_info);
		check_common_len = true;
		break;
	case IEEE80211_ML_CONTROL_TYPE_PRIO_ACCESS:
		if (control & IEEE80211_MLC_PRIO_ACCESS_PRES_AP_MLD_MAC_ADDR)
			common += ETH_ALEN;
		break;
	default:
		
		return true;
	}

	if (len < fixed + common)
		return false;

	if (!check_common_len)
		return true;

	
	return mle->variable[0] >= common;
}


static inline bool ieee80211_mle_type_ok(const u8 *data, u8 type, size_t len)
{
	const struct ieee80211_multi_link_elem *mle = (const void *)data;
	u16 control;

	if (!ieee80211_mle_size_ok(data, len))
		return false;

	control = le16_to_cpu(mle->control);

	if (u16_get_bits(control, IEEE80211_ML_CONTROL_TYPE) == type)
		return true;

	return false;
}

enum ieee80211_mle_subelems {
	IEEE80211_MLE_SUBELEM_PER_STA_PROFILE		= 0,
	IEEE80211_MLE_SUBELEM_FRAGMENT		        = 254,
};

#define IEEE80211_MLE_STA_CONTROL_LINK_ID			0x000f
#define IEEE80211_MLE_STA_CONTROL_COMPLETE_PROFILE		0x0010
#define IEEE80211_MLE_STA_CONTROL_STA_MAC_ADDR_PRESENT		0x0020
#define IEEE80211_MLE_STA_CONTROL_BEACON_INT_PRESENT		0x0040
#define IEEE80211_MLE_STA_CONTROL_TSF_OFFS_PRESENT		0x0080
#define IEEE80211_MLE_STA_CONTROL_DTIM_INFO_PRESENT		0x0100
#define IEEE80211_MLE_STA_CONTROL_NSTR_LINK_PAIR_PRESENT	0x0200
#define IEEE80211_MLE_STA_CONTROL_NSTR_BITMAP_SIZE		0x0400
#define IEEE80211_MLE_STA_CONTROL_BSS_PARAM_CHANGE_CNT_PRESENT	0x0800

struct ieee80211_mle_per_sta_profile {
	__le16 control;
	u8 sta_info_len;
	u8 variable[];
} __packed;


static inline bool ieee80211_mle_basic_sta_prof_size_ok(const u8 *data,
							size_t len)
{
	const struct ieee80211_mle_per_sta_profile *prof = (const void *)data;
	u16 control;
	u8 fixed = sizeof(*prof);
	u8 info_len = 1;

	if (len < fixed)
		return false;

	control = le16_to_cpu(prof->control);

	if (control & IEEE80211_MLE_STA_CONTROL_STA_MAC_ADDR_PRESENT)
		info_len += 6;
	if (control & IEEE80211_MLE_STA_CONTROL_BEACON_INT_PRESENT)
		info_len += 2;
	if (control & IEEE80211_MLE_STA_CONTROL_TSF_OFFS_PRESENT)
		info_len += 8;
	if (control & IEEE80211_MLE_STA_CONTROL_DTIM_INFO_PRESENT)
		info_len += 2;
	if (control & IEEE80211_MLE_STA_CONTROL_COMPLETE_PROFILE &&
	    control & IEEE80211_MLE_STA_CONTROL_NSTR_LINK_PAIR_PRESENT) {
		if (control & IEEE80211_MLE_STA_CONTROL_NSTR_BITMAP_SIZE)
			info_len += 2;
		else
			info_len += 1;
	}
	if (control & IEEE80211_MLE_STA_CONTROL_BSS_PARAM_CHANGE_CNT_PRESENT)
		info_len += 1;

	return prof->sta_info_len >= info_len &&
	       fixed + prof->sta_info_len - 1 <= len;
}


static inline u8
ieee80211_mle_basic_sta_prof_bss_param_ch_cnt(const struct ieee80211_mle_per_sta_profile *prof)
{
	u16 control = le16_to_cpu(prof->control);
	const u8 *pos = prof->variable;

	if (!(control & IEEE80211_MLE_STA_CONTROL_BSS_PARAM_CHANGE_CNT_PRESENT))
		return 0;

	if (control & IEEE80211_MLE_STA_CONTROL_STA_MAC_ADDR_PRESENT)
		pos += 6;
	if (control & IEEE80211_MLE_STA_CONTROL_BEACON_INT_PRESENT)
		pos += 2;
	if (control & IEEE80211_MLE_STA_CONTROL_TSF_OFFS_PRESENT)
		pos += 8;
	if (control & IEEE80211_MLE_STA_CONTROL_DTIM_INFO_PRESENT)
		pos += 2;
	if (control & IEEE80211_MLE_STA_CONTROL_COMPLETE_PROFILE &&
	    control & IEEE80211_MLE_STA_CONTROL_NSTR_LINK_PAIR_PRESENT) {
		if (control & IEEE80211_MLE_STA_CONTROL_NSTR_BITMAP_SIZE)
			pos += 2;
		else
			pos += 1;
	}

	return *pos;
}

#define IEEE80211_MLE_STA_RECONF_CONTROL_LINK_ID			0x000f
#define IEEE80211_MLE_STA_RECONF_CONTROL_COMPLETE_PROFILE		0x0010
#define IEEE80211_MLE_STA_RECONF_CONTROL_STA_MAC_ADDR_PRESENT		0x0020
#define IEEE80211_MLE_STA_RECONF_CONTROL_AP_REM_TIMER_PRESENT		0x0040
#define IEEE80211_MLE_STA_RECONF_CONTROL_OPERATION_UPDATE_TYPE		0x0780
#define IEEE80211_MLE_STA_RECONF_CONTROL_OPERATION_PARAMS_PRESENT	0x0800


static inline bool ieee80211_mle_reconf_sta_prof_size_ok(const u8 *data,
							 size_t len)
{
	const struct ieee80211_mle_per_sta_profile *prof = (const void *)data;
	u16 control;
	u8 fixed = sizeof(*prof);
	u8 info_len = 1;

	if (len < fixed)
		return false;

	control = le16_to_cpu(prof->control);

	if (control & IEEE80211_MLE_STA_RECONF_CONTROL_STA_MAC_ADDR_PRESENT)
		info_len += ETH_ALEN;
	if (control & IEEE80211_MLE_STA_RECONF_CONTROL_AP_REM_TIMER_PRESENT)
		info_len += 2;
	if (control & IEEE80211_MLE_STA_RECONF_CONTROL_OPERATION_PARAMS_PRESENT)
		info_len += 2;

	return prof->sta_info_len >= info_len &&
	       fixed + prof->sta_info_len - 1 <= len;
}

#define for_each_mle_subelement(_elem, _data, _len)			\
	if (ieee80211_mle_size_ok(_data, _len))				\
		for_each_element(_elem,					\
				 _data + ieee80211_mle_common_size(_data),\
				 _len - ieee80211_mle_common_size(_data))

#endif 
