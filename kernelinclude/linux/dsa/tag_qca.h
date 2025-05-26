/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __TAG_QCA_H
#define __TAG_QCA_H

#include <linux/types.h>

struct dsa_switch;
struct sk_buff;

#define QCA_HDR_LEN	2
#define QCA_HDR_VERSION	0x2

#define QCA_HDR_RECV_VERSION		GENMASK(15, 14)
#define QCA_HDR_RECV_PRIORITY		GENMASK(13, 11)
#define QCA_HDR_RECV_TYPE		GENMASK(10, 6)
#define QCA_HDR_RECV_FRAME_IS_TAGGED	BIT(3)
#define QCA_HDR_RECV_SOURCE_PORT	GENMASK(2, 0)


#define QCA_HDR_RECV_TYPE_NORMAL	0x0
#define QCA_HDR_RECV_TYPE_MIB		0x1
#define QCA_HDR_RECV_TYPE_RW_REG_ACK	0x2

#define QCA_HDR_XMIT_VERSION		GENMASK(15, 14)
#define QCA_HDR_XMIT_PRIORITY		GENMASK(13, 11)
#define QCA_HDR_XMIT_CONTROL		GENMASK(10, 8)
#define QCA_HDR_XMIT_FROM_CPU		BIT(7)
#define QCA_HDR_XMIT_DP_BIT		GENMASK(6, 0)


#define QCA_HDR_XMIT_TYPE_NORMAL	0x0
#define QCA_HDR_XMIT_TYPE_RW_REG	0x1


#define QCA_HDR_MGMT_CHECK_CODE_VAL	0x5


#define QCA_HDR_MGMT_SEQ_LEN		4 
#define QCA_HDR_MGMT_COMMAND_LEN	4 
#define QCA_HDR_MGMT_DATA1_LEN		4 
#define QCA_HDR_MGMT_HEADER_LEN		(QCA_HDR_MGMT_SEQ_LEN + \
					QCA_HDR_MGMT_COMMAND_LEN + \
					QCA_HDR_MGMT_DATA1_LEN)

#define QCA_HDR_MGMT_DATA2_LEN		28 
#define QCA_HDR_MGMT_PADDING_LEN	18 

#define QCA_HDR_MGMT_PKT_LEN		(QCA_HDR_MGMT_HEADER_LEN + \
					QCA_HDR_LEN + \
					QCA_HDR_MGMT_DATA2_LEN + \
					QCA_HDR_MGMT_PADDING_LEN)

#define QCA_HDR_MGMT_SEQ_NUM		GENMASK(31, 0)  
#define QCA_HDR_MGMT_CHECK_CODE		GENMASK(31, 29) 
#define QCA_HDR_MGMT_CMD		BIT(28)		
#define QCA_HDR_MGMT_LENGTH		GENMASK(23, 20) 
#define QCA_HDR_MGMT_ADDR		GENMASK(18, 0)  


struct qca_mgmt_ethhdr {
	__le32 command;		
	__le32 seq;		
	__le32 mdio_data;		
	__be16 hdr;		
} __packed;

enum mdio_cmd {
	MDIO_WRITE = 0x0,
	MDIO_READ
};

struct mib_ethhdr {
	__le32 data[3];		
	__be16 hdr;		
} __packed;

struct qca_tagger_data {
	void (*rw_reg_ack_handler)(struct dsa_switch *ds,
				   struct sk_buff *skb);
	void (*mib_autocast_handler)(struct dsa_switch *ds,
				     struct sk_buff *skb);
};

#endif 
