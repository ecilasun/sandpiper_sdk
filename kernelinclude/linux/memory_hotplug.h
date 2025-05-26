/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_MEMORY_HOTPLUG_H
#define __LINUX_MEMORY_HOTPLUG_H

#include <linux/mmzone.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/bug.h>

struct page;
struct zone;
struct pglist_data;
struct mem_section;
struct memory_group;
struct resource;
struct vmem_altmap;
struct dev_pagemap;

#ifdef CONFIG_HAVE_ARCH_NODEDATA_EXTENSION

extern pg_data_t *arch_alloc_nodedata(int nid);
extern void arch_refresh_nodedata(int nid, pg_data_t *pgdat);

#else 

#define arch_alloc_nodedata(nid)	generic_alloc_nodedata(nid)

#ifdef CONFIG_NUMA

#define generic_alloc_nodedata(nid)				\
({								\
	memblock_alloc(sizeof(*pgdat), SMP_CACHE_BYTES);	\
})

extern pg_data_t *node_data[];
static inline void arch_refresh_nodedata(int nid, pg_data_t *pgdat)
{
	node_data[nid] = pgdat;
}

#else 


static inline pg_data_t *generic_alloc_nodedata(int nid)
{
	BUG();
	return NULL;
}
static inline void arch_refresh_nodedata(int nid, pg_data_t *pgdat)
{
}
#endif 
#endif 

#ifdef CONFIG_MEMORY_HOTPLUG
struct page *pfn_to_online_page(unsigned long pfn);


enum {
	
	MMOP_OFFLINE = 0,
	
	MMOP_ONLINE,
	
	MMOP_ONLINE_KERNEL,
	
	MMOP_ONLINE_MOVABLE,
};


typedef int __bitwise mhp_t;


#define MHP_NONE		((__force mhp_t)0)

#define MHP_MERGE_RESOURCE	((__force mhp_t)BIT(0))


#define MHP_MEMMAP_ON_MEMORY   ((__force mhp_t)BIT(1))

#define MHP_NID_IS_MGID		((__force mhp_t)BIT(2))


struct mhp_params {
	struct vmem_altmap *altmap;
	pgprot_t pgprot;
	struct dev_pagemap *pgmap;
};

bool mhp_range_allowed(u64 start, u64 size, bool need_mapping);
struct range mhp_get_pluggable_range(bool need_mapping);


static inline unsigned zone_span_seqbegin(struct zone *zone)
{
	return read_seqbegin(&zone->span_seqlock);
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
	return read_seqretry(&zone->span_seqlock, iv);
}
static inline void zone_span_writelock(struct zone *zone)
{
	write_seqlock(&zone->span_seqlock);
}
static inline void zone_span_writeunlock(struct zone *zone)
{
	write_sequnlock(&zone->span_seqlock);
}
static inline void zone_seqlock_init(struct zone *zone)
{
	seqlock_init(&zone->span_seqlock);
}
extern void adjust_present_page_count(struct page *page,
				      struct memory_group *group,
				      long nr_pages);

extern int mhp_init_memmap_on_memory(unsigned long pfn, unsigned long nr_pages,
				     struct zone *zone);
extern void mhp_deinit_memmap_on_memory(unsigned long pfn, unsigned long nr_pages);
extern int online_pages(unsigned long pfn, unsigned long nr_pages,
			struct zone *zone, struct memory_group *group);
extern void __offline_isolated_pages(unsigned long start_pfn,
				     unsigned long end_pfn);

typedef void (*online_page_callback_t)(struct page *page, unsigned int order);

extern void generic_online_page(struct page *page, unsigned int order);
extern int set_online_page_callback(online_page_callback_t callback);
extern int restore_online_page_callback(online_page_callback_t callback);

extern int try_online_node(int nid);

extern int arch_add_memory(int nid, u64 start, u64 size,
			   struct mhp_params *params);
extern u64 max_mem_size;

extern int mhp_online_type_from_str(const char *str);


extern int mhp_default_online_type;

extern bool movable_node_enabled;
static inline bool movable_node_is_enabled(void)
{
	return movable_node_enabled;
}

extern void arch_remove_memory(u64 start, u64 size, struct vmem_altmap *altmap);
extern void __remove_pages(unsigned long start_pfn, unsigned long nr_pages,
			   struct vmem_altmap *altmap);


extern int __add_pages(int nid, unsigned long start_pfn, unsigned long nr_pages,
		       struct mhp_params *params);

#ifndef CONFIG_ARCH_HAS_ADD_PAGES
static inline int add_pages(int nid, unsigned long start_pfn,
		unsigned long nr_pages, struct mhp_params *params)
{
	return __add_pages(nid, start_pfn, nr_pages, params);
}
#else 
int add_pages(int nid, unsigned long start_pfn, unsigned long nr_pages,
	      struct mhp_params *params);
#endif 

void get_online_mems(void);
void put_online_mems(void);

