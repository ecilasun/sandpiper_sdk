/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __ASM_KASAN_H
#define __ASM_KASAN_H

#ifdef CONFIG_KASAN

#include <asm/kasan_def.h>

#define KASAN_SHADOW_SCALE_SHIFT 3



asmlinkage void kasan_early_init(void);
extern void kasan_init(void);

#else
static inline void kasan_init(void) { }
#endif

#endif
