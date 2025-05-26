/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_GENERIC_BITOPS_GENERIC_NON_ATOMIC_H
#define __ASM_GENERIC_BITOPS_GENERIC_NON_ATOMIC_H

#include <linux/bits.h>
#include <asm/barrier.h>

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif




static __always_inline void
generic___set_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p  |= mask;
}

static __always_inline void
generic___clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p &= ~mask;
}


static __always_inline void
generic___change_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p ^= mask;
}


static __always_inline bool
generic___test_and_set_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old | mask;
	return (old & mask) != 0;
}


static __always_inline bool
generic___test_and_clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old & ~mask;
	return (old & mask) != 0;
}


static __always_inline bool
generic___test_and_change_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old ^ mask;
	return (old & mask) != 0;
}


static __always_inline bool
generic_test_bit(unsigned long nr, const volatile unsigned long *addr)
{
	
	return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}


static __always_inline bool
generic_test_bit_acquire(unsigned long nr, const volatile unsigned long *addr)
{
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	return 1UL & (smp_load_acquire(p) >> (nr & (BITS_PER_LONG-1)));
}


#define const___set_bit			generic___set_bit
#define const___clear_bit		generic___clear_bit
#define const___change_bit		generic___change_bit
#define const___test_and_set_bit	generic___test_and_set_bit
#define const___test_and_clear_bit	generic___test_and_clear_bit
#define const___test_and_change_bit	generic___test_and_change_bit
#define const_test_bit_acquire		generic_test_bit_acquire


static __always_inline bool
const_test_bit(unsigned long nr, const volatile unsigned long *addr)
{
	const unsigned long *p = (const unsigned long *)addr + BIT_WORD(nr);
	unsigned long mask = BIT_MASK(nr);
	unsigned long val = *p;

	return !!(val & mask);
}

#endif 
