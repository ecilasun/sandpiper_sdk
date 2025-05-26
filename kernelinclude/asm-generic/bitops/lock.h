/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_BITOPS_LOCK_H_
#define _ASM_GENERIC_BITOPS_LOCK_H_

#include <linux/atomic.h>
#include <linux/compiler.h>
#include <asm/barrier.h>


static __always_inline int
arch_test_and_set_bit_lock(unsigned int nr, volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	if (READ_ONCE(*p) & mask)
		return 1;

	old = raw_atomic_long_fetch_or_acquire(mask, (atomic_long_t *)p);
	return !!(old & mask);
}



static __always_inline void
arch_clear_bit_unlock(unsigned int nr, volatile unsigned long *p)
{
	p += BIT_WORD(nr);
	raw_atomic_long_fetch_andnot_release(BIT_MASK(nr), (atomic_long_t *)p);
}


static inline void
arch___clear_bit_unlock(unsigned int nr, volatile unsigned long *p)
{
	unsigned long old;

	p += BIT_WORD(nr);
	old = READ_ONCE(*p);
	old &= ~BIT_MASK(nr);
	raw_atomic_long_set_release((atomic_long_t *)p, old);
}


#ifndef arch_clear_bit_unlock_is_negative_byte
static inline bool arch_clear_bit_unlock_is_negative_byte(unsigned int nr,
							  volatile unsigned long *p)
{
	long old;
	unsigned long mask = BIT_MASK(nr);

	p += BIT_WORD(nr);
	old = raw_atomic_long_fetch_andnot_release(mask, (atomic_long_t *)p);
	return !!(old & BIT(7));
}
#define arch_clear_bit_unlock_is_negative_byte arch_clear_bit_unlock_is_negative_byte
#endif

#include <asm-generic/bitops/instrumented-lock.h>

#endif 
