/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_GENERIC_BITOPS_INSTRUMENTED_LOCK_H
#define _ASM_GENERIC_BITOPS_INSTRUMENTED_LOCK_H

#include <linux/instrumented.h>


static inline void clear_bit_unlock(long nr, volatile unsigned long *addr)
{
	kcsan_release();
	instrument_atomic_write(addr + BIT_WORD(nr), sizeof(long));
	arch_clear_bit_unlock(nr, addr);
}


static inline void __clear_bit_unlock(long nr, volatile unsigned long *addr)
{
	kcsan_release();
	instrument_write(addr + BIT_WORD(nr), sizeof(long));
	arch___clear_bit_unlock(nr, addr);
}


static inline bool test_and_set_bit_lock(long nr, volatile unsigned long *addr)
{
	instrument_atomic_read_write(addr + BIT_WORD(nr), sizeof(long));
	return arch_test_and_set_bit_lock(nr, addr);
}

#if defined(arch_clear_bit_unlock_is_negative_byte)

static inline bool
clear_bit_unlock_is_negative_byte(long nr, volatile unsigned long *addr)
{
	kcsan_release();
	instrument_atomic_write(addr + BIT_WORD(nr), sizeof(long));
	return arch_clear_bit_unlock_is_negative_byte(nr, addr);
}

#define clear_bit_unlock_is_negative_byte clear_bit_unlock_is_negative_byte
#endif

#endif 
