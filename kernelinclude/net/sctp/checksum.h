/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __sctp_checksum_h__
#define __sctp_checksum_h__

#include <linux/types.h>
#include <linux/sctp.h>
#include <linux/crc32c.h>
#include <linux/crc32.h>

static inline __wsum sctp_csum_update(const void *buff, int len, __wsum sum)
{
	
	return (__force __wsum)crc32c((__force __u32)sum, buff, len);
}

static inline __wsum sctp_csum_combine(__wsum csum, __wsum csum2,
				       int offset, int len)
{
	return (__force __wsum)__crc32c_le_combine((__force __u32)csum,
						   (__force __u32)csum2, len);
}

static const struct skb_checksum_ops sctp_csum_ops = {
	.update  = sctp_csum_update,
	.combine = sctp_csum_combine,
};

static inline __le32 sctp_compute_cksum(const struct sk_buff *skb,
					unsigned int offset)
{
	struct sctphdr *sh = (struct sctphdr *)(skb->data + offset);
	__le32 old = sh->checksum;
	__wsum new;

	sh->checksum = 0;
	new = ~__skb_checksum(skb, offset, skb->len - offset, ~(__wsum)0,
			      &sctp_csum_ops);
	sh->checksum = old;

	return cpu_to_le32((__force __u32)new);
}

#endif 
