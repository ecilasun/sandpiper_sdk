/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _UDPLITE_H
#define _UDPLITE_H

#include <net/ip6_checksum.h>
#include <net/udp.h>


#define UDPLITE_SEND_CSCOV   10 
#define UDPLITE_RECV_CSCOV   11 

extern struct proto 		udplite_prot;
extern struct udp_table		udplite_table;


static __inline__ int udplite_getfrag(void *from, char *to, int  offset,
				      int len, int odd, struct sk_buff *skb)
{
	struct msghdr *msg = from;
	return copy_from_iter_full(to, len, &msg->msg_iter) ? 0 : -EFAULT;
}


static inline int udplite_checksum_init(struct sk_buff *skb, struct udphdr *uh)
{
	u16 cscov;

        
	if (uh->check == 0) {
		net_dbg_ratelimited("UDPLite: zeroed checksum field\n");
		return 1;
	}

	cscov = ntohs(uh->len);

	if (cscov == 0)		 
		;
	else if (cscov < 8  || cscov > skb->len) {
		
		net_dbg_ratelimited("UDPLite: bad csum coverage %d/%d\n",
				    cscov, skb->len);
		return 1;

	} else if (cscov < skb->len) {
        	UDP_SKB_CB(skb)->partial_cov = 1;
		UDP_SKB_CB(skb)->cscov = cscov;
		if (skb->ip_summed == CHECKSUM_COMPLETE)
			skb->ip_summed = CHECKSUM_NONE;
		skb->csum_valid = 0;
        }

	return 0;
}


static inline __wsum udplite_csum(struct sk_buff *skb)
{
	const int off = skb_transport_offset(skb);
	const struct sock *sk = skb->sk;
	int len = skb->len - off;

	if (udp_test_bit(UDPLITE_SEND_CC, sk)) {
		u16 pcslen = READ_ONCE(udp_sk(sk)->pcslen);

		if (pcslen < len) {
			if (pcslen > 0)
				len = pcslen;
			udp_hdr(skb)->len = htons(pcslen);
		}
	}
	skb->ip_summed = CHECKSUM_NONE;     

	return skb_checksum(skb, off, len, 0);
}

void udplite4_register(void);
#endif	
