/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASMARM_PGTABLE_NOMMU_H
#define _ASMARM_PGTABLE_NOMMU_H

#ifndef __ASSEMBLY__

#include <linux/slab.h>
#include <asm/processor.h>
#include <asm/page.h>


#define pgd_present(pgd)	(1)
#define pgd_none(pgd)		(0)
#define pgd_bad(pgd)		(0)
#define pgd_clear(pgdp)

#define PGDIR_SHIFT		21

#define PGDIR_SIZE		(1UL << PGDIR_SHIFT)
#define PGDIR_MASK		(~(PGDIR_SIZE-1))


#define PAGE_NONE	__pgprot(0)
#define PAGE_SHARED	__pgprot(0)
#define PAGE_COPY	__pgprot(0)
#define PAGE_READONLY	__pgprot(0)
#define PAGE_KERNEL	__pgprot(0)

#define swapper_pg_dir ((pgd_t *) 0)


typedef pte_t *pte_addr_t;


#define pgprot_noncached(prot)	(prot)
#define pgprot_writecombine(prot) (prot)
#define pgprot_device(prot)	(prot)



extern unsigned int kobjsize(const void *objp);


#define	VMALLOC_START	0UL
#define	VMALLOC_END	0xffffffffUL

#define FIRST_USER_ADDRESS      0UL

#else 


#define v3_tlb_fns	(0)
#define v4_tlb_fns	(0)
#define v4wb_tlb_fns	(0)
#define v4wbi_tlb_fns	(0)
#define v6wbi_tlb_fns	(0)
#define v7wbi_tlb_fns	(0)

#define v3_user_fns	(0)
#define v4_user_fns	(0)
#define v4_mc_user_fns	(0)
#define v4wb_user_fns	(0)
#define v4wt_user_fns	(0)
#define v6_user_fns	(0)
#define xscale_mc_user_fns (0)

#endif 

#endif 
