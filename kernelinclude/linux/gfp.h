/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GFP_H
#define __LINUX_GFP_H

#include <linux/gfp_types.h>

#include <linux/mmzone.h>
#include <linux/topology.h>

struct vm_area_struct;


#define GFP_MOVABLE_MASK (__GFP_RECLAIMABLE|__GFP_MOVABLE)
#define GFP_MOVABLE_SHIFT 3

static inline int gfp_migratetype(const gfp_t gfp_flags)
{
	VM_WARN_ON((gfp_flags & GFP_MOVABLE_MASK) == GFP_MOVABLE_MASK);
	BUILD_BUG_ON((1UL << GFP_MOVABLE_SHIFT) != ___GFP_MOVABLE);
	BUILD_BUG_ON((___GFP_MOVABLE >> GFP_MOVABLE_SHIFT) != MIGRATE_MOVABLE);
	BUILD_BUG_ON((___GFP_RECLAIMABLE >> GFP_MOVABLE_SHIFT) != MIGRATE_RECLAIMABLE);
	BUILD_BUG_ON(((___GFP_MOVABLE | ___GFP_RECLAIMABLE) >>
		      GFP_MOVABLE_SHIFT) != MIGRATE_HIGHATOMIC);

	if (unlikely(page_group_by_mobility_disabled))
		return MIGRATE_UNMOVABLE;

	
	return (__force unsigned long)(gfp_flags & GFP_MOVABLE_MASK) >> GFP_MOVABLE_SHIFT;
}
#undef GFP_MOVABLE_MASK
#undef GFP_MOVABLE_SHIFT

static inline bool gfpflags_allow_blocking(const gfp_t gfp_flags)
{
	return !!(gfp_flags & __GFP_DIRECT_RECLAIM);
}

#ifdef CONFIG_HIGHMEM
#define OPT_ZONE_HIGHMEM ZONE_HIGHMEM
#else
#define OPT_ZONE_HIGHMEM ZONE_NORMAL
#endif

#ifdef CONFIG_ZONE_DMA
#define OPT_ZONE_DMA ZONE_DMA
#else
#define OPT_ZONE_DMA ZONE_NORMAL
#endif

#ifdef CONFIG_ZONE_DMA32
#define OPT_ZONE_DMA32 ZONE_DMA32
#else
#define OPT_ZONE_DMA32 ZONE_NORMAL
#endif



#if defined(CONFIG_ZONE_DEVICE) && (MAX_NR_ZONES-1) <= 4

#define GFP_ZONES_SHIFT 2
#else
#define GFP_ZONES_SHIFT ZONES_SHIFT
#endif

#if 16 * GFP_ZONES_SHIFT > BITS_PER_LONG
#error GFP_ZONES_SHIFT too large to create GFP_ZONE_TABLE integer
#endif

#define GFP_ZONE_TABLE ( \
	(ZONE_NORMAL << 0 * GFP_ZONES_SHIFT)				       \
	| (OPT_ZONE_DMA << ___GFP_DMA * GFP_ZONES_SHIFT)		       \
	| (OPT_ZONE_HIGHMEM << ___GFP_HIGHMEM * GFP_ZONES_SHIFT)	       \
	| (OPT_ZONE_DMA32 << ___GFP_DMA32 * GFP_ZONES_SHIFT)		       \
	| (ZONE_NORMAL << ___GFP_MOVABLE * GFP_ZONES_SHIFT)		       \
	| (OPT_ZONE_DMA << (___GFP_MOVABLE | ___GFP_DMA) * GFP_ZONES_SHIFT)    \
	| (ZONE_MOVABLE << (___GFP_MOVABLE | ___GFP_HIGHMEM) * GFP_ZONES_SHIFT)\
	| (OPT_ZONE_DMA32 << (___GFP_MOVABLE | ___GFP_DMA32) * GFP_ZONES_SHIFT)\
)


