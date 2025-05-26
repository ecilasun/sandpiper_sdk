/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_ICMP_H
#define	_LINUX_ICMP_H

#include <linux/skbuff.h>
#include <uapi/linux/icmp.h>
#include <uapi/linux/errqueue.h>

static inline struct icmphdr *icmp_hdr(const struct sk_buff *skb)
{
	return (struct icmphdr *)skb_transport_header(skb);
}

static inline bool icmp_is_err(int type)
{
	switch (type) {
	case ICMP_DEST_UNREACH:
	case ICMP_SOURCE_QUENCH:
	case ICMP_REDIRECT:
	case ICMP_TIME_EXCEEDED:
	case ICMP_PARAMETERPROB:
		return true;
	}

	return false;
}

void ip_icmp_error_rfc4884(const struct sk_buff *skb,
			   struct sock_ee_data_rfc4884 *out,
			   int thlen, int off);

#endif	
