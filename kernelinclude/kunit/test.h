/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _KUNIT_TEST_H
#define _KUNIT_TEST_H

#include <kunit/assert.h>
#include <kunit/try-catch.h>

#include <linux/args.h>
#include <linux/compiler.h>
#include <linux/container_of.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/jump_label.h>
#include <linux/kconfig.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/types.h>

#include <asm/rwonce.h>


DECLARE_STATIC_KEY_FALSE(kunit_running);

struct kunit;


#define KUNIT_LOG_SIZE 2048


#define KUNIT_PARAM_DESC_SIZE 128


#define KUNIT_STATUS_COMMENT_SIZE 256


#define KUNIT_INDENT_LEN		4
#define KUNIT_SUBTEST_INDENT		"    "
#define KUNIT_SUBSUBTEST_INDENT		"        "


enum kunit_status {
	KUNIT_SUCCESS,
	KUNIT_FAILURE,
	KUNIT_SKIPPED,
};




enum kunit_speed {
	KUNIT_SPEED_UNSET,
	KUNIT_SPEED_VERY_SLOW,
	KUNIT_SPEED_SLOW,
	KUNIT_SPEED_NORMAL,
	KUNIT_SPEED_MAX = KUNIT_SPEED_NORMAL,
};


struct kunit_attributes {
	enum kunit_speed speed;
};


struct kunit_case {
	void (*run_case)(struct kunit *test);
	const char *name;
	const void* (*generate_params)(const void *prev, char *desc);
	struct kunit_attributes attr;

	
	enum kunit_status status;
	char *module_name;
	char *log;
};

static inline char *kunit_status_to_ok_not_ok(enum kunit_status status)
{
	switch (status) {
	case KUNIT_SKIPPED:
	case KUNIT_SUCCESS:
		return "ok";
	case KUNIT_FAILURE:
		return "not ok";
	}
	return "invalid";
}


#define KUNIT_CASE(test_name)			\
		{ .run_case = test_name, .name = #test_name,	\
		  .module_name = KBUILD_MODNAME}


#define KUNIT_CASE_ATTR(test_name, attributes)			\
		{ .run_case = test_name, .name = #test_name,	\
		  .attr = attributes, .module_name = KBUILD_MODNAME}



#define KUNIT_CASE_SLOW(test_name)			\
		{ .run_case = test_name, .name = #test_name,	\
		  .attr.speed = KUNIT_SPEED_SLOW, .module_name = KBUILD_MODNAME}


#define KUNIT_CASE_PARAM(test_name, gen_params)			\
		{ .run_case = test_name, .name = #test_name,	\
		  .generate_params = gen_params, .module_name = KBUILD_MODNAME}


#define KUNIT_CASE_PARAM_ATTR(test_name, gen_params, attributes)	\
		{ .run_case = test_name, .name = #test_name,	\
		  .generate_params = gen_params,				\
		  .attr = attributes, .module_name = KBUILD_MODNAME}


struct kunit_suite {
	const char name[256];
	int (*suite_init)(struct kunit_suite *suite);
	void (*suite_exit)(struct kunit_suite *suite);
	int (*init)(struct kunit *test);
	void (*exit)(struct kunit *test);
	struct kunit_case *test_cases;
	struct kunit_attributes attr;

	
	char status_comment[KUNIT_STATUS_COMMENT_SIZE];
	struct dentry *debugfs;
	char *log;
	int suite_init_err;
};


struct kunit_suite_set {
	struct kunit_suite * const *start;
	struct kunit_suite * const *end;
};


struct kunit {
	void *priv;

	
	const char *name; 
	char *log; 
	struct kunit_try_catch try_catch;
	
	const void *param_value;
	
	int param_index;
	
	spinlock_t lock; 
	enum kunit_status status; 
	
	struct list_head resources; 

	char status_comment[KUNIT_STATUS_COMMENT_SIZE];
};

static inline void kunit_set_failure(struct kunit *test)
{
	WRITE_ONCE(test->status, KUNIT_FAILURE);
}

bool kunit_enabled(void);
const char *kunit_action(void);
const char *kunit_filter_glob(void);
char *kunit_filter(void);
char *kunit_filter_action(void);

void kunit_init_test(struct kunit *test, const char *name, char *log);

