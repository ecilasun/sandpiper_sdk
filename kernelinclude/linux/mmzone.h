/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MMZONE_H
#define _LINUX_MMZONE_H

#ifndef __ASSEMBLY__
#ifndef __GENERATING_BOUNDS_H

#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/list_nulls.h>
#include <linux/wait.h>
#include <linux/bitops.h>
#include <linux/cache.h>
#include <linux/threads.h>
#include <linux/numa.h>
#include <linux/init.h>
#include <linux/seqlock.h>
#include <linux/nodemask.h>
#include <linux/pageblock-flags.h>
#include <linux/page-flags-layout.h>
#include <linux/atomic.h>
#include <linux/mm_types.h>
#include <linux/page-flags.h>
#include <linux/local_lock.h>
#include <asm/page.h>


#ifndef CONFIG_ARCH_FORCE_MAX_ORDER
#define MAX_ORDER 10
#else
#define MAX_ORDER CONFIG_ARCH_FORCE_MAX_ORDER
#endif
#define MAX_ORDER_NR_PAGES (1 << MAX_ORDER)

#define IS_MAX_ORDER_ALIGNED(pfn) IS_ALIGNED(pfn, MAX_ORDER_NR_PAGES)

#define NR_PAGE_ORDERS (MAX_ORDER + 1)


#define PAGE_ALLOC_COSTLY_ORDER 3

enum migratetype {
	MIGRATE_UNMOVABLE,
	MIGRATE_MOVABLE,
	MIGRATE_RECLAIMABLE,
	MIGRATE_PCPTYPES,	
	MIGRATE_HIGHATOMIC = MIGRATE_PCPTYPES,
#ifdef CONFIG_CMA
	
	MIGRATE_CMA,
#endif
#ifdef CONFIG_MEMORY_ISOLATION
	MIGRATE_ISOLATE,	
#endif
	MIGRATE_TYPES
};


extern const char * const migratetype_names[MIGRATE_TYPES];

#ifdef CONFIG_CMA
#  define is_migrate_cma(migratetype) unlikely((migratetype) == MIGRATE_CMA)
#  define is_migrate_cma_page(_page) (get_pageblock_migratetype(_page) == MIGRATE_CMA)
#else
#  define is_migrate_cma(migratetype) false
#  define is_migrate_cma_page(_page) false
#endif

static inline bool is_migrate_movable(int mt)
{
	return is_migrate_cma(mt) || mt == MIGRATE_MOVABLE;
}


static inline bool migratetype_is_mergeable(int mt)
{
	return mt < MIGRATE_PCPTYPES;
}

#define for_each_migratetype_order(order, type) \
	for (order = 0; order < NR_PAGE_ORDERS; order++) \
		for (type = 0; type < MIGRATE_TYPES; type++)

extern int page_group_by_mobility_disabled;

#define MIGRATETYPE_MASK ((1UL << PB_migratetype_bits) - 1)

#define get_pageblock_migratetype(page)					\
	get_pfnblock_flags_mask(page, page_to_pfn(page), MIGRATETYPE_MASK)

#define folio_migratetype(folio)				\
	get_pfnblock_flags_mask(&folio->page, folio_pfn(folio),		\
			MIGRATETYPE_MASK)
struct free_area {
	struct list_head	free_list[MIGRATE_TYPES];
	unsigned long		nr_free;
};

struct pglist_data;

#ifdef CONFIG_NUMA
enum numa_stat_item {
	NUMA_HIT,		
	NUMA_MISS,		
	NUMA_FOREIGN,		
	NUMA_INTERLEAVE_HIT,	
	NUMA_LOCAL,		
	NUMA_OTHER,		
	NR_VM_NUMA_EVENT_ITEMS
};
#else
#define NR_VM_NUMA_EVENT_ITEMS 0
#endif

enum zone_stat_item {
	
	NR_FREE_PAGES,
	NR_ZONE_LRU_BASE, 
	NR_ZONE_INACTIVE_ANON = NR_ZONE_LRU_BASE,
	NR_ZONE_ACTIVE_ANON,
	NR_ZONE_INACTIVE_FILE,
	NR_ZONE_ACTIVE_FILE,
	NR_ZONE_UNEVICTABLE,
	NR_ZONE_WRITE_PENDING,	
	NR_MLOCK,		
	
	NR_BOUNCE,
#if IS_ENABLED(CONFIG_ZSMALLOC)
	NR_ZSPAGES,		
#endif
	NR_FREE_CMA_PAGES,
#ifdef CONFIG_UNACCEPTED_MEMORY
	NR_UNACCEPTED,
#endif
	NR_VM_ZONE_STAT_ITEMS };

enum node_stat_item {
	NR_LRU_BASE,
	NR_INACTIVE_ANON = NR_LRU_BASE, 
	NR_ACTIVE_ANON,		
	NR_INACTIVE_FILE,	
	NR_ACTIVE_FILE,		
	NR_UNEVICTABLE,		
	NR_SLAB_RECLAIMABLE_B,
	NR_SLAB_UNRECLAIMABLE_B,
	NR_ISOLATED_ANON,	
	NR_ISOLATED_FILE,	
	WORKINGSET_NODES,
	WORKINGSET_REFAULT_BASE,
	WORKINGSET_REFAULT_ANON = WORKINGSET_REFAULT_BASE,
	WORKINGSET_REFAULT_FILE,
	WORKINGSET_ACTIVATE_BASE,
	WORKINGSET_ACTIVATE_ANON = WORKINGSET_ACTIVATE_BASE,
	WORKINGSET_ACTIVATE_FILE,
	WORKINGSET_RESTORE_BASE,
	WORKINGSET_RESTORE_ANON = WORKINGSET_RESTORE_BASE,
	WORKINGSET_RESTORE_FILE,
	WORKINGSET_NODERECLAIM,
	NR_ANON_MAPPED,	
	NR_FILE_MAPPED,	
	NR_FILE_PAGES,
	NR_FILE_DIRTY,
	NR_WRITEBACK,
	NR_WRITEBACK_TEMP,	
	NR_SHMEM,		
	NR_SHMEM_THPS,
	NR_SHMEM_PMDMAPPED,
	NR_FILE_THPS,
	NR_FILE_PMDMAPPED,
	NR_ANON_THPS,
	NR_VMSCAN_WRITE,
	NR_VMSCAN_IMMEDIATE,	
	NR_DIRTIED,		
	NR_WRITTEN,		
	NR_THROTTLED_WRITTEN,	
	NR_KERNEL_MISC_RECLAIMABLE,	
	NR_FOLL_PIN_ACQUIRED,	
	NR_FOLL_PIN_RELEASED,	
	NR_KERNEL_STACK_KB,	
#if IS_ENABLED(CONFIG_SHADOW_CALL_STACK)
	NR_KERNEL_SCS_KB,	
#endif
	NR_PAGETABLE,		
	NR_SECONDARY_PAGETABLE, 
#ifdef CONFIG_SWAP
	NR_SWAPCACHE,
#endif
#ifdef CONFIG_NUMA_BALANCING
	PGPROMOTE_SUCCESS,	
	PGPROMOTE_CANDIDATE,	
#endif
	NR_VM_NODE_STAT_ITEMS
};


