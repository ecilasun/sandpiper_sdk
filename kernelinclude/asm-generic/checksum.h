/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_CHECKSUM_H
#define __ASM_GENERIC_CHECKSUM_H


extern __wsum csum_partial(const void *buff, int len, __wsum sum);

#ifndef ip_fast_csum

extern __sum16 ip_fast_csum(const void *iph, unsigned int ihl);
#endif

#ifndef csum_fold

static inline __sum16 csum_fold(__wsum csum)
{
	u32 sum = (__force u32)csum;
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);
	return (__force __sum16)~sum;
}
#endif

#ifndef csum_tcpudp_nofold

extern __wsum
csum_tcpudp_nofold(__be32 saddr, __be32 daddr, __u32 len,
		   __u8 proto, __wsum sum);
#endif

#ifndef csum_tcpudp_magic
static inline __sum16
csum_tcpudp_magic(__be32 saddr, __be32 daddr, __u32 len,
		  __u8 proto, __wsum sum)
{
	return csum_fold(csum_tcpudp_nofold(saddr, daddr, len, proto, sum));
}
#endif


extern __sum16 ip_compute_csum(const void *buff, int len);

#endif 
