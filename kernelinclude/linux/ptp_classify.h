/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _PTP_CLASSIFY_H_
#define _PTP_CLASSIFY_H_

#include <asm/unaligned.h>
#include <linux/ip.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <net/checksum.h>

#define PTP_CLASS_NONE  0x00 
#define PTP_CLASS_V1    0x01 
#define PTP_CLASS_V2    0x02 
#define PTP_CLASS_VMASK 0x0f 
#define PTP_CLASS_IPV4  0x10 
#define PTP_CLASS_IPV6  0x20 
#define PTP_CLASS_L2    0x40 
#define PTP_CLASS_PMASK	0x70 
#define PTP_CLASS_VLAN	0x80 

#define PTP_CLASS_V1_IPV4 (PTP_CLASS_V1 | PTP_CLASS_IPV4)
#define PTP_CLASS_V1_IPV6 (PTP_CLASS_V1 | PTP_CLASS_IPV6) 
#define PTP_CLASS_V2_IPV4 (PTP_CLASS_V2 | PTP_CLASS_IPV4)
#define PTP_CLASS_V2_IPV6 (PTP_CLASS_V2 | PTP_CLASS_IPV6)
#define PTP_CLASS_V2_L2   (PTP_CLASS_V2 | PTP_CLASS_L2)
#define PTP_CLASS_V2_VLAN (PTP_CLASS_V2 | PTP_CLASS_VLAN)
#define PTP_CLASS_L4      (PTP_CLASS_IPV4 | PTP_CLASS_IPV6)

#define PTP_MSGTYPE_SYNC        0x0
#define PTP_MSGTYPE_DELAY_REQ   0x1
#define PTP_MSGTYPE_PDELAY_REQ  0x2
#define PTP_MSGTYPE_PDELAY_RESP 0x3

#define PTP_EV_PORT 319
#define PTP_GEN_PORT 320
#define PTP_GEN_BIT 0x08 

#define OFF_PTP_SOURCE_UUID	22 
#define OFF_PTP_SEQUENCE_ID	30


#define PTP_FLAG_TWOSTEP	BIT(1)


#define IP6_HLEN	40
#define UDP_HLEN	8
#define OFF_IHL		14
#define IPV4_HLEN(data) (((struct iphdr *)(data + OFF_IHL))->ihl << 2)

struct clock_identity {
	u8 id[8];
} __packed;

struct port_identity {
	struct clock_identity	clock_identity;
	__be16			port_number;
} __packed;

struct ptp_header {
	u8			tsmt;  
	u8			ver;   
	__be16			message_length;
	u8			domain_number;
	u8			reserved1;
	u8			flag_field[2];
	__be64			correction;
	__be32			reserved2;
	struct port_identity	source_port_identity;
	__be16			sequence_id;
	u8			control;
	u8			log_message_interval;
} __packed;

#if defined(CONFIG_NET_PTP_CLASSIFY)

unsigned int ptp_classify_raw(const struct sk_buff *skb);


struct ptp_header *ptp_parse_header(struct sk_buff *skb, unsigned int type);


static inline u8 ptp_get_msgtype(const struct ptp_header *hdr,
				 unsigned int type)
{
	u8 msgtype;

	if (unlikely(type & PTP_CLASS_V1)) {
		
		msgtype = hdr->control;
	} else {
		msgtype = hdr->tsmt & 0x0f;
	}

	return msgtype;
}


static inline __wsum ptp_check_diff8(__be64 old, __be64 new, __wsum oldsum)
{
	__be64 diff[2] = { ~old, new };

	return csum_partial(diff, sizeof(diff), oldsum);
}


static inline
void ptp_header_update_correction(struct sk_buff *skb, unsigned int type,
				  struct ptp_header *hdr, s64 correction)
{
	__be64 correction_old;
	struct udphdr *uhdr;

	
	memcpy(&correction_old,  &hdr->correction, sizeof(correction_old));

	
	put_unaligned_be64((u64)correction, &hdr->correction);

	switch (type & PTP_CLASS_PMASK) {
	case PTP_CLASS_IPV4:
	case PTP_CLASS_IPV6:
		
		uhdr = (struct udphdr *)((char *)hdr - sizeof(struct udphdr));
		break;
	default:
		return;
	}

	
	uhdr->check = csum_fold(ptp_check_diff8(correction_old,
						hdr->correction,
						~csum_unfold(uhdr->check)));
	if (!uhdr->check)
		uhdr->check = CSUM_MANGLED_0;

	skb->ip_summed = CHECKSUM_NONE;
}


bool ptp_msg_is_sync(struct sk_buff *skb, unsigned int type);

void __init ptp_classifier_init(void);
#else
static inline void ptp_classifier_init(void)
{
}
static inline unsigned int ptp_classify_raw(struct sk_buff *skb)
{
	return PTP_CLASS_NONE;
}
static inline struct ptp_header *ptp_parse_header(struct sk_buff *skb,
						  unsigned int type)
{
	return NULL;
}
static inline u8 ptp_get_msgtype(const struct ptp_header *hdr,
				 unsigned int type)
{
	
	return PTP_MSGTYPE_SYNC;
}
static inline bool ptp_msg_is_sync(struct sk_buff *skb, unsigned int type)
{
	return false;
}

static inline
void ptp_header_update_correction(struct sk_buff *skb, unsigned int type,
				  struct ptp_header *hdr, s64 correction)
{
}
#endif
#endif 
