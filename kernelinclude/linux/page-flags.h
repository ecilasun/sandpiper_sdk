/* SPDX-License-Identifier: GPL-2.0 */


#ifndef PAGE_FLAGS_H
#define PAGE_FLAGS_H

#include <linux/types.h>
#include <linux/bug.h>
#include <linux/mmdebug.h>
#ifndef __GENERATING_BOUNDS_H
#include <linux/mm_types.h>
#include <generated/bounds.h>
#endif 




enum pageflags {
	PG_locked,		
	PG_writeback,		
	PG_referenced,
	PG_uptodate,
	PG_dirty,
	PG_lru,
	PG_head,		
	PG_waiters,		
	PG_active,
	PG_workingset,
	PG_error,
	PG_slab,
	PG_owner_priv_1,	
	PG_arch_1,
	PG_reserved,
	PG_private,		
	PG_private_2,		
	PG_mappedtodisk,	
	PG_reclaim,		
	PG_swapbacked,		
	PG_unevictable,		
#ifdef CONFIG_MMU
	PG_mlocked,		
#endif
#ifdef CONFIG_ARCH_USES_PG_UNCACHED
	PG_uncached,		
#endif
#ifdef CONFIG_MEMORY_FAILURE
	PG_hwpoison,		
#endif
#if defined(CONFIG_PAGE_IDLE_FLAG) && defined(CONFIG_64BIT)
	PG_young,
	PG_idle,
#endif
#ifdef CONFIG_ARCH_USES_PG_ARCH_X
	PG_arch_2,
	PG_arch_3,
#endif
	__NR_PAGEFLAGS,

	PG_readahead = PG_reclaim,

	
	PG_anon_exclusive = PG_mappedtodisk,

	
	PG_checked = PG_owner_priv_1,

	
	PG_swapcache = PG_owner_priv_1,	

	
	PG_fscache = PG_private_2,	

	
	
	PG_pinned = PG_owner_priv_1,
	
	PG_savepinned = PG_dirty,
	
	PG_foreign = PG_owner_priv_1,
	
	PG_xen_remapped = PG_owner_priv_1,

	
	PG_isolated = PG_reclaim,

	
	PG_reported = PG_uptodate,

#ifdef CONFIG_MEMORY_HOTPLUG
	
	PG_vmemmap_self_hosted = PG_owner_priv_1,
#endif

	

	
	PG_has_hwpoisoned = PG_error,
	PG_large_rmappable = PG_workingset, 
};

#define PAGEFLAGS_MASK		((1UL << NR_PAGEFLAGS) - 1)

#ifndef __GENERATING_BOUNDS_H

#ifdef CONFIG_HUGETLB_PAGE_OPTIMIZE_VMEMMAP
DECLARE_STATIC_KEY_FALSE(hugetlb_optimize_vmemmap_key);


static __always_inline const struct page *page_fixed_fake_head(const struct page *page)
{
	if (!static_branch_unlikely(&hugetlb_optimize_vmemmap_key))
		return page;

	
	if (IS_ALIGNED((unsigned long)page, PAGE_SIZE) &&
	    test_bit(PG_head, &page->flags)) {
		
		unsigned long head = READ_ONCE(page[1].compound_head);

		if (likely(head & 1))
			return (const struct page *)(head - 1);
	}
	return page;
}
#else
static inline const struct page *page_fixed_fake_head(const struct page *page)
{
	return page;
}
#endif

static __always_inline int page_is_fake_head(struct page *page)
{
	return page_fixed_fake_head(page) != page;
}

static inline unsigned long _compound_head(const struct page *page)
{
	unsigned long head = READ_ONCE(page->compound_head);

	if (unlikely(head & 1))
		return head - 1;
	return (unsigned long)page_fixed_fake_head(page);
}

#define compound_head(page)	((typeof(page))_compound_head(page))


