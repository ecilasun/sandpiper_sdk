/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _NET_GSO_H
#define _NET_GSO_H

#include <linux/skbuff.h>


struct skb_gso_cb {
	union {
		int	mac_offset;
		int	data_offset;
	};
	int	encap_level;
	__wsum	csum;
	__u16	csum_start;
};
#define SKB_GSO_CB_OFFSET	32
#define SKB_GSO_CB(skb) ((struct skb_gso_cb *)((skb)->cb + SKB_GSO_CB_OFFSET))

static inline int skb_tnl_header_len(const struct sk_buff *inner_skb)
{
	return (skb_mac_header(inner_skb) - inner_skb->head) -
		SKB_GSO_CB(inner_skb)->mac_offset;
}

static inline int gso_pskb_expand_head(struct sk_buff *skb, int extra)
{
	int new_headroom, headroom;
	int ret;

	headroom = skb_headroom(skb);
	ret = pskb_expand_head(skb, extra, 0, GFP_ATOMIC);
	if (ret)
		return ret;

	new_headroom = skb_headroom(skb);
	SKB_GSO_CB(skb)->mac_offset += (new_headroom - headroom);
	return 0;
}

static inline void gso_reset_checksum(struct sk_buff *skb, __wsum res)
{
	
	if (skb->remcsum_offload)
		return;

	SKB_GSO_CB(skb)->csum = res;
	SKB_GSO_CB(skb)->csum_start = skb_checksum_start(skb) - skb->head;
}


static inline __sum16 gso_make_checksum(struct sk_buff *skb, __wsum res)
{
	unsigned char *csum_start = skb_transport_header(skb);
	int plen = (skb->head + SKB_GSO_CB(skb)->csum_start) - csum_start;
	__wsum partial = SKB_GSO_CB(skb)->csum;

	SKB_GSO_CB(skb)->csum = res;
	SKB_GSO_CB(skb)->csum_start = csum_start - skb->head;

	return csum_fold(csum_partial(csum_start, plen, partial));
}

struct sk_buff *__skb_gso_segment(struct sk_buff *skb,
				  netdev_features_t features, bool tx_path);

static inline struct sk_buff *skb_gso_segment(struct sk_buff *skb,
					      netdev_features_t features)
{
	return __skb_gso_segment(skb, features, true);
}

struct sk_buff *skb_eth_gso_segment(struct sk_buff *skb,
				    netdev_features_t features, __be16 type);

struct sk_buff *skb_mac_gso_segment(struct sk_buff *skb,
				    netdev_features_t features);

bool skb_gso_validate_network_len(const struct sk_buff *skb, unsigned int mtu);

bool skb_gso_validate_mac_len(const struct sk_buff *skb, unsigned int len);

static inline void skb_gso_error_unwind(struct sk_buff *skb, __be16 protocol,
					int pulled_hlen, u16 mac_offset,
					int mac_len)
{
	skb->protocol = protocol;
	skb->encapsulation = 1;
	skb_push(skb, pulled_hlen);
	skb_reset_transport_header(skb);
	skb->mac_header = mac_offset;
	skb->network_header = skb->mac_header + mac_len;
	skb->mac_len = mac_len;
}

#endif 
