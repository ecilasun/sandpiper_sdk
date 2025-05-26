/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_DATAPAGE_H
#define __VDSO_DATAPAGE_H

#ifndef __ASSEMBLY__

#include <linux/compiler.h>
#include <uapi/linux/time.h>
#include <uapi/linux/types.h>
#include <uapi/asm-generic/errno-base.h>

#include <vdso/bits.h>
#include <vdso/clocksource.h>
#include <vdso/ktime.h>
#include <vdso/limits.h>
#include <vdso/math64.h>
#include <vdso/processor.h>
#include <vdso/time.h>
#include <vdso/time32.h>
#include <vdso/time64.h>

#ifdef CONFIG_ARCH_HAS_VDSO_DATA
#include <asm/vdso/data.h>
#else
struct arch_vdso_data {};
#endif

#define VDSO_BASES	(CLOCK_TAI + 1)
#define VDSO_HRES	(BIT(CLOCK_REALTIME)		| \
			 BIT(CLOCK_MONOTONIC)		| \
			 BIT(CLOCK_BOOTTIME)		| \
			 BIT(CLOCK_TAI))
#define VDSO_COARSE	(BIT(CLOCK_REALTIME_COARSE)	| \
			 BIT(CLOCK_MONOTONIC_COARSE))
#define VDSO_RAW	(BIT(CLOCK_MONOTONIC_RAW))

#define CS_HRES_COARSE	0
#define CS_RAW		1
#define CS_BASES	(CS_RAW + 1)


struct vdso_timestamp {
	u64	sec;
	u64	nsec;
};


struct vdso_data {
	u32			seq;

	s32			clock_mode;
	u64			cycle_last;
	u64			mask;
	u32			mult;
	u32			shift;

	union {
		struct vdso_timestamp	basetime[VDSO_BASES];
		struct timens_offset	offset[VDSO_BASES];
	};

	s32			tz_minuteswest;
	s32			tz_dsttime;
	u32			hrtimer_res;
	u32			__unused;

	struct arch_vdso_data	arch_data;
};


extern struct vdso_data _vdso_data[CS_BASES] __attribute__((visibility("hidden")));
extern struct vdso_data _timens_data[CS_BASES] __attribute__((visibility("hidden")));


#ifdef ENABLE_COMPAT_VDSO
#include <asm/vdso/compat_gettimeofday.h>
#else
#include <asm/vdso/gettimeofday.h>
#endif 

#endif 

#endif 
