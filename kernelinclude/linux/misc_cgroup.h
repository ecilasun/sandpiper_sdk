/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _MISC_CGROUP_H_
#define _MISC_CGROUP_H_


enum misc_res_type {
#ifdef CONFIG_KVM_AMD_SEV
	
	MISC_CG_RES_SEV,
	
	MISC_CG_RES_SEV_ES,
#endif
	MISC_CG_RES_TYPES
};

struct misc_cg;

#ifdef CONFIG_CGROUP_MISC

#include <linux/cgroup.h>


struct misc_res {
	u64 max;
	atomic64_t usage;
	atomic64_t events;
};


struct misc_cg {
	struct cgroup_subsys_state css;

	
	struct cgroup_file events_file;

	struct misc_res res[MISC_CG_RES_TYPES];
};

u64 misc_cg_res_total_usage(enum misc_res_type type);
int misc_cg_set_capacity(enum misc_res_type type, u64 capacity);
int misc_cg_try_charge(enum misc_res_type type, struct misc_cg *cg, u64 amount);
void misc_cg_uncharge(enum misc_res_type type, struct misc_cg *cg, u64 amount);


static inline struct misc_cg *css_misc(struct cgroup_subsys_state *css)
{
	return css ? container_of(css, struct misc_cg, css) : NULL;
}


static inline struct misc_cg *get_current_misc_cg(void)
{
	return css_misc(task_get_css(current, misc_cgrp_id));
}


static inline void put_misc_cg(struct misc_cg *cg)
{
	if (cg)
		css_put(&cg->css);
}

#else 

static inline u64 misc_cg_res_total_usage(enum misc_res_type type)
{
	return 0;
}

static inline int misc_cg_set_capacity(enum misc_res_type type, u64 capacity)
{
	return 0;
}

static inline int misc_cg_try_charge(enum misc_res_type type,
				     struct misc_cg *cg,
				     u64 amount)
{
	return 0;
}

static inline void misc_cg_uncharge(enum misc_res_type type,
				    struct misc_cg *cg,
				    u64 amount)
{
}

static inline struct misc_cg *get_current_misc_cg(void)
{
	return NULL;
}

static inline void put_misc_cg(struct misc_cg *cg)
{
}

#endif 
#endif 