static __always_inline bool vmstat_item_print_in_thp(enum node_stat_item item)
{
	if (!IS_ENABLED(CONFIG_TRANSPARENT_HUGEPAGE))
		return false;

	return item == NR_ANON_THPS ||
	       item == NR_FILE_THPS ||
	       item == NR_SHMEM_THPS ||
	       item == NR_SHMEM_PMDMAPPED ||
	       item == NR_FILE_PMDMAPPED;
}


static __always_inline bool vmstat_item_in_bytes(int idx)
{
	
	return (idx == NR_SLAB_RECLAIMABLE_B ||
		idx == NR_SLAB_UNRECLAIMABLE_B);
}


#define LRU_BASE 0
#define LRU_ACTIVE 1
#define LRU_FILE 2

enum lru_list {
	LRU_INACTIVE_ANON = LRU_BASE,
	LRU_ACTIVE_ANON = LRU_BASE + LRU_ACTIVE,
	LRU_INACTIVE_FILE = LRU_BASE + LRU_FILE,
	LRU_ACTIVE_FILE = LRU_BASE + LRU_FILE + LRU_ACTIVE,
	LRU_UNEVICTABLE,
	NR_LRU_LISTS
};

enum vmscan_throttle_state {
	VMSCAN_THROTTLE_WRITEBACK,
	VMSCAN_THROTTLE_ISOLATED,
	VMSCAN_THROTTLE_NOPROGRESS,
	VMSCAN_THROTTLE_CONGESTED,
	NR_VMSCAN_THROTTLE,
};

#define for_each_lru(lru) for (lru = 0; lru < NR_LRU_LISTS; lru++)

#define for_each_evictable_lru(lru) for (lru = 0; lru <= LRU_ACTIVE_FILE; lru++)

static inline bool is_file_lru(enum lru_list lru)
{
	return (lru == LRU_INACTIVE_FILE || lru == LRU_ACTIVE_FILE);
}

static inline bool is_active_lru(enum lru_list lru)
{
	return (lru == LRU_ACTIVE_ANON || lru == LRU_ACTIVE_FILE);
}

#define WORKINGSET_ANON 0
#define WORKINGSET_FILE 1
#define ANON_AND_FILE 2

enum lruvec_flags {
	
	LRUVEC_CGROUP_CONGESTED,
	LRUVEC_NODE_CONGESTED,
};

#endif 


#define MIN_NR_GENS		2U
#define MAX_NR_GENS		4U


#define MAX_NR_TIERS		4U

#ifndef __GENERATING_BOUNDS_H

struct lruvec;
struct page_vma_mapped_walk;

#define LRU_GEN_MASK		((BIT(LRU_GEN_WIDTH) - 1) << LRU_GEN_PGOFF)
#define LRU_REFS_MASK		((BIT(LRU_REFS_WIDTH) - 1) << LRU_REFS_PGOFF)

#ifdef CONFIG_LRU_GEN

enum {
	LRU_GEN_ANON,
	LRU_GEN_FILE,
};

enum {
	LRU_GEN_CORE,
	LRU_GEN_MM_WALK,
	LRU_GEN_NONLEAF_YOUNG,
	NR_LRU_GEN_CAPS
};

#define MIN_LRU_BATCH		BITS_PER_LONG
#define MAX_LRU_BATCH		(MIN_LRU_BATCH * 64)


#ifdef CONFIG_LRU_GEN_STATS
#define NR_HIST_GENS		MAX_NR_GENS
#else
#define NR_HIST_GENS		1U
#endif


struct lru_gen_folio {
	
	unsigned long max_seq;
	
	unsigned long min_seq[ANON_AND_FILE];
	
	unsigned long timestamps[MAX_NR_GENS];
	
	struct list_head folios[MAX_NR_GENS][ANON_AND_FILE][MAX_NR_ZONES];
	
	long nr_pages[MAX_NR_GENS][ANON_AND_FILE][MAX_NR_ZONES];
	
	unsigned long avg_refaulted[ANON_AND_FILE][MAX_NR_TIERS];
	
	unsigned long avg_total[ANON_AND_FILE][MAX_NR_TIERS];
	
	unsigned long protected[NR_HIST_GENS][ANON_AND_FILE][MAX_NR_TIERS - 1];
	
	atomic_long_t evicted[NR_HIST_GENS][ANON_AND_FILE][MAX_NR_TIERS];
	atomic_long_t refaulted[NR_HIST_GENS][ANON_AND_FILE][MAX_NR_TIERS];
	
	bool enabled;
#ifdef CONFIG_MEMCG
	
	u8 gen;
	
	u8 seg;
	
	struct hlist_nulls_node list;
#endif
};

