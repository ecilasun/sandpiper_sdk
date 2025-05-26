/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _INET_DSCP_H
#define _INET_DSCP_H

#include <linux/types.h>


typedef u8 __bitwise dscp_t;

#define INET_DSCP_MASK 0xfc

static inline dscp_t inet_dsfield_to_dscp(__u8 dsfield)
{
	return (__force dscp_t)(dsfield & INET_DSCP_MASK);
}

static inline __u8 inet_dscp_to_dsfield(dscp_t dscp)
{
	return (__force __u8)dscp;
}

static inline bool inet_validate_dscp(__u8 val)
{
	return !(val & ~INET_DSCP_MASK);
}

#endif 
