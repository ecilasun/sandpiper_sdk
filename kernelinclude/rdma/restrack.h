/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */


#ifndef _RDMA_RESTRACK_H_
#define _RDMA_RESTRACK_H_

#include <linux/typecheck.h>
#include <linux/sched.h>
#include <linux/kref.h>
#include <linux/completion.h>
#include <linux/sched/task.h>
#include <uapi/rdma/rdma_netlink.h>
#include <linux/xarray.h>

struct ib_device;
struct sk_buff;


enum rdma_restrack_type {
	
	RDMA_RESTRACK_PD,
	
	RDMA_RESTRACK_CQ,
	
	RDMA_RESTRACK_QP,
	
	RDMA_RESTRACK_CM_ID,
	
	RDMA_RESTRACK_MR,
	
	RDMA_RESTRACK_CTX,
	
	RDMA_RESTRACK_COUNTER,
	
	RDMA_RESTRACK_SRQ,
	
	RDMA_RESTRACK_MAX
};


struct rdma_restrack_entry {
	
	bool			valid;
	
	u8			no_track : 1;
	
	struct kref		kref;
	
	struct completion	comp;
	
	struct task_struct	*task;
	
	const char		*kern_name;
	
	enum rdma_restrack_type	type;
	
	bool			user;
	
	u32 id;
};

int rdma_restrack_count(struct ib_device *dev,
			enum rdma_restrack_type type);

static inline bool rdma_is_kernel_res(const struct rdma_restrack_entry *res)
{
	return !res->user;
}


int __must_check rdma_restrack_get(struct rdma_restrack_entry *res);


int rdma_restrack_put(struct rdma_restrack_entry *res);


int rdma_nl_put_driver_u32(struct sk_buff *msg, const char *name, u32 value);
int rdma_nl_put_driver_u32_hex(struct sk_buff *msg, const char *name,
			       u32 value);
int rdma_nl_put_driver_u64(struct sk_buff *msg, const char *name, u64 value);
int rdma_nl_put_driver_u64_hex(struct sk_buff *msg, const char *name,
			       u64 value);
int rdma_nl_put_driver_string(struct sk_buff *msg, const char *name,
			      const char *str);
int rdma_nl_stat_hwcounter_entry(struct sk_buff *msg, const char *name,
				 u64 value);

struct rdma_restrack_entry *rdma_restrack_get_byid(struct ib_device *dev,
						   enum rdma_restrack_type type,
						   u32 id);


static inline void rdma_restrack_no_track(struct rdma_restrack_entry *res)
{
	res->no_track = true;
}
static inline bool rdma_restrack_is_tracked(struct rdma_restrack_entry *res)
{
	return !res->no_track;
}
#endif 
