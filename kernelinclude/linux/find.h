/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_FIND_H_
#define __LINUX_FIND_H_

#ifndef __LINUX_BITMAP_H
#error only <linux/bitmap.h> can be included directly
#endif

#include <linux/bitops.h>

unsigned long _find_next_bit(const unsigned long *addr1, unsigned long nbits,
				unsigned long start);
unsigned long _find_next_and_bit(const unsigned long *addr1, const unsigned long *addr2,
					unsigned long nbits, unsigned long start);
unsigned long _find_next_andnot_bit(const unsigned long *addr1, const unsigned long *addr2,
					unsigned long nbits, unsigned long start);
unsigned long _find_next_or_bit(const unsigned long *addr1, const unsigned long *addr2,
					unsigned long nbits, unsigned long start);
unsigned long _find_next_zero_bit(const unsigned long *addr, unsigned long nbits,
					 unsigned long start);
extern unsigned long _find_first_bit(const unsigned long *addr, unsigned long size);
unsigned long __find_nth_bit(const unsigned long *addr, unsigned long size, unsigned long n);
unsigned long __find_nth_and_bit(const unsigned long *addr1, const unsigned long *addr2,
				unsigned long size, unsigned long n);
unsigned long __find_nth_andnot_bit(const unsigned long *addr1, const unsigned long *addr2,
					unsigned long size, unsigned long n);
unsigned long __find_nth_and_andnot_bit(const unsigned long *addr1, const unsigned long *addr2,
					const unsigned long *addr3, unsigned long size,
					unsigned long n);
extern unsigned long _find_first_and_bit(const unsigned long *addr1,
					 const unsigned long *addr2, unsigned long size);
extern unsigned long _find_first_zero_bit(const unsigned long *addr, unsigned long size);
extern unsigned long _find_last_bit(const unsigned long *addr, unsigned long size);

#ifdef __BIG_ENDIAN
unsigned long _find_first_zero_bit_le(const unsigned long *addr, unsigned long size);
unsigned long _find_next_zero_bit_le(const  unsigned long *addr, unsigned
					long size, unsigned long offset);
unsigned long _find_next_bit_le(const unsigned long *addr, unsigned
				long size, unsigned long offset);
#endif

#ifndef find_next_bit

static inline
unsigned long find_next_bit(const unsigned long *addr, unsigned long size,
			    unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val;

		if (unlikely(offset >= size))
			return size;

		val = *addr & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	return _find_next_bit(addr, size, offset);
}
#endif

#ifndef find_next_and_bit

static inline
unsigned long find_next_and_bit(const unsigned long *addr1,
		const unsigned long *addr2, unsigned long size,
		unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val;

		if (unlikely(offset >= size))
			return size;

		val = *addr1 & *addr2 & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	return _find_next_and_bit(addr1, addr2, size, offset);
}
#endif

#ifndef find_next_andnot_bit

static inline
unsigned long find_next_andnot_bit(const unsigned long *addr1,
		const unsigned long *addr2, unsigned long size,
		unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val;

		if (unlikely(offset >= size))
			return size;

		val = *addr1 & ~*addr2 & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	return _find_next_andnot_bit(addr1, addr2, size, offset);
}
#endif

#ifndef find_next_or_bit

static inline
unsigned long find_next_or_bit(const unsigned long *addr1,
		const unsigned long *addr2, unsigned long size,
		unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val;

		if (unlikely(offset >= size))
			return size;

		val = (*addr1 | *addr2) & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	return _find_next_or_bit(addr1, addr2, size, offset);
}
#endif

#ifndef find_next_zero_bit

static inline
unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size,
				 unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val;

		if (unlikely(offset >= size))
			return size;

		val = *addr | ~GENMASK(size - 1, offset);
		return val == ~0UL ? size : ffz(val);
	}

	return _find_next_zero_bit(addr, size, offset);
}
#endif

#ifndef find_first_bit

static inline
unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
	if (small_const_nbits(size)) {
		unsigned long val = *addr & GENMASK(size - 1, 0);

		return val ? __ffs(val) : size;
	}

	return _find_first_bit(addr, size);
}
#endif


static inline
unsigned long find_nth_bit(const unsigned long *addr, unsigned long size, unsigned long n)
{
	if (n >= size)
		return size;

	if (small_const_nbits(size)) {
		unsigned long val =  *addr & GENMASK(size - 1, 0);

		return val ? fns(val, n) : size;
	}

	return __find_nth_bit(addr, size, n);
}


