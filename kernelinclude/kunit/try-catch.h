/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _KUNIT_TRY_CATCH_H
#define _KUNIT_TRY_CATCH_H

#include <linux/types.h>

typedef void (*kunit_try_catch_func_t)(void *);

struct completion;
struct kunit;


struct kunit_try_catch {
	
	struct kunit *test;
	struct completion *try_completion;
	int try_result;
	kunit_try_catch_func_t try;
	kunit_try_catch_func_t catch;
	void *context;
};

void kunit_try_catch_run(struct kunit_try_catch *try_catch, void *context);

void __noreturn kunit_try_catch_throw(struct kunit_try_catch *try_catch);

static inline int kunit_try_catch_get_result(struct kunit_try_catch *try_catch)
{
	return try_catch->try_result;
}

#endif 
