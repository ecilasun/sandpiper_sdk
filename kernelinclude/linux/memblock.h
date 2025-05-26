/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LINUX_MEMBLOCK_H
#define _LINUX_MEMBLOCK_H



#include <linux/init.h>
#include <linux/mm.h>
#include <asm/dma.h>

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;


extern unsigned long max_pfn;

extern unsigned long long max_possible_pfn;


enum memblock_flags {
	MEMBLOCK_NONE		= 0x0,	
	MEMBLOCK_HOTPLUG	= 0x1,	
	MEMBLOCK_MIRROR		= 0x2,	
	MEMBLOCK_NOMAP		= 0x4,	
	MEMBLOCK_DRIVER_MANAGED = 0x8,	
};


struct memblock_region {
	phys_addr_t base;
	phys_addr_t size;
	enum memblock_flags flags;
#ifdef CONFIG_NUMA
	int nid;
#endif
};


struct memblock_type {
	unsigned long cnt;
	unsigned long max;
	phys_addr_t total_size;
	struct memblock_region *regions;
	char *name;
};


struct memblock {
	bool bottom_up;  
	phys_addr_t current_limit;
	struct memblock_type memory;
	struct memblock_type reserved;
};

extern struct memblock memblock;

#ifndef CONFIG_ARCH_KEEP_MEMBLOCK
#define __init_memblock __meminit
#define __initdata_memblock __meminitdata
void memblock_discard(void);
#else
#define __init_memblock
#define __initdata_memblock
static inline void memblock_discard(void) {}
#endif

void memblock_allow_resize(void);
int memblock_add_node(phys_addr_t base, phys_addr_t size, int nid,
		      enum memblock_flags flags);
int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
int memblock_phys_free(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);
#ifdef CONFIG_HAVE_MEMBLOCK_PHYS_MAP
int memblock_physmem_add(phys_addr_t base, phys_addr_t size);
#endif
void memblock_trim_memory(phys_addr_t align);
unsigned long memblock_addrs_overlap(phys_addr_t base1, phys_addr_t size1,
				     phys_addr_t base2, phys_addr_t size2);
bool memblock_overlaps_region(struct memblock_type *type,
			      phys_addr_t base, phys_addr_t size);
int memblock_mark_hotplug(phys_addr_t base, phys_addr_t size);
int memblock_clear_hotplug(phys_addr_t base, phys_addr_t size);
int memblock_mark_mirror(phys_addr_t base, phys_addr_t size);
int memblock_mark_nomap(phys_addr_t base, phys_addr_t size);
int memblock_clear_nomap(phys_addr_t base, phys_addr_t size);

void memblock_free_all(void);
void memblock_free(void *ptr, size_t size);
void reset_all_zones_managed_pages(void);


void __next_mem_range(u64 *idx, int nid, enum memblock_flags flags,
		      struct memblock_type *type_a,
		      struct memblock_type *type_b, phys_addr_t *out_start,
		      phys_addr_t *out_end, int *out_nid);

void __next_mem_range_rev(u64 *idx, int nid, enum memblock_flags flags,
			  struct memblock_type *type_a,
			  struct memblock_type *type_b, phys_addr_t *out_start,
			  phys_addr_t *out_end, int *out_nid);

void memblock_free_late(phys_addr_t base, phys_addr_t size);

#ifdef CONFIG_HAVE_MEMBLOCK_PHYS_MAP
static inline void __next_physmem_range(u64 *idx, struct memblock_type *type,
					phys_addr_t *out_start,
					phys_addr_t *out_end)
{
	extern struct memblock_type physmem;

	__next_mem_range(idx, NUMA_NO_NODE, MEMBLOCK_NONE, &physmem, type,
			 out_start, out_end, NULL);
}


#define for_each_physmem_range(i, type, p_start, p_end)			\
	for (i = 0, __next_physmem_range(&i, type, p_start, p_end);	\
	     i != (u64)ULLONG_MAX;					\
	     __next_physmem_range(&i, type, p_start, p_end))
#endif 


#define __for_each_mem_range(i, type_a, type_b, nid, flags,		\
			   p_start, p_end, p_nid)			\
	for (i = 0, __next_mem_range(&i, nid, flags, type_a, type_b,	\
				     p_start, p_end, p_nid);		\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range(&i, nid, flags, type_a, type_b,		\
			      p_start, p_end, p_nid))


#define __for_each_mem_range_rev(i, type_a, type_b, nid, flags,		\
				 p_start, p_end, p_nid)			\
	for (i = (u64)ULLONG_MAX,					\
		     __next_mem_range_rev(&i, nid, flags, type_a, type_b, \
					  p_start, p_end, p_nid);	\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range_rev(&i, nid, flags, type_a, type_b,	\
				  p_start, p_end, p_nid))


