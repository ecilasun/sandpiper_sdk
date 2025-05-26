/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PGTABLE_H
#define _LINUX_PGTABLE_H

#include <linux/pfn.h>
#include <asm/pgtable.h>

#define PMD_ORDER	(PMD_SHIFT - PAGE_SHIFT)
#define PUD_ORDER	(PUD_SHIFT - PAGE_SHIFT)

#ifndef __ASSEMBLY__
#ifdef CONFIG_MMU

#include <linux/mm_types.h>
#include <linux/bug.h>
#include <linux/errno.h>
#include <asm-generic/pgtable_uffd.h>
#include <linux/page_table_check.h>

#if 5 - defined(__PAGETABLE_P4D_FOLDED) - defined(__PAGETABLE_PUD_FOLDED) - \
	defined(__PAGETABLE_PMD_FOLDED) != CONFIG_PGTABLE_LEVELS
#error CONFIG_PGTABLE_LEVELS is not consistent with __PAGETABLE_{P4D,PUD,PMD}_FOLDED
#endif


#ifndef USER_PGTABLES_CEILING
#define USER_PGTABLES_CEILING	0UL
#endif


#ifndef FIRST_USER_ADDRESS
#define FIRST_USER_ADDRESS	0UL
#endif


#ifndef pmd_pgtable
#define pmd_pgtable(pmd) pmd_page(pmd)
#endif



static inline unsigned long pte_index(unsigned long address)
{
	return (address >> PAGE_SHIFT) & (PTRS_PER_PTE - 1);
}

#ifndef pmd_index
static inline unsigned long pmd_index(unsigned long address)
{
	return (address >> PMD_SHIFT) & (PTRS_PER_PMD - 1);
}
#define pmd_index pmd_index
#endif

#ifndef pud_index
static inline unsigned long pud_index(unsigned long address)
{
	return (address >> PUD_SHIFT) & (PTRS_PER_PUD - 1);
}
#define pud_index pud_index
#endif

#ifndef pgd_index

#define pgd_index(a)  (((a) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#endif

#ifndef pte_offset_kernel
static inline pte_t *pte_offset_kernel(pmd_t *pmd, unsigned long address)
{
	return (pte_t *)pmd_page_vaddr(*pmd) + pte_index(address);
}
#define pte_offset_kernel pte_offset_kernel
#endif

#ifdef CONFIG_HIGHPTE
#define __pte_map(pmd, address) \
	((pte_t *)kmap_local_page(pmd_page(*(pmd))) + pte_index((address)))
#define pte_unmap(pte)	do {	\
	kunmap_local((pte));	\
	rcu_read_unlock();	\
} while (0)
#else
static inline pte_t *__pte_map(pmd_t *pmd, unsigned long address)
{
	return pte_offset_kernel(pmd, address);
}
static inline void pte_unmap(pte_t *pte)
{
	rcu_read_unlock();
}
#endif

void pte_free_defer(struct mm_struct *mm, pgtable_t pgtable);


#ifndef pmd_offset
static inline pmd_t *pmd_offset(pud_t *pud, unsigned long address)
{
	return pud_pgtable(*pud) + pmd_index(address);
}
#define pmd_offset pmd_offset
#endif

#ifndef pud_offset
static inline pud_t *pud_offset(p4d_t *p4d, unsigned long address)
{
	return p4d_pgtable(*p4d) + pud_index(address);
}
#define pud_offset pud_offset
#endif

static inline pgd_t *pgd_offset_pgd(pgd_t *pgd, unsigned long address)
{
	return (pgd + pgd_index(address));
};


#ifndef pgd_offset
#define pgd_offset(mm, address)		pgd_offset_pgd((mm)->pgd, (address))
#endif


#ifndef pgd_offset_k
#define pgd_offset_k(address)		pgd_offset(&init_mm, (address))
#endif


static inline pmd_t *pmd_off(struct mm_struct *mm, unsigned long va)
{
	return pmd_offset(pud_offset(p4d_offset(pgd_offset(mm, va), va), va), va);
}

static inline pmd_t *pmd_off_k(unsigned long va)
{
	return pmd_offset(pud_offset(p4d_offset(pgd_offset_k(va), va), va), va);
}

static inline pte_t *virt_to_kpte(unsigned long vaddr)
{
	pmd_t *pmd = pmd_off_k(vaddr);

	return pmd_none(*pmd) ? NULL : pte_offset_kernel(pmd, vaddr);
}

#ifndef pmd_young
static inline int pmd_young(pmd_t pmd)
{
	return 0;
}
#endif


#ifndef __HAVE_ARCH_ENTER_LAZY_MMU_MODE
#define arch_enter_lazy_mmu_mode()	do {} while (0)
#define arch_leave_lazy_mmu_mode()	do {} while (0)
#define arch_flush_lazy_mmu_mode()	do {} while (0)
#endif

#ifndef set_ptes

#ifndef pte_next_pfn
static inline pte_t pte_next_pfn(pte_t pte)
{
	return __pte(pte_val(pte) + (1UL << PFN_PTE_SHIFT));
}
#endif


