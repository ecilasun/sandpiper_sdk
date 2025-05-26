/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef VIRT_H
#define VIRT_H

#include <asm/ptrace.h>


#define BOOT_CPU_MODE_MISMATCH	PSR_N_BIT

#ifndef __ASSEMBLY__
#include <asm/cacheflush.h>

#ifdef CONFIG_ARM_VIRT_EXT

extern int __boot_cpu_mode;

static inline void sync_boot_mode(void)
{
	
	sync_cache_r(&__boot_cpu_mode);
}

#else
#define __boot_cpu_mode	(SVC_MODE)
#define sync_boot_mode()
#endif

#ifndef ZIMAGE
void hyp_mode_check(void);


static inline bool is_hyp_mode_available(void)
{
	return ((__boot_cpu_mode & MODE_MASK) == HYP_MODE &&
		!(__boot_cpu_mode & BOOT_CPU_MODE_MISMATCH));
}


static inline bool is_hyp_mode_mismatched(void)
{
	return !!(__boot_cpu_mode & BOOT_CPU_MODE_MISMATCH);
}

static inline bool is_kernel_in_hyp_mode(void)
{
	return false;
}

#endif

#else



#define HVC_SET_VECTORS 0
#define HVC_SOFT_RESTART 1

#endif 

#define HVC_STUB_ERR	0xbadca11

#endif 