enum {
	MM_LEAF_TOTAL,		
	MM_LEAF_OLD,		
	MM_LEAF_YOUNG,		
	MM_NONLEAF_TOTAL,	
	MM_NONLEAF_FOUND,	
	MM_NONLEAF_ADDED,	
	NR_MM_STATS
};


#define NR_BLOOM_FILTERS	2

struct lru_gen_mm_state {
	
	unsigned long seq;
	
	struct list_head *head;
	
	struct list_head *tail;
	
	unsigned long *filters[NR_BLOOM_FILTERS];
	
	unsigned long stats[NR_HIST_GENS][NR_MM_STATS];
};

struct lru_gen_mm_walk {
	
	struct lruvec *lruvec;
	
	unsigned long max_seq;
	
	unsigned long next_addr;
	
	int nr_pages[MAX_NR_GENS][ANON_AND_FILE][MAX_NR_ZONES];
	
	int mm_stats[NR_MM_STATS];
	
	int batched;
	bool can_swap;
	bool force_scan;
};

void lru_gen_init_lruvec(struct lruvec *lruvec);
void lru_gen_look_around(struct page_vma_mapped_walk *pvmw);

#ifdef CONFIG_MEMCG


#define MEMCG_NR_GENS	3
#define MEMCG_NR_BINS	8

struct lru_gen_memcg {
	
	unsigned long seq;
	
	unsigned long nr_memcgs[MEMCG_NR_GENS];
	
	struct hlist_nulls_head	fifo[MEMCG_NR_GENS][MEMCG_NR_BINS];
	
	spinlock_t lock;
};

void lru_gen_init_pgdat(struct pglist_data *pgdat);

void lru_gen_init_memcg(struct mem_cgroup *memcg);
void lru_gen_exit_memcg(struct mem_cgroup *memcg);
void lru_gen_online_memcg(struct mem_cgroup *memcg);
void lru_gen_offline_memcg(struct mem_cgroup *memcg);
void lru_gen_release_memcg(struct mem_cgroup *memcg);
void lru_gen_soft_reclaim(struct mem_cgroup *memcg, int nid);

#else 

#define MEMCG_NR_GENS	1

struct lru_gen_memcg {
};

static inline void lru_gen_init_pgdat(struct pglist_data *pgdat)
{
}

#endif 

#else 

static inline void lru_gen_init_pgdat(struct pglist_data *pgdat)
{
}

static inline void lru_gen_init_lruvec(struct lruvec *lruvec)
{
}

static inline void lru_gen_look_around(struct page_vma_mapped_walk *pvmw)
{
}

#ifdef CONFIG_MEMCG

static inline void lru_gen_init_memcg(struct mem_cgroup *memcg)
{
}

static inline void lru_gen_exit_memcg(struct mem_cgroup *memcg)
{
}

static inline void lru_gen_online_memcg(struct mem_cgroup *memcg)
{
}

static inline void lru_gen_offline_memcg(struct mem_cgroup *memcg)
{
}

static inline void lru_gen_release_memcg(struct mem_cgroup *memcg)
{
}

static inline void lru_gen_soft_reclaim(struct mem_cgroup *memcg, int nid)
{
}

#endif 

#endif 

struct lruvec {
	struct list_head		lists[NR_LRU_LISTS];
	
	spinlock_t			lru_lock;
	
	unsigned long			anon_cost;
	unsigned long			file_cost;
	
	atomic_long_t			nonresident_age;
	
	unsigned long			refaults[ANON_AND_FILE];
	
	unsigned long			flags;
#ifdef CONFIG_LRU_GEN
	
	struct lru_gen_folio		lrugen;
	
	struct lru_gen_mm_state		mm_state;
#endif
#ifdef CONFIG_MEMCG
	struct pglist_data *pgdat;
#endif
};


#define ISOLATE_UNMAPPED	((__force isolate_mode_t)0x2)

#define ISOLATE_ASYNC_MIGRATE	((__force isolate_mode_t)0x4)

#define ISOLATE_UNEVICTABLE	((__force isolate_mode_t)0x8)


typedef unsigned __bitwise isolate_mode_t;

enum zone_watermarks {
	WMARK_MIN,
	WMARK_LOW,
	WMARK_HIGH,
	WMARK_PROMO,
	NR_WMARK
};


#ifdef CONFIG_TRANSPARENT_HUGEPAGE
#define NR_PCP_THP 2
#else
#define NR_PCP_THP 0
#endif
#define NR_LOWORDER_PCP_LISTS (MIGRATE_PCPTYPES * (PAGE_ALLOC_COSTLY_ORDER + 1))
#define NR_PCP_LISTS (NR_LOWORDER_PCP_LISTS + NR_PCP_THP)

#define min_wmark_pages(z) (z->_watermark[WMARK_MIN] + z->watermark_boost)
#define low_wmark_pages(z) (z->_watermark[WMARK_LOW] + z->watermark_boost)
#define high_wmark_pages(z) (z->_watermark[WMARK_HIGH] + z->watermark_boost)
#define wmark_pages(z, i) (z->_watermark[i] + z->watermark_boost)

struct per_cpu_pages {
	spinlock_t lock;	
	int count;		
	int high;		
	int batch;		
	short free_factor;	
#ifdef CONFIG_NUMA
	short expire;		
#endif

	
	struct list_head lists[NR_PCP_LISTS];
} ____cacheline_aligned_in_smp;

struct per_cpu_zonestat {
#ifdef CONFIG_SMP
	s8 vm_stat_diff[NR_VM_ZONE_STAT_ITEMS];
	s8 stat_threshold;
#endif
#ifdef CONFIG_NUMA
	
	unsigned long vm_numa_event[NR_VM_NUMA_EVENT_ITEMS];
#endif
};

struct per_cpu_nodestat {
	s8 stat_threshold;
	s8 vm_node_stat_diff[NR_VM_NODE_STAT_ITEMS];
};

#endif 