static inline void set_ptes(struct mm_struct *mm, unsigned long addr,
		pte_t *ptep, pte_t pte, unsigned int nr)
{
	page_table_check_ptes_set(mm, ptep, pte, nr);

	arch_enter_lazy_mmu_mode();
	for (;;) {
		set_pte(ptep, pte);
		if (--nr == 0)
			break;
		ptep++;
		pte = pte_next_pfn(pte);
	}
	arch_leave_lazy_mmu_mode();
}
#endif
#define set_pte_at(mm, addr, ptep, pte) set_ptes(mm, addr, ptep, pte, 1)

#ifndef __HAVE_ARCH_PTEP_SET_ACCESS_FLAGS
extern int ptep_set_access_flags(struct vm_area_struct *vma,
				 unsigned long address, pte_t *ptep,
				 pte_t entry, int dirty);
#endif

#ifndef __HAVE_ARCH_PMDP_SET_ACCESS_FLAGS
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
extern int pmdp_set_access_flags(struct vm_area_struct *vma,
				 unsigned long address, pmd_t *pmdp,
				 pmd_t entry, int dirty);
extern int pudp_set_access_flags(struct vm_area_struct *vma,
				 unsigned long address, pud_t *pudp,
				 pud_t entry, int dirty);
#else
static inline int pmdp_set_access_flags(struct vm_area_struct *vma,
					unsigned long address, pmd_t *pmdp,
					pmd_t entry, int dirty)
{
	BUILD_BUG();
	return 0;
}
static inline int pudp_set_access_flags(struct vm_area_struct *vma,
					unsigned long address, pud_t *pudp,
					pud_t entry, int dirty)
{
	BUILD_BUG();
	return 0;
}
#endif 
#endif

#ifndef ptep_get
static inline pte_t ptep_get(pte_t *ptep)
{
	return READ_ONCE(*ptep);
}
#endif

#ifndef pmdp_get
static inline pmd_t pmdp_get(pmd_t *pmdp)
{
	return READ_ONCE(*pmdp);
}
#endif

#ifndef __HAVE_ARCH_PTEP_TEST_AND_CLEAR_YOUNG
static inline int ptep_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long address,
					    pte_t *ptep)
{
	pte_t pte = ptep_get(ptep);
	int r = 1;
	if (!pte_young(pte))
		r = 0;
	else
		set_pte_at(vma->vm_mm, address, ptep, pte_mkold(pte));
	return r;
}
#endif

#ifndef __HAVE_ARCH_PMDP_TEST_AND_CLEAR_YOUNG
#if defined(CONFIG_TRANSPARENT_HUGEPAGE) || defined(CONFIG_ARCH_HAS_NONLEAF_PMD_YOUNG)
static inline int pmdp_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long address,
					    pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;
	int r = 1;
	if (!pmd_young(pmd))
		r = 0;
	else
		set_pmd_at(vma->vm_mm, address, pmdp, pmd_mkold(pmd));
	return r;
}
#else
static inline int pmdp_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long address,
					    pmd_t *pmdp)
{
	BUILD_BUG();
	return 0;
}
#endif 
#endif

#ifndef __HAVE_ARCH_PTEP_CLEAR_YOUNG_FLUSH
int ptep_clear_flush_young(struct vm_area_struct *vma,
			   unsigned long address, pte_t *ptep);
#endif

#ifndef __HAVE_ARCH_PMDP_CLEAR_YOUNG_FLUSH
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
extern int pmdp_clear_flush_young(struct vm_area_struct *vma,
				  unsigned long address, pmd_t *pmdp);
#else

static inline int pmdp_clear_flush_young(struct vm_area_struct *vma,
					 unsigned long address, pmd_t *pmdp)
{
	BUILD_BUG();
	return 0;
}
#endif 
#endif

#ifndef arch_has_hw_nonleaf_pmd_young

static inline bool arch_has_hw_nonleaf_pmd_young(void)
{
	return IS_ENABLED(CONFIG_ARCH_HAS_NONLEAF_PMD_YOUNG);
}
#endif

#ifndef arch_has_hw_pte_young

static inline bool arch_has_hw_pte_young(void)
{
	return false;
}
#endif

#ifndef arch_check_zapped_pte
static inline void arch_check_zapped_pte(struct vm_area_struct *vma,
					 pte_t pte)
{
}
#endif

#ifndef arch_check_zapped_pmd
static inline void arch_check_zapped_pmd(struct vm_area_struct *vma,
					 pmd_t pmd)
{
}
#endif

#ifndef __HAVE_ARCH_PTEP_GET_AND_CLEAR
static inline pte_t ptep_get_and_clear(struct mm_struct *mm,
				       unsigned long address,
				       pte_t *ptep)
{
	pte_t pte = ptep_get(ptep);
	pte_clear(mm, address, ptep);
	page_table_check_pte_clear(mm, pte);
	return pte;
}
#endif

static inline void ptep_clear(struct mm_struct *mm, unsigned long addr,
			      pte_t *ptep)
{
	ptep_get_and_clear(mm, addr, ptep);
}

#ifdef CONFIG_GUP_GET_PXX_LOW_HIGH

static inline pte_t ptep_get_lockless(pte_t *ptep)
{
	pte_t pte;

	do {
		pte.pte_low = ptep->pte_low;
		smp_rmb();
		pte.pte_high = ptep->pte_high;
		smp_rmb();
	} while (unlikely(pte.pte_low != ptep->pte_low));

	return pte;
}
#define ptep_get_lockless ptep_get_lockless

