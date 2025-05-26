/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_NOMMU_H
#define __ASM_GENERIC_NOMMU_H


#include <asm-generic/mm_hooks.h>

static inline void switch_mm(struct mm_struct *prev,
			struct mm_struct *next,
			struct task_struct *tsk)
{
}

#include <asm-generic/mmu_context.h>

#endif 