#define page_folio(p)		(_Generic((p),				\
	const struct page *:	(const struct folio *)_compound_head(p), \
	struct page *:		(struct folio *)_compound_head(p)))


#define folio_page(folio, n)	nth_page(&(folio)->page, n)

static __always_inline int PageTail(struct page *page)
{
	return READ_ONCE(page->compound_head) & 1 || page_is_fake_head(page);
}

static __always_inline int PageCompound(struct page *page)
{
	return test_bit(PG_head, &page->flags) ||
	       READ_ONCE(page->compound_head) & 1;
}

#define	PAGE_POISON_PATTERN	-1l
static inline int PagePoisoned(const struct page *page)
{
	return READ_ONCE(page->flags) == PAGE_POISON_PATTERN;
}

#ifdef CONFIG_DEBUG_VM
void page_init_poison(struct page *page, size_t size);
#else
static inline void page_init_poison(struct page *page, size_t size)
{
}
#endif

static unsigned long *folio_flags(struct folio *folio, unsigned n)
{
	struct page *page = &folio->page;

	VM_BUG_ON_PGFLAGS(PageTail(page), page);
	VM_BUG_ON_PGFLAGS(n > 0 && !test_bit(PG_head, &page->flags), page);
	return &page[n].flags;
}


#define PF_POISONED_CHECK(page) ({					\
		VM_BUG_ON_PGFLAGS(PagePoisoned(page), page);		\
		page; })
#define PF_ANY(page, enforce)	PF_POISONED_CHECK(page)
#define PF_HEAD(page, enforce)	PF_POISONED_CHECK(compound_head(page))
#define PF_ONLY_HEAD(page, enforce) ({					\
		VM_BUG_ON_PGFLAGS(PageTail(page), page);		\
		PF_POISONED_CHECK(page); })
#define PF_NO_TAIL(page, enforce) ({					\
		VM_BUG_ON_PGFLAGS(enforce && PageTail(page), page);	\
		PF_POISONED_CHECK(compound_head(page)); })
#define PF_NO_COMPOUND(page, enforce) ({				\
		VM_BUG_ON_PGFLAGS(enforce && PageCompound(page), page);	\
		PF_POISONED_CHECK(page); })
#define PF_SECOND(page, enforce) ({					\
		VM_BUG_ON_PGFLAGS(!PageHead(page), page);		\
		PF_POISONED_CHECK(&page[1]); })


#define FOLIO_PF_ANY		0
#define FOLIO_PF_HEAD		0
#define FOLIO_PF_ONLY_HEAD	0
#define FOLIO_PF_NO_TAIL	0
#define FOLIO_PF_NO_COMPOUND	0
#define FOLIO_PF_SECOND		1


