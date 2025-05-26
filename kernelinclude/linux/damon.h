/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _DAMON_H_
#define _DAMON_H_

#include <linux/memcontrol.h>
#include <linux/mutex.h>
#include <linux/time64.h>
#include <linux/types.h>
#include <linux/random.h>


#define DAMON_MIN_REGION	PAGE_SIZE

#define DAMOS_MAX_SCORE		(99)


static inline unsigned long damon_rand(unsigned long l, unsigned long r)
{
	return l + get_random_u32_below(r - l);
}


struct damon_addr_range {
	unsigned long start;
	unsigned long end;
};


struct damon_region {
	struct damon_addr_range ar;
	unsigned long sampling_addr;
	unsigned int nr_accesses;
	struct list_head list;

	unsigned int age;

	unsigned int last_nr_accesses;
};


struct damon_target {
	struct pid *pid;
	unsigned int nr_regions;
	struct list_head regions_list;
	struct list_head list;
};


enum damos_action {
	DAMOS_WILLNEED,
	DAMOS_COLD,
	DAMOS_PAGEOUT,
	DAMOS_HUGEPAGE,
	DAMOS_NOHUGEPAGE,
	DAMOS_LRU_PRIO,
	DAMOS_LRU_DEPRIO,
	DAMOS_STAT,		
	NR_DAMOS_ACTIONS,
};


struct damos_quota {
	unsigned long ms;
	unsigned long sz;
	unsigned long reset_interval;

	unsigned int weight_sz;
	unsigned int weight_nr_accesses;
	unsigned int weight_age;


	
	unsigned long total_charged_sz;
	unsigned long total_charged_ns;

	unsigned long esz;	

	
	unsigned long charged_sz;
	unsigned long charged_from;
	struct damon_target *charge_target_from;
	unsigned long charge_addr_from;

	
	unsigned long histogram[DAMOS_MAX_SCORE + 1];
	unsigned int min_score;
};


enum damos_wmark_metric {
	DAMOS_WMARK_NONE,
	DAMOS_WMARK_FREE_MEM_RATE,
	NR_DAMOS_WMARK_METRICS,
};


struct damos_watermarks {
	enum damos_wmark_metric metric;
	unsigned long interval;
	unsigned long high;
	unsigned long mid;
	unsigned long low;


	bool activated;
};


struct damos_stat {
	unsigned long nr_tried;
	unsigned long sz_tried;
	unsigned long nr_applied;
	unsigned long sz_applied;
	unsigned long qt_exceeds;
};


enum damos_filter_type {
	DAMOS_FILTER_TYPE_ANON,
	DAMOS_FILTER_TYPE_MEMCG,
	DAMOS_FILTER_TYPE_ADDR,
	DAMOS_FILTER_TYPE_TARGET,
	NR_DAMOS_FILTER_TYPES,
};


struct damos_filter {
	enum damos_filter_type type;
	bool matching;
	union {
		unsigned short memcg_id;
		struct damon_addr_range addr_range;
		int target_idx;
	};
	struct list_head list;
};


struct damos_access_pattern {
	unsigned long min_sz_region;
	unsigned long max_sz_region;
	unsigned int min_nr_accesses;
	unsigned int max_nr_accesses;
	unsigned int min_age_region;
	unsigned int max_age_region;
};


struct damos {
	struct damos_access_pattern pattern;
	enum damos_action action;
	struct damos_quota quota;
	struct damos_watermarks wmarks;
	struct list_head filters;
	struct damos_stat stat;
	struct list_head list;
};


enum damon_ops_id {
	DAMON_OPS_VADDR,
	DAMON_OPS_FVADDR,
	DAMON_OPS_PADDR,
	NR_DAMON_OPS,
};

struct damon_ctx;


struct damon_operations {
	enum damon_ops_id id;
	void (*init)(struct damon_ctx *context);
	void (*update)(struct damon_ctx *context);
	void (*prepare_access_checks)(struct damon_ctx *context);
	unsigned int (*check_accesses)(struct damon_ctx *context);
	void (*reset_aggregated)(struct damon_ctx *context);
	int (*get_scheme_score)(struct damon_ctx *context,
			struct damon_target *t, struct damon_region *r,
			struct damos *scheme);
	unsigned long (*apply_scheme)(struct damon_ctx *context,
			struct damon_target *t, struct damon_region *r,
			struct damos *scheme);
	bool (*target_valid)(struct damon_target *t);
	void (*cleanup)(struct damon_ctx *context);
};


struct damon_callback {
	void *private;

	int (*before_start)(struct damon_ctx *context);
	int (*after_wmarks_check)(struct damon_ctx *context);
	int (*after_sampling)(struct damon_ctx *context);
	int (*after_aggregation)(struct damon_ctx *context);
	int (*before_damos_apply)(struct damon_ctx *context,
			struct damon_target *target,
			struct damon_region *region,
			struct damos *scheme);
	void (*before_terminate)(struct damon_ctx *context);
};


struct damon_attrs {
	unsigned long sample_interval;
	unsigned long aggr_interval;
	unsigned long ops_update_interval;
	unsigned long min_nr_regions;
	unsigned long max_nr_regions;
};