enum zone_type {
	
#ifdef CONFIG_ZONE_DMA
	ZONE_DMA,
#endif
#ifdef CONFIG_ZONE_DMA32
	ZONE_DMA32,
#endif
	
	ZONE_NORMAL,
#ifdef CONFIG_HIGHMEM
	
	ZONE_HIGHMEM,
#endif
	
	ZONE_MOVABLE,
#ifdef CONFIG_ZONE_DEVICE
	ZONE_DEVICE,
#endif
	__MAX_NR_ZONES

};

#ifndef __GENERATING_BOUNDS_H

#define ASYNC_AND_SYNC 2

struct zone {
	

	
	unsigned long _watermark[NR_WMARK];
	unsigned long watermark_boost;

	unsigned long nr_reserved_highatomic;

	
	long lowmem_reserve[MAX_NR_ZONES];

#ifdef CONFIG_NUMA
	int node;
#endif
	struct pglist_data	*zone_pgdat;
	struct per_cpu_pages	__percpu *per_cpu_pageset;
	struct per_cpu_zonestat	__percpu *per_cpu_zonestats;
	
	int pageset_high;
	int pageset_batch;

#ifndef CONFIG_SPARSEMEM
	
	unsigned long		*pageblock_flags;
#endif 

	
	unsigned long		zone_start_pfn;

	
	atomic_long_t		managed_pages;
	unsigned long		spanned_pages;
	unsigned long		present_pages;
#if defined(CONFIG_MEMORY_HOTPLUG)
	unsigned long		present_early_pages;
#endif
#ifdef CONFIG_CMA
	unsigned long		cma_pages;
#endif

	const char		*name;

#ifdef CONFIG_MEMORY_ISOLATION
	
	unsigned long		nr_isolate_pageblock;
#endif

#ifdef CONFIG_MEMORY_HOTPLUG
	
	seqlock_t		span_seqlock;
#endif

	int initialized;

	
	CACHELINE_PADDING(_pad1_);

	
	struct free_area	free_area[NR_PAGE_ORDERS];

#ifdef CONFIG_UNACCEPTED_MEMORY
	
	struct list_head	unaccepted_pages;
#endif

	
	unsigned long		flags;

	
	spinlock_t		lock;

	
	CACHELINE_PADDING(_pad2_);

	
	unsigned long percpu_drift_mark;

#if defined CONFIG_COMPACTION || defined CONFIG_CMA
	
	unsigned long		compact_cached_free_pfn;
	
	unsigned long		compact_cached_migrate_pfn[ASYNC_AND_SYNC];
	unsigned long		compact_init_migrate_pfn;
	unsigned long		compact_init_free_pfn;
#endif

#ifdef CONFIG_COMPACTION
	
	unsigned int		compact_considered;
	unsigned int		compact_defer_shift;
	int			compact_order_failed;
#endif

#if defined CONFIG_COMPACTION || defined CONFIG_CMA
	
	bool			compact_blockskip_flush;
#endif

	bool			contiguous;

	CACHELINE_PADDING(_pad3_);
	
	atomic_long_t		vm_stat[NR_VM_ZONE_STAT_ITEMS];
	atomic_long_t		vm_numa_event[NR_VM_NUMA_EVENT_ITEMS];
} ____cacheline_internodealigned_in_smp;

enum pgdat_flags {
	PGDAT_DIRTY,			
	PGDAT_WRITEBACK,		
	PGDAT_RECLAIM_LOCKED,		
};

enum zone_flags {
	ZONE_BOOSTED_WATERMARK,		
	ZONE_RECLAIM_ACTIVE,		
};

static inline unsigned long zone_managed_pages(struct zone *zone)
{
	return (unsigned long)atomic_long_read(&zone->managed_pages);
}

static inline unsigned long zone_cma_pages(struct zone *zone)
{
#ifdef CONFIG_CMA
	return zone->cma_pages;
#else
	return 0;
#endif
}

static inline unsigned long zone_end_pfn(const struct zone *zone)
{
	return zone->zone_start_pfn + zone->spanned_pages;
}

static inline bool zone_spans_pfn(const struct zone *zone, unsigned long pfn)
{
	return zone->zone_start_pfn <= pfn && pfn < zone_end_pfn(zone);
}

static inline bool zone_is_initialized(struct zone *zone)
{
	return zone->initialized;
}

static inline bool zone_is_empty(struct zone *zone)
{
	return zone->spanned_pages == 0;
}

#ifndef BUILD_VDSO32_64



#define SECTIONS_PGOFF		((sizeof(unsigned long)*8) - SECTIONS_WIDTH)
#define NODES_PGOFF		(SECTIONS_PGOFF - NODES_WIDTH)
#define ZONES_PGOFF		(NODES_PGOFF - ZONES_WIDTH)
#define LAST_CPUPID_PGOFF	(ZONES_PGOFF - LAST_CPUPID_WIDTH)
#define KASAN_TAG_PGOFF		(LAST_CPUPID_PGOFF - KASAN_TAG_WIDTH)
#define LRU_GEN_PGOFF		(KASAN_TAG_PGOFF - LRU_GEN_WIDTH)
#define LRU_REFS_PGOFF		(LRU_GEN_PGOFF - LRU_REFS_WIDTH)


#define SECTIONS_PGSHIFT	(SECTIONS_PGOFF * (SECTIONS_WIDTH != 0))
#define NODES_PGSHIFT		(NODES_PGOFF * (NODES_WIDTH != 0))
#define ZONES_PGSHIFT		(ZONES_PGOFF * (ZONES_WIDTH != 0))
#define LAST_CPUPID_PGSHIFT	(LAST_CPUPID_PGOFF * (LAST_CPUPID_WIDTH != 0))
#define KASAN_TAG_PGSHIFT	(KASAN_TAG_PGOFF * (KASAN_TAG_WIDTH != 0))


