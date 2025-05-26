/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _BLK_CGROUP_H
#define _BLK_CGROUP_H


#include <linux/types.h>

struct bio;
struct cgroup_subsys_state;
struct gendisk;

#define FC_APPID_LEN              129

#ifdef CONFIG_BLK_CGROUP
extern struct cgroup_subsys_state * const blkcg_root_css;

void blkcg_schedule_throttle(struct gendisk *disk, bool use_memdelay);
void blkcg_maybe_throttle_current(void);
bool blk_cgroup_congested(void);
void blkcg_pin_online(struct cgroup_subsys_state *blkcg_css);
void blkcg_unpin_online(struct cgroup_subsys_state *blkcg_css);
struct list_head *blkcg_get_cgwb_list(struct cgroup_subsys_state *css);
struct cgroup_subsys_state *bio_blkcg_css(struct bio *bio);

#else	

#define blkcg_root_css	((struct cgroup_subsys_state *)ERR_PTR(-EINVAL))

static inline void blkcg_maybe_throttle_current(void) { }
static inline bool blk_cgroup_congested(void) { return false; }
static inline struct cgroup_subsys_state *bio_blkcg_css(struct bio *bio)
{
	return NULL;
}
#endif	

int blkcg_set_fc_appid(char *app_id, u64 cgrp_id, size_t app_id_len);
char *blkcg_get_fc_appid(struct bio *bio);

#endif	