static inline
unsigned long find_nth_and_bit(const unsigned long *addr1, const unsigned long *addr2,
				unsigned long size, unsigned long n)
{
	if (n >= size)
		return size;

	if (small_const_nbits(size)) {
		unsigned long val =  *addr1 & *addr2 & GENMASK(size - 1, 0);

		return val ? fns(val, n) : size;
	}

	return __find_nth_and_bit(addr1, addr2, size, n);
}


static inline
unsigned long find_nth_andnot_bit(const unsigned long *addr1, const unsigned long *addr2,
				unsigned long size, unsigned long n)
{
	if (n >= size)
		return size;

	if (small_const_nbits(size)) {
		unsigned long val =  *addr1 & (~*addr2) & GENMASK(size - 1, 0);

		return val ? fns(val, n) : size;
	}

	return __find_nth_andnot_bit(addr1, addr2, size, n);
}


static __always_inline
unsigned long find_nth_and_andnot_bit(const unsigned long *addr1,
					const unsigned long *addr2,
					const unsigned long *addr3,
					unsigned long size, unsigned long n)
{
	if (n >= size)
		return size;

	if (small_const_nbits(size)) {
		unsigned long val =  *addr1 & *addr2 & (~*addr3) & GENMASK(size - 1, 0);

		return val ? fns(val, n) : size;
	}

	return __find_nth_and_andnot_bit(addr1, addr2, addr3, size, n);
}

#ifndef find_first_and_bit

static inline
unsigned long find_first_and_bit(const unsigned long *addr1,
				 const unsigned long *addr2,
				 unsigned long size)
{
	if (small_const_nbits(size)) {
		unsigned long val = *addr1 & *addr2 & GENMASK(size - 1, 0);

		return val ? __ffs(val) : size;
	}

	return _find_first_and_bit(addr1, addr2, size);
}
#endif

#ifndef find_first_zero_bit

static inline
unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	if (small_const_nbits(size)) {
		unsigned long val = *addr | ~GENMASK(size - 1, 0);

		return val == ~0UL ? size : ffz(val);
	}

	return _find_first_zero_bit(addr, size);
}
#endif

#ifndef find_last_bit

static inline
unsigned long find_last_bit(const unsigned long *addr, unsigned long size)
{
	if (small_const_nbits(size)) {
		unsigned long val = *addr & GENMASK(size - 1, 0);

		return val ? __fls(val) : size;
	}

	return _find_last_bit(addr, size);
}
#endif


static inline
unsigned long find_next_and_bit_wrap(const unsigned long *addr1,
					const unsigned long *addr2,
					unsigned long size, unsigned long offset)
{
	unsigned long bit = find_next_and_bit(addr1, addr2, size, offset);

	if (bit < size)
		return bit;

	bit = find_first_and_bit(addr1, addr2, offset);
	return bit < offset ? bit : size;
}


static inline
unsigned long find_next_bit_wrap(const unsigned long *addr,
					unsigned long size, unsigned long offset)
{
	unsigned long bit = find_next_bit(addr, size, offset);

	if (bit < size)
		return bit;

	bit = find_first_bit(addr, offset);
	return bit < offset ? bit : size;
}


static inline
unsigned long __for_each_wrap(const unsigned long *bitmap, unsigned long size,
				 unsigned long start, unsigned long n)
{
	unsigned long bit;

	
	if (n > start) {
		
		bit = find_next_bit(bitmap, size, n);
		if (bit < size)
			return bit;

		
		n = 0;
	}

	
	bit = find_next_bit(bitmap, start, n);
	return bit < start ? bit : size;
}


extern unsigned long find_next_clump8(unsigned long *clump,
				      const unsigned long *addr,
				      unsigned long size, unsigned long offset);

#define find_first_clump8(clump, bits, size) \
	find_next_clump8((clump), (bits), (size), 0)

#if defined(__LITTLE_ENDIAN)

static inline unsigned long find_next_zero_bit_le(const void *addr,
		unsigned long size, unsigned long offset)
{
	return find_next_zero_bit(addr, size, offset);
}

static inline unsigned long find_next_bit_le(const void *addr,
		unsigned long size, unsigned long offset)
{
	return find_next_bit(addr, size, offset);
}

static inline unsigned long find_first_zero_bit_le(const void *addr,
		unsigned long size)
{
	return find_first_zero_bit(addr, size);
}

#elif defined(__BIG_ENDIAN)

