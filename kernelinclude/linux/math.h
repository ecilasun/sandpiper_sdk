/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MATH_H
#define _LINUX_MATH_H

#include <linux/types.h>
#include <asm/div64.h>
#include <uapi/linux/kernel.h>


#define __round_mask(x, y) ((__typeof__(x))((y)-1))


#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)


#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define DIV_ROUND_UP __KERNEL_DIV_ROUND_UP

#define DIV_ROUND_DOWN_ULL(ll, d) \
	({ unsigned long long _tmp = (ll); do_div(_tmp, d); _tmp; })

#define DIV_ROUND_UP_ULL(ll, d) \
	DIV_ROUND_DOWN_ULL((unsigned long long)(ll) + (d) - 1, (d))

#if BITS_PER_LONG == 32
# define DIV_ROUND_UP_SECTOR_T(ll,d) DIV_ROUND_UP_ULL(ll, d)
#else
# define DIV_ROUND_UP_SECTOR_T(ll,d) DIV_ROUND_UP(ll,d)
#endif


#define roundup(x, y) (					\
{							\
	typeof(y) __y = y;				\
	(((x) + (__y - 1)) / __y) * __y;		\
}							\
)

#define rounddown(x, y) (				\
{							\
	typeof(x) __x = (x);				\
	__x - (__x % (y));				\
}							\
)


#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(x) __x = x;				\
	typeof(divisor) __d = divisor;			\
	(((typeof(x))-1) > 0 ||				\
	 ((typeof(divisor))-1) > 0 ||			\
	 (((__x) > 0) == ((__d) > 0))) ?		\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
}							\
)

#define DIV_ROUND_CLOSEST_ULL(x, divisor)(		\
{							\
	typeof(divisor) __d = divisor;			\
	unsigned long long _tmp = (x) + (__d) / 2;	\
	do_div(_tmp, __d);				\
	_tmp;						\
}							\
)

#define __STRUCT_FRACT(type)				\
struct type##_fract {					\
	__##type numerator;				\
	__##type denominator;				\
};
__STRUCT_FRACT(s16)
__STRUCT_FRACT(u16)
__STRUCT_FRACT(s32)
__STRUCT_FRACT(u32)
#undef __STRUCT_FRACT


#define mult_frac(x, n, d)	\
({				\
	typeof(x) x_ = (x);	\
	typeof(n) n_ = (n);	\
	typeof(d) d_ = (d);	\
				\
	typeof(x_) q = x_ / d_;	\
	typeof(x_) r = x_ % d_;	\
	q * n_ + r * n_ / d_;	\
})

#define sector_div(a, b) do_div(a, b)


#define abs(x)	__abs_choose_expr(x, long long,				\
		__abs_choose_expr(x, long,				\
		__abs_choose_expr(x, int,				\
		__abs_choose_expr(x, short,				\
		__abs_choose_expr(x, char,				\
		__builtin_choose_expr(					\
			__builtin_types_compatible_p(typeof(x), char),	\
			(char)({ signed char __x = (x); __x<0?-__x:__x; }), \
			((void)0)))))))

#define __abs_choose_expr(x, type, other) __builtin_choose_expr(	\
	__builtin_types_compatible_p(typeof(x),   signed type) ||	\
	__builtin_types_compatible_p(typeof(x), unsigned type),		\
	({ signed type __x = (x); __x < 0 ? -__x : __x; }), other)


#define abs_diff(a, b) ({			\
	typeof(a) __a = (a);			\
	typeof(b) __b = (b);			\
	(void)(&__a == &__b);			\
	__a > __b ? (__a - __b) : (__b - __a);	\
})


static inline u32 reciprocal_scale(u32 val, u32 ep_ro)
{
	return (u32)(((u64) val * ep_ro) >> 32);
}

u64 int_pow(u64 base, unsigned int exp);
unsigned long int_sqrt(unsigned long);

#if BITS_PER_LONG < 64
u32 int_sqrt64(u64 x);
#else
static inline u32 int_sqrt64(u64 x)
{
	return (u32)int_sqrt(x);
}
#endif

#endif	