#define for_each_mem_range(i, p_start, p_end) \
	__for_each_mem_range(i, &memblock.memory, NULL, NUMA_NO_NODE,	\
			     MEMBLOCK_HOTPLUG | MEMBLOCK_DRIVER_MANAGED, \
			     p_start, p_end, NULL)


#define for_each_mem_range_rev(i, p_start, p_end)			\
	__for_each_mem_range_rev(i, &memblock.memory, NULL, NUMA_NO_NODE, \
				 MEMBLOCK_HOTPLUG | MEMBLOCK_DRIVER_MANAGED,\
				 p_start, p_end, NULL)


#define for_each_reserved_mem_range(i, p_start, p_end)			\
	__for_each_mem_range(i, &memblock.reserved, NULL, NUMA_NO_NODE,	\
			     MEMBLOCK_NONE, p_start, p_end, NULL)

static inline bool memblock_is_hotpluggable(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_HOTPLUG;
}

static inline bool memblock_is_mirror(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_MIRROR;
}

static inline bool memblock_is_nomap(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_NOMAP;
}

static inline bool memblock_is_driver_managed(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_DRIVER_MANAGED;
}

int memblock_search_pfn_nid(unsigned long pfn, unsigned long *start_pfn,
			    unsigned long  *end_pfn);
void __next_mem_pfn_range(int *idx, int nid, unsigned long *out_start_pfn,
			  unsigned long *out_end_pfn, int *out_nid);


#define for_each_mem_pfn_range(i, nid, p_start, p_end, p_nid)		\
	for (i = -1, __next_mem_pfn_range(&i, nid, p_start, p_end, p_nid); \
	     i >= 0; __next_mem_pfn_range(&i, nid, p_start, p_end, p_nid))

#ifdef CONFIG_DEFERRED_STRUCT_PAGE_INIT
void __next_mem_pfn_range_in_zone(u64 *idx, struct zone *zone,
				  unsigned long *out_spfn,
				  unsigned long *out_epfn);

#define for_each_free_mem_pfn_range_in_zone(i, zone, p_start, p_end)	\
	for (i = 0,							\
	     __next_mem_pfn_range_in_zone(&i, zone, p_start, p_end);	\
	     i != U64_MAX;					\
	     __next_mem_pfn_range_in_zone(&i, zone, p_start, p_end))


#define for_each_free_mem_pfn_range_in_zone_from(i, zone, p_start, p_end) \
	for (; i != U64_MAX;					  \
	     __next_mem_pfn_range_in_zone(&i, zone, p_start, p_end))

int __init deferred_page_init_max_threads(const struct cpumask *node_cpumask);

#endif 


#define for_each_free_mem_range(i, nid, flags, p_start, p_end, p_nid)	\
	__for_each_mem_range(i, &memblock.memory, &memblock.reserved,	\
			     nid, flags, p_start, p_end, p_nid)


#define for_each_free_mem_range_reverse(i, nid, flags, p_start, p_end,	\
					p_nid)				\
	__for_each_mem_range_rev(i, &memblock.memory, &memblock.reserved, \
				 nid, flags, p_start, p_end, p_nid)

int memblock_set_node(phys_addr_t base, phys_addr_t size,
		      struct memblock_type *type, int nid);

#ifdef CONFIG_NUMA
static inline void memblock_set_region_node(struct memblock_region *r, int nid)
{
	r->nid = nid;
}

static inline int memblock_get_region_node(const struct memblock_region *r)
{
	return r->nid;
}
#else
static inline void memblock_set_region_node(struct memblock_region *r, int nid)
{
}

static inline int memblock_get_region_node(const struct memblock_region *r)
{
	return 0;
}
#endif 


#define MEMBLOCK_ALLOC_ANYWHERE	(~(phys_addr_t)0)
#define MEMBLOCK_ALLOC_ACCESSIBLE	0
#define MEMBLOCK_ALLOC_NOLEAKTRACE	1


#define MEMBLOCK_LOW_LIMIT 0

#ifndef ARCH_LOW_ADDRESS_LIMIT
#define ARCH_LOW_ADDRESS_LIMIT  0xffffffffUL
#endif

phys_addr_t memblock_phys_alloc_range(phys_addr_t size, phys_addr_t align,
				      phys_addr_t start, phys_addr_t end);
phys_addr_t memblock_alloc_range_nid(phys_addr_t size,
				      phys_addr_t align, phys_addr_t start,
				      phys_addr_t end, int nid, bool exact_nid);
phys_addr_t memblock_phys_alloc_try_nid(phys_addr_t size, phys_addr_t align, int nid);

static __always_inline phys_addr_t memblock_phys_alloc(phys_addr_t size,
						       phys_addr_t align)
{
	return memblock_phys_alloc_range(size, align, 0,
					 MEMBLOCK_ALLOC_ACCESSIBLE);
}

void *memblock_alloc_exact_nid_raw(phys_addr_t size, phys_addr_t align,
				 phys_addr_t min_addr, phys_addr_t max_addr,
				 int nid);
