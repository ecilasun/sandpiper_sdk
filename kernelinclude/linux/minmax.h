/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MINMAX_H
#define _LINUX_MINMAX_H

#include <linux/build_bug.h>
#include <linux/compiler.h>
#include <linux/const.h>
#include <linux/types.h>


#define __typecheck(x, y) \
	(!!(sizeof((typeof(x) *)1 == (typeof(y) *)1)))


#define __is_signed(x) 								\
	__builtin_choose_expr(__is_constexpr(is_signed_type(typeof(x))),	\
		is_signed_type(typeof(x)), 0)


#define __is_noneg_int(x)	\
	(__builtin_choose_expr(__is_constexpr(x) && __is_signed(x), x, -1) >= 0)

#define __types_ok(x, y) 					\
	(__is_signed(x) == __is_signed(y) ||			\
		__is_signed((x) + 0) == __is_signed((y) + 0) ||	\
		__is_noneg_int(x) || __is_noneg_int(y))

#define __cmp_op_min <
#define __cmp_op_max >

#define __cmp(op, x, y)	((x) __cmp_op_##op (y) ? (x) : (y))

#define __cmp_once(op, x, y, unique_x, unique_y) ({	\
	typeof(x) unique_x = (x);			\
	typeof(y) unique_y = (y);			\
	static_assert(__types_ok(x, y),			\
		#op "(" #x ", " #y ") signedness error, fix types or consider u" #op "() before " #op "_t()"); \
	__cmp(op, unique_x, unique_y); })

#define __careful_cmp(op, x, y)					\
	__builtin_choose_expr(__is_constexpr((x) - (y)),	\
		__cmp(op, x, y),				\
		__cmp_once(op, x, y, __UNIQUE_ID(__x), __UNIQUE_ID(__y)))

#define __clamp(val, lo, hi)	\
	((val) >= (hi) ? (hi) : ((val) <= (lo) ? (lo) : (val)))

#define __clamp_once(val, lo, hi, unique_val, unique_lo, unique_hi) ({		\
	typeof(val) unique_val = (val);						\
	typeof(lo) unique_lo = (lo);						\
	typeof(hi) unique_hi = (hi);						\
	static_assert(__builtin_choose_expr(__is_constexpr((lo) > (hi)), 	\
			(lo) <= (hi), true),					\
		"clamp() low limit " #lo " greater than high limit " #hi);	\
	static_assert(__types_ok(val, lo), "clamp() 'lo' signedness error");	\
	static_assert(__types_ok(val, hi), "clamp() 'hi' signedness error");	\
	__clamp(unique_val, unique_lo, unique_hi); })

#define __careful_clamp(val, lo, hi) ({					\
	__builtin_choose_expr(__is_constexpr((val) - (lo) + (hi)),	\
		__clamp(val, lo, hi),					\
		__clamp_once(val, lo, hi, __UNIQUE_ID(__val),		\
			     __UNIQUE_ID(__lo), __UNIQUE_ID(__hi))); })


#define min(x, y)	__careful_cmp(min, x, y)


#define max(x, y)	__careful_cmp(max, x, y)


#define umin(x, y)	\
	__careful_cmp(min, (x) + 0u + 0ul + 0ull, (y) + 0u + 0ul + 0ull)


#define umax(x, y)	\
	__careful_cmp(max, (x) + 0u + 0ul + 0ull, (y) + 0u + 0ul + 0ull)


#define min3(x, y, z) min((typeof(x))min(x, y), z)


#define max3(x, y, z) max((typeof(x))max(x, y), z)


#define min_not_zero(x, y) ({			\
	typeof(x) __x = (x);			\
	typeof(y) __y = (y);			\
	__x == 0 ? __y : ((__y == 0) ? __x : min(__x, __y)); })


#define clamp(val, lo, hi) __careful_clamp(val, lo, hi)




#define min_t(type, x, y)	__careful_cmp(min, (type)(x), (type)(y))


#define max_t(type, x, y)	__careful_cmp(max, (type)(x), (type)(y))


#define __minmax_array(op, array, len) ({				\
	typeof(&(array)[0]) __array = (array);				\
	typeof(len) __len = (len);					\
	__unqual_scalar_typeof(__array[0]) __element = __array[--__len];\
	while (__len--)							\
		__element = op(__element, __array[__len]);		\
	__element; })


#define min_array(array, len) __minmax_array(min, array, len)


#define max_array(array, len) __minmax_array(max, array, len)


#define clamp_t(type, val, lo, hi) __careful_clamp((type)(val), (type)(lo), (type)(hi))


#define clamp_val(val, lo, hi) clamp_t(typeof(val), val, lo, hi)

static inline bool in_range64(u64 val, u64 start, u64 len)
{
	return (val - start) < len;
}

static inline bool in_range32(u32 val, u32 start, u32 len)
{
	return (val - start) < len;
}


#define in_range(val, start, len)					\
	((sizeof(start) | sizeof(len) | sizeof(val)) <= sizeof(u32) ?	\
		in_range32(val, start, len) : in_range64(val, start, len))


#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

#endif	