#define GFP_ZONE_BAD ( \
	1 << (___GFP_DMA | ___GFP_HIGHMEM)				      \
	| 1 << (___GFP_DMA | ___GFP_DMA32)				      \
	| 1 << (___GFP_DMA32 | ___GFP_HIGHMEM)				      \
	| 1 << (___GFP_DMA | ___GFP_DMA32 | ___GFP_HIGHMEM)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_HIGHMEM | ___GFP_DMA)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_DMA32 | ___GFP_DMA)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_DMA32 | ___GFP_HIGHMEM)		      \
	| 1 << (___GFP_MOVABLE | ___GFP_DMA32 | ___GFP_DMA | ___GFP_HIGHMEM)  \
)

static inline enum zone_type gfp_zone(gfp_t flags)
{
	enum zone_type z;
	int bit = (__force int) (flags & GFP_ZONEMASK);

	z = (GFP_ZONE_TABLE >> (bit * GFP_ZONES_SHIFT)) &
					 ((1 << GFP_ZONES_SHIFT) - 1);
	VM_BUG_ON((GFP_ZONE_BAD >> bit) & 1);
	return z;
}



static inline int gfp_zonelist(gfp_t flags)
{
#ifdef CONFIG_NUMA
	if (unlikely(flags & __GFP_THISNODE))
		return ZONELIST_NOFALLBACK;
#endif
	return ZONELIST_FALLBACK;
}


static inline struct zonelist *node_zonelist(int nid, gfp_t flags)
{
	return NODE_DATA(nid)->node_zonelists + gfp_zonelist(flags);
}

#ifndef HAVE_ARCH_FREE_PAGE
static inline void arch_free_page(struct page *page, int order) { }
#endif
#ifndef HAVE_ARCH_ALLOC_PAGE
static inline void arch_alloc_page(struct page *page, int order) { }
#endif

struct page *__alloc_pages(gfp_t gfp, unsigned int order, int preferred_nid,
		nodemask_t *nodemask);
struct folio *__folio_alloc(gfp_t gfp, unsigned int order, int preferred_nid,
		nodemask_t *nodemask);

unsigned long __alloc_pages_bulk(gfp_t gfp, int preferred_nid,
				nodemask_t *nodemask, int nr_pages,
				struct list_head *page_list,
				struct page **page_array);

unsigned long alloc_pages_bulk_array_mempolicy(gfp_t gfp,
				unsigned long nr_pages,
				struct page **page_array);


static inline unsigned long
alloc_pages_bulk_list(gfp_t gfp, unsigned long nr_pages, struct list_head *list)
{
	return __alloc_pages_bulk(gfp, numa_mem_id(), NULL, nr_pages, list, NULL);
}

static inline unsigned long
alloc_pages_bulk_array(gfp_t gfp, unsigned long nr_pages, struct page **page_array)
{
	return __alloc_pages_bulk(gfp, numa_mem_id(), NULL, nr_pages, NULL, page_array);
}

static inline unsigned long
alloc_pages_bulk_array_node(gfp_t gfp, int nid, unsigned long nr_pages, struct page **page_array)
{
	if (nid == NUMA_NO_NODE)
		nid = numa_mem_id();

	return __alloc_pages_bulk(gfp, nid, NULL, nr_pages, NULL, page_array);
}

static inline void warn_if_node_offline(int this_node, gfp_t gfp_mask)
{
	gfp_t warn_gfp = gfp_mask & (__GFP_THISNODE|__GFP_NOWARN);

	if (warn_gfp != (__GFP_THISNODE|__GFP_NOWARN))
		return;

	if (node_online(this_node))
		return;

	pr_warn("%pGg allocation from offline node %d\n", &gfp_mask, this_node);
	dump_stack();
}


static inline struct page *
__alloc_pages_node(int nid, gfp_t gfp_mask, unsigned int order)
{
	VM_BUG_ON(nid < 0 || nid >= MAX_NUMNODES);
	warn_if_node_offline(nid, gfp_mask);

	return __alloc_pages(gfp_mask, order, nid, NULL);
}

static inline
struct folio *__folio_alloc_node(gfp_t gfp, unsigned int order, int nid)
{
	VM_BUG_ON(nid < 0 || nid >= MAX_NUMNODES);
	warn_if_node_offline(nid, gfp);

