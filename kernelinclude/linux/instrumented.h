/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_INSTRUMENTED_H
#define _LINUX_INSTRUMENTED_H

#include <linux/compiler.h>
#include <linux/kasan-checks.h>
#include <linux/kcsan-checks.h>
#include <linux/kmsan-checks.h>
#include <linux/types.h>


static __always_inline void instrument_read(const volatile void *v, size_t size)
{
	kasan_check_read(v, size);
	kcsan_check_read(v, size);
}


static __always_inline void instrument_write(const volatile void *v, size_t size)
{
	kasan_check_write(v, size);
	kcsan_check_write(v, size);
}


static __always_inline void instrument_read_write(const volatile void *v, size_t size)
{
	kasan_check_write(v, size);
	kcsan_check_read_write(v, size);
}


static __always_inline void instrument_atomic_read(const volatile void *v, size_t size)
{
	kasan_check_read(v, size);
	kcsan_check_atomic_read(v, size);
}


static __always_inline void instrument_atomic_write(const volatile void *v, size_t size)
{
	kasan_check_write(v, size);
	kcsan_check_atomic_write(v, size);
}


static __always_inline void instrument_atomic_read_write(const volatile void *v, size_t size)
{
	kasan_check_write(v, size);
	kcsan_check_atomic_read_write(v, size);
}


static __always_inline void
instrument_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	kasan_check_read(from, n);
	kcsan_check_read(from, n);
	kmsan_copy_to_user(to, from, n, 0);
}


static __always_inline void
instrument_copy_from_user_before(const void *to, const void __user *from, unsigned long n)
{
	kasan_check_write(to, n);
	kcsan_check_write(to, n);
}


static __always_inline void
instrument_copy_from_user_after(const void *to, const void __user *from,
				unsigned long n, unsigned long left)
{
	kmsan_unpoison_memory(to, n - left);
}


#define instrument_get_user(to)				\
({							\
	u64 __tmp = (u64)(to);				\
	kmsan_unpoison_memory(&__tmp, sizeof(__tmp));	\
	to = __tmp;					\
})



#define instrument_put_user(from, ptr, size)			\
({								\
	kmsan_copy_to_user(ptr, &from, sizeof(from), 0);	\
})

#endif 
