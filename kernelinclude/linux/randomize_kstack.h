/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _LINUX_RANDOMIZE_KSTACK_H
#define _LINUX_RANDOMIZE_KSTACK_H

#ifdef CONFIG_RANDOMIZE_KSTACK_OFFSET
#include <linux/kernel.h>
#include <linux/jump_label.h>
#include <linux/percpu-defs.h>

DECLARE_STATIC_KEY_MAYBE(CONFIG_RANDOMIZE_KSTACK_OFFSET_DEFAULT,
			 randomize_kstack_offset);
DECLARE_PER_CPU(u32, kstack_offset);


#if __has_builtin(__builtin_alloca_uninitialized)
#define __kstack_alloca __builtin_alloca_uninitialized
#else
#define __kstack_alloca __builtin_alloca
#endif


#define KSTACK_OFFSET_MAX(x)	((x) & 0x3FF)


#define add_random_kstack_offset() do {					\
	if (static_branch_maybe(CONFIG_RANDOMIZE_KSTACK_OFFSET_DEFAULT,	\
				&randomize_kstack_offset)) {		\
		u32 offset = raw_cpu_read(kstack_offset);		\
		u8 *ptr = __kstack_alloca(KSTACK_OFFSET_MAX(offset));	\
			\
		asm volatile("" :: "r"(ptr) : "memory");		\
	}								\
} while (0)


#define choose_random_kstack_offset(rand) do {				\
	if (static_branch_maybe(CONFIG_RANDOMIZE_KSTACK_OFFSET_DEFAULT,	\
				&randomize_kstack_offset)) {		\
		u32 offset = raw_cpu_read(kstack_offset);		\
		offset = ror32(offset, 5) ^ (rand);			\
		raw_cpu_write(kstack_offset, offset);			\
	}								\
} while (0)
#else 
#define add_random_kstack_offset()		do { } while (0)
#define choose_random_kstack_offset(rand)	do { } while (0)
#endif 

#endif
