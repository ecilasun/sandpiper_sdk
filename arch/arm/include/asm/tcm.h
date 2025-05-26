/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASMARM_TCM_H
#define __ASMARM_TCM_H

#ifdef CONFIG_HAVE_TCM

#include <linux/compiler.h>


#define __tcmdata __section(".tcm.data")

#define __tcmconst __section(".tcm.rodata")

#define __tcmfunc __attribute__((long_call)) __section(".tcm.text") noinline

#define __tcmlocalfunc __section(".tcm.text")

void *tcm_alloc(size_t len);
void tcm_free(void *addr, size_t len);
bool tcm_dtcm_present(void);
bool tcm_itcm_present(void);

void __init tcm_init(void);
#else

static inline void tcm_init(void)
{
}
#endif
#endif
