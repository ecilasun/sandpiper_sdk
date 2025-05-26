/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_MACH_CPUTYPE_H
#define __ASM_MACH_CPUTYPE_H

#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/cputype.h>
#endif



extern unsigned int mmp_chip_id;

#if defined(CONFIG_MACH_MMP2_DT)
static inline int cpu_is_mmp2(void)
{
	return (((read_cpuid_id() >> 8) & 0xff) == 0x58) &&
		(((mmp_chip_id & 0xfff) == 0x410) ||
		 ((mmp_chip_id & 0xfff) == 0x610));
}
#else
#define cpu_is_mmp2()	(0)
#endif

#ifdef CONFIG_MACH_MMP3_DT
static inline int cpu_is_mmp3(void)
{
	return (((read_cpuid_id() >> 8) & 0xff) == 0x58) &&
		((mmp_chip_id & 0xffff) == 0x2128);
}

static inline int cpu_is_mmp3_a0(void)
{
	return (cpu_is_mmp3() &&
		((mmp_chip_id & 0x00ff0000) == 0x00a00000));
}

static inline int cpu_is_mmp3_b0(void)
{
	return (cpu_is_mmp3() &&
		((mmp_chip_id & 0x00ff0000) == 0x00b00000));
}

#else
#define cpu_is_mmp3()		(0)
#define cpu_is_mmp3_a0()	(0)
#define cpu_is_mmp3_b0()	(0)
#endif

#endif 