#if CONFIG_PGTABLE_LEVELS > 2
static inline pmd_t pmdp_get_lockless(pmd_t *pmdp)
{
	pmd_t pmd;

	do {
		pmd.pmd_low = pmdp->pmd_low;
		smp_rmb();
		pmd.pmd_high = pmdp->pmd_high;
		smp_rmb();
	} while (unlikely(pmd.pmd_low != pmdp->pmd_low));

	return pmd;
}
#define pmdp_get_lockless pmdp_get_lockless
#define pmdp_get_lockless_sync() tlb_remove_table_sync_one()
#endif 
#endif 


#ifndef ptep_get_lockless
static inline pte_t ptep_get_lockless(pte_t *ptep)
{
	return ptep_get(ptep);
}
#endif

#ifndef pmdp_get_lockless
static inline pmd_t pmdp_get_lockless(pmd_t *pmdp)
{
	return pmdp_get(pmdp);
}
static inline void pmdp_get_lockless_sync(void)
{
}
#endif

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
#ifndef __HAVE_ARCH_PMDP_HUGE_GET_AND_CLEAR
static inline pmd_t pmdp_huge_get_and_clear(struct mm_struct *mm,
					    unsigned long address,
					    pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;

	pmd_clear(pmdp);
	page_table_check_pmd_clear(mm, pmd);

	return pmd;
}
#endif 
#ifndef __HAVE_ARCH_PUDP_HUGE_GET_AND_CLEAR
static inline pud_t pudp_huge_get_and_clear(struct mm_struct *mm,
					    unsigned long address,
					    pud_t *pudp)
{
	pud_t pud = *pudp;

	pud_clear(pudp);
	page_table_check_pud_clear(mm, pud);

	return pud;
}
#endif 
#endif 

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
#ifndef __HAVE_ARCH_PMDP_HUGE_GET_AND_CLEAR_FULL
static inline pmd_t pmdp_huge_get_and_clear_full(struct vm_area_struct *vma,
					    unsigned long address, pmd_t *pmdp,
					    int full)
{
	return pmdp_huge_get_and_clear(vma->vm_mm, address, pmdp);
}
#endif

#ifndef __HAVE_ARCH_PUDP_HUGE_GET_AND_CLEAR_FULL
static inline pud_t pudp_huge_get_and_clear_full(struct vm_area_struct *vma,
					    unsigned long address, pud_t *pudp,
					    int full)
{
	return pudp_huge_get_and_clear(vma->vm_mm, address, pudp);
}
#endif
#endif 

#ifndef __HAVE_ARCH_PTEP_GET_AND_CLEAR_FULL
static inline pte_t ptep_get_and_clear_full(struct mm_struct *mm,
					    unsigned long address, pte_t *ptep,
					    int full)
{
	return ptep_get_and_clear(mm, address, ptep);
}
#endif



#ifndef __HAVE_ARCH_UPDATE_MMU_TLB
static inline void update_mmu_tlb(struct vm_area_struct *vma,
				unsigned long address, pte_t *ptep)
{
}
#define __HAVE_ARCH_UPDATE_MMU_TLB
#endif


#ifndef __HAVE_ARCH_PTE_CLEAR_NOT_PRESENT_FULL
static inline void pte_clear_not_present_full(struct mm_struct *mm,
					      unsigned long address,
					      pte_t *ptep,
					      int full)
{
	pte_clear(mm, address, ptep);
}
#endif

#ifndef __HAVE_ARCH_PTEP_CLEAR_FLUSH
extern pte_t ptep_clear_flush(struct vm_area_struct *vma,
			      unsigned long address,
			      pte_t *ptep);
#endif

#ifndef __HAVE_ARCH_PMDP_HUGE_CLEAR_FLUSH
extern pmd_t pmdp_huge_clear_flush(struct vm_area_struct *vma,
			      unsigned long address,
			      pmd_t *pmdp);
extern pud_t pudp_huge_clear_flush(struct vm_area_struct *vma,
			      unsigned long address,
			      pud_t *pudp);
#endif

#ifndef pte_mkwrite
static inline pte_t pte_mkwrite(pte_t pte, struct vm_area_struct *vma)
{
	return pte_mkwrite_novma(pte);
}
#endif

#if defined(CONFIG_ARCH_WANT_PMD_MKWRITE) && !defined(pmd_mkwrite)
static inline pmd_t pmd_mkwrite(pmd_t pmd, struct vm_area_struct *vma)
{
	return pmd_mkwrite_novma(pmd);
}
#endif

#ifndef __HAVE_ARCH_PTEP_SET_WRPROTECT
struct mm_struct;
static inline void ptep_set_wrprotect(struct mm_struct *mm, unsigned long address, pte_t *ptep)
{
	pte_t old_pte = ptep_get(ptep);
	set_pte_at(mm, address, ptep, pte_wrprotect(old_pte));
}
#endif


#ifndef pte_sw_mkyoung
static inline pte_t pte_sw_mkyoung(pte_t pte)
{
	return pte;
}
#define pte_sw_mkyoung	pte_sw_mkyoung
#endif

