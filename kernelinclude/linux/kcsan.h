/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_KCSAN_H
#define _LINUX_KCSAN_H

#include <linux/kcsan-checks.h>
#include <linux/types.h>

#ifdef CONFIG_KCSAN


struct kcsan_ctx {
	int disable_count; 
	int disable_scoped; 
	int atomic_next; 

	
	int atomic_nest_count;
	bool in_flat_atomic;

	
	unsigned long access_mask;

	
	struct list_head scoped_accesses;

#ifdef CONFIG_KCSAN_WEAK_MEMORY
	
	struct kcsan_scoped_access reorder_access;
#endif
};


void kcsan_init(void);

#else 

static inline void kcsan_init(void)			{ }

#endif 

#endif 
