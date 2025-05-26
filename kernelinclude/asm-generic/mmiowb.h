/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_MMIOWB_H
#define __ASM_GENERIC_MMIOWB_H


#ifdef CONFIG_MMIOWB

#include <linux/compiler.h>
#include <asm-generic/mmiowb_types.h>

#ifndef arch_mmiowb_state
#include <asm/percpu.h>
#include <asm/smp.h>

DECLARE_PER_CPU(struct mmiowb_state, __mmiowb_state);
#define __mmiowb_state()	raw_cpu_ptr(&__mmiowb_state)
#else
#define __mmiowb_state()	arch_mmiowb_state()
#endif	

static inline void mmiowb_set_pending(void)
{
	struct mmiowb_state *ms = __mmiowb_state();

	if (likely(ms->nesting_count))
		ms->mmiowb_pending = ms->nesting_count;
}

static inline void mmiowb_spin_lock(void)
{
	struct mmiowb_state *ms = __mmiowb_state();
	ms->nesting_count++;
}

static inline void mmiowb_spin_unlock(void)
{
	struct mmiowb_state *ms = __mmiowb_state();

	if (unlikely(ms->mmiowb_pending)) {
		ms->mmiowb_pending = 0;
		mmiowb();
	}

	ms->nesting_count--;
}
#else
#define mmiowb_set_pending()		do { } while (0)
#define mmiowb_spin_lock()		do { } while (0)
#define mmiowb_spin_unlock()		do { } while (0)
#endif	
#endif	
