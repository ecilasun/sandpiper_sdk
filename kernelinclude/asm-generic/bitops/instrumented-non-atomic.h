/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_GENERIC_BITOPS_INSTRUMENTED_NON_ATOMIC_H
#define _ASM_GENERIC_BITOPS_INSTRUMENTED_NON_ATOMIC_H

#include <linux/instrumented.h>


static __always_inline void
___set_bit(unsigned long nr, volatile unsigned long *addr)
{
	instrument_write(addr + BIT_WORD(nr), sizeof(long));
	arch___set_bit(nr, addr);
}


static __always_inline void
___clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	instrument_write(addr + BIT_WORD(nr), sizeof(long));
	arch___clear_bit(nr, addr);
}


static __always_inline void
___change_bit(unsigned long nr, volatile unsigned long *addr)
{
	instrument_write(addr + BIT_WORD(nr), sizeof(long));
	arch___change_bit(nr, addr);
}

static __always_inline void __instrument_read_write_bitop(long nr, volatile unsigned long *addr)
{
	if (IS_ENABLED(CONFIG_KCSAN_ASSUME_PLAIN_WRITES_ATOMIC)) {
		
		kcsan_check_read(addr + BIT_WORD(nr), sizeof(long));
		
		instrument_write(addr + BIT_WORD(nr), sizeof(long));
	} else {
		instrument_read_write(addr + BIT_WORD(nr), sizeof(long));
	}
}


static __always_inline bool
___test_and_set_bit(unsigned long nr, volatile unsigned long *addr)
{
	__instrument_read_write_bitop(nr, addr);
	return arch___test_and_set_bit(nr, addr);
}


static __always_inline bool
___test_and_clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	__instrument_read_write_bitop(nr, addr);
	return arch___test_and_clear_bit(nr, addr);
}


static __always_inline bool
___test_and_change_bit(unsigned long nr, volatile unsigned long *addr)
{
	__instrument_read_write_bitop(nr, addr);
	return arch___test_and_change_bit(nr, addr);
}


static __always_inline bool
_test_bit(unsigned long nr, const volatile unsigned long *addr)
{
	instrument_atomic_read(addr + BIT_WORD(nr), sizeof(long));
	return arch_test_bit(nr, addr);
}


static __always_inline bool
_test_bit_acquire(unsigned long nr, const volatile unsigned long *addr)
{
	instrument_atomic_read(addr + BIT_WORD(nr), sizeof(long));
	return arch_test_bit_acquire(nr, addr);
}

#endif 
