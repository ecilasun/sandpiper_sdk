/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_BITOPS_H
#define _LINUX_BITOPS_H

#include <asm/types.h>
#include <linux/bits.h>
#include <linux/typecheck.h>

#include <uapi/linux/kernel.h>


#ifdef __LITTLE_ENDIAN
#  define aligned_byte_mask(n) ((1UL << 8*(n))-1)
#else
#  define aligned_byte_mask(n) (~0xffUL << (BITS_PER_LONG - 8 - 8*(n)))
#endif

#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
#define BITS_TO_U64(nr)		__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u64))
#define BITS_TO_U32(nr)		__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u32))
#define BITS_TO_BYTES(nr)	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(char))

extern unsigned int __sw_hweight8(unsigned int w);
extern unsigned int __sw_hweight16(unsigned int w);
extern unsigned int __sw_hweight32(unsigned int w);
extern unsigned long __sw_hweight64(__u64 w);



#include <asm-generic/bitops/generic-non-atomic.h>


#define bitop(op, nr, addr)						\
	((__builtin_constant_p(nr) &&					\
	  __builtin_constant_p((uintptr_t)(addr) != (uintptr_t)NULL) &&	\
	  (uintptr_t)(addr) != (uintptr_t)NULL &&			\
	  __builtin_constant_p(*(const unsigned long *)(addr))) ?	\
	 const##op(nr, addr) : op(nr, addr))

#define __set_bit(nr, addr)		bitop(___set_bit, nr, addr)
#define __clear_bit(nr, addr)		bitop(___clear_bit, nr, addr)
#define __change_bit(nr, addr)		bitop(___change_bit, nr, addr)
#define __test_and_set_bit(nr, addr)	bitop(___test_and_set_bit, nr, addr)
#define __test_and_clear_bit(nr, addr)	bitop(___test_and_clear_bit, nr, addr)
#define __test_and_change_bit(nr, addr)	bitop(___test_and_change_bit, nr, addr)
#define test_bit(nr, addr)		bitop(_test_bit, nr, addr)
#define test_bit_acquire(nr, addr)	bitop(_test_bit_acquire, nr, addr)


#include <asm/bitops.h>


#define __check_bitop_pr(name)						\
	static_assert(__same_type(arch_##name, generic_##name) &&	\
		      __same_type(const_##name, generic_##name) &&	\
		      __same_type(_##name, generic_##name))

__check_bitop_pr(__set_bit);
__check_bitop_pr(__clear_bit);
__check_bitop_pr(__change_bit);
__check_bitop_pr(__test_and_set_bit);
__check_bitop_pr(__test_and_clear_bit);
__check_bitop_pr(__test_and_change_bit);
__check_bitop_pr(test_bit);
__check_bitop_pr(test_bit_acquire);

#undef __check_bitop_pr

static inline int get_bitmask_order(unsigned int count)
{
	int order;

	order = fls(count);
	return order;	
}

static __always_inline unsigned long hweight_long(unsigned long w)
{
	return sizeof(w) == 4 ? hweight32(w) : hweight64((__u64)w);
}


static inline __u64 rol64(__u64 word, unsigned int shift)
{
	return (word << (shift & 63)) | (word >> ((-shift) & 63));
}


static inline __u64 ror64(__u64 word, unsigned int shift)
{
	return (word >> (shift & 63)) | (word << ((-shift) & 63));
}


static inline __u32 rol32(__u32 word, unsigned int shift)
{
	return (word << (shift & 31)) | (word >> ((-shift) & 31));
}


static inline __u32 ror32(__u32 word, unsigned int shift)
{
	return (word >> (shift & 31)) | (word << ((-shift) & 31));
}


static inline __u16 rol16(__u16 word, unsigned int shift)
{
	return (word << (shift & 15)) | (word >> ((-shift) & 15));
}


static inline __u16 ror16(__u16 word, unsigned int shift)
{
	return (word >> (shift & 15)) | (word << ((-shift) & 15));
}


static inline __u8 rol8(__u8 word, unsigned int shift)
{
	return (word << (shift & 7)) | (word >> ((-shift) & 7));
}


static inline __u8 ror8(__u8 word, unsigned int shift)
{
	return (word >> (shift & 7)) | (word << ((-shift) & 7));
}


static __always_inline __s32 sign_extend32(__u32 value, int index)
{
	__u8 shift = 31 - index;
	return (__s32)(value << shift) >> shift;
}


static __always_inline __s64 sign_extend64(__u64 value, int index)
{
	__u8 shift = 63 - index;
	return (__s64)(value << shift) >> shift;
}

static inline unsigned fls_long(unsigned long l)
{
	if (sizeof(l) == 4)
		return fls(l);
	return fls64(l);
}

static inline int get_count_order(unsigned int count)
{
	if (count == 0)
		return -1;

	return fls(--count);
}


static inline int get_count_order_long(unsigned long l)
{
	if (l == 0UL)
		return -1;
	return (int)fls_long(--l);
}


static inline unsigned long __ffs64(u64 word)
{
#if BITS_PER_LONG == 32
	if (((u32)word) == 0UL)
		return __ffs((u32)(word >> 32)) + 32;
#elif BITS_PER_LONG != 64
#error BITS_PER_LONG not 32 or 64
#endif
	return __ffs((unsigned long)word);
}


static inline unsigned long fns(unsigned long word, unsigned int n)
{
	unsigned int bit;

	while (word) {
		bit = __ffs(word);
		if (n-- == 0)
			return bit;
		__clear_bit(bit, &word);
	}

	return BITS_PER_LONG;
}


static __always_inline void assign_bit(long nr, volatile unsigned long *addr,
				       bool value)
{
	if (value)
		set_bit(nr, addr);
	else
		clear_bit(nr, addr);
}

static __always_inline void __assign_bit(long nr, volatile unsigned long *addr,
					 bool value)
{
	if (value)
		__set_bit(nr, addr);
	else
		__clear_bit(nr, addr);
}


#define __ptr_set_bit(nr, addr)                         \
	({                                              \
		typecheck_pointer(*(addr));             \
		__set_bit(nr, (unsigned long *)(addr)); \
	})


#define __ptr_clear_bit(nr, addr)                         \
	({                                                \
		typecheck_pointer(*(addr));               \
		__clear_bit(nr, (unsigned long *)(addr)); \
	})


#define __ptr_test_bit(nr, addr)                       \
	({                                             \
		typecheck_pointer(*(addr));            \
		test_bit(nr, (unsigned long *)(addr)); \
	})

#ifdef __KERNEL__

#ifndef set_mask_bits
#define set_mask_bits(ptr, mask, bits)	\
({								\
	const typeof(*(ptr)) mask__ = (mask), bits__ = (bits);	\
	typeof(*(ptr)) old__, new__;				\
								\
	old__ = READ_ONCE(*(ptr));				\
	do {							\
		new__ = (old__ & ~mask__) | bits__;		\
	} while (!try_cmpxchg(ptr, &old__, new__));		\
								\
	old__;							\
})
#endif

#ifndef bit_clear_unless
#define bit_clear_unless(ptr, clear, test)	\
({								\
	const typeof(*(ptr)) clear__ = (clear), test__ = (test);\
	typeof(*(ptr)) old__, new__;				\
								\
	old__ = READ_ONCE(*(ptr));				\
	do {							\
		if (old__ & test__)				\
			break;					\
		new__ = old__ & ~clear__;			\
	} while (!try_cmpxchg(ptr, &old__, new__));		\
								\
	!(old__ & test__);					\
})
#endif

#endif 
#endif