#ifdef NODE_NOT_IN_PAGE_FLAGS
#define ZONEID_SHIFT		(SECTIONS_SHIFT + ZONES_SHIFT)
#define ZONEID_PGOFF		((SECTIONS_PGOFF < ZONES_PGOFF) ? \
						SECTIONS_PGOFF : ZONES_PGOFF)
#else
#define ZONEID_SHIFT		(NODES_SHIFT + ZONES_SHIFT)
#define ZONEID_PGOFF		((NODES_PGOFF < ZONES_PGOFF) ? \
						NODES_PGOFF : ZONES_PGOFF)
#endif

#define ZONEID_PGSHIFT		(ZONEID_PGOFF * (ZONEID_SHIFT != 0))

#define ZONES_MASK		((1UL << ZONES_WIDTH) - 1)
#define NODES_MASK		((1UL << NODES_WIDTH) - 1)
#define SECTIONS_MASK		((1UL << SECTIONS_WIDTH) - 1)
#define LAST_CPUPID_MASK	((1UL << LAST_CPUPID_SHIFT) - 1)
#define KASAN_TAG_MASK		((1UL << KASAN_TAG_WIDTH) - 1)
#define ZONEID_MASK		((1UL << ZONEID_SHIFT) - 1)

static inline enum zone_type page_zonenum(const struct page *page)
{
	ASSERT_EXCLUSIVE_BITS(page->flags, ZONES_MASK << ZONES_PGSHIFT);
	return (page->flags >> ZONES_PGSHIFT) & ZONES_MASK;
}

static inline enum zone_type folio_zonenum(const struct folio *folio)
{
	return page_zonenum(&folio->page);
}

#ifdef CONFIG_ZONE_DEVICE
static inline bool is_zone_device_page(const struct page *page)
{
	return page_zonenum(page) == ZONE_DEVICE;
}


static inline bool zone_device_pages_have_same_pgmap(const struct page *a,
						     const struct page *b)
{
	if (is_zone_device_page(a) != is_zone_device_page(b))
		return false;
	if (!is_zone_device_page(a))
		return true;
	return a->pgmap == b->pgmap;
}

extern void memmap_init_zone_device(struct zone *, unsigned long,
				    unsigned long, struct dev_pagemap *);
#else
static inline bool is_zone_device_page(const struct page *page)
{
	return false;
}
static inline bool zone_device_pages_have_same_pgmap(const struct page *a,
						     const struct page *b)
{
	return true;
}
#endif

static inline bool folio_is_zone_device(const struct folio *folio)
{
	return is_zone_device_page(&folio->page);
}

static inline bool is_zone_movable_page(const struct page *page)
{
	return page_zonenum(page) == ZONE_MOVABLE;
}

static inline bool folio_is_zone_movable(const struct folio *folio)
{
	return folio_zonenum(folio) == ZONE_MOVABLE;
}
#endif


static inline bool zone_intersects(struct zone *zone,
		unsigned long start_pfn, unsigned long nr_pages)
{
	if (zone_is_empty(zone))
		return false;
	if (start_pfn >= zone_end_pfn(zone) ||
	    start_pfn + nr_pages <= zone->zone_start_pfn)
		return false;

	return true;
}


#define DEF_PRIORITY 12


#define MAX_ZONES_PER_ZONELIST (MAX_NUMNODES * MAX_NR_ZONES)

enum {
	ZONELIST_FALLBACK,	
#ifdef CONFIG_NUMA
	
	ZONELIST_NOFALLBACK,	
#endif
	MAX_ZONELISTS
};


struct zoneref {
	struct zone *zone;	
	int zone_idx;		
};


struct zonelist {
	struct zoneref _zonerefs[MAX_ZONES_PER_ZONELIST + 1];
};


extern struct page *mem_map;

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
struct deferred_split {
	spinlock_t split_queue_lock;
	struct list_head split_queue;
	unsigned long split_queue_len;
};
#endif

#ifdef CONFIG_MEMORY_FAILURE

struct memory_failure_stats {
	
	unsigned long total;
	
	unsigned long ignored;
	unsigned long failed;
	unsigned long delayed;
	unsigned long recovered;
};
#endif


typedef struct pglist_data {
	
	struct zone node_zones[MAX_NR_ZONES];

	
	struct zonelist node_zonelists[MAX_ZONELISTS];

	int nr_zones; 
#ifdef CONFIG_FLATMEM	
	struct page *node_mem_map;
#ifdef CONFIG_PAGE_EXTENSION
	struct page_ext *node_page_ext;
#endif
#endif
#if defined(CONFIG_MEMORY_HOTPLUG) || defined(CONFIG_DEFERRED_STRUCT_PAGE_INIT)
	
	spinlock_t node_size_lock;
#endif
	unsigned long node_start_pfn;
	unsigned long node_present_pages; 
	unsigned long node_spanned_pages; 
	int node_id;
	wait_queue_head_t kswapd_wait;
	wait_queue_head_t pfmemalloc_wait;

	
	wait_queue_head_t reclaim_wait[NR_VMSCAN_THROTTLE];

	atomic_t nr_writeback_throttled;
	unsigned long nr_reclaim_start;	
#ifdef CONFIG_MEMORY_HOTPLUG
	struct mutex kswapd_lock;
#endif
	struct task_struct *kswapd;	
	int kswapd_order;
	enum zone_type kswapd_highest_zoneidx;

	int kswapd_failures;		

#ifdef CONFIG_COMPACTION
	int kcompactd_max_order;
	enum zone_type kcompactd_highest_zoneidx;
	wait_queue_head_t kcompactd_wait;
	struct task_struct *kcompactd;
	bool proactive_compact_trigger;
#endif
	
	unsigned long		totalreserve_pages;

#ifdef CONFIG_NUMA
	
	unsigned long		min_unmapped_pages;
	unsigned long		min_slab_pages;
#endif 

	
	CACHELINE_PADDING(_pad1_);

#ifdef CONFIG_DEFERRED_STRUCT_PAGE_INIT
	
	unsigned long first_deferred_pfn;
#endif 

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	struct deferred_split deferred_split_queue;
#endif

#ifdef CONFIG_NUMA_BALANCING
	
	unsigned int nbp_rl_start;
	
	unsigned long nbp_rl_nr_cand;
	
	unsigned int nbp_threshold;
	
	unsigned int nbp_th_start;
	
	unsigned long nbp_th_nr_cand;
#endif
	

	
	struct lruvec		__lruvec;

	unsigned long		flags;

#ifdef CONFIG_LRU_GEN
	
	struct lru_gen_mm_walk mm_walk;
	
	struct lru_gen_memcg memcg_lru;
#endif

	CACHELINE_PADDING(_pad2_);

	
	struct per_cpu_nodestat __percpu *per_cpu_nodestats;
	atomic_long_t		vm_stat[NR_VM_NODE_STAT_ITEMS];
#ifdef CONFIG_NUMA
	struct memory_tier __rcu *memtier;
#endif
#ifdef CONFIG_MEMORY_FAILURE
	struct memory_failure_stats mf_stats;
#endif
} pg_data_t;

