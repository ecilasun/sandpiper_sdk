/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_ARM_MMU_CONTEXT_H
#define __ASM_ARM_MMU_CONTEXT_H

#include <linux/compiler.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/preempt.h>

#include <asm/cacheflush.h>
#include <asm/cachetype.h>
#include <asm/proc-fns.h>
#include <asm/smp_plat.h>
#include <asm-generic/mm_hooks.h>

void __check_vmalloc_seq(struct mm_struct *mm);

#ifdef CONFIG_MMU
static inline void check_vmalloc_seq(struct mm_struct *mm)
{
	if (!IS_ENABLED(CONFIG_ARM_LPAE) &&
	    unlikely(atomic_read(&mm->context.vmalloc_seq) !=
		     atomic_read(&init_mm.context.vmalloc_seq)))
		__check_vmalloc_seq(mm);
}
#endif

#ifdef CONFIG_CPU_HAS_ASID

void check_and_switch_context(struct mm_struct *mm, struct task_struct *tsk);

#define init_new_context init_new_context
static inline int
init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
	atomic64_set(&mm->context.id, 0);
	return 0;
}

#ifdef CONFIG_ARM_ERRATA_798181
void a15_erratum_get_cpumask(int this_cpu, struct mm_struct *mm,
			     cpumask_t *mask);
#else  
static inline void a15_erratum_get_cpumask(int this_cpu, struct mm_struct *mm,
					   cpumask_t *mask)
{
}
#endif 

#else	

#ifdef CONFIG_MMU

static inline void check_and_switch_context(struct mm_struct *mm,
					    struct task_struct *tsk)
{
	check_vmalloc_seq(mm);

	if (irqs_disabled())
		
		mm->context.switch_pending = 1;
	else
		cpu_switch_mm(mm->pgd, mm);
}

#ifndef MODULE
#define finish_arch_post_lock_switch \
	finish_arch_post_lock_switch
static inline void finish_arch_post_lock_switch(void)
{
	struct mm_struct *mm = current->mm;

	if (mm && mm->context.switch_pending) {
		
		preempt_disable();
		if (mm->context.switch_pending) {
			mm->context.switch_pending = 0;
			cpu_switch_mm(mm->pgd, mm);
		}
		preempt_enable_no_resched();
	}
}
#endif 

#endif	

#endif	

#define activate_mm(prev,next)		switch_mm(prev, next, NULL)


static inline void
switch_mm(struct mm_struct *prev, struct mm_struct *next,
	  struct task_struct *tsk)
{
#ifdef CONFIG_MMU
	unsigned int cpu = smp_processor_id();

	
	if (cache_ops_need_broadcast() &&
	    !cpumask_empty(mm_cpumask(next)) &&
	    !cpumask_test_cpu(cpu, mm_cpumask(next)))
		__flush_icache_all();

	if (!cpumask_test_and_set_cpu(cpu, mm_cpumask(next)) || prev != next) {
		check_and_switch_context(next, tsk);
		if (cache_is_vivt())
			cpumask_clear_cpu(cpu, mm_cpumask(prev));
	}
#endif
}

#ifdef CONFIG_VMAP_STACK
static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
	if (mm != &init_mm)
		check_vmalloc_seq(mm);
}
#define enter_lazy_tlb enter_lazy_tlb
#endif

#include <asm-generic/mmu_context.h>

#endif