	return __folio_alloc(gfp, order, nid, NULL);
}


static inline struct page *alloc_pages_node(int nid, gfp_t gfp_mask,
						unsigned int order)
{
	if (nid == NUMA_NO_NODE)
		nid = numa_mem_id();

	return __alloc_pages_node(nid, gfp_mask, order);
}

#ifdef CONFIG_NUMA
struct page *alloc_pages(gfp_t gfp, unsigned int order);
struct folio *folio_alloc(gfp_t gfp, unsigned order);
struct folio *vma_alloc_folio(gfp_t gfp, int order, struct vm_area_struct *vma,
		unsigned long addr, bool hugepage);
#else
static inline struct page *alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	return alloc_pages_node(numa_node_id(), gfp_mask, order);
}
static inline struct folio *folio_alloc(gfp_t gfp, unsigned int order)
{
	return __folio_alloc_node(gfp, order, numa_node_id());
}
#define vma_alloc_folio(gfp, order, vma, addr, hugepage)		\
	folio_alloc(gfp, order)
#endif
#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)
static inline struct page *alloc_page_vma(gfp_t gfp,
		struct vm_area_struct *vma, unsigned long addr)
{
	struct folio *folio = vma_alloc_folio(gfp, 0, vma, addr, false);

	return &folio->page;
}

extern unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order);
extern unsigned long get_zeroed_page(gfp_t gfp_mask);

void *alloc_pages_exact(size_t size, gfp_t gfp_mask) __alloc_size(1);
void free_pages_exact(void *virt, size_t size);
__meminit void *alloc_pages_exact_nid(int nid, size_t size, gfp_t gfp_mask) __alloc_size(2);

#define __get_free_page(gfp_mask) \
		__get_free_pages((gfp_mask), 0)

#define __get_dma_pages(gfp_mask, order) \
		__get_free_pages((gfp_mask) | GFP_DMA, (order))

extern void __free_pages(struct page *page, unsigned int order);
extern void free_pages(unsigned long addr, unsigned int order);

struct page_frag_cache;
extern void __page_frag_cache_drain(struct page *page, unsigned int count);
extern void *page_frag_alloc_align(struct page_frag_cache *nc,
				   unsigned int fragsz, gfp_t gfp_mask,
				   unsigned int align_mask);

static inline void *page_frag_alloc(struct page_frag_cache *nc,
			     unsigned int fragsz, gfp_t gfp_mask)
{
	return page_frag_alloc_align(nc, fragsz, gfp_mask, ~0u);
}

extern void page_frag_free(void *addr);

#define __free_page(page) __free_pages((page), 0)
#define free_page(addr) free_pages((addr), 0)

void page_alloc_init_cpuhp(void);
void drain_zone_pages(struct zone *zone, struct per_cpu_pages *pcp);
void drain_all_pages(struct zone *zone);
void drain_local_pages(struct zone *zone);

void page_alloc_init_late(void);


extern gfp_t gfp_allowed_mask;


bool gfp_pfmemalloc_allowed(gfp_t gfp_mask);

static inline bool gfp_has_io_fs(gfp_t gfp)
{
	return (gfp & (__GFP_IO | __GFP_FS)) == (__GFP_IO | __GFP_FS);
}


static inline bool gfp_compaction_allowed(gfp_t gfp_mask)
{
	return IS_ENABLED(CONFIG_COMPACTION) && (gfp_mask & __GFP_IO);
}

extern gfp_t vma_thp_gfp_mask(struct vm_area_struct *vma);

#ifdef CONFIG_CONTIG_ALLOC

extern int alloc_contig_range(unsigned long start, unsigned long end,
			      unsigned migratetype, gfp_t gfp_mask);
extern struct page *alloc_contig_pages(unsigned long nr_pages, gfp_t gfp_mask,
				       int nid, nodemask_t *nodemask);
#endif
void free_contig_range(unsigned long pfn, unsigned long nr_pages);

#endif 