#define node_present_pages(nid)	(NODE_DATA(nid)->node_present_pages)
#define node_spanned_pages(nid)	(NODE_DATA(nid)->node_spanned_pages)

#define node_start_pfn(nid)	(NODE_DATA(nid)->node_start_pfn)
#define node_end_pfn(nid) pgdat_end_pfn(NODE_DATA(nid))

static inline unsigned long pgdat_end_pfn(pg_data_t *pgdat)
{
	return pgdat->node_start_pfn + pgdat->node_spanned_pages;
}

#include <linux/memory_hotplug.h>

void build_all_zonelists(pg_data_t *pgdat);
void wakeup_kswapd(struct zone *zone, gfp_t gfp_mask, int order,
		   enum zone_type highest_zoneidx);
bool __zone_watermark_ok(struct zone *z, unsigned int order, unsigned long mark,
			 int highest_zoneidx, unsigned int alloc_flags,
			 long free_pages);
bool zone_watermark_ok(struct zone *z, unsigned int order,
		unsigned long mark, int highest_zoneidx,
		unsigned int alloc_flags);
bool zone_watermark_ok_safe(struct zone *z, unsigned int order,
		unsigned long mark, int highest_zoneidx);

enum meminit_context {
	MEMINIT_EARLY,
	MEMINIT_HOTPLUG,
};

extern void init_currently_empty_zone(struct zone *zone, unsigned long start_pfn,
				     unsigned long size);

extern void lruvec_init(struct lruvec *lruvec);

static inline struct pglist_data *lruvec_pgdat(struct lruvec *lruvec)
{
#ifdef CONFIG_MEMCG
	return lruvec->pgdat;
#else
	return container_of(lruvec, struct pglist_data, __lruvec);
#endif
}

#ifdef CONFIG_HAVE_MEMORYLESS_NODES
int local_memory_node(int node_id);
#else
static inline int local_memory_node(int node_id) { return node_id; };
#endif


#define zone_idx(zone)		((zone) - (zone)->zone_pgdat->node_zones)

#ifdef CONFIG_ZONE_DEVICE
static inline bool zone_is_zone_device(struct zone *zone)
{
	return zone_idx(zone) == ZONE_DEVICE;
}
#else
static inline bool zone_is_zone_device(struct zone *zone)
{
	return false;
}
#endif


static inline bool managed_zone(struct zone *zone)
{
	return zone_managed_pages(zone);
}


static inline bool populated_zone(struct zone *zone)
{
	return zone->present_pages;
}

#ifdef CONFIG_NUMA
static inline int zone_to_nid(struct zone *zone)
{
	return zone->node;
}

static inline void zone_set_nid(struct zone *zone, int nid)
{
	zone->node = nid;
}
#else
static inline int zone_to_nid(struct zone *zone)
{
	return 0;
}

static inline void zone_set_nid(struct zone *zone, int nid) {}
#endif

extern int movable_zone;

static inline int is_highmem_idx(enum zone_type idx)
{
#ifdef CONFIG_HIGHMEM
	return (idx == ZONE_HIGHMEM ||
		(idx == ZONE_MOVABLE && movable_zone == ZONE_HIGHMEM));
#else
	return 0;
#endif
}


static inline int is_highmem(struct zone *zone)
{
	return is_highmem_idx(zone_idx(zone));
}

#ifdef CONFIG_ZONE_DMA
bool has_managed_dma(void);
#else
static inline bool has_managed_dma(void)
{
	return false;
}
#endif


#ifndef CONFIG_NUMA

extern struct pglist_data contig_page_data;
static inline struct pglist_data *NODE_DATA(int nid)
{
	return &contig_page_data;
}

#else 

#include <asm/mmzone.h>

#endif 

extern struct pglist_data *first_online_pgdat(void);
extern struct pglist_data *next_online_pgdat(struct pglist_data *pgdat);
extern struct zone *next_zone(struct zone *zone);


#define for_each_online_pgdat(pgdat)			\
	for (pgdat = first_online_pgdat();		\
	     pgdat;					\
	     pgdat = next_online_pgdat(pgdat))

#define for_each_zone(zone)			        \
	for (zone = (first_online_pgdat())->node_zones; \
	     zone;					\
	     zone = next_zone(zone))

#define for_each_populated_zone(zone)		        \
	for (zone = (first_online_pgdat())->node_zones; \
	     zone;					\
	     zone = next_zone(zone))			\
		if (!populated_zone(zone))		\
			; 		\
		else

static inline struct zone *zonelist_zone(struct zoneref *zoneref)
{
	return zoneref->zone;
}

static inline int zonelist_zone_idx(struct zoneref *zoneref)
{
	return zoneref->zone_idx;
}

static inline int zonelist_node_idx(struct zoneref *zoneref)
{
	return zone_to_nid(zoneref->zone);
}

struct zoneref *__next_zones_zonelist(struct zoneref *z,
					enum zone_type highest_zoneidx,
					nodemask_t *nodes);


