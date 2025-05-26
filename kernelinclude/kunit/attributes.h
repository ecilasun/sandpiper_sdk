/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _KUNIT_ATTRIBUTES_H
#define _KUNIT_ATTRIBUTES_H


struct kunit_attr_filter {
	struct kunit_attr *attr;
	char *input;
};


const char *kunit_attr_filter_name(struct kunit_attr_filter filter);


void kunit_print_attr(void *test_or_suite, bool is_test, unsigned int test_level);


int kunit_get_filter_count(char *input);


struct kunit_attr_filter kunit_next_attr_filter(char **filters, int *err);


struct kunit_suite *kunit_filter_attr_tests(const struct kunit_suite *const suite,
		struct kunit_attr_filter filter, char *action, int *err);

#endif 
