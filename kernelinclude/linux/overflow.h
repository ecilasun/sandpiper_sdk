/* SPDX-License-Identifier: GPL-2.0 OR MIT */
#ifndef __LINUX_OVERFLOW_H
#define __LINUX_OVERFLOW_H

#include <linux/compiler.h>
#include <linux/limits.h>
#include <linux/const.h>


#define __type_half_max(type) ((type)1 << (8*sizeof(type) - 1 - is_signed_type(type)))
#define __type_max(T) ((T)((__type_half_max(T) - 1) + __type_half_max(T)))
#define type_max(t)	__type_max(typeof(t))
#define __type_min(T) ((T)((T)-type_max(T)-(T)1))
#define type_min(t)	__type_min(typeof(t))


#define is_non_negative(a) ((a) > 0 || (a) == 0)
#define is_negative(a) (!(is_non_negative(a)))


static inline bool __must_check __must_check_overflow(bool overflow)
{
	return unlikely(overflow);
}


#define check_add_overflow(a, b, d)	\
	__must_check_overflow(__builtin_add_overflow(a, b, d))


#define check_sub_overflow(a, b, d)	\
	__must_check_overflow(__builtin_sub_overflow(a, b, d))


#define check_mul_overflow(a, b, d)	\
	__must_check_overflow(__builtin_mul_overflow(a, b, d))


#define check_shl_overflow(a, s, d) __must_check_overflow(({		\
	typeof(a) _a = a;						\
	typeof(s) _s = s;						\
	typeof(d) _d = d;						\
	u64 _a_full = _a;						\
	unsigned int _to_shift =					\
		is_non_negative(_s) && _s < 8 * sizeof(*d) ? _s : 0;	\
	*_d = (_a_full << _to_shift);					\
	(_to_shift != _s || is_negative(*_d) || is_negative(_a) ||	\
	(*_d >> _to_shift) != _a);					\
}))

#define __overflows_type_constexpr(x, T) (			\
	is_unsigned_type(typeof(x)) ?				\
		(x) > type_max(T) :				\
	is_unsigned_type(typeof(T)) ?				\
		(x) < 0 || (x) > type_max(T) :			\
	(x) < type_min(T) || (x) > type_max(T))

#define __overflows_type(x, T)		({	\
	typeof(T) v = 0;			\
	check_add_overflow((x), v, &v);		\
})


#define overflows_type(n, T)					\
	__builtin_choose_expr(__is_constexpr(n),		\
			      __overflows_type_constexpr(n, T),	\
			      __overflows_type(n, T))


#define castable_to_type(n, T)						\
	__builtin_choose_expr(__is_constexpr(n),			\
			      !__overflows_type_constexpr(n, T),	\
			      __same_type(n, T))


static inline size_t __must_check size_mul(size_t factor1, size_t factor2)
{
	size_t bytes;

	if (check_mul_overflow(factor1, factor2, &bytes))
		return SIZE_MAX;

	return bytes;
}


static inline size_t __must_check size_add(size_t addend1, size_t addend2)
{
	size_t bytes;

	if (check_add_overflow(addend1, addend2, &bytes))
		return SIZE_MAX;

	return bytes;
}


static inline size_t __must_check size_sub(size_t minuend, size_t subtrahend)
{
	size_t bytes;

	if (minuend == SIZE_MAX || subtrahend == SIZE_MAX ||
	    check_sub_overflow(minuend, subtrahend, &bytes))
		return SIZE_MAX;

	return bytes;
}


#define array_size(a, b)	size_mul(a, b)


#define array3_size(a, b, c)	size_mul(size_mul(a, b), c)


#define flex_array_size(p, member, count)				\
	__builtin_choose_expr(__is_constexpr(count),			\
		(count) * sizeof(*(p)->member) + __must_be_array((p)->member),	\
		size_mul(count, sizeof(*(p)->member) + __must_be_array((p)->member)))


#define struct_size(p, member, count)					\
	__builtin_choose_expr(__is_constexpr(count),			\
		sizeof(*(p)) + flex_array_size(p, member, count),	\
		size_add(sizeof(*(p)), flex_array_size(p, member, count)))


#define struct_size_t(type, member, count)					\
	struct_size((type *)NULL, member, count)

#endif 
