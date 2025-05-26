/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_MMU_CONTEXT_H
#define __ASM_GENERIC_MMU_CONTEXT_H



struct task_struct;
struct mm_struct;


#ifndef enter_lazy_tlb
static inline void enter_lazy_tlb(struct mm_struct *mm,
			struct task_struct *tsk)
{
}
#endif


#ifndef init_new_context
static inline int init_new_context(struct task_struct *tsk,
			struct mm_struct *mm)
{
	return 0;
}
#endif


#ifndef destroy_context
static inline void destroy_context(struct mm_struct *mm)
{
}
#endif


#ifndef activate_mm
static inline void activate_mm(struct mm_struct *prev_mm,
			       struct mm_struct *next_mm)
{
	switch_mm(prev_mm, next_mm, current);
}
#endif


#ifndef deactivate_mm
static inline void deactivate_mm(struct task_struct *tsk,
			struct mm_struct *mm)
{
}
#endif

#endif 
