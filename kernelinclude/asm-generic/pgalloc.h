/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_PGALLOC_H
#define __ASM_GENERIC_PGALLOC_H

#ifdef CONFIG_MMU

#define GFP_PGTABLE_KERNEL	(GFP_KERNEL | __GFP_ZERO)
#define GFP_PGTABLE_USER	(GFP_PGTABLE_KERNEL | __GFP_ACCOUNT)


static inline pte_t *__pte_alloc_one_kernel(struct mm_struct *mm)
{
	struct ptdesc *ptdesc = pagetable_alloc(GFP_PGTABLE_KERNEL &
			~__GFP_HIGHMEM, 0);

	if (!ptdesc)
		return NULL;
	return ptdesc_address(ptdesc);
}

#ifndef __HAVE_ARCH_PTE_ALLOC_ONE_KERNEL

static inline pte_t *pte_alloc_one_kernel(struct mm_struct *mm)
{
	return __pte_alloc_one_kernel(mm);
}
#endif


static inline void pte_free_kernel(struct mm_struct *mm, pte_t *pte)
{
	pagetable_free(virt_to_ptdesc(pte));
}


static inline pgtable_t __pte_alloc_one(struct mm_struct *mm, gfp_t gfp)
{
	struct ptdesc *ptdesc;

	ptdesc = pagetable_alloc(gfp, 0);
	if (!ptdesc)
		return NULL;
	if (!pagetable_pte_ctor(ptdesc)) {
		pagetable_free(ptdesc);
		return NULL;
	}

	return ptdesc_page(ptdesc);
}

#ifndef __HAVE_ARCH_PTE_ALLOC_ONE

static inline pgtable_t pte_alloc_one(struct mm_struct *mm)
{
	return __pte_alloc_one(mm, GFP_PGTABLE_USER);
}
#endif




static inline void pte_free(struct mm_struct *mm, struct page *pte_page)
{
	struct ptdesc *ptdesc = page_ptdesc(pte_page);

	pagetable_pte_dtor(ptdesc);
	pagetable_free(ptdesc);
}


#if CONFIG_PGTABLE_LEVELS > 2

#ifndef __HAVE_ARCH_PMD_ALLOC_ONE

static inline pmd_t *pmd_alloc_one(struct mm_struct *mm, unsigned long addr)
{
	struct ptdesc *ptdesc;
	gfp_t gfp = GFP_PGTABLE_USER;

	if (mm == &init_mm)
		gfp = GFP_PGTABLE_KERNEL;
	ptdesc = pagetable_alloc(gfp, 0);
	if (!ptdesc)
		return NULL;
	if (!pagetable_pmd_ctor(ptdesc)) {
		pagetable_free(ptdesc);
		return NULL;
	}
	return ptdesc_address(ptdesc);
}
#endif

#ifndef __HAVE_ARCH_PMD_FREE
static inline void pmd_free(struct mm_struct *mm, pmd_t *pmd)
{
	struct ptdesc *ptdesc = virt_to_ptdesc(pmd);

	BUG_ON((unsigned long)pmd & (PAGE_SIZE-1));
	pagetable_pmd_dtor(ptdesc);
	pagetable_free(ptdesc);
}
#endif

#endif 

#if CONFIG_PGTABLE_LEVELS > 3

static inline pud_t *__pud_alloc_one(struct mm_struct *mm, unsigned long addr)
{
	gfp_t gfp = GFP_PGTABLE_USER;
	struct ptdesc *ptdesc;

	if (mm == &init_mm)
		gfp = GFP_PGTABLE_KERNEL;
	gfp &= ~__GFP_HIGHMEM;

	ptdesc = pagetable_alloc(gfp, 0);
	if (!ptdesc)
		return NULL;
	return ptdesc_address(ptdesc);
}

#ifndef __HAVE_ARCH_PUD_ALLOC_ONE

static inline pud_t *pud_alloc_one(struct mm_struct *mm, unsigned long addr)
{
	return __pud_alloc_one(mm, addr);
}
#endif

static inline void __pud_free(struct mm_struct *mm, pud_t *pud)
{
	BUG_ON((unsigned long)pud & (PAGE_SIZE-1));
	pagetable_free(virt_to_ptdesc(pud));
}

#ifndef __HAVE_ARCH_PUD_FREE
static inline void pud_free(struct mm_struct *mm, pud_t *pud)
{
	__pud_free(mm, pud);
}
#endif

#endif 

#ifndef __HAVE_ARCH_PGD_FREE
static inline void pgd_free(struct mm_struct *mm, pgd_t *pgd)
{
	pagetable_free(virt_to_ptdesc(pgd));
}
#endif

#endif 

#endif 