void mem_hotplug_begin(void);
void mem_hotplug_done(void);


static inline void pgdat_kswapd_lock(pg_data_t *pgdat)
{
	mutex_lock(&pgdat->kswapd_lock);
}

static inline void pgdat_kswapd_unlock(pg_data_t *pgdat)
{
	mutex_unlock(&pgdat->kswapd_lock);
}

static inline void pgdat_kswapd_lock_init(pg_data_t *pgdat)
{
	mutex_init(&pgdat->kswapd_lock);
}

#else 
#define pfn_to_online_page(pfn)			\
({						\
	struct page *___page = NULL;		\
	if (pfn_valid(pfn))			\
		___page = pfn_to_page(pfn);	\
	___page;				\
 })

static inline unsigned zone_span_seqbegin(struct zone *zone)
{
	return 0;
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
	return 0;
}
static inline void zone_span_writelock(struct zone *zone) {}
static inline void zone_span_writeunlock(struct zone *zone) {}
static inline void zone_seqlock_init(struct zone *zone) {}

static inline int try_online_node(int nid)
{
	return 0;
}

static inline void get_online_mems(void) {}
static inline void put_online_mems(void) {}

static inline void mem_hotplug_begin(void) {}
static inline void mem_hotplug_done(void) {}

static inline bool movable_node_is_enabled(void)
{
	return false;
}

static inline void pgdat_kswapd_lock(pg_data_t *pgdat) {}
static inline void pgdat_kswapd_unlock(pg_data_t *pgdat) {}
static inline void pgdat_kswapd_lock_init(pg_data_t *pgdat) {}
#endif 


struct range arch_get_mappable_range(void);

#if defined(CONFIG_MEMORY_HOTPLUG) || defined(CONFIG_DEFERRED_STRUCT_PAGE_INIT)

static inline
void pgdat_resize_lock(struct pglist_data *pgdat, unsigned long *flags)
{
	spin_lock_irqsave(&pgdat->node_size_lock, *flags);
}
static inline
void pgdat_resize_unlock(struct pglist_data *pgdat, unsigned long *flags)
{
	spin_unlock_irqrestore(&pgdat->node_size_lock, *flags);
}
static inline
void pgdat_resize_init(struct pglist_data *pgdat)
{
	spin_lock_init(&pgdat->node_size_lock);
}
#else 

static inline void pgdat_resize_lock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_unlock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_init(struct pglist_data *pgdat) {}
#endif 

#ifdef CONFIG_MEMORY_HOTREMOVE

extern void try_offline_node(int nid);
extern int offline_pages(unsigned long start_pfn, unsigned long nr_pages,
			 struct zone *zone, struct memory_group *group);
extern int remove_memory(u64 start, u64 size);
extern void __remove_memory(u64 start, u64 size);
extern int offline_and_remove_memory(u64 start, u64 size);

#else
static inline void try_offline_node(int nid) {}

static inline int offline_pages(unsigned long start_pfn, unsigned long nr_pages,
				struct zone *zone, struct memory_group *group)
{
	return -EINVAL;
}

static inline int remove_memory(u64 start, u64 size)
{
	return -EBUSY;
}

static inline void __remove_memory(u64 start, u64 size) {}
#endif 

#ifdef CONFIG_MEMORY_HOTPLUG
extern void __ref free_area_init_core_hotplug(struct pglist_data *pgdat);
extern int __add_memory(int nid, u64 start, u64 size, mhp_t mhp_flags);
extern int add_memory(int nid, u64 start, u64 size, mhp_t mhp_flags);
extern int add_memory_resource(int nid, struct resource *resource,
			       mhp_t mhp_flags);
extern int add_memory_driver_managed(int nid, u64 start, u64 size,
				     const char *resource_name,
				     mhp_t mhp_flags);
extern void move_pfn_range_to_zone(struct zone *zone, unsigned long start_pfn,
				   unsigned long nr_pages,
				   struct vmem_altmap *altmap, int migratetype);
extern void remove_pfn_range_from_zone(struct zone *zone,
				       unsigned long start_pfn,
				       unsigned long nr_pages);
extern int sparse_add_section(int nid, unsigned long pfn,
		unsigned long nr_pages, struct vmem_altmap *altmap,
		struct dev_pagemap *pgmap);
extern void sparse_remove_section(unsigned long pfn, unsigned long nr_pages,
				  struct vmem_altmap *altmap);
extern struct page *sparse_decode_mem_map(unsigned long coded_mem_map,
					  unsigned long pnum);
extern struct zone *zone_for_pfn_range(int online_type, int nid,
		struct memory_group *group, unsigned long start_pfn,
		unsigned long nr_pages);
extern int arch_create_linear_mapping(int nid, u64 start, u64 size,
				      struct mhp_params *params);
void arch_remove_linear_mapping(u64 start, u64 size);
#endif 

#endif 