#ifndef __HAVE_ARCH_PMDP_SET_WRPROTECT
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static inline void pmdp_set_wrprotect(struct mm_struct *mm,
				      unsigned long address, pmd_t *pmdp)
{
	pmd_t old_pmd = *pmdp;
	set_pmd_at(mm, address, pmdp, pmd_wrprotect(old_pmd));
}
#else
static inline void pmdp_set_wrprotect(struct mm_struct *mm,
				      unsigned long address, pmd_t *pmdp)
{
	BUILD_BUG();
}
#endif 
#endif
#ifndef __HAVE_ARCH_PUDP_SET_WRPROTECT
#ifdef CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static inline void pudp_set_wrprotect(struct mm_struct *mm,
				      unsigned long address, pud_t *pudp)
{
	pud_t old_pud = *pudp;

	set_pud_at(mm, address, pudp, pud_wrprotect(old_pud));
}
#else
static inline void pudp_set_wrprotect(struct mm_struct *mm,
				      unsigned long address, pud_t *pudp)
{
	BUILD_BUG();
}
#endif 
#endif 
#endif

#ifndef pmdp_collapse_flush
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
extern pmd_t pmdp_collapse_flush(struct vm_area_struct *vma,
				 unsigned long address, pmd_t *pmdp);
#else
static inline pmd_t pmdp_collapse_flush(struct vm_area_struct *vma,
					unsigned long address,
					pmd_t *pmdp)
{
	BUILD_BUG();
	return *pmdp;
}
#define pmdp_collapse_flush pmdp_collapse_flush
#endif 
#endif

#ifndef __HAVE_ARCH_PGTABLE_DEPOSIT
extern void pgtable_trans_huge_deposit(struct mm_struct *mm, pmd_t *pmdp,
				       pgtable_t pgtable);
#endif

#ifndef __HAVE_ARCH_PGTABLE_WITHDRAW
extern pgtable_t pgtable_trans_huge_withdraw(struct mm_struct *mm, pmd_t *pmdp);
#endif

#ifndef arch_needs_pgtable_deposit
#define arch_needs_pgtable_deposit() (false)
#endif

#ifdef CONFIG_TRANSPARENT_HUGEPAGE

static inline pmd_t generic_pmdp_establish(struct vm_area_struct *vma,
		unsigned long address, pmd_t *pmdp, pmd_t pmd)
{
	pmd_t old_pmd = *pmdp;
	set_pmd_at(vma->vm_mm, address, pmdp, pmd);
	return old_pmd;
}
#endif

#ifndef __HAVE_ARCH_PMDP_INVALIDATE
extern pmd_t pmdp_invalidate(struct vm_area_struct *vma, unsigned long address,
			    pmd_t *pmdp);
#endif

#ifndef __HAVE_ARCH_PMDP_INVALIDATE_AD


extern pmd_t pmdp_invalidate_ad(struct vm_area_struct *vma,
				unsigned long address, pmd_t *pmdp);
#endif

#ifndef __HAVE_ARCH_PTE_SAME
static inline int pte_same(pte_t pte_a, pte_t pte_b)
{
	return pte_val(pte_a) == pte_val(pte_b);
}
#endif

#ifndef __HAVE_ARCH_PTE_UNUSED

static inline int pte_unused(pte_t pte)
{
	return 0;
}
#endif

#ifndef pte_access_permitted
#define pte_access_permitted(pte, write) \
	(pte_present(pte) && (!(write) || pte_write(pte)))
#endif

#ifndef pmd_access_permitted
#define pmd_access_permitted(pmd, write) \
	(pmd_present(pmd) && (!(write) || pmd_write(pmd)))
#endif

#ifndef pud_access_permitted
#define pud_access_permitted(pud, write) \
	(pud_present(pud) && (!(write) || pud_write(pud)))
#endif

#ifndef p4d_access_permitted
#define p4d_access_permitted(p4d, write) \
	(p4d_present(p4d) && (!(write) || p4d_write(p4d)))
#endif

#ifndef pgd_access_permitted
#define pgd_access_permitted(pgd, write) \
	(pgd_present(pgd) && (!(write) || pgd_write(pgd)))
#endif

#ifndef __HAVE_ARCH_PMD_SAME
static inline int pmd_same(pmd_t pmd_a, pmd_t pmd_b)
{
	return pmd_val(pmd_a) == pmd_val(pmd_b);
}
#endif

#ifndef pud_same
static inline int pud_same(pud_t pud_a, pud_t pud_b)
{
	return pud_val(pud_a) == pud_val(pud_b);
}
#define pud_same pud_same
#endif

#ifndef __HAVE_ARCH_P4D_SAME
static inline int p4d_same(p4d_t p4d_a, p4d_t p4d_b)
{
	return p4d_val(p4d_a) == p4d_val(p4d_b);
}
#endif

#ifndef __HAVE_ARCH_PGD_SAME
static inline int pgd_same(pgd_t pgd_a, pgd_t pgd_b)
{
	return pgd_val(pgd_a) == pgd_val(pgd_b);
}
#endif


