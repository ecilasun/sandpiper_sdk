/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _BPF_CGROUP_DEFS_H
#define _BPF_CGROUP_DEFS_H

#ifdef CONFIG_CGROUP_BPF

#include <linux/list.h>
#include <linux/percpu-refcount.h>
#include <linux/workqueue.h>

struct bpf_prog_array;

#ifdef CONFIG_BPF_LSM

#define CGROUP_LSM_NUM 10
#else
#define CGROUP_LSM_NUM 0
#endif

enum cgroup_bpf_attach_type {
	CGROUP_BPF_ATTACH_TYPE_INVALID = -1,
	CGROUP_INET_INGRESS = 0,
	CGROUP_INET_EGRESS,
	CGROUP_INET_SOCK_CREATE,
	CGROUP_SOCK_OPS,
	CGROUP_DEVICE,
	CGROUP_INET4_BIND,
	CGROUP_INET6_BIND,
	CGROUP_INET4_CONNECT,
	CGROUP_INET6_CONNECT,
	CGROUP_INET4_POST_BIND,
	CGROUP_INET6_POST_BIND,
	CGROUP_UDP4_SENDMSG,
	CGROUP_UDP6_SENDMSG,
	CGROUP_SYSCTL,
	CGROUP_UDP4_RECVMSG,
	CGROUP_UDP6_RECVMSG,
	CGROUP_GETSOCKOPT,
	CGROUP_SETSOCKOPT,
	CGROUP_INET4_GETPEERNAME,
	CGROUP_INET6_GETPEERNAME,
	CGROUP_INET4_GETSOCKNAME,
	CGROUP_INET6_GETSOCKNAME,
	CGROUP_INET_SOCK_RELEASE,
	CGROUP_LSM_START,
	CGROUP_LSM_END = CGROUP_LSM_START + CGROUP_LSM_NUM - 1,
	MAX_CGROUP_BPF_ATTACH_TYPE
};

struct cgroup_bpf {
	
	struct bpf_prog_array __rcu *effective[MAX_CGROUP_BPF_ATTACH_TYPE];

	
	struct hlist_head progs[MAX_CGROUP_BPF_ATTACH_TYPE];
	u8 flags[MAX_CGROUP_BPF_ATTACH_TYPE];

	
	struct list_head storages;

	
	struct bpf_prog_array *inactive;

	
	struct percpu_ref refcnt;

	
	struct work_struct release_work;
};

#else 
struct cgroup_bpf {};
#endif 

#endif
