/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_NET_XDP_PRIV_H__
#define __LINUX_NET_XDP_PRIV_H__

#include <linux/rhashtable.h>
#include <net/xdp.h>


struct xdp_mem_allocator {
	struct xdp_mem_info mem;
	union {
		void *allocator;
		struct page_pool *page_pool;
	};
	struct rhash_head node;
	struct rcu_head rcu;
};

#endif 
