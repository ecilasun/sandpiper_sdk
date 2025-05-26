/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _KUNIT_STATIC_STUB_H
#define _KUNIT_STATIC_STUB_H

#if !IS_ENABLED(CONFIG_KUNIT)


#define KUNIT_STATIC_STUB_REDIRECT(real_fn_name, args...) do {} while (0)

#else

#include <kunit/test.h>
#include <kunit/test-bug.h>

#include <linux/compiler.h> 
#include <linux/sched.h> 



#define KUNIT_STATIC_STUB_REDIRECT(real_fn_name, args...)		\
do {									\
	typeof(&real_fn_name) replacement;				\
	struct kunit *current_test = kunit_get_current_test();		\
									\
	if (likely(!current_test))					\
		break;							\
									\
	replacement = kunit_hooks.get_static_stub_address(current_test,	\
							&real_fn_name);	\
									\
	if (unlikely(replacement))					\
		return replacement(args);				\
} while (0)


void __kunit_activate_static_stub(struct kunit *test,
				  void *real_fn_addr,
				  void *replacement_addr);


#define kunit_activate_static_stub(test, real_fn_addr, replacement_addr) do {	\
	typecheck_fn(typeof(&real_fn_addr), replacement_addr);			\
	__kunit_activate_static_stub(test, real_fn_addr, replacement_addr);	\
} while (0)



void kunit_deactivate_static_stub(struct kunit *test, void *real_fn_addr);

#endif
#endif
