/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _KUNIT_TEST_BUG_H
#define _KUNIT_TEST_BUG_H

#include <linux/stddef.h> 

#if IS_ENABLED(CONFIG_KUNIT)

#include <linux/jump_label.h> 
#include <linux/sched.h>


DECLARE_STATIC_KEY_FALSE(kunit_running);


extern struct kunit_hooks_table {
	__printf(3, 4) void (*fail_current_test)(const char*, int, const char*, ...);
	void *(*get_static_stub_address)(struct kunit *test, void *real_fn_addr);
} kunit_hooks;


static inline struct kunit *kunit_get_current_test(void)
{
	if (!static_branch_unlikely(&kunit_running))
		return NULL;

	return current->kunit_test;
}



#define kunit_fail_current_test(fmt, ...) do {					\
		if (static_branch_unlikely(&kunit_running)) {			\
				\
			kunit_hooks.fail_current_test(__FILE__, __LINE__,	\
						  fmt, ##__VA_ARGS__);		\
		}								\
	} while (0)

#else

static inline struct kunit *kunit_get_current_test(void) { return NULL; }

#define kunit_fail_current_test(fmt, ...) do {} while (0)

#endif

#endif 
