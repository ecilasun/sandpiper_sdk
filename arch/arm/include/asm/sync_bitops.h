/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SYNC_BITOPS_H__
#define __ASM_SYNC_BITOPS_H__

#include <asm/bitops.h>





#define sync_set_bit(nr, p)		_set_bit(nr, p)
#define sync_clear_bit(nr, p)		_clear_bit(nr, p)
#define sync_change_bit(nr, p)		_change_bit(nr, p)
#define sync_test_bit(nr, addr)		test_bit(nr, addr)



int _sync_test_and_set_bit(int nr, volatile unsigned long * p);
#define sync_test_and_set_bit(nr, p)	_sync_test_and_set_bit(nr, p)

int _sync_test_and_clear_bit(int nr, volatile unsigned long * p);
#define sync_test_and_clear_bit(nr, p)	_sync_test_and_clear_bit(nr, p)

int _sync_test_and_change_bit(int nr, volatile unsigned long * p);
#define sync_test_and_change_bit(nr, p)	_sync_test_and_change_bit(nr, p)

#define arch_sync_cmpxchg(ptr, old, new)				\
({									\
	__typeof__(*(ptr)) __ret;					\
	__smp_mb__before_atomic();					\
	__ret = arch_cmpxchg_relaxed((ptr), (old), (new));		\
	__smp_mb__after_atomic();					\
	__ret;								\
})

#endif
