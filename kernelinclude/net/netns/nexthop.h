/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __NETNS_NEXTHOP_H__
#define __NETNS_NEXTHOP_H__

#include <linux/notifier.h>
#include <linux/rbtree.h>

struct netns_nexthop {
	struct rb_root		rb_root;	
	struct hlist_head	*devhash;	

	unsigned int		seq;		
	u32			last_id_allocated;
	struct blocking_notifier_head notifier_chain;
};
#endif
