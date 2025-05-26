/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _NET_FAILOVER_H
#define _NET_FAILOVER_H

#include <net/failover.h>


struct net_failover_info {
	
	struct net_device __rcu *primary_dev;

	
	struct net_device __rcu *standby_dev;

	
	struct rtnl_link_stats64 primary_stats;

	
	struct rtnl_link_stats64 standby_stats;

	
	struct rtnl_link_stats64 failover_stats;

	
	spinlock_t stats_lock;
};

struct failover *net_failover_create(struct net_device *standby_dev);
void net_failover_destroy(struct failover *failover);

#define FAILOVER_VLAN_FEATURES	(NETIF_F_HW_CSUM | NETIF_F_SG | \
				 NETIF_F_FRAGLIST | NETIF_F_ALL_TSO | \
				 NETIF_F_HIGHDMA | NETIF_F_LRO)

#define FAILOVER_ENC_FEATURES	(NETIF_F_HW_CSUM | NETIF_F_SG | \
				 NETIF_F_RXCSUM | NETIF_F_ALL_TSO)

#endif 
