/* SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2020 Marvell.
 */

#ifndef __SOC_OTX2_ASM_H
#define __SOC_OTX2_ASM_H

#include <linux/types.h>
#if defined(CONFIG_ARM64)

#define otx2_lmt_flush(ioaddr)                          \
({                                                      \
	u64 result = 0;                                 \
	__asm__ volatile(".cpu  generic+lse\n"          \
			 "ldeor xzr, %x[rf], [%[rs]]"   \
			 : [rf]"=r" (result)            \
			 : [rs]"r" (ioaddr));           \
	(result);                                       \
})

#define cn10k_lmt_flush(val, addr)			\
({							\
	__asm__ volatile(".cpu  generic+lse\n"		\
			 "steorl %x[rf],[%[rs]]"		\
			 : [rf] "+r"(val)		\
			 : [rs] "r"(addr));		\
})

static inline u64 otx2_atomic64_fetch_add(u64 incr, u64 *ptr)
{
	u64 result;

	asm volatile (".cpu  generic+lse\n"
		      "ldadda %x[i], %x[r], [%[b]]"
		      : [r] "=r" (result), "+m" (*ptr)
		      : [i] "r" (incr), [b] "r" (ptr)
		      : "memory");
	return result;
}

#else
#define otx2_lmt_flush(ioaddr)          ({ 0; })
#define cn10k_lmt_flush(val, addr)	({ addr = val; })
#define otx2_atomic64_fetch_add(incr, ptr)	({ incr; })
#endif

#endif 