#ifndef find_next_zero_bit_le
static inline
unsigned long find_next_zero_bit_le(const void *addr, unsigned
		long size, unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val = *(const unsigned long *)addr;

		if (unlikely(offset >= size))
			return size;

		val = swab(val) | ~GENMASK(size - 1, offset);
		return val == ~0UL ? size : ffz(val);
	}

	return _find_next_zero_bit_le(addr, size, offset);
}
#endif

#ifndef find_first_zero_bit_le
static inline
unsigned long find_first_zero_bit_le(const void *addr, unsigned long size)
{
	if (small_const_nbits(size)) {
		unsigned long val = swab(*(const unsigned long *)addr) | ~GENMASK(size - 1, 0);

		return val == ~0UL ? size : ffz(val);
	}

	return _find_first_zero_bit_le(addr, size);
}
#endif

#ifndef find_next_bit_le
static inline
unsigned long find_next_bit_le(const void *addr, unsigned
		long size, unsigned long offset)
{
	if (small_const_nbits(size)) {
		unsigned long val = *(const unsigned long *)addr;

		if (unlikely(offset >= size))
			return size;

		val = swab(val) & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	return _find_next_bit_le(addr, size, offset);
}
#endif

#else
#error "Please fix <asm/byteorder.h>"
#endif

#define for_each_set_bit(bit, addr, size) \
	for ((bit) = 0; (bit) = find_next_bit((addr), (size), (bit)), (bit) < (size); (bit)++)

#define for_each_and_bit(bit, addr1, addr2, size) \
	for ((bit) = 0;									\
	     (bit) = find_next_and_bit((addr1), (addr2), (size), (bit)), (bit) < (size);\
	     (bit)++)

#define for_each_andnot_bit(bit, addr1, addr2, size) \
	for ((bit) = 0;									\
	     (bit) = find_next_andnot_bit((addr1), (addr2), (size), (bit)), (bit) < (size);\
	     (bit)++)

#define for_each_or_bit(bit, addr1, addr2, size) \
	for ((bit) = 0;									\
	     (bit) = find_next_or_bit((addr1), (addr2), (size), (bit)), (bit) < (size);\
	     (bit)++)


#define for_each_set_bit_from(bit, addr, size) \
	for (; (bit) = find_next_bit((addr), (size), (bit)), (bit) < (size); (bit)++)

#define for_each_clear_bit(bit, addr, size) \
	for ((bit) = 0;									\
	     (bit) = find_next_zero_bit((addr), (size), (bit)), (bit) < (size);		\
	     (bit)++)


#define for_each_clear_bit_from(bit, addr, size) \
	for (; (bit) = find_next_zero_bit((addr), (size), (bit)), (bit) < (size); (bit)++)


#define for_each_set_bitrange(b, e, addr, size)			\
	for ((b) = 0;						\
	     (b) = find_next_bit((addr), (size), b),		\
	     (e) = find_next_zero_bit((addr), (size), (b) + 1),	\
	     (b) < (size);					\
	     (b) = (e) + 1)


#define for_each_set_bitrange_from(b, e, addr, size)		\
	for (;							\
	     (b) = find_next_bit((addr), (size), (b)),		\
	     (e) = find_next_zero_bit((addr), (size), (b) + 1),	\
	     (b) < (size);					\
	     (b) = (e) + 1)


#define for_each_clear_bitrange(b, e, addr, size)		\
	for ((b) = 0;						\
	     (b) = find_next_zero_bit((addr), (size), (b)),	\
	     (e) = find_next_bit((addr), (size), (b) + 1),	\
	     (b) < (size);					\
	     (b) = (e) + 1)


#define for_each_clear_bitrange_from(b, e, addr, size)		\
	for (;							\
	     (b) = find_next_zero_bit((addr), (size), (b)),	\
	     (e) = find_next_bit((addr), (size), (b) + 1),	\
	     (b) < (size);					\
	     (b) = (e) + 1)


#define for_each_set_bit_wrap(bit, addr, size, start) \
	for ((bit) = find_next_bit_wrap((addr), (size), (start));		\
	     (bit) < (size);							\
	     (bit) = __for_each_wrap((addr), (size), (start), (bit) + 1))


#define for_each_set_clump8(start, clump, bits, size) \
	for ((start) = find_first_clump8(&(clump), (bits), (size)); \
	     (start) < (size); \
	     (start) = find_next_clump8(&(clump), (bits), (size), (start) + 8))

#endif 