#define set_pte_safe(ptep, pte) \
({ \
	WARN_ON_ONCE(pte_present(*ptep) && !pte_same(*ptep, pte)); \
	set_pte(ptep, pte); \
})

#define set_pmd_safe(pmdp, pmd) \
({ \
	WARN_ON_ONCE(pmd_present(*pmdp) && !pmd_same(*pmdp, pmd)); \
	set_pmd(pmdp, pmd); \
})

#define set_pud_safe(pudp, pud) \
({ \
	WARN_ON_ONCE(pud_present(*pudp) && !pud_same(*pudp, pud)); \
	set_pud(pudp, pud); \
})

#define set_p4d_safe(p4dp, p4d) \
({ \
	WARN_ON_ONCE(p4d_present(*p4dp) && !p4d_same(*p4dp, p4d)); \
	set_p4d(p4dp, p4d); \
})

#define set_pgd_safe(pgdp, pgd) \
({ \
	WARN_ON_ONCE(pgd_present(*pgdp) && !pgd_same(*pgdp, pgd)); \
	set_pgd(pgdp, pgd); \
})

#ifndef __HAVE_ARCH_DO_SWAP_PAGE

static inline void arch_do_swap_page(struct mm_struct *mm,
				     struct vm_area_struct *vma,
				     unsigned long addr,
				     pte_t pte, pte_t oldpte)
{

}
#endif

#ifndef __HAVE_ARCH_UNMAP_ONE

static inline int arch_unmap_one(struct mm_struct *mm,
				  struct vm_area_struct *vma,
				  unsigned long addr,
				  pte_t orig_pte)
{
	return 0;
}
#endif


#ifndef __HAVE_ARCH_PREPARE_TO_SWAP
static inline int arch_prepare_to_swap(struct page *page)
{
	return 0;
}
#endif

#ifndef __HAVE_ARCH_SWAP_INVALIDATE
static inline void arch_swap_invalidate_page(int type, pgoff_t offset)
{
}

static inline void arch_swap_invalidate_area(int type)
{
}
#endif

#ifndef __HAVE_ARCH_SWAP_RESTORE
static inline void arch_swap_restore(swp_entry_t entry, struct folio *folio)
{
}
#endif

#ifndef __HAVE_ARCH_PGD_OFFSET_GATE
#define pgd_offset_gate(mm, addr)	pgd_offset(mm, addr)
#endif

#ifndef __HAVE_ARCH_MOVE_PTE
#define move_pte(pte, prot, old_addr, new_addr)	(pte)
#endif

#ifndef pte_accessible
# define pte_accessible(mm, pte)	((void)(pte), 1)
#endif

#ifndef flush_tlb_fix_spurious_fault
#define flush_tlb_fix_spurious_fault(vma, address, ptep) flush_tlb_page(vma, address)
#endif



#define pgd_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;	\
	(__boundary - 1 < (end) - 1)? __boundary: (end);		\
})

#ifndef p4d_addr_end
#define p4d_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + P4D_SIZE) & P4D_MASK;	\
	(__boundary - 1 < (end) - 1)? __boundary: (end);		\
})
#endif

#ifndef pud_addr_end
#define pud_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;	\
	(__boundary - 1 < (end) - 1)? __boundary: (end);		\
})
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)						\
({	unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;	\
	(__boundary - 1 < (end) - 1)? __boundary: (end);		\
})
#endif


void pgd_clear_bad(pgd_t *);

#ifndef __PAGETABLE_P4D_FOLDED
void p4d_clear_bad(p4d_t *);
#else
#define p4d_clear_bad(p4d)        do { } while (0)
#endif

#ifndef __PAGETABLE_PUD_FOLDED
void pud_clear_bad(pud_t *);
#else
#define pud_clear_bad(p4d)        do { } while (0)
#endif

void pmd_clear_bad(pmd_t *);

static inline int pgd_none_or_clear_bad(pgd_t *pgd)
{
	if (pgd_none(*pgd))
		return 1;
	if (unlikely(pgd_bad(*pgd))) {
		pgd_clear_bad(pgd);
		return 1;
	}
	return 0;
}

static inline int p4d_none_or_clear_bad(p4d_t *p4d)
{
	if (p4d_none(*p4d))
		return 1;
	if (unlikely(p4d_bad(*p4d))) {
		p4d_clear_bad(p4d);
		return 1;
	}
	return 0;
}

static inline int pud_none_or_clear_bad(pud_t *pud)
{
	if (pud_none(*pud))
		return 1;
	if (unlikely(pud_bad(*pud))) {
		pud_clear_bad(pud);
		return 1;
	}
	return 0;
}

static inline int pmd_none_or_clear_bad(pmd_t *pmd)
{
	if (pmd_none(*pmd))
		return 1;
	if (unlikely(pmd_bad(*pmd))) {
		pmd_clear_bad(pmd);
		return 1;
	}
	return 0;
}

static inline pte_t __ptep_modify_prot_start(struct vm_area_struct *vma,
					     unsigned long addr,
					     pte_t *ptep)
{
	
	return ptep_get_and_clear(vma->vm_mm, addr, ptep);
}

static inline void __ptep_modify_prot_commit(struct vm_area_struct *vma,
					     unsigned long addr,
					     pte_t *ptep, pte_t pte)
{
	
	set_pte_at(vma->vm_mm, addr, ptep, pte);
}