static __always_inline struct zoneref *next_zones_zonelist(struct zoneref *z,
					enum zone_type highest_zoneidx,
					nodemask_t *nodes)
{
	if (likely(!nodes && zonelist_zone_idx(z) <= highest_zoneidx))
		return z;
	return __next_zones_zonelist(z, highest_zoneidx, nodes);
}


static inline struct zoneref *first_zones_zonelist(struct zonelist *zonelist,
					enum zone_type highest_zoneidx,
					nodemask_t *nodes)
{
	return next_zones_zonelist(zonelist->_zonerefs,
							highest_zoneidx, nodes);
}


#define for_each_zone_zonelist_nodemask(zone, z, zlist, highidx, nodemask) \
	for (z = first_zones_zonelist(zlist, highidx, nodemask), zone = zonelist_zone(z);	\
		zone;							\
		z = next_zones_zonelist(++z, highidx, nodemask),	\
			zone = zonelist_zone(z))

#define for_next_zone_zonelist_nodemask(zone, z, highidx, nodemask) \
	for (zone = z->zone;	\
		zone;							\
		z = next_zones_zonelist(++z, highidx, nodemask),	\
			zone = zonelist_zone(z))



#define for_each_zone_zonelist(zone, z, zlist, highidx) \
	for_each_zone_zonelist_nodemask(zone, z, zlist, highidx, NULL)


static inline bool movable_only_nodes(nodemask_t *nodes)
{
	struct zonelist *zonelist;
	struct zoneref *z;
	int nid;

	if (nodes_empty(*nodes))
		return false;

	
	nid = first_node(*nodes);
	zonelist = &NODE_DATA(nid)->node_zonelists[ZONELIST_FALLBACK];
	z = first_zones_zonelist(zonelist, ZONE_NORMAL,	nodes);
	return (!z->zone) ? true : false;
}


#ifdef CONFIG_SPARSEMEM
#include <asm/sparsemem.h>
#endif

#ifdef CONFIG_FLATMEM
#define pfn_to_nid(pfn)		(0)
#endif

#ifdef CONFIG_SPARSEMEM


#define PA_SECTION_SHIFT	(SECTION_SIZE_BITS)
#define PFN_SECTION_SHIFT	(SECTION_SIZE_BITS - PAGE_SHIFT)

#define NR_MEM_SECTIONS		(1UL << SECTIONS_SHIFT)

#define PAGES_PER_SECTION       (1UL << PFN_SECTION_SHIFT)
#define PAGE_SECTION_MASK	(~(PAGES_PER_SECTION-1))

#define SECTION_BLOCKFLAGS_BITS \
	((1UL << (PFN_SECTION_SHIFT - pageblock_order)) * NR_PAGEBLOCK_BITS)

#if (MAX_ORDER + PAGE_SHIFT) > SECTION_SIZE_BITS
#error Allocator MAX_ORDER exceeds SECTION_SIZE
#endif

static inline unsigned long pfn_to_section_nr(unsigned long pfn)
{
	return pfn >> PFN_SECTION_SHIFT;
}
static inline unsigned long section_nr_to_pfn(unsigned long sec)
{
	return sec << PFN_SECTION_SHIFT;
}

#define SECTION_ALIGN_UP(pfn)	(((pfn) + PAGES_PER_SECTION - 1) & PAGE_SECTION_MASK)
#define SECTION_ALIGN_DOWN(pfn)	((pfn) & PAGE_SECTION_MASK)

#define SUBSECTION_SHIFT 21
#define SUBSECTION_SIZE (1UL << SUBSECTION_SHIFT)

#define PFN_SUBSECTION_SHIFT (SUBSECTION_SHIFT - PAGE_SHIFT)
#define PAGES_PER_SUBSECTION (1UL << PFN_SUBSECTION_SHIFT)
#define PAGE_SUBSECTION_MASK (~(PAGES_PER_SUBSECTION-1))

#if SUBSECTION_SHIFT > SECTION_SIZE_BITS
#error Subsection size exceeds section size
#else
#define SUBSECTIONS_PER_SECTION (1UL << (SECTION_SIZE_BITS - SUBSECTION_SHIFT))
#endif

#define SUBSECTION_ALIGN_UP(pfn) ALIGN((pfn), PAGES_PER_SUBSECTION)
#define SUBSECTION_ALIGN_DOWN(pfn) ((pfn) & PAGE_SUBSECTION_MASK)

struct mem_section_usage {
	struct rcu_head rcu;
#ifdef CONFIG_SPARSEMEM_VMEMMAP
	DECLARE_BITMAP(subsection_map, SUBSECTIONS_PER_SECTION);
#endif
	
	unsigned long pageblock_flags[0];
};

void subsection_map_init(unsigned long pfn, unsigned long nr_pages);

struct page;
struct page_ext;
struct mem_section {
	
	unsigned long section_mem_map;

	struct mem_section_usage *usage;
#ifdef CONFIG_PAGE_EXTENSION
	
	struct page_ext *page_ext;
	unsigned long pad;
#endif
	
};

#ifdef CONFIG_SPARSEMEM_EXTREME
#define SECTIONS_PER_ROOT       (PAGE_SIZE / sizeof (struct mem_section))
#else
#define SECTIONS_PER_ROOT	1
#endif

#define SECTION_NR_TO_ROOT(sec)	((sec) / SECTIONS_PER_ROOT)
#define NR_SECTION_ROOTS	DIV_ROUND_UP(NR_MEM_SECTIONS, SECTIONS_PER_ROOT)
#define SECTION_ROOT_MASK	(SECTIONS_PER_ROOT - 1)

#ifdef CONFIG_SPARSEMEM_EXTREME
extern struct mem_section **mem_section;
#else
extern struct mem_section mem_section[NR_SECTION_ROOTS][SECTIONS_PER_ROOT];
#endif

static inline unsigned long *section_to_usemap(struct mem_section *ms)
{
	return ms->usage->pageblock_flags;
}

