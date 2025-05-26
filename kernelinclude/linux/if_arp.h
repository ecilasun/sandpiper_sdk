/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_IF_ARP_H
#define _LINUX_IF_ARP_H

#include <linux/skbuff.h>
#include <uapi/linux/if_arp.h>

static inline struct arphdr *arp_hdr(const struct sk_buff *skb)
{
	return (struct arphdr *)skb_network_header(skb);
}

static inline unsigned int arp_hdr_len(const struct net_device *dev)
{
	switch (dev->type) {
#if IS_ENABLED(CONFIG_FIREWIRE_NET)
	case ARPHRD_IEEE1394:
		
		return sizeof(struct arphdr) + dev->addr_len + sizeof(u32) * 2;
#endif
	default:
		
		return sizeof(struct arphdr) + (dev->addr_len + sizeof(u32)) * 2;
	}
}

static inline bool dev_is_mac_header_xmit(const struct net_device *dev)
{
	switch (dev->type) {
	case ARPHRD_TUNNEL:
	case ARPHRD_TUNNEL6:
	case ARPHRD_SIT:
	case ARPHRD_IPGRE:
	case ARPHRD_IP6GRE:
	case ARPHRD_VOID:
	case ARPHRD_NONE:
	case ARPHRD_RAWIP:
	case ARPHRD_PIMREG:
	
	case ARPHRD_PPP:
		return false;
	default:
		return true;
	}
}

#endif	
