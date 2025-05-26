/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LINUX_SCHED_CLOCK
#define LINUX_SCHED_CLOCK

#include <linux/types.h>

#ifdef CONFIG_GENERIC_SCHED_CLOCK

struct clock_read_data {
	u64 epoch_ns;
	u64 epoch_cyc;
	u64 sched_clock_mask;
	u64 (*read_sched_clock)(void);
	u32 mult;
	u32 shift;
};

extern struct clock_read_data *sched_clock_read_begin(unsigned int *seq);
extern int sched_clock_read_retry(unsigned int seq);

extern void generic_sched_clock_init(void);

extern void sched_clock_register(u64 (*read)(void), int bits,
				 unsigned long rate);
#else
static inline void generic_sched_clock_init(void) { }

static inline void sched_clock_register(u64 (*read)(void), int bits,
					unsigned long rate)
{
}
#endif

#endif