int kunit_run_tests(struct kunit_suite *suite);

size_t kunit_suite_num_test_cases(struct kunit_suite *suite);

unsigned int kunit_test_case_num(struct kunit_suite *suite,
				 struct kunit_case *test_case);

struct kunit_suite_set
kunit_filter_suites(const struct kunit_suite_set *suite_set,
		    const char *filter_glob,
		    char *filters,
		    char *filter_action,
		    int *err);
void kunit_free_suite_set(struct kunit_suite_set suite_set);

int __kunit_test_suites_init(struct kunit_suite * const * const suites, int num_suites);

void __kunit_test_suites_exit(struct kunit_suite **suites, int num_suites);

void kunit_exec_run_tests(struct kunit_suite_set *suite_set, bool builtin);
void kunit_exec_list_tests(struct kunit_suite_set *suite_set, bool include_attr);

#if IS_BUILTIN(CONFIG_KUNIT)
int kunit_run_all_tests(void);
#else
static inline int kunit_run_all_tests(void)
{
	return 0;
}
#endif 

#define __kunit_test_suites(unique_array, ...)				       \
	static struct kunit_suite *unique_array[]			       \
	__aligned(sizeof(struct kunit_suite *))				       \
	__used __section(".kunit_test_suites") = { __VA_ARGS__ }


