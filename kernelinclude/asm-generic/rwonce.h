/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __ASM_GENERIC_RWONCE_H
#define __ASM_GENERIC_RWONCE_H

#ifndef __ASSEMBLY__

#include <linux/compiler_types.h>
#include <linux/kasan-checks.h>
#include <linux/kcsan-checks.h>


#define compiletime_assert_rwonce_type(t)					\
	compiletime_assert(__native_word(t) || sizeof(t) == sizeof(long long),	\
		"Unsupported access size for {READ,WRITE}_ONCE().")


#ifndef __READ_ONCE
#define __READ_ONCE(x)	(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define READ_ONCE(x)							\
({									\
	compiletime_assert_rwonce_type(x);				\
	__READ_ONCE(x);							\
})

#define __WRITE_ONCE(x, val)						\
do {									\
	*(volatile typeof(x) *)&(x) = (val);				\
} while (0)

#define WRITE_ONCE(x, val)						\
do {									\
	compiletime_assert_rwonce_type(x);				\
	__WRITE_ONCE(x, val);						\
} while (0)

static __no_sanitize_or_inline
unsigned long __read_once_word_nocheck(const void *addr)
{
	return __READ_ONCE(*(unsigned long *)addr);
}


#define READ_ONCE_NOCHECK(x)						\
({									\
	compiletime_assert(sizeof(x) == sizeof(unsigned long),		\
		"Unsupported access size for READ_ONCE_NOCHECK().");	\
	(typeof(x))__read_once_word_nocheck(&(x));			\
})

static __no_kasan_or_inline
unsigned long read_word_at_a_time(const void *addr)
{
	kasan_check_read(addr, 1);
	return *(unsigned long *)addr;
}

#endif 
#endif	