#ifndef __HAVE_ARCH_PTEP_MODIFY_PROT_TRANSACTION

static inline pte_t ptep_modify_prot_start(struct vm_area_struct *vma,
					   unsigned long addr,
					   pte_t *ptep)
{
	return __ptep_modify_prot_start(vma, addr, ptep);
}


static inline void ptep_modify_prot_commit(struct vm_area_struct *vma,
					   unsigned long addr,
					   pte_t *ptep, pte_t old_pte, pte_t pte)
{
	__ptep_modify_prot_commit(vma, addr, ptep, pte);
}
#endif 
#endif 



#ifndef pgprot_nx
#define pgprot_nx(prot)	(prot)
#endif

#ifndef pgprot_noncached
#define pgprot_noncached(prot)	(prot)
#endif

#ifndef pgprot_writecombine
#define pgprot_writecombine pgprot_noncached
#endif

#ifndef pgprot_writethrough
#define pgprot_writethrough pgprot_noncached
#endif

#ifndef pgprot_device
#define pgprot_device pgprot_noncached
#endif

#ifndef pgprot_mhp
#define pgprot_mhp(prot)	(prot)
#endif

#ifdef CONFIG_MMU
#ifndef pgprot_modify
#define pgprot_modify pgprot_modify
static inline pgprot_t pgprot_modify(pgprot_t oldprot, pgprot_t newprot)
{
	if (pgprot_val(oldprot) == pgprot_val(pgprot_noncached(oldprot)))
		newprot = pgprot_noncached(newprot);
	if (pgprot_val(oldprot) == pgprot_val(pgprot_writecombine(oldprot)))
		newprot = pgprot_writecombine(newprot);
	if (pgprot_val(oldprot) == pgprot_val(pgprot_device(oldprot)))
		newprot = pgprot_device(newprot);
	return newprot;
}
#endif
#endif 

#ifndef pgprot_encrypted
#define pgprot_encrypted(prot)	(prot)
#endif

#ifndef pgprot_decrypted
#define pgprot_decrypted(prot)	(prot)
#endif


#ifndef __HAVE_ARCH_START_CONTEXT_SWITCH
#define arch_start_context_switch(prev)	do {} while (0)
#endif

#ifdef CONFIG_HAVE_ARCH_SOFT_DIRTY
#ifndef CONFIG_ARCH_ENABLE_THP_MIGRATION
static inline pmd_t pmd_swp_mksoft_dirty(pmd_t pmd)
{
	return pmd;
}

static inline int pmd_swp_soft_dirty(pmd_t pmd)
{
	return 0;
}

static inline pmd_t pmd_swp_clear_soft_dirty(pmd_t pmd)
{
	return pmd;
}
#endif
#else 
static inline int pte_soft_dirty(pte_t pte)
{
	return 0;
}

static inline int pmd_soft_dirty(pmd_t pmd)
{
	return 0;
}

static inline pte_t pte_mksoft_dirty(pte_t pte)
{
	return pte;
}

static inline pmd_t pmd_mksoft_dirty(pmd_t pmd)
{
	return pmd;
}

static inline pte_t pte_clear_soft_dirty(pte_t pte)
{
	return pte;
}

static inline pmd_t pmd_clear_soft_dirty(pmd_t pmd)
{
	return pmd;
}

static inline pte_t pte_swp_mksoft_dirty(pte_t pte)
{
	return pte;
}

static inline int pte_swp_soft_dirty(pte_t pte)
{
	return 0;
}

static inline pte_t pte_swp_clear_soft_dirty(pte_t pte)
{
	return pte;
}

static inline pmd_t pmd_swp_mksoft_dirty(pmd_t pmd)
{
	return pmd;
}

static inline int pmd_swp_soft_dirty(pmd_t pmd)
{
	return 0;
}

static inline pmd_t pmd_swp_clear_soft_dirty(pmd_t pmd)
{
	return pmd;
}
#endif

#ifndef __HAVE_PFNMAP_TRACKING



static inline int track_pfn_remap(struct vm_area_struct *vma, pgprot_t *prot,
				  unsigned long pfn, unsigned long addr,
				  unsigned long size)
{
	return 0;
}


static inline void track_pfn_insert(struct vm_area_struct *vma, pgprot_t *prot,
				    pfn_t pfn)
{
}


static inline int track_pfn_copy(struct vm_area_struct *vma)
{
	return 0;
}


static inline void untrack_pfn(struct vm_area_struct *vma,
			       unsigned long pfn, unsigned long size,
			       bool mm_wr_locked)
{
}


static inline void untrack_pfn_clear(struct vm_area_struct *vma)
{
}
#else
extern int track_pfn_remap(struct vm_area_struct *vma, pgprot_t *prot,
			   unsigned long pfn, unsigned long addr,
			   unsigned long size);
extern void track_pfn_insert(struct vm_area_struct *vma, pgprot_t *prot,
			     pfn_t pfn);
extern int track_pfn_copy(struct vm_area_struct *vma);
extern void untrack_pfn(struct vm_area_struct *vma, unsigned long pfn,
			unsigned long size, bool mm_wr_locked);
