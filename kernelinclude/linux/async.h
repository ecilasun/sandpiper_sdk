/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASYNC_H__
#define __ASYNC_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/numa.h>
#include <linux/device.h>

typedef u64 async_cookie_t;
typedef void (*async_func_t) (void *data, async_cookie_t cookie);
struct async_domain {
	struct list_head pending;
	unsigned registered:1;
};


#define ASYNC_DOMAIN(_name) \
	struct async_domain _name = { .pending = LIST_HEAD_INIT(_name.pending),	\
				      .registered = 1 }


#define ASYNC_DOMAIN_EXCLUSIVE(_name) \
	struct async_domain _name = { .pending = LIST_HEAD_INIT(_name.pending), \
				      .registered = 0 }

async_cookie_t async_schedule_node(async_func_t func, void *data,
				   int node);
async_cookie_t async_schedule_node_domain(async_func_t func, void *data,
					  int node,
					  struct async_domain *domain);


static inline async_cookie_t async_schedule(async_func_t func, void *data)
{
	return async_schedule_node(func, data, NUMA_NO_NODE);
}


static inline async_cookie_t
async_schedule_domain(async_func_t func, void *data,
		      struct async_domain *domain)
{
	return async_schedule_node_domain(func, data, NUMA_NO_NODE, domain);
}


static inline async_cookie_t
async_schedule_dev(async_func_t func, struct device *dev)
{
	return async_schedule_node(func, dev, dev_to_node(dev));
}

bool async_schedule_dev_nocall(async_func_t func, struct device *dev);


static inline async_cookie_t
async_schedule_dev_domain(async_func_t func, struct device *dev,
			  struct async_domain *domain)
{
	return async_schedule_node_domain(func, dev, dev_to_node(dev), domain);
}

extern void async_synchronize_full(void);
extern void async_synchronize_full_domain(struct async_domain *domain);
extern void async_synchronize_cookie(async_cookie_t cookie);
extern void async_synchronize_cookie_domain(async_cookie_t cookie,
					    struct async_domain *domain);
extern bool current_is_async(void);
#endif
