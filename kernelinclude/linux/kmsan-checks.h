/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_KMSAN_CHECKS_H
#define _LINUX_KMSAN_CHECKS_H

#include <linux/types.h>

#ifdef CONFIG_KMSAN


void kmsan_poison_memory(const void *address, size_t size, gfp_t flags);


void kmsan_unpoison_memory(const void *address, size_t size);


void kmsan_check_memory(const void *address, size_t size);


void kmsan_copy_to_user(void __user *to, const void *from, size_t to_copy,
			size_t left);

#else

static inline void kmsan_poison_memory(const void *address, size_t size,
				       gfp_t flags)
{
}
static inline void kmsan_unpoison_memory(const void *address, size_t size)
{
}
static inline void kmsan_check_memory(const void *address, size_t size)
{
}
static inline void kmsan_copy_to_user(void __user *to, const void *from,
				      size_t to_copy, size_t left)
{
}

#endif

#endif 
