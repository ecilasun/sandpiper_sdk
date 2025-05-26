/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_COMPACTION_H
#define _LINUX_COMPACTION_H


enum compact_priority {
	COMPACT_PRIO_SYNC_FULL,
	MIN_COMPACT_PRIORITY = COMPACT_PRIO_SYNC_FULL,
	COMPACT_PRIO_SYNC_LIGHT,
	MIN_COMPACT_COSTLY_PRIORITY = COMPACT_PRIO_SYNC_LIGHT,
	DEF_COMPACT_PRIORITY = COMPACT_PRIO_SYNC_LIGHT,
	COMPACT_PRIO_ASYNC,
	INIT_COMPACT_PRIORITY = COMPACT_PRIO_ASYNC
};



enum compact_result {
	
	COMPACT_NOT_SUITABLE_ZONE,
	
	COMPACT_SKIPPED,
	
	COMPACT_DEFERRED,

	
	COMPACT_NO_SUITABLE_PAGE,
	
	COMPACT_CONTINUE,

	
	COMPACT_COMPLETE,
	
	COMPACT_PARTIAL_SKIPPED,

	
	COMPACT_CONTENDED,

	
	COMPACT_SUCCESS,
};

struct alloc_context; 


static inline unsigned long compact_gap(unsigned int order)
{
	
	return 2UL << order;
}

#ifdef CONFIG_COMPACTION

extern unsigned int extfrag_for_order(struct zone *zone, unsigned int order);
extern int fragmentation_index(struct zone *zone, unsigned int order);
extern enum compact_result try_to_compact_pages(gfp_t gfp_mask,
		unsigned int order, unsigned int alloc_flags,
		const struct alloc_context *ac, enum compact_priority prio,
		struct page **page);
extern void reset_isolation_suitable(pg_data_t *pgdat);
extern bool compaction_suitable(struct zone *zone, int order,
					       int highest_zoneidx);

extern void compaction_defer_reset(struct zone *zone, int order,
				bool alloc_success);

bool compaction_zonelist_suitable(struct alloc_context *ac, int order,
					int alloc_flags);

extern void __meminit kcompactd_run(int nid);
extern void __meminit kcompactd_stop(int nid);
extern void wakeup_kcompactd(pg_data_t *pgdat, int order, int highest_zoneidx);

#else
static inline void reset_isolation_suitable(pg_data_t *pgdat)
{
}

static inline bool compaction_suitable(struct zone *zone, int order,
						      int highest_zoneidx)
{
	return false;
}

static inline void kcompactd_run(int nid)
{
}
static inline void kcompactd_stop(int nid)
{
}

static inline void wakeup_kcompactd(pg_data_t *pgdat,
				int order, int highest_zoneidx)
{
}

#endif 

struct node;
#if defined(CONFIG_COMPACTION) && defined(CONFIG_SYSFS) && defined(CONFIG_NUMA)
extern int compaction_register_node(struct node *node);
extern void compaction_unregister_node(struct node *node);

#else

static inline int compaction_register_node(struct node *node)
{
	return 0;
}

static inline void compaction_unregister_node(struct node *node)
{
}
#endif 

#endif 
