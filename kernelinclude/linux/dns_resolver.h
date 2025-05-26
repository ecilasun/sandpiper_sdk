

#ifndef _LINUX_DNS_RESOLVER_H
#define _LINUX_DNS_RESOLVER_H

#include <uapi/linux/dns_resolver.h>

struct net;
extern int dns_query(struct net *net, const char *type, const char *name, size_t namelen,
		     const char *options, char **_result, time64_t *_expiry,
		     bool invalidate);

#endif 