#define TESTPAGEFLAG(uname, lname, policy)				\
static __always_inline bool folio_test_##lname(struct folio *folio)	\
{ return test_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); }	\
static __always_inline int Page##uname(struct page *page)		\
{ return test_bit(PG_##lname, &policy(page, 0)->flags); }

#define SETPAGEFLAG(uname, lname, policy)				\
static __always_inline							\
void folio_set_##lname(struct folio *folio)				\
{ set_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); }		\
static __always_inline void SetPage##uname(struct page *page)		\
{ set_bit(PG_##lname, &policy(page, 1)->flags); }

#define CLEARPAGEFLAG(uname, lname, policy)				\
static __always_inline							\
void folio_clear_##lname(struct folio *folio)				\
{ clear_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); }		\
static __always_inline void ClearPage##uname(struct page *page)		\
{ clear_bit(PG_##lname, &policy(page, 1)->flags); }

#define __SETPAGEFLAG(uname, lname, policy)				\
static __always_inline							\
void __folio_set_##lname(struct folio *folio)				\
{ __set_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); }		\
static __always_inline void __SetPage##uname(struct page *page)		\
{ __set_bit(PG_##lname, &policy(page, 1)->flags); }

#define __CLEARPAGEFLAG(uname, lname, policy)				\
static __always_inline							\
void __folio_clear_##lname(struct folio *folio)				\
{ __clear_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); }	\
static __always_inline void __ClearPage##uname(struct page *page)	\
{ __clear_bit(PG_##lname, &policy(page, 1)->flags); }

#define TESTSETFLAG(uname, lname, policy)				\
static __always_inline							\
bool folio_test_set_##lname(struct folio *folio)			\
{ return test_and_set_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); } \
static __always_inline int TestSetPage##uname(struct page *page)	\
{ return test_and_set_bit(PG_##lname, &policy(page, 1)->flags); }

#define TESTCLEARFLAG(uname, lname, policy)				\
static __always_inline							\
bool folio_test_clear_##lname(struct folio *folio)			\
{ return test_and_clear_bit(PG_##lname, folio_flags(folio, FOLIO_##policy)); } \
static __always_inline int TestClearPage##uname(struct page *page)	\
{ return test_and_clear_bit(PG_##lname, &policy(page, 1)->flags); }

#define PAGEFLAG(uname, lname, policy)					\
	TESTPAGEFLAG(uname, lname, policy)				\
	SETPAGEFLAG(uname, lname, policy)				\
	CLEARPAGEFLAG(uname, lname, policy)

#define __PAGEFLAG(uname, lname, policy)				\
	TESTPAGEFLAG(uname, lname, policy)				\
	__SETPAGEFLAG(uname, lname, policy)				\
	__CLEARPAGEFLAG(uname, lname, policy)

#define TESTSCFLAG(uname, lname, policy)				\
	TESTSETFLAG(uname, lname, policy)				\
	TESTCLEARFLAG(uname, lname, policy)

#define FOLIO_TEST_FLAG_FALSE(name)					\
static inline bool folio_test_##name(const struct folio *folio)		\
{ return false; }
#define FOLIO_SET_FLAG_NOOP(name)					\
static inline void folio_set_##name(struct folio *folio) { }
#define FOLIO_CLEAR_FLAG_NOOP(name)					\
static inline void folio_clear_##name(struct folio *folio) { }
#define __FOLIO_SET_FLAG_NOOP(name)					\
static inline void __folio_set_##name(struct folio *folio) { }
#define __FOLIO_CLEAR_FLAG_NOOP(name)					\
static inline void __folio_clear_##name(struct folio *folio) { }
#define FOLIO_TEST_SET_FLAG_FALSE(name)					\
static inline bool folio_test_set_##name(struct folio *folio)		\
{ return false; }
#define FOLIO_TEST_CLEAR_FLAG_FALSE(name)				\
static inline bool folio_test_clear_##name(struct folio *folio)		\
{ return false; }

#define FOLIO_FLAG_FALSE(name)						\
FOLIO_TEST_FLAG_FALSE(name)						\
FOLIO_SET_FLAG_NOOP(name)						\
FOLIO_CLEAR_FLAG_NOOP(name)

#define TESTPAGEFLAG_FALSE(uname, lname)				\
FOLIO_TEST_FLAG_FALSE(lname)						\
static inline int Page##uname(const struct page *page) { return 0; }

#define SETPAGEFLAG_NOOP(uname, lname)					\
FOLIO_SET_FLAG_NOOP(lname)						\
static inline void SetPage##uname(struct page *page) {  }

#define CLEARPAGEFLAG_NOOP(uname, lname)				\
FOLIO_CLEAR_FLAG_NOOP(lname)						\
static inline void ClearPage##uname(struct page *page) {  }

#define __CLEARPAGEFLAG_NOOP(uname, lname)				\
__FOLIO_CLEAR_FLAG_NOOP(lname)						\
static inline void __ClearPage##uname(struct page *page) {  }

#define TESTSETFLAG_FALSE(uname, lname)					\
FOLIO_TEST_SET_FLAG_FALSE(lname)					\
static inline int TestSetPage##uname(struct page *page) { return 0; }

#define TESTCLEARFLAG_FALSE(uname, lname)				\
FOLIO_TEST_CLEAR_FLAG_FALSE(lname)					\
static inline int TestClearPage##uname(struct page *page) { return 0; }

#define PAGEFLAG_FALSE(uname, lname) TESTPAGEFLAG_FALSE(uname, lname)	\
	SETPAGEFLAG_NOOP(uname, lname) CLEARPAGEFLAG_NOOP(uname, lname)

#define TESTSCFLAG_FALSE(uname, lname)					\
	TESTSETFLAG_FALSE(uname, lname) TESTCLEARFLAG_FALSE(uname, lname)

__PAGEFLAG(Locked, locked, PF_NO_TAIL)
PAGEFLAG(Waiters, waiters, PF_ONLY_HEAD)
PAGEFLAG(Error, error, PF_NO_TAIL) TESTCLEARFLAG(Error, error, PF_NO_TAIL)
PAGEFLAG(Referenced, referenced, PF_HEAD)
	TESTCLEARFLAG(Referenced, referenced, PF_HEAD)
	__SETPAGEFLAG(Referenced, referenced, PF_HEAD)
PAGEFLAG(Dirty, dirty, PF_HEAD) TESTSCFLAG(Dirty, dirty, PF_HEAD)
	__CLEARPAGEFLAG(Dirty, dirty, PF_HEAD)
PAGEFLAG(LRU, lru, PF_HEAD) __CLEARPAGEFLAG(LRU, lru, PF_HEAD)
	TESTCLEARFLAG(LRU, lru, PF_HEAD)
PAGEFLAG(Active, active, PF_HEAD) __CLEARPAGEFLAG(Active, active, PF_HEAD)
	TESTCLEARFLAG(Active, active, PF_HEAD)
PAGEFLAG(Workingset, workingset, PF_HEAD)
	TESTCLEARFLAG(Workingset, workingset, PF_HEAD)
__PAGEFLAG(Slab, slab, PF_NO_TAIL)
PAGEFLAG(Checked, checked, PF_NO_COMPOUND)	   


PAGEFLAG(Pinned, pinned, PF_NO_COMPOUND)
	TESTSCFLAG(Pinned, pinned, PF_NO_COMPOUND)
PAGEFLAG(SavePinned, savepinned, PF_NO_COMPOUND);
PAGEFLAG(Foreign, foreign, PF_NO_COMPOUND);
PAGEFLAG(XenRemapped, xen_remapped, PF_NO_COMPOUND)
	TESTCLEARFLAG(XenRemapped, xen_remapped, PF_NO_COMPOUND)

PAGEFLAG(Reserved, reserved, PF_NO_COMPOUND)
	__CLEARPAGEFLAG(Reserved, reserved, PF_NO_COMPOUND)
	__SETPAGEFLAG(Reserved, reserved, PF_NO_COMPOUND)
PAGEFLAG(SwapBacked, swapbacked, PF_NO_TAIL)
	__CLEARPAGEFLAG(SwapBacked, swapbacked, PF_NO_TAIL)
	__SETPAGEFLAG(SwapBacked, swapbacked, PF_NO_TAIL)


PAGEFLAG(Private, private, PF_ANY)
PAGEFLAG(Private2, private_2, PF_ANY) TESTSCFLAG(Private2, private_2, PF_ANY)
PAGEFLAG(OwnerPriv1, owner_priv_1, PF_ANY)
	TESTCLEARFLAG(OwnerPriv1, owner_priv_1, PF_ANY)


TESTPAGEFLAG(Writeback, writeback, PF_NO_TAIL)
	TESTSCFLAG(Writeback, writeback, PF_NO_TAIL)
PAGEFLAG(MappedToDisk, mappedtodisk, PF_NO_TAIL)


PAGEFLAG(Reclaim, reclaim, PF_NO_TAIL)
	TESTCLEARFLAG(Reclaim, reclaim, PF_NO_TAIL)
PAGEFLAG(Readahead, readahead, PF_NO_COMPOUND)
	TESTCLEARFLAG(Readahead, readahead, PF_NO_COMPOUND)

#ifdef CONFIG_HIGHMEM

#define PageHighMem(__p) is_highmem_idx(page_zonenum(__p))
#define folio_test_highmem(__f)	is_highmem_idx(folio_zonenum(__f))
#else
PAGEFLAG_FALSE(HighMem, highmem)
#endif

#ifdef CONFIG_SWAP
static __always_inline bool folio_test_swapcache(struct folio *folio)
{
	return folio_test_swapbacked(folio) &&
			test_bit(PG_swapcache, folio_flags(folio, 0));
}

static __always_inline bool PageSwapCache(struct page *page)
{
	return folio_test_swapcache(page_folio(page));
}

SETPAGEFLAG(SwapCache, swapcache, PF_NO_TAIL)
CLEARPAGEFLAG(SwapCache, swapcache, PF_NO_TAIL)
#else
PAGEFLAG_FALSE(SwapCache, swapcache)
#endif

PAGEFLAG(Unevictable, unevictable, PF_HEAD)
	__CLEARPAGEFLAG(Unevictable, unevictable, PF_HEAD)
	TESTCLEARFLAG(Unevictable, unevictable, PF_HEAD)

#ifdef CONFIG_MMU
PAGEFLAG(Mlocked, mlocked, PF_NO_TAIL)
	__CLEARPAGEFLAG(Mlocked, mlocked, PF_NO_TAIL)
	TESTSCFLAG(Mlocked, mlocked, PF_NO_TAIL)
#else
PAGEFLAG_FALSE(Mlocked, mlocked) __CLEARPAGEFLAG_NOOP(Mlocked, mlocked)
	TESTSCFLAG_FALSE(Mlocked, mlocked)
#endif

#ifdef CONFIG_ARCH_USES_PG_UNCACHED
PAGEFLAG(Uncached, uncached, PF_NO_COMPOUND)
#else
PAGEFLAG_FALSE(Uncached, uncached)
#endif

#ifdef CONFIG_MEMORY_FAILURE
PAGEFLAG(HWPoison, hwpoison, PF_ANY)
TESTSCFLAG(HWPoison, hwpoison, PF_ANY)
#define __PG_HWPOISON (1UL << PG_hwpoison)
#define MAGIC_HWPOISON	0x48575053U	
extern void SetPageHWPoisonTakenOff(struct page *page);
extern void ClearPageHWPoisonTakenOff(struct page *page);
extern bool take_page_off_buddy(struct page *page);
extern bool put_page_back_buddy(struct page *page);
#else
PAGEFLAG_FALSE(HWPoison, hwpoison)
#define __PG_HWPOISON 0
#endif

#if defined(CONFIG_PAGE_IDLE_FLAG) && defined(CONFIG_64BIT)
TESTPAGEFLAG(Young, young, PF_ANY)
SETPAGEFLAG(Young, young, PF_ANY)
TESTCLEARFLAG(Young, young, PF_ANY)
PAGEFLAG(Idle, idle, PF_ANY)
#endif


__PAGEFLAG(Reported, reported, PF_NO_COMPOUND)

#ifdef CONFIG_MEMORY_HOTPLUG
PAGEFLAG(VmemmapSelfHosted, vmemmap_self_hosted, PF_ANY)
#else
PAGEFLAG_FALSE(VmemmapSelfHosted, vmemmap_self_hosted)
#endif


#define PAGE_MAPPING_ANON	0x1
#define PAGE_MAPPING_MOVABLE	0x2
#define PAGE_MAPPING_KSM	(PAGE_MAPPING_ANON | PAGE_MAPPING_MOVABLE)
#define PAGE_MAPPING_FLAGS	(PAGE_MAPPING_ANON | PAGE_MAPPING_MOVABLE)


#define PAGE_MAPPING_DAX_SHARED	((void *)0x1)

static __always_inline bool folio_mapping_flags(struct folio *folio)
{
	return ((unsigned long)folio->mapping & PAGE_MAPPING_FLAGS) != 0;
}

static __always_inline int PageMappingFlags(struct page *page)
{
	return ((unsigned long)page->mapping & PAGE_MAPPING_FLAGS) != 0;
}

static __always_inline bool folio_test_anon(struct folio *folio)
{
	return ((unsigned long)folio->mapping & PAGE_MAPPING_ANON) != 0;
}

static __always_inline bool PageAnon(struct page *page)
{
	return folio_test_anon(page_folio(page));
}

static __always_inline bool __folio_test_movable(const struct folio *folio)
{
	return ((unsigned long)folio->mapping & PAGE_MAPPING_FLAGS) ==
			PAGE_MAPPING_MOVABLE;
}

static __always_inline int __PageMovable(struct page *page)
{
	return ((unsigned long)page->mapping & PAGE_MAPPING_FLAGS) ==
				PAGE_MAPPING_MOVABLE;
}

#ifdef CONFIG_KSM

static __always_inline bool folio_test_ksm(struct folio *folio)
{
	return ((unsigned long)folio->mapping & PAGE_MAPPING_FLAGS) ==
				PAGE_MAPPING_KSM;
}

static __always_inline bool PageKsm(struct page *page)
{
	return folio_test_ksm(page_folio(page));
}
#else
TESTPAGEFLAG_FALSE(Ksm, ksm)
#endif

u64 stable_page_flags(struct page *page);


static inline bool folio_test_uptodate(struct folio *folio)
{
	bool ret = test_bit(PG_uptodate, folio_flags(folio, 0));
	
	if (ret)
		smp_rmb();

	return ret;
}

static inline int PageUptodate(struct page *page)
{
	return folio_test_uptodate(page_folio(page));
}

static __always_inline void __folio_mark_uptodate(struct folio *folio)
{
	smp_wmb();
	__set_bit(PG_uptodate, folio_flags(folio, 0));
}

static __always_inline void folio_mark_uptodate(struct folio *folio)
{
	
	smp_wmb();
	set_bit(PG_uptodate, folio_flags(folio, 0));
}

static __always_inline void __SetPageUptodate(struct page *page)
{
	__folio_mark_uptodate((struct folio *)page);
}

static __always_inline void SetPageUptodate(struct page *page)
{
	folio_mark_uptodate((struct folio *)page);
}

CLEARPAGEFLAG(Uptodate, uptodate, PF_NO_TAIL)

bool __folio_start_writeback(struct folio *folio, bool keep_write);
bool set_page_writeback(struct page *page);

#define folio_start_writeback(folio)			\
	__folio_start_writeback(folio, false)
#define folio_start_writeback_keepwrite(folio)	\
	__folio_start_writeback(folio, true)

static inline bool test_set_page_writeback(struct page *page)
{
	return set_page_writeback(page);
}

static __always_inline bool folio_test_head(struct folio *folio)
{
	return test_bit(PG_head, folio_flags(folio, FOLIO_PF_ANY));
}

static __always_inline int PageHead(struct page *page)
{
	PF_POISONED_CHECK(page);
	return test_bit(PG_head, &page->flags) && !page_is_fake_head(page);
}

__SETPAGEFLAG(Head, head, PF_ANY)
__CLEARPAGEFLAG(Head, head, PF_ANY)
CLEARPAGEFLAG(Head, head, PF_ANY)


static inline bool folio_test_large(struct folio *folio)
{
	return folio_test_head(folio);
}

static __always_inline void set_compound_head(struct page *page, struct page *head)
{
	WRITE_ONCE(page->compound_head, (unsigned long)head + 1);
}

static __always_inline void clear_compound_head(struct page *page)
{
	WRITE_ONCE(page->compound_head, 0);
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static inline void ClearPageCompound(struct page *page)
{
	BUG_ON(!PageHead(page));
	ClearPageHead(page);
}
PAGEFLAG(LargeRmappable, large_rmappable, PF_SECOND)
#else
TESTPAGEFLAG_FALSE(LargeRmappable, large_rmappable)
#endif

#define PG_head_mask ((1UL << PG_head))

#ifdef CONFIG_TRANSPARENT_HUGEPAGE

static inline int PageTransHuge(struct page *page)
{
	VM_BUG_ON_PAGE(PageTail(page), page);
	return PageHead(page);
}


static inline int PageTransCompound(struct page *page)
{
	return PageCompound(page);
}


static inline int PageTransTail(struct page *page)
{
	return PageTail(page);
}
#else
TESTPAGEFLAG_FALSE(TransHuge, transhuge)
TESTPAGEFLAG_FALSE(TransCompound, transcompound)
TESTPAGEFLAG_FALSE(TransCompoundMap, transcompoundmap)
TESTPAGEFLAG_FALSE(TransTail, transtail)
#endif

#if defined(CONFIG_MEMORY_FAILURE) && defined(CONFIG_TRANSPARENT_HUGEPAGE)

PAGEFLAG(HasHWPoisoned, has_hwpoisoned, PF_SECOND)
	TESTSCFLAG(HasHWPoisoned, has_hwpoisoned, PF_SECOND)
#else
PAGEFLAG_FALSE(HasHWPoisoned, has_hwpoisoned)
	TESTSCFLAG_FALSE(HasHWPoisoned, has_hwpoisoned)
#endif



#define PAGE_TYPE_BASE	0xf0000000

#define PAGE_MAPCOUNT_RESERVE	-128
#define PG_buddy	0x00000080
#define PG_offline	0x00000100
#define PG_table	0x00000200
#define PG_guard	0x00000400
#define PG_hugetlb	0x00000800

#define PageType(page, flag)						\
	((page->page_type & (PAGE_TYPE_BASE | flag)) == PAGE_TYPE_BASE)
#define folio_test_type(folio, flag)					\
	((folio->page.page_type & (PAGE_TYPE_BASE | flag)) == PAGE_TYPE_BASE)

static inline int page_type_has_type(unsigned int page_type)
{
	return (int)page_type < PAGE_MAPCOUNT_RESERVE;
}

static inline int page_has_type(struct page *page)
{
	return page_type_has_type(page->page_type);
}

#define FOLIO_TYPE_OPS(lname, fname)					\
static __always_inline bool folio_test_##fname(const struct folio *folio)\
{									\
	return folio_test_type(folio, PG_##lname);			\
}									\
static __always_inline void __folio_set_##fname(struct folio *folio)	\
{									\
	VM_BUG_ON_FOLIO(!folio_test_type(folio, 0), folio);		\
	folio->page.page_type &= ~PG_##lname;				\
}									\
static __always_inline void __folio_clear_##fname(struct folio *folio)	\
{									\
	VM_BUG_ON_FOLIO(!folio_test_##fname(folio), folio);		\
	folio->page.page_type |= PG_##lname;				\
}

#define PAGE_TYPE_OPS(uname, lname, fname)				\
FOLIO_TYPE_OPS(lname, fname)						\
static __always_inline int Page##uname(const struct page *page)		\
{									\
	return PageType(page, PG_##lname);				\
}									\
static __always_inline void __SetPage##uname(struct page *page)		\
{									\
	VM_BUG_ON_PAGE(!PageType(page, 0), page);			\
	page->page_type &= ~PG_##lname;					\
}									\
static __always_inline void __ClearPage##uname(struct page *page)	\
{									\
	VM_BUG_ON_PAGE(!Page##uname(page), page);			\
	page->page_type |= PG_##lname;					\
}


PAGE_TYPE_OPS(Buddy, buddy, buddy)


PAGE_TYPE_OPS(Offline, offline, offline)

extern void page_offline_freeze(void);
extern void page_offline_thaw(void);
extern void page_offline_begin(void);
extern void page_offline_end(void);


PAGE_TYPE_OPS(Table, table, pgtable)


PAGE_TYPE_OPS(Guard, guard, guard)

#ifdef CONFIG_HUGETLB_PAGE
FOLIO_TYPE_OPS(hugetlb, hugetlb)
#else
FOLIO_TEST_FLAG_FALSE(hugetlb)
#endif


static inline bool PageHuge(const struct page *page)
{
	return folio_test_hugetlb(page_folio(page));
}


static inline bool is_page_hwpoison(struct page *page)
{
	if (PageHWPoison(page))
		return true;
	return PageHuge(page) && PageHWPoison(compound_head(page));
}

extern bool is_free_buddy_page(struct page *page);

PAGEFLAG(Isolated, isolated, PF_ANY);

static __always_inline int PageAnonExclusive(struct page *page)
{
	VM_BUG_ON_PGFLAGS(!PageAnon(page), page);
	VM_BUG_ON_PGFLAGS(PageHuge(page) && !PageHead(page), page);
	return test_bit(PG_anon_exclusive, &PF_ANY(page, 1)->flags);
}

static __always_inline void SetPageAnonExclusive(struct page *page)
{
	VM_BUG_ON_PGFLAGS(!PageAnon(page) || PageKsm(page), page);
	VM_BUG_ON_PGFLAGS(PageHuge(page) && !PageHead(page), page);
	set_bit(PG_anon_exclusive, &PF_ANY(page, 1)->flags);
}

static __always_inline void ClearPageAnonExclusive(struct page *page)
{
	VM_BUG_ON_PGFLAGS(!PageAnon(page) || PageKsm(page), page);
	VM_BUG_ON_PGFLAGS(PageHuge(page) && !PageHead(page), page);
	clear_bit(PG_anon_exclusive, &PF_ANY(page, 1)->flags);
}

static __always_inline void __ClearPageAnonExclusive(struct page *page)
{
	VM_BUG_ON_PGFLAGS(!PageAnon(page), page);
	VM_BUG_ON_PGFLAGS(PageHuge(page) && !PageHead(page), page);
	__clear_bit(PG_anon_exclusive, &PF_ANY(page, 1)->flags);
}

#ifdef CONFIG_MMU
#define __PG_MLOCKED		(1UL << PG_mlocked)
#else
#define __PG_MLOCKED		0
#endif


#define PAGE_FLAGS_CHECK_AT_FREE				\
	(1UL << PG_lru		| 1UL << PG_locked	|	\
	 1UL << PG_private	| 1UL << PG_private_2	|	\
	 1UL << PG_writeback	| 1UL << PG_reserved	|	\
	 1UL << PG_slab		| 1UL << PG_active 	|	\
	 1UL << PG_unevictable	| __PG_MLOCKED | LRU_GEN_MASK)


#define PAGE_FLAGS_CHECK_AT_PREP	\
	((PAGEFLAGS_MASK & ~__PG_HWPOISON) | LRU_GEN_MASK | LRU_REFS_MASK)


#define PAGE_FLAGS_SECOND						\
	(0xffUL 		| 1UL << PG_has_hwpoisoned |	\
	 1UL << PG_large_rmappable)

#define PAGE_FLAGS_PRIVATE				\
	(1UL << PG_private | 1UL << PG_private_2)

static inline int page_has_private(struct page *page)
{
	return !!(page->flags & PAGE_FLAGS_PRIVATE);
}

static inline bool folio_has_private(struct folio *folio)
{
	return page_has_private(&folio->page);
}

#undef PF_ANY
#undef PF_HEAD
#undef PF_ONLY_HEAD
#undef PF_NO_TAIL
#undef PF_NO_COMPOUND
#undef PF_SECOND
#endif 

#endif	
