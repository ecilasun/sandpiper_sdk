/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_TYPES_H
#define _LINUX_SCHED_TYPES_H

#include <linux/types.h>


struct task_cputime {
	u64				stime;
	u64				utime;
	unsigned long long		sum_exec_runtime;
};

#endif
