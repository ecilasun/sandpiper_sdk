/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_CPUFEATURE_H
#define __ASM_CPUFEATURE_H

#include <linux/log2.h>
#include <asm/hwcap.h>


#define MAX_CPU_FEATURES	64
#define __hwcap_feature(x)	ilog2(HWCAP_ ## x)
#define __hwcap2_feature(x)	(32 + ilog2(HWCAP2_ ## x))
#define cpu_feature(x)		__hwcap2_feature(x)

static inline bool cpu_have_feature(unsigned int num)
{
	return num < 32 ? elf_hwcap & BIT(num) : elf_hwcap2 & BIT(num - 32);
}

#endif