#define kunit_test_suites(__suites...)						\
	__kunit_test_suites(__UNIQUE_ID(array),				\
			    ##__suites)

#define kunit_test_suite(suite)	kunit_test_suites(&suite)


#define kunit_test_init_section_suites(__suites...)			\
	__kunit_test_suites(CONCATENATE(__UNIQUE_ID(array), _probe),	\
			    ##__suites)

#define kunit_test_init_section_suite(suite)	\
	kunit_test_init_section_suites(&suite)

#define kunit_suite_for_each_test_case(suite, test_case)		\
	for (test_case = suite->test_cases; test_case->run_case; test_case++)

enum kunit_status kunit_suite_has_succeeded(struct kunit_suite *suite);


void *kunit_kmalloc_array(struct kunit *test, size_t n, size_t size, gfp_t gfp);


static inline void *kunit_kmalloc(struct kunit *test, size_t size, gfp_t gfp)
{
	return kunit_kmalloc_array(test, 1, size, gfp);
}


void kunit_kfree(struct kunit *test, const void *ptr);


static inline void *kunit_kzalloc(struct kunit *test, size_t size, gfp_t gfp)
{
	return kunit_kmalloc(test, size, gfp | __GFP_ZERO);
}


static inline void *kunit_kcalloc(struct kunit *test, size_t n, size_t size, gfp_t gfp)
{
	return kunit_kmalloc_array(test, n, size, gfp | __GFP_ZERO);
}

void kunit_cleanup(struct kunit *test);

void __printf(2, 3) kunit_log_append(char *log, const char *fmt, ...);


#define kunit_mark_skipped(test_or_suite, fmt, ...)			\
	do {								\
		WRITE_ONCE((test_or_suite)->status, KUNIT_SKIPPED);	\
		scnprintf((test_or_suite)->status_comment,		\
			  KUNIT_STATUS_COMMENT_SIZE,			\
			  fmt, ##__VA_ARGS__);				\
	} while (0)


#define kunit_skip(test_or_suite, fmt, ...)				\
	do {								\
		kunit_mark_skipped((test_or_suite), fmt, ##__VA_ARGS__);\
		kunit_try_catch_throw(&((test_or_suite)->try_catch));	\
	} while (0)


#define kunit_log(lvl, test_or_suite, fmt, ...)				\
	do {								\
		printk(lvl fmt, ##__VA_ARGS__);				\
		kunit_log_append((test_or_suite)->log,	fmt,		\
				 ##__VA_ARGS__);			\
	} while (0)

#define kunit_printk(lvl, test, fmt, ...)				\
	kunit_log(lvl, test, KUNIT_SUBTEST_INDENT "# %s: " fmt,		\
		  (test)->name,	##__VA_ARGS__)


#define kunit_info(test, fmt, ...) \
	kunit_printk(KERN_INFO, test, fmt, ##__VA_ARGS__)


#define kunit_warn(test, fmt, ...) \
	kunit_printk(KERN_WARNING, test, fmt, ##__VA_ARGS__)


#define kunit_err(test, fmt, ...) \
	kunit_printk(KERN_ERR, test, fmt, ##__VA_ARGS__)


#define KUNIT_SUCCEED(test) do {} while (0)

void __noreturn __kunit_abort(struct kunit *test);

void __kunit_do_failed_assertion(struct kunit *test,
			       const struct kunit_loc *loc,
			       enum kunit_assert_type type,
			       const struct kunit_assert *assert,
			       assert_format_t assert_format,
			       const char *fmt, ...);

#define _KUNIT_FAILED(test, assert_type, assert_class, assert_format, INITIALIZER, fmt, ...) do { \
	static const struct kunit_loc __loc = KUNIT_CURRENT_LOC;	       \
	const struct assert_class __assertion = INITIALIZER;		       \
	__kunit_do_failed_assertion(test,				       \
				    &__loc,				       \
				    assert_type,			       \
				    &__assertion.assert,		       \
				    assert_format,			       \
				    fmt,				       \
				    ##__VA_ARGS__);			       \
	if (assert_type == KUNIT_ASSERTION)				       \
		__kunit_abort(test);					       \
} while (0)


#define KUNIT_FAIL_ASSERTION(test, assert_type, fmt, ...)		       \
	_KUNIT_FAILED(test,						       \
		      assert_type,					       \
		      kunit_fail_assert,				       \
		      kunit_fail_assert_format,				       \
		      {},						       \
		      fmt,						       \
		      ##__VA_ARGS__)


#define KUNIT_FAIL(test, fmt, ...)					       \
	KUNIT_FAIL_ASSERTION(test,					       \
			     KUNIT_EXPECTATION,				       \
			     fmt,					       \
			     ##__VA_ARGS__)


#define KUNIT_INIT_ASSERT(initializers...) { initializers }

#define KUNIT_UNARY_ASSERTION(test,					       \
			      assert_type,				       \
			      condition_,				       \
			      expected_true_,				       \
			      fmt,					       \
			      ...)					       \
do {									       \
	if (likely(!!(condition_) == !!expected_true_))			       \
		break;							       \
									       \
	_KUNIT_FAILED(test,						       \
		      assert_type,					       \
		      kunit_unary_assert,				       \
		      kunit_unary_assert_format,			       \
		      KUNIT_INIT_ASSERT(.condition = #condition_,	       \
					.expected_true = expected_true_),      \
		      fmt,						       \
		      ##__VA_ARGS__);					       \
} while (0)

#define KUNIT_TRUE_MSG_ASSERTION(test, assert_type, condition, fmt, ...)       \
	KUNIT_UNARY_ASSERTION(test,					       \
			      assert_type,				       \
			      condition,				       \
			      true,					       \
			      fmt,					       \
			      ##__VA_ARGS__)

#define KUNIT_FALSE_MSG_ASSERTION(test, assert_type, condition, fmt, ...)      \
	KUNIT_UNARY_ASSERTION(test,					       \
			      assert_type,				       \
			      condition,				       \
			      false,					       \
			      fmt,					       \
			      ##__VA_ARGS__)


#define KUNIT_BASE_BINARY_ASSERTION(test,				       \
				    assert_class,			       \
				    format_func,			       \
				    assert_type,			       \
				    left,				       \
				    op,					       \
				    right,				       \
				    fmt,				       \
				    ...)				       \
do {									       \
	const typeof(left) __left = (left);				       \
	const typeof(right) __right = (right);				       \
	static const struct kunit_binary_assert_text __text = {		       \
		.operation = #op,					       \
		.left_text = #left,					       \
		.right_text = #right,					       \
	};								       \
									       \
	if (likely(__left op __right))					       \
		break;							       \
									       \
	_KUNIT_FAILED(test,						       \
		      assert_type,					       \
		      assert_class,					       \
		      format_func,					       \
		      KUNIT_INIT_ASSERT(.text = &__text,		       \
					.left_value = __left,		       \
					.right_value = __right),	       \
		      fmt,						       \
		      ##__VA_ARGS__);					       \
} while (0)

#define KUNIT_BINARY_INT_ASSERTION(test,				       \
				   assert_type,				       \
				   left,				       \
				   op,					       \
				   right,				       \
				   fmt,					       \
				    ...)				       \
	KUNIT_BASE_BINARY_ASSERTION(test,				       \
				    kunit_binary_assert,		       \
				    kunit_binary_assert_format,		       \
				    assert_type,			       \
				    left, op, right,			       \
				    fmt,				       \
				    ##__VA_ARGS__)

#define KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   assert_type,				       \
				   left,				       \
				   op,					       \
				   right,				       \
				   fmt,					       \
				    ...)				       \
	KUNIT_BASE_BINARY_ASSERTION(test,				       \
				    kunit_binary_ptr_assert,		       \
				    kunit_binary_ptr_assert_format,	       \
				    assert_type,			       \
				    left, op, right,			       \
				    fmt,				       \
				    ##__VA_ARGS__)

#define KUNIT_BINARY_STR_ASSERTION(test,				       \
				   assert_type,				       \
				   left,				       \
				   op,					       \
				   right,				       \
				   fmt,					       \
				   ...)					       \
do {									       \
	const char *__left = (left);					       \
	const char *__right = (right);					       \
	static const struct kunit_binary_assert_text __text = {		       \
		.operation = #op,					       \
		.left_text = #left,					       \
		.right_text = #right,					       \
	};								       \
									       \
	if (likely(strcmp(__left, __right) op 0))			       \
		break;							       \
									       \
									       \
	_KUNIT_FAILED(test,						       \
		      assert_type,					       \
		      kunit_binary_str_assert,				       \
		      kunit_binary_str_assert_format,			       \
		      KUNIT_INIT_ASSERT(.text = &__text,		       \
					.left_value = __left,		       \
					.right_value = __right),	       \
		      fmt,						       \
		      ##__VA_ARGS__);					       \
} while (0)

#define KUNIT_MEM_ASSERTION(test,					       \
			    assert_type,				       \
			    left,					       \
			    op,						       \
			    right,					       \
			    size_,					       \
			    fmt,					       \
			    ...)					       \
do {									       \
	const void *__left = (left);					       \
	const void *__right = (right);					       \
	const size_t __size = (size_);					       \
	static const struct kunit_binary_assert_text __text = {		       \
		.operation = #op,					       \
		.left_text = #left,					       \
		.right_text = #right,					       \
	};								       \
									       \
	if (likely(__left && __right))					       \
		if (likely(memcmp(__left, __right, __size) op 0))	       \
			break;						       \
									       \
	_KUNIT_FAILED(test,						       \
		      assert_type,					       \
		      kunit_mem_assert,					       \
		      kunit_mem_assert_format,				       \
		      KUNIT_INIT_ASSERT(.text = &__text,		       \
					.left_value = __left,		       \
					.right_value = __right,		       \
					.size = __size),		       \
		      fmt,						       \
		      ##__VA_ARGS__);					       \
} while (0)

#define KUNIT_PTR_NOT_ERR_OR_NULL_MSG_ASSERTION(test,			       \
						assert_type,		       \
						ptr,			       \
						fmt,			       \
						...)			       \
do {									       \
	const typeof(ptr) __ptr = (ptr);				       \
									       \
	if (!IS_ERR_OR_NULL(__ptr))					       \
		break;							       \
									       \
	_KUNIT_FAILED(test,						       \
		      assert_type,					       \
		      kunit_ptr_not_err_assert,				       \
		      kunit_ptr_not_err_assert_format,			       \
		      KUNIT_INIT_ASSERT(.text = #ptr, .value = __ptr),	       \
		      fmt,						       \
		      ##__VA_ARGS__);					       \
} while (0)


#define KUNIT_EXPECT_TRUE(test, condition) \
	KUNIT_EXPECT_TRUE_MSG(test, condition, NULL)

#define KUNIT_EXPECT_TRUE_MSG(test, condition, fmt, ...)		       \
	KUNIT_TRUE_MSG_ASSERTION(test,					       \
				 KUNIT_EXPECTATION,			       \
				 condition,				       \
				 fmt,					       \
				 ##__VA_ARGS__)


#define KUNIT_EXPECT_FALSE(test, condition) \
	KUNIT_EXPECT_FALSE_MSG(test, condition, NULL)

#define KUNIT_EXPECT_FALSE_MSG(test, condition, fmt, ...)		       \
	KUNIT_FALSE_MSG_ASSERTION(test,					       \
				  KUNIT_EXPECTATION,			       \
				  condition,				       \
				  fmt,					       \
				  ##__VA_ARGS__)


#define KUNIT_EXPECT_EQ(test, left, right) \
	KUNIT_EXPECT_EQ_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_EQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, ==, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_EXPECT_PTR_EQ(test, left, right)				       \
	KUNIT_EXPECT_PTR_EQ_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_PTR_EQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, ==, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_EXPECT_NE(test, left, right) \
	KUNIT_EXPECT_NE_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_NE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, !=, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_EXPECT_PTR_NE(test, left, right)				       \
	KUNIT_EXPECT_PTR_NE_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_PTR_NE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, !=, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_EXPECT_LT(test, left, right) \
	KUNIT_EXPECT_LT_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_LT_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, <, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_EXPECT_LE(test, left, right) \
	KUNIT_EXPECT_LE_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_LE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, <=, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_EXPECT_GT(test, left, right) \
	KUNIT_EXPECT_GT_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_GT_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, >, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_EXPECT_GE(test, left, right) \
	KUNIT_EXPECT_GE_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_GE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, >=, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_EXPECT_STREQ(test, left, right) \
	KUNIT_EXPECT_STREQ_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_STREQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_STR_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, ==, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_EXPECT_STRNEQ(test, left, right) \
	KUNIT_EXPECT_STRNEQ_MSG(test, left, right, NULL)

#define KUNIT_EXPECT_STRNEQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_STR_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   left, !=, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_EXPECT_MEMEQ(test, left, right, size) \
	KUNIT_EXPECT_MEMEQ_MSG(test, left, right, size, NULL)

#define KUNIT_EXPECT_MEMEQ_MSG(test, left, right, size, fmt, ...)	       \
	KUNIT_MEM_ASSERTION(test,					       \
			    KUNIT_EXPECTATION,				       \
			    left, ==, right,				       \
			    size,					       \
			    fmt,					       \
			    ##__VA_ARGS__)


#define KUNIT_EXPECT_MEMNEQ(test, left, right, size) \
	KUNIT_EXPECT_MEMNEQ_MSG(test, left, right, size, NULL)

#define KUNIT_EXPECT_MEMNEQ_MSG(test, left, right, size, fmt, ...)	       \
	KUNIT_MEM_ASSERTION(test,					       \
			    KUNIT_EXPECTATION,				       \
			    left, !=, right,				       \
			    size,					       \
			    fmt,					       \
			    ##__VA_ARGS__)


#define KUNIT_EXPECT_NULL(test, ptr)				               \
	KUNIT_EXPECT_NULL_MSG(test,					       \
			      ptr,					       \
			      NULL)

#define KUNIT_EXPECT_NULL_MSG(test, ptr, fmt, ...)	                       \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   ptr, ==, NULL,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_EXPECT_NOT_NULL(test, ptr)			               \
	KUNIT_EXPECT_NOT_NULL_MSG(test,					       \
				  ptr,					       \
				  NULL)

#define KUNIT_EXPECT_NOT_NULL_MSG(test, ptr, fmt, ...)	                       \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_EXPECTATION,			       \
				   ptr, !=, NULL,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_EXPECT_NOT_ERR_OR_NULL(test, ptr) \
	KUNIT_EXPECT_NOT_ERR_OR_NULL_MSG(test, ptr, NULL)

#define KUNIT_EXPECT_NOT_ERR_OR_NULL_MSG(test, ptr, fmt, ...)		       \
	KUNIT_PTR_NOT_ERR_OR_NULL_MSG_ASSERTION(test,			       \
						KUNIT_EXPECTATION,	       \
						ptr,			       \
						fmt,			       \
						##__VA_ARGS__)

#define KUNIT_ASSERT_FAILURE(test, fmt, ...) \
	KUNIT_FAIL_ASSERTION(test, KUNIT_ASSERTION, fmt, ##__VA_ARGS__)


#define KUNIT_ASSERT_TRUE(test, condition) \
	KUNIT_ASSERT_TRUE_MSG(test, condition, NULL)

#define KUNIT_ASSERT_TRUE_MSG(test, condition, fmt, ...)		       \
	KUNIT_TRUE_MSG_ASSERTION(test,					       \
				 KUNIT_ASSERTION,			       \
				 condition,				       \
				 fmt,					       \
				 ##__VA_ARGS__)


#define KUNIT_ASSERT_FALSE(test, condition) \
	KUNIT_ASSERT_FALSE_MSG(test, condition, NULL)

#define KUNIT_ASSERT_FALSE_MSG(test, condition, fmt, ...)		       \
	KUNIT_FALSE_MSG_ASSERTION(test,					       \
				  KUNIT_ASSERTION,			       \
				  condition,				       \
				  fmt,					       \
				  ##__VA_ARGS__)


#define KUNIT_ASSERT_EQ(test, left, right) \
	KUNIT_ASSERT_EQ_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_EQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, ==, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_ASSERT_PTR_EQ(test, left, right) \
	KUNIT_ASSERT_PTR_EQ_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_PTR_EQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, ==, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_ASSERT_NE(test, left, right) \
	KUNIT_ASSERT_NE_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_NE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, !=, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_ASSERT_PTR_NE(test, left, right) \
	KUNIT_ASSERT_PTR_NE_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_PTR_NE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, !=, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)

#define KUNIT_ASSERT_LT(test, left, right) \
	KUNIT_ASSERT_LT_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_LT_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, <, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)

#define KUNIT_ASSERT_LE(test, left, right) \
	KUNIT_ASSERT_LE_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_LE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, <=, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_ASSERT_GT(test, left, right) \
	KUNIT_ASSERT_GT_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_GT_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, >, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_ASSERT_GE(test, left, right) \
	KUNIT_ASSERT_GE_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_GE_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_INT_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, >=, right,			       \
				   fmt,					       \
				    ##__VA_ARGS__)


#define KUNIT_ASSERT_STREQ(test, left, right) \
	KUNIT_ASSERT_STREQ_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_STREQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_STR_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, ==, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_ASSERT_STRNEQ(test, left, right) \
	KUNIT_ASSERT_STRNEQ_MSG(test, left, right, NULL)

#define KUNIT_ASSERT_STRNEQ_MSG(test, left, right, fmt, ...)		       \
	KUNIT_BINARY_STR_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   left, !=, right,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_ASSERT_NULL(test, ptr) \
	KUNIT_ASSERT_NULL_MSG(test,					       \
			      ptr,					       \
			      NULL)

#define KUNIT_ASSERT_NULL_MSG(test, ptr, fmt, ...) \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   ptr, ==, NULL,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_ASSERT_NOT_NULL(test, ptr) \
	KUNIT_ASSERT_NOT_NULL_MSG(test,					       \
				  ptr,					       \
				  NULL)

#define KUNIT_ASSERT_NOT_NULL_MSG(test, ptr, fmt, ...) \
	KUNIT_BINARY_PTR_ASSERTION(test,				       \
				   KUNIT_ASSERTION,			       \
				   ptr, !=, NULL,			       \
				   fmt,					       \
				   ##__VA_ARGS__)


#define KUNIT_ASSERT_NOT_ERR_OR_NULL(test, ptr) \
	KUNIT_ASSERT_NOT_ERR_OR_NULL_MSG(test, ptr, NULL)

#define KUNIT_ASSERT_NOT_ERR_OR_NULL_MSG(test, ptr, fmt, ...)		       \
	KUNIT_PTR_NOT_ERR_OR_NULL_MSG_ASSERTION(test,			       \
						KUNIT_ASSERTION,	       \
						ptr,			       \
						fmt,			       \
						##__VA_ARGS__)


#define KUNIT_ARRAY_PARAM(name, array, get_desc)						\
	static const void *name##_gen_params(const void *prev, char *desc)			\
	{											\
		typeof((array)[0]) *__next = prev ? ((typeof(__next)) prev) + 1 : (array);	\
		if (__next - (array) < ARRAY_SIZE((array))) {					\
			void (*__get_desc)(typeof(__next), char *) = get_desc;			\
			if (__get_desc)								\
				__get_desc(__next, desc);					\
			return __next;								\
		}										\
		return NULL;									\
	}

// TODO(dlatypov@google.com): consider eventually migrating users to explicitly
// include resource.h themselves if they need it.
#include <kunit/resource.h>

#endif 
