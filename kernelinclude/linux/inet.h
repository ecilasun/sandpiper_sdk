/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_INET_H
#define _LINUX_INET_H

#include <linux/types.h>
#include <net/net_namespace.h>
#include <linux/socket.h>


#define INET_ADDRSTRLEN		(16)
#define INET6_ADDRSTRLEN	(48)

extern __be32 in_aton(const char *str);
extern int in4_pton(const char *src, int srclen, u8 *dst, int delim, const char **end);
extern int in6_pton(const char *src, int srclen, u8 *dst, int delim, const char **end);

extern int inet_pton_with_scope(struct net *net, unsigned short af,
		const char *src, const char *port, struct sockaddr_storage *addr);
extern bool inet_addr_is_any(struct sockaddr *addr);

#endif	
