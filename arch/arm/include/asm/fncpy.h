/* SPDX-License-Identifier: GPL-2.0-only */




#ifndef __ASM_FNCPY_H
#define __ASM_FNCPY_H

#include <linux/types.h>
#include <linux/string.h>

#include <asm/bug.h>
#include <asm/cacheflush.h>


#define FNCPY_ALIGN 8

#define fncpy(dest_buf, funcp, size) ({					\
	uintptr_t __funcp_address;					\
	typeof(funcp) __result;						\
									\
	asm("" : "=r" (__funcp_address) : "0" (funcp));			\
									\
									\
	BUG_ON((uintptr_t)(dest_buf) & (FNCPY_ALIGN - 1) ||		\
		(__funcp_address & ~(uintptr_t)1 & (FNCPY_ALIGN - 1)));	\
									\
	memcpy(dest_buf, (void const *)(__funcp_address & ~1), size);	\
	flush_icache_range((unsigned long)(dest_buf),			\
		(unsigned long)(dest_buf) + (size));			\
									\
	asm("" : "=r" (__result)					\
		: "0" ((uintptr_t)(dest_buf) | (__funcp_address & 1)));	\
									\
	__result;							\
})

#endif 
