/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_KASAN_CHECKS_H
#define _LINUX_KASAN_CHECKS_H

#include <linux/types.h>




#if defined(CONFIG_KASAN_GENERIC) || defined(CONFIG_KASAN_SW_TAGS)
bool __kasan_check_read(const volatile void *p, unsigned int size);
bool __kasan_check_write(const volatile void *p, unsigned int size);
#else
static inline bool __kasan_check_read(const volatile void *p, unsigned int size)
{
	return true;
}
static inline bool __kasan_check_write(const volatile void *p, unsigned int size)
{
	return true;
}
#endif


#ifdef __SANITIZE_ADDRESS__
#define kasan_check_read __kasan_check_read
#define kasan_check_write __kasan_check_write
#else
static inline bool kasan_check_read(const volatile void *p, unsigned int size)
{
	return true;
}
static inline bool kasan_check_write(const volatile void *p, unsigned int size)
{
	return true;
}
#endif

#endif