void *memblock_alloc_try_nid_raw(phys_addr_t size, phys_addr_t align,
				 phys_addr_t min_addr, phys_addr_t max_addr,
				 int nid);
void *memblock_alloc_try_nid(phys_addr_t size, phys_addr_t align,
			     phys_addr_t min_addr, phys_addr_t max_addr,
			     int nid);

static __always_inline void *memblock_alloc(phys_addr_t size, phys_addr_t align)
{
	return memblock_alloc_try_nid(size, align, MEMBLOCK_LOW_LIMIT,
				      MEMBLOCK_ALLOC_ACCESSIBLE, NUMA_NO_NODE);
}

static inline void *memblock_alloc_raw(phys_addr_t size,
					       phys_addr_t align)
{
	return memblock_alloc_try_nid_raw(size, align, MEMBLOCK_LOW_LIMIT,
					  MEMBLOCK_ALLOC_ACCESSIBLE,
					  NUMA_NO_NODE);
}

static inline void *memblock_alloc_from(phys_addr_t size,
						phys_addr_t align,
						phys_addr_t min_addr)
{
	return memblock_alloc_try_nid(size, align, min_addr,
				      MEMBLOCK_ALLOC_ACCESSIBLE, NUMA_NO_NODE);
}

static inline void *memblock_alloc_low(phys_addr_t size,
					       phys_addr_t align)
{
	return memblock_alloc_try_nid(size, align, MEMBLOCK_LOW_LIMIT,
				      ARCH_LOW_ADDRESS_LIMIT, NUMA_NO_NODE);
}

static inline void *memblock_alloc_node(phys_addr_t size,
						phys_addr_t align, int nid)
{
	return memblock_alloc_try_nid(size, align, MEMBLOCK_LOW_LIMIT,
				      MEMBLOCK_ALLOC_ACCESSIBLE, nid);
}


static inline __init_memblock void memblock_set_bottom_up(bool enable)
{
	memblock.bottom_up = enable;
}


static inline __init_memblock bool memblock_bottom_up(void)
{
	return memblock.bottom_up;
}

phys_addr_t memblock_phys_mem_size(void);
phys_addr_t memblock_reserved_size(void);
phys_addr_t memblock_start_of_DRAM(void);
phys_addr_t memblock_end_of_DRAM(void);
void memblock_enforce_memory_limit(phys_addr_t memory_limit);
void memblock_cap_memory_range(phys_addr_t base, phys_addr_t size);
void memblock_mem_limit_remove_map(phys_addr_t limit);
bool memblock_is_memory(phys_addr_t addr);
bool memblock_is_map_memory(phys_addr_t addr);
bool memblock_is_region_memory(phys_addr_t base, phys_addr_t size);
bool memblock_is_reserved(phys_addr_t addr);
bool memblock_is_region_reserved(phys_addr_t base, phys_addr_t size);

void memblock_dump_all(void);


void memblock_set_current_limit(phys_addr_t limit);


phys_addr_t memblock_get_current_limit(void);




static inline unsigned long memblock_region_memory_base_pfn(const struct memblock_region *reg)
{
	return PFN_UP(reg->base);
}


static inline unsigned long memblock_region_memory_end_pfn(const struct memblock_region *reg)
{
	return PFN_DOWN(reg->base + reg->size);
}


static inline unsigned long memblock_region_reserved_base_pfn(const struct memblock_region *reg)
{
	return PFN_DOWN(reg->base);
}


static inline unsigned long memblock_region_reserved_end_pfn(const struct memblock_region *reg)
{
	return PFN_UP(reg->base + reg->size);
}


#define for_each_mem_region(region)					\
	for (region = memblock.memory.regions;				\
	     region < (memblock.memory.regions + memblock.memory.cnt);	\
	     region++)


#define for_each_reserved_mem_region(region)				\
	for (region = memblock.reserved.regions;			\
	     region < (memblock.reserved.regions + memblock.reserved.cnt); \
	     region++)

extern void *alloc_large_system_hash(const char *tablename,
				     unsigned long bucketsize,
				     unsigned long numentries,
				     int scale,
				     int flags,
				     unsigned int *_hash_shift,
				     unsigned int *_hash_mask,
				     unsigned long low_limit,
				     unsigned long high_limit);

#define HASH_EARLY	0x00000001	
#define HASH_ZERO	0x00000002	


#ifdef CONFIG_NUMA
#define HASHDIST_DEFAULT IS_ENABLED(CONFIG_64BIT)
extern int hashdist;		
#else
#define hashdist (0)
#endif

#ifdef CONFIG_MEMTEST
void early_memtest(phys_addr_t start, phys_addr_t end);
void memtest_report_meminfo(struct seq_file *m);
#else
static inline void early_memtest(phys_addr_t start, phys_addr_t end) { }
static inline void memtest_report_meminfo(struct seq_file *m) { }
#endif


#endif 
