/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_STACKDEPOT_H
#define _LINUX_STACKDEPOT_H

#include <linux/gfp.h>

typedef u32 depot_stack_handle_t;


#define STACK_DEPOT_EXTRA_BITS 5


#ifdef CONFIG_STACKDEPOT
int stack_depot_init(void);

void __init stack_depot_request_early_init(void);


int __init stack_depot_early_init(void);
#else
static inline int stack_depot_init(void) { return 0; }

static inline void stack_depot_request_early_init(void) { }

static inline int stack_depot_early_init(void)	{ return 0; }
#endif


depot_stack_handle_t __stack_depot_save(unsigned long *entries,
					unsigned int nr_entries,
					gfp_t gfp_flags, bool can_alloc);


depot_stack_handle_t stack_depot_save(unsigned long *entries,
				      unsigned int nr_entries, gfp_t gfp_flags);


unsigned int stack_depot_fetch(depot_stack_handle_t handle,
			       unsigned long **entries);


void stack_depot_print(depot_stack_handle_t stack);


int stack_depot_snprint(depot_stack_handle_t handle, char *buf, size_t size,
		       int spaces);


depot_stack_handle_t __must_check stack_depot_set_extra_bits(
			depot_stack_handle_t handle, unsigned int extra_bits);


unsigned int stack_depot_get_extra_bits(depot_stack_handle_t handle);

#endif
