/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _ASM_ARM_HUGETLB_3LEVEL_H
#define _ASM_ARM_HUGETLB_3LEVEL_H



#define __HAVE_ARCH_HUGE_PTEP_GET
static inline pte_t huge_ptep_get(pte_t *ptep)
{
	pte_t retval = *ptep;
	if (pte_val(retval))
		pte_val(retval) |= L_PTE_VALID;
	return retval;
}

#endif 