extern void untrack_pfn_clear(struct vm_area_struct *vma);
#endif

#ifdef CONFIG_MMU
#ifdef __HAVE_COLOR_ZERO_PAGE
static inline int is_zero_pfn(unsigned long pfn)
{
	extern unsigned long zero_pfn;
	unsigned long offset_from_zero_pfn = pfn - zero_pfn;
	return offset_from_zero_pfn <= (zero_page_mask >> PAGE_SHIFT);
}

#define my_zero_pfn(addr)	page_to_pfn(ZERO_PAGE(addr))

#else
static inline int is_zero_pfn(unsigned long pfn)
{
	extern unsigned long zero_pfn;
	return pfn == zero_pfn;
}

static inline unsigned long my_zero_pfn(unsigned long addr)
{
	extern unsigned long zero_pfn;
	return zero_pfn;
}
#endif
#else
static inline int is_zero_pfn(unsigned long pfn)
{
	return 0;
}

static inline unsigned long my_zero_pfn(unsigned long addr)
{
	return 0;
}
#endif 

#ifdef CONFIG_MMU

#ifndef CONFIG_TRANSPARENT_HUGEPAGE
static inline int pmd_trans_huge(pmd_t pmd)
{
	return 0;
}
#ifndef pmd_write
static inline int pmd_write(pmd_t pmd)
{
	BUG();
	return 0;
}
#endif 
#endif 

#ifndef pud_write
static inline int pud_write(pud_t pud)
{
	BUG();
	return 0;
}
#endif 

#if !defined(CONFIG_ARCH_HAS_PTE_DEVMAP) || !defined(CONFIG_TRANSPARENT_HUGEPAGE)
static inline int pmd_devmap(pmd_t pmd)
{
	return 0;
}
static inline int pud_devmap(pud_t pud)
{
	return 0;
}
static inline int pgd_devmap(pgd_t pgd)
{
	return 0;
}
#endif

#if !defined(CONFIG_TRANSPARENT_HUGEPAGE) || \
	!defined(CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD)
static inline int pud_trans_huge(pud_t pud)
{
	return 0;
}
#endif

static inline int pud_trans_unstable(pud_t *pud)
{
#if defined(CONFIG_TRANSPARENT_HUGEPAGE) && \
	defined(CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD)
	pud_t pudval = READ_ONCE(*pud);

	if (pud_none(pudval) || pud_trans_huge(pudval) || pud_devmap(pudval))
		return 1;
	if (unlikely(pud_bad(pudval))) {
		pud_clear_bad(pud);
		return 1;
	}
#endif
	return 0;
}

#ifndef CONFIG_NUMA_BALANCING

static inline int pte_protnone(pte_t pte)
{
	return 0;
}

static inline int pmd_protnone(pmd_t pmd)
{
	return 0;
}
#endif 

#endif 

#ifdef CONFIG_HAVE_ARCH_HUGE_VMAP

#ifndef __PAGETABLE_P4D_FOLDED
int p4d_set_huge(p4d_t *p4d, phys_addr_t addr, pgprot_t prot);
void p4d_clear_huge(p4d_t *p4d);
#else
static inline int p4d_set_huge(p4d_t *p4d, phys_addr_t addr, pgprot_t prot)
{
	return 0;
}
static inline void p4d_clear_huge(p4d_t *p4d) { }
#endif 

int pud_set_huge(pud_t *pud, phys_addr_t addr, pgprot_t prot);
int pmd_set_huge(pmd_t *pmd, phys_addr_t addr, pgprot_t prot);
int pud_clear_huge(pud_t *pud);
int pmd_clear_huge(pmd_t *pmd);
int p4d_free_pud_page(p4d_t *p4d, unsigned long addr);
int pud_free_pmd_page(pud_t *pud, unsigned long addr);
int pmd_free_pte_page(pmd_t *pmd, unsigned long addr);
#else	
static inline int p4d_set_huge(p4d_t *p4d, phys_addr_t addr, pgprot_t prot)
{
	return 0;
}
static inline int pud_set_huge(pud_t *pud, phys_addr_t addr, pgprot_t prot)
{
	return 0;
}
static inline int pmd_set_huge(pmd_t *pmd, phys_addr_t addr, pgprot_t prot)
{
	return 0;
}
static inline void p4d_clear_huge(p4d_t *p4d) { }
static inline int pud_clear_huge(pud_t *pud)
{
	return 0;
}
static inline int pmd_clear_huge(pmd_t *pmd)
{
	return 0;
}
static inline int p4d_free_pud_page(p4d_t *p4d, unsigned long addr)
{
	return 0;
}
static inline int pud_free_pmd_page(pud_t *pud, unsigned long addr)
{
	return 0;
}
static inline int pmd_free_pte_page(pmd_t *pmd, unsigned long addr)
{
	return 0;
}
#endif	

#ifndef __HAVE_ARCH_FLUSH_PMD_TLB_RANGE
#ifdef CONFIG_TRANSPARENT_HUGEPAGE

#define flush_pmd_tlb_range(vma, addr, end)	flush_tlb_range(vma, addr, end)
#define flush_pud_tlb_range(vma, addr, end)	flush_tlb_range(vma, addr, end)
#else
#define flush_pmd_tlb_range(vma, addr, end)	BUILD_BUG()
#define flush_pud_tlb_range(vma, addr, end)	BUILD_BUG()
#endif
#endif

