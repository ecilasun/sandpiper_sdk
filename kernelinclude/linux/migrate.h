/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MIGRATE_H
#define _LINUX_MIGRATE_H

#include <linux/mm.h>
#include <linux/mempolicy.h>
#include <linux/migrate_mode.h>
#include <linux/hugetlb.h>

typedef struct folio *new_folio_t(struct folio *folio, unsigned long private);
typedef void free_folio_t(struct folio *folio, unsigned long private);

struct migration_target_control;


#define MIGRATEPAGE_SUCCESS		0
#define MIGRATEPAGE_UNMAP		1


struct movable_operations {
	bool (*isolate_page)(struct page *, isolate_mode_t);
	int (*migrate_page)(struct page *dst, struct page *src,
			enum migrate_mode);
	void (*putback_page)(struct page *);
};


extern const char *migrate_reason_names[MR_TYPES];

#ifdef CONFIG_MIGRATION

void putback_movable_pages(struct list_head *l);
int migrate_folio_extra(struct address_space *mapping, struct folio *dst,
		struct folio *src, enum migrate_mode mode, int extra_count);
int migrate_folio(struct address_space *mapping, struct folio *dst,
		struct folio *src, enum migrate_mode mode);
int migrate_pages(struct list_head *l, new_folio_t new, free_folio_t free,
		  unsigned long private, enum migrate_mode mode, int reason,
		  unsigned int *ret_succeeded);
struct folio *alloc_migration_target(struct folio *src, unsigned long private);
bool isolate_movable_page(struct page *page, isolate_mode_t mode);

int migrate_huge_page_move_mapping(struct address_space *mapping,
		struct folio *dst, struct folio *src);
void migration_entry_wait_on_locked(swp_entry_t entry, spinlock_t *ptl)
		__releases(ptl);
void folio_migrate_flags(struct folio *newfolio, struct folio *folio);
void folio_migrate_copy(struct folio *newfolio, struct folio *folio);
int folio_migrate_mapping(struct address_space *mapping,
		struct folio *newfolio, struct folio *folio, int extra_count);

#else

static inline void putback_movable_pages(struct list_head *l) {}
static inline int migrate_pages(struct list_head *l, new_folio_t new,
		free_folio_t free, unsigned long private,
		enum migrate_mode mode, int reason, unsigned int *ret_succeeded)
	{ return -ENOSYS; }
static inline struct folio *alloc_migration_target(struct folio *src,
		unsigned long private)
	{ return NULL; }
static inline bool isolate_movable_page(struct page *page, isolate_mode_t mode)
	{ return false; }

static inline int migrate_huge_page_move_mapping(struct address_space *mapping,
				  struct folio *dst, struct folio *src)
{
	return -ENOSYS;
}

#endif 

#ifdef CONFIG_COMPACTION
bool PageMovable(struct page *page);
void __SetPageMovable(struct page *page, const struct movable_operations *ops);
void __ClearPageMovable(struct page *page);
#else
static inline bool PageMovable(struct page *page) { return false; }
static inline void __SetPageMovable(struct page *page,
		const struct movable_operations *ops)
{
}
static inline void __ClearPageMovable(struct page *page)
{
}
#endif

static inline bool folio_test_movable(struct folio *folio)
{
	return PageMovable(&folio->page);
}

static inline
const struct movable_operations *folio_movable_ops(struct folio *folio)
{
	VM_BUG_ON(!__folio_test_movable(folio));

	return (const struct movable_operations *)
		((unsigned long)folio->mapping - PAGE_MAPPING_MOVABLE);
}

static inline
const struct movable_operations *page_movable_ops(struct page *page)
{
	VM_BUG_ON(!__PageMovable(page));

	return (const struct movable_operations *)
		((unsigned long)page->mapping - PAGE_MAPPING_MOVABLE);
}

#ifdef CONFIG_NUMA_BALANCING
int migrate_misplaced_page(struct page *page, struct vm_area_struct *vma,
			   int node);
#else
static inline int migrate_misplaced_page(struct page *page,
					 struct vm_area_struct *vma, int node)
{
	return -EAGAIN; 
}
#endif 

#ifdef CONFIG_MIGRATION


#define MIGRATE_PFN_VALID	(1UL << 0)
#define MIGRATE_PFN_MIGRATE	(1UL << 1)
#define MIGRATE_PFN_WRITE	(1UL << 3)
#define MIGRATE_PFN_SHIFT	6

static inline struct page *migrate_pfn_to_page(unsigned long mpfn)
{
	if (!(mpfn & MIGRATE_PFN_VALID))
		return NULL;
	return pfn_to_page(mpfn >> MIGRATE_PFN_SHIFT);
}

static inline unsigned long migrate_pfn(unsigned long pfn)
{
	return (pfn << MIGRATE_PFN_SHIFT) | MIGRATE_PFN_VALID;
}

enum migrate_vma_direction {
	MIGRATE_VMA_SELECT_SYSTEM = 1 << 0,
	MIGRATE_VMA_SELECT_DEVICE_PRIVATE = 1 << 1,
	MIGRATE_VMA_SELECT_DEVICE_COHERENT = 1 << 2,
};

struct migrate_vma {
	struct vm_area_struct	*vma;
	
	unsigned long		*dst;
	unsigned long		*src;
	unsigned long		cpages;
	unsigned long		npages;
	unsigned long		start;
	unsigned long		end;

	
	void			*pgmap_owner;
	unsigned long		flags;

	
	struct page		*fault_page;
};

int migrate_vma_setup(struct migrate_vma *args);
void migrate_vma_pages(struct migrate_vma *migrate);
void migrate_vma_finalize(struct migrate_vma *migrate);
int migrate_device_range(unsigned long *src_pfns, unsigned long start,
			unsigned long npages);
void migrate_device_pages(unsigned long *src_pfns, unsigned long *dst_pfns,
			unsigned long npages);
void migrate_device_finalize(unsigned long *src_pfns,
			unsigned long *dst_pfns, unsigned long npages);

#endif 

#endif 
