

#ifndef _TIPC_HDR_H
#define _TIPC_HDR_H

#include <linux/random.h>

#define KEEPALIVE_MSG_MASK 0x0e080000  

struct tipc_basic_hdr {
	__be32 w[4];
};

static inline __be32 tipc_hdr_rps_key(struct tipc_basic_hdr *hdr)
{
	u32 w0 = ntohl(hdr->w[0]);
	bool keepalive_msg = (w0 & KEEPALIVE_MSG_MASK) == KEEPALIVE_MSG_MASK;
	__be32 key;

	
	if (likely(!keepalive_msg))
		return hdr->w[3];

	
	get_random_bytes(&key, sizeof(key));
	return key;
}

#endif
