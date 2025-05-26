/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NF_REJECT_H
#define _NF_REJECT_H

#include <linux/types.h>
#include <uapi/linux/in.h>

static inline bool nf_reject_verify_csum(struct sk_buff *skb, int dataoff,
					  __u8 proto)
{
	
	switch (proto) {
		
		case IPPROTO_UDP: {
			const struct udphdr *udp_hdr;
			struct udphdr _udp_hdr;

			udp_hdr = skb_header_pointer(skb, dataoff,
						     sizeof(_udp_hdr),
						     &_udp_hdr);
			if (!udp_hdr || udp_hdr->check)
				return true;

			return false;
		}
		case IPPROTO_GRE:

		
		case IPPROTO_AH:
		case IPPROTO_ESP:
		case IPPROTO_SCTP:

		
		case IPPROTO_UDPLITE:
		case IPPROTO_DCCP:
			return false;
	}
	return true;
}

#endif 
