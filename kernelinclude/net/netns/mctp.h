/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __NETNS_MCTP_H__
#define __NETNS_MCTP_H__

#include <linux/mutex.h>
#include <linux/types.h>

struct netns_mctp {
	
	struct list_head routes;

	
	struct mutex bind_lock;
	struct hlist_head binds;

	
	spinlock_t keys_lock;
	struct hlist_head keys;

	
	unsigned int default_net;

	
	struct mutex neigh_lock;
	struct list_head neighbours;
};

#endif 