struct damon_ctx {
	struct damon_attrs attrs;


	
	unsigned long passed_sample_intervals;
	
	unsigned long next_aggregation_sis;
	
	unsigned long next_ops_update_sis;
	
	struct completion kdamond_started;


	struct task_struct *kdamond;
	struct mutex kdamond_lock;

	struct damon_operations ops;
	struct damon_callback callback;

	struct list_head adaptive_targets;
	struct list_head schemes;
};

static inline struct damon_region *damon_next_region(struct damon_region *r)
{
	return container_of(r->list.next, struct damon_region, list);
}

static inline struct damon_region *damon_prev_region(struct damon_region *r)
{
	return container_of(r->list.prev, struct damon_region, list);
}

static inline struct damon_region *damon_last_region(struct damon_target *t)
{
	return list_last_entry(&t->regions_list, struct damon_region, list);
}

static inline struct damon_region *damon_first_region(struct damon_target *t)
{
	return list_first_entry(&t->regions_list, struct damon_region, list);
}

static inline unsigned long damon_sz_region(struct damon_region *r)
{
	return r->ar.end - r->ar.start;
}


#define damon_for_each_region(r, t) \
	list_for_each_entry(r, &t->regions_list, list)

#define damon_for_each_region_from(r, t) \
	list_for_each_entry_from(r, &t->regions_list, list)

#define damon_for_each_region_safe(r, next, t) \
	list_for_each_entry_safe(r, next, &t->regions_list, list)

#define damon_for_each_target(t, ctx) \
	list_for_each_entry(t, &(ctx)->adaptive_targets, list)

#define damon_for_each_target_safe(t, next, ctx)	\
	list_for_each_entry_safe(t, next, &(ctx)->adaptive_targets, list)

#define damon_for_each_scheme(s, ctx) \
	list_for_each_entry(s, &(ctx)->schemes, list)

#define damon_for_each_scheme_safe(s, next, ctx) \
	list_for_each_entry_safe(s, next, &(ctx)->schemes, list)

#define damos_for_each_filter(f, scheme) \
	list_for_each_entry(f, &(scheme)->filters, list)

#define damos_for_each_filter_safe(f, next, scheme) \
	list_for_each_entry_safe(f, next, &(scheme)->filters, list)

#ifdef CONFIG_DAMON

struct damon_region *damon_new_region(unsigned long start, unsigned long end);


static inline void damon_insert_region(struct damon_region *r,
		struct damon_region *prev, struct damon_region *next,
		struct damon_target *t)
{
	__list_add(&r->list, &prev->list, &next->list);
	t->nr_regions++;
}

void damon_add_region(struct damon_region *r, struct damon_target *t);
void damon_destroy_region(struct damon_region *r, struct damon_target *t);
int damon_set_regions(struct damon_target *t, struct damon_addr_range *ranges,
		unsigned int nr_ranges);

struct damos_filter *damos_new_filter(enum damos_filter_type type,
		bool matching);
void damos_add_filter(struct damos *s, struct damos_filter *f);
void damos_destroy_filter(struct damos_filter *f);

struct damos *damon_new_scheme(struct damos_access_pattern *pattern,
			enum damos_action action, struct damos_quota *quota,
			struct damos_watermarks *wmarks);
void damon_add_scheme(struct damon_ctx *ctx, struct damos *s);
void damon_destroy_scheme(struct damos *s);

struct damon_target *damon_new_target(void);
void damon_add_target(struct damon_ctx *ctx, struct damon_target *t);
bool damon_targets_empty(struct damon_ctx *ctx);
void damon_free_target(struct damon_target *t);
void damon_destroy_target(struct damon_target *t);
unsigned int damon_nr_regions(struct damon_target *t);

struct damon_ctx *damon_new_ctx(void);
void damon_destroy_ctx(struct damon_ctx *ctx);
int damon_set_attrs(struct damon_ctx *ctx, struct damon_attrs *attrs);
void damon_set_schemes(struct damon_ctx *ctx,
			struct damos **schemes, ssize_t nr_schemes);
int damon_nr_running_ctxs(void);
bool damon_is_registered_ops(enum damon_ops_id id);
int damon_register_ops(struct damon_operations *ops);
int damon_select_ops(struct damon_ctx *ctx, enum damon_ops_id id);

static inline bool damon_target_has_pid(const struct damon_ctx *ctx)
{
	return ctx->ops.id == DAMON_OPS_VADDR || ctx->ops.id == DAMON_OPS_FVADDR;
}

static inline unsigned int damon_max_nr_accesses(const struct damon_attrs *attrs)
{
	
	return min(attrs->aggr_interval / attrs->sample_interval,
			(unsigned long)UINT_MAX);
}


int damon_start(struct damon_ctx **ctxs, int nr_ctxs, bool exclusive);
int damon_stop(struct damon_ctx **ctxs, int nr_ctxs);

int damon_set_region_biggest_system_ram_default(struct damon_target *t,
				unsigned long *start, unsigned long *end);

#endif	

#endif	
