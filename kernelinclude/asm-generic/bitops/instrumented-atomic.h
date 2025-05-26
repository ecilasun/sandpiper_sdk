/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_GENERIC_BITOPS_INSTRUMENTED_ATOMIC_H
#define _ASM_GENERIC_BITOPS_INSTRUMENTED_ATOMIC_H

#include <linux/instrumented.h>


static __always_inline void set_bit(long nr, volatile unsigned long *addr)
{
	instrument_atomic_write(addr + BIT_WORD(nr), sizeof(long));
	arch_set_bit(nr, addr);
}


static __always_inline void clear_bit(long nr, volatile unsigned long *addr)
{
	instrument_atomic_write(addr + BIT_WORD(nr), sizeof(long));
	arch_clear_bit(nr, addr);
}


static __always_inline void change_bit(long nr, volatile unsigned long *addr)
{
	instrument_atomic_write(addr + BIT_WORD(nr), sizeof(long));
	arch_change_bit(nr, addr);
}


static __always_inline bool test_and_set_bit(long nr, volatile unsigned long *addr)
{
	kcsan_mb();
	instrument_atomic_read_write(addr + BIT_WORD(nr), sizeof(long));
	return arch_test_and_set_bit(nr, addr);
}


static __always_inline bool test_and_clear_bit(long nr, volatile unsigned long *addr)
{
	kcsan_mb();
	instrument_atomic_read_write(addr + BIT_WORD(nr), sizeof(long));
	return arch_test_and_clear_bit(nr, addr);
}


static __always_inline bool test_and_change_bit(long nr, volatile unsigned long *addr)
{
	kcsan_mb();
	instrument_atomic_read_write(addr + BIT_WORD(nr), sizeof(long));
	return arch_test_and_change_bit(nr, addr);
}

#endif 