struct file;
int phys_mem_access_prot_allowed(struct file *file, unsigned long pfn,
			unsigned long size, pgprot_t *vma_prot);

#ifndef CONFIG_X86_ESPFIX64
static inline void init_espfix_bsp(void) { }
#endif

extern void __init pgtable_cache_init(void);

#ifndef __HAVE_ARCH_PFN_MODIFY_ALLOWED
static inline bool pfn_modify_allowed(unsigned long pfn, pgprot_t prot)
{
	return true;
}

static inline bool arch_has_pfn_modify_check(void)
{
	return false;
}
#endif 



#ifndef PAGE_KERNEL_RO
# define PAGE_KERNEL_RO PAGE_KERNEL
#endif

#ifndef PAGE_KERNEL_EXEC
# define PAGE_KERNEL_EXEC PAGE_KERNEL
#endif


#define		__PGTBL_PGD_MODIFIED	0
#define		__PGTBL_P4D_MODIFIED	1
#define		__PGTBL_PUD_MODIFIED	2
#define		__PGTBL_PMD_MODIFIED	3
#define		__PGTBL_PTE_MODIFIED	4

#define		PGTBL_PGD_MODIFIED	BIT(__PGTBL_PGD_MODIFIED)
#define		PGTBL_P4D_MODIFIED	BIT(__PGTBL_P4D_MODIFIED)
#define		PGTBL_PUD_MODIFIED	BIT(__PGTBL_PUD_MODIFIED)
#define		PGTBL_PMD_MODIFIED	BIT(__PGTBL_PMD_MODIFIED)
#define		PGTBL_PTE_MODIFIED	BIT(__PGTBL_PTE_MODIFIED)


typedef unsigned int pgtbl_mod_mask;

#endif 

#if !defined(MAX_POSSIBLE_PHYSMEM_BITS) && !defined(CONFIG_64BIT)
#ifdef CONFIG_PHYS_ADDR_T_64BIT

#error Missing MAX_POSSIBLE_PHYSMEM_BITS definition
#else
#define MAX_POSSIBLE_PHYSMEM_BITS 32
#endif
#endif

#ifndef has_transparent_hugepage
#define has_transparent_hugepage() IS_BUILTIN(CONFIG_TRANSPARENT_HUGEPAGE)
#endif

#ifndef has_transparent_pud_hugepage
#define has_transparent_pud_hugepage() IS_BUILTIN(CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD)
#endif

#ifndef mm_p4d_folded
#define mm_p4d_folded(mm)	__is_defined(__PAGETABLE_P4D_FOLDED)
#endif

#ifndef mm_pud_folded
#define mm_pud_folded(mm)	__is_defined(__PAGETABLE_PUD_FOLDED)
#endif

#ifndef mm_pmd_folded
#define mm_pmd_folded(mm)	__is_defined(__PAGETABLE_PMD_FOLDED)
#endif

#ifndef p4d_offset_lockless
#define p4d_offset_lockless(pgdp, pgd, address) p4d_offset(&(pgd), address)
#endif
#ifndef pud_offset_lockless
#define pud_offset_lockless(p4dp, p4d, address) pud_offset(&(p4d), address)
#endif
#ifndef pmd_offset_lockless
#define pmd_offset_lockless(pudp, pud, address) pmd_offset(&(pud), address)
#endif


#ifndef pgd_leaf
#define pgd_leaf(x)	0
#endif
#ifndef p4d_leaf
#define p4d_leaf(x)	0
#endif
#ifndef pud_leaf
#define pud_leaf(x)	0
#endif
#ifndef pmd_leaf
#define pmd_leaf(x)	0
#endif

#ifndef pgd_leaf_size
#define pgd_leaf_size(x) (1ULL << PGDIR_SHIFT)
#endif
#ifndef p4d_leaf_size
#define p4d_leaf_size(x) P4D_SIZE
#endif
#ifndef pud_leaf_size
#define pud_leaf_size(x) PUD_SIZE
#endif
#ifndef pmd_leaf_size
#define pmd_leaf_size(x) PMD_SIZE
#endif
#ifndef pte_leaf_size
#define pte_leaf_size(x) PAGE_SIZE
#endif



#ifndef MAX_PTRS_PER_PTE
#define MAX_PTRS_PER_PTE PTRS_PER_PTE
#endif

#ifndef MAX_PTRS_PER_PMD
#define MAX_PTRS_PER_PMD PTRS_PER_PMD
#endif

#ifndef MAX_PTRS_PER_PUD
#define MAX_PTRS_PER_PUD PTRS_PER_PUD
#endif

#ifndef MAX_PTRS_PER_P4D
#define MAX_PTRS_PER_P4D PTRS_PER_P4D
#endif


#define DECLARE_VM_GET_PAGE_PROT					\
pgprot_t vm_get_page_prot(unsigned long vm_flags)			\
{									\
		return protection_map[vm_flags &			\
			(VM_READ | VM_WRITE | VM_EXEC | VM_SHARED)];	\
}									\
EXPORT_SYMBOL(vm_get_page_prot);

#endif 