static inline struct mem_section *__nr_to_section(unsigned long nr)
{
	unsigned long root = SECTION_NR_TO_ROOT(nr);

	if (unlikely(root >= NR_SECTION_ROOTS))
		return NULL;

#ifdef CONFIG_SPARSEMEM_EXTREME
	if (!mem_section || !mem_section[root])
		return NULL;
#endif
	return &mem_section[root][nr & SECTION_ROOT_MASK];
}
extern size_t mem_section_usage_size(void);


enum {
	SECTION_MARKED_PRESENT_BIT,
	SECTION_HAS_MEM_MAP_BIT,
	SECTION_IS_ONLINE_BIT,
	SECTION_IS_EARLY_BIT,
#ifdef CONFIG_ZONE_DEVICE
	SECTION_TAINT_ZONE_DEVICE_BIT,
#endif
	SECTION_MAP_LAST_BIT,
};

#define SECTION_MARKED_PRESENT		BIT(SECTION_MARKED_PRESENT_BIT)
#define SECTION_HAS_MEM_MAP		BIT(SECTION_HAS_MEM_MAP_BIT)
#define SECTION_IS_ONLINE		BIT(SECTION_IS_ONLINE_BIT)
#define SECTION_IS_EARLY		BIT(SECTION_IS_EARLY_BIT)
#ifdef CONFIG_ZONE_DEVICE
#define SECTION_TAINT_ZONE_DEVICE	BIT(SECTION_TAINT_ZONE_DEVICE_BIT)
#endif
#define SECTION_MAP_MASK		(~(BIT(SECTION_MAP_LAST_BIT) - 1))
#define SECTION_NID_SHIFT		SECTION_MAP_LAST_BIT

static inline struct page *__section_mem_map_addr(struct mem_section *section)
{
	unsigned long map = section->section_mem_map;
	map &= SECTION_MAP_MASK;
	return (struct page *)map;
}

static inline int present_section(struct mem_section *section)
{
	return (section && (section->section_mem_map & SECTION_MARKED_PRESENT));
}

static inline int present_section_nr(unsigned long nr)
{
	return present_section(__nr_to_section(nr));
}

static inline int valid_section(struct mem_section *section)
{
	return (section && (section->section_mem_map & SECTION_HAS_MEM_MAP));
}

static inline int early_section(struct mem_section *section)
{
	return (section && (section->section_mem_map & SECTION_IS_EARLY));
}

static inline int valid_section_nr(unsigned long nr)
{
	return valid_section(__nr_to_section(nr));
}

static inline int online_section(struct mem_section *section)
{
	return (section && (section->section_mem_map & SECTION_IS_ONLINE));
}

#ifdef CONFIG_ZONE_DEVICE
static inline int online_device_section(struct mem_section *section)
{
	unsigned long flags = SECTION_IS_ONLINE | SECTION_TAINT_ZONE_DEVICE;

	return section && ((section->section_mem_map & flags) == flags);
}
#else
static inline int online_device_section(struct mem_section *section)
{
	return 0;
}
#endif

static inline int online_section_nr(unsigned long nr)
{
	return online_section(__nr_to_section(nr));
}

#ifdef CONFIG_MEMORY_HOTPLUG
void online_mem_sections(unsigned long start_pfn, unsigned long end_pfn);
void offline_mem_sections(unsigned long start_pfn, unsigned long end_pfn);
#endif

static inline struct mem_section *__pfn_to_section(unsigned long pfn)
{
	return __nr_to_section(pfn_to_section_nr(pfn));
}

extern unsigned long __highest_present_section_nr;

static inline int subsection_map_index(unsigned long pfn)
{
	return (pfn & ~(PAGE_SECTION_MASK)) / PAGES_PER_SUBSECTION;
}

#ifdef CONFIG_SPARSEMEM_VMEMMAP
static inline int pfn_section_valid(struct mem_section *ms, unsigned long pfn)
{
	int idx = subsection_map_index(pfn);

	return test_bit(idx, READ_ONCE(ms->usage)->subsection_map);
}
#else
static inline int pfn_section_valid(struct mem_section *ms, unsigned long pfn)
{
	return 1;
}
#endif

#ifndef CONFIG_HAVE_ARCH_PFN_VALID

static inline int pfn_valid(unsigned long pfn)
{
	struct mem_section *ms;
	int ret;

	
	if (PHYS_PFN(PFN_PHYS(pfn)) != pfn)
		return 0;

	if (pfn_to_section_nr(pfn) >= NR_MEM_SECTIONS)
		return 0;
	ms = __pfn_to_section(pfn);
	rcu_read_lock_sched();
	if (!valid_section(ms)) {
		rcu_read_unlock_sched();
		return 0;
	}
	
	ret = early_section(ms) || pfn_section_valid(ms, pfn);
	rcu_read_unlock_sched();

	return ret;
}
#endif

static inline int pfn_in_present_section(unsigned long pfn)
{
	if (pfn_to_section_nr(pfn) >= NR_MEM_SECTIONS)
		return 0;
	return present_section(__pfn_to_section(pfn));
}

static inline unsigned long next_present_section_nr(unsigned long section_nr)
{
	while (++section_nr <= __highest_present_section_nr) {
		if (present_section_nr(section_nr))
			return section_nr;
	}

	return -1;
}


#ifdef CONFIG_NUMA
#define pfn_to_nid(pfn)							\
({									\
	unsigned long __pfn_to_nid_pfn = (pfn);				\
	page_to_nid(pfn_to_page(__pfn_to_nid_pfn));			\
})
#else
#define pfn_to_nid(pfn)		(0)
#endif

void sparse_init(void);
#else
#define sparse_init()	do {} while (0)
#define sparse_index_init(_sec, _nid)  do {} while (0)
#define pfn_in_present_section pfn_valid
#define subsection_map_init(_pfn, _nr_pages) do {} while (0)
#endif 

#endif 
#endif 
#endif 
