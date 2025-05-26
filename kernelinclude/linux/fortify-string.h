/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_FORTIFY_STRING_H_
#define _LINUX_FORTIFY_STRING_H_

#include <linux/bug.h>
#include <linux/const.h>
#include <linux/limits.h>

#define __FORTIFY_INLINE extern __always_inline __gnu_inline __overloadable
#define __RENAME(x) __asm__(#x)

void fortify_panic(const char *name) __noreturn __cold;
void __read_overflow(void) __compiletime_error("detected read beyond size of object (1st parameter)");
void __read_overflow2(void) __compiletime_error("detected read beyond size of object (2nd parameter)");
void __read_overflow2_field(size_t avail, size_t wanted) __compiletime_warning("detected read beyond size of field (2nd parameter); maybe use struct_group()?");
void __write_overflow(void) __compiletime_error("detected write beyond size of object (1st parameter)");
void __write_overflow_field(size_t avail, size_t wanted) __compiletime_warning("detected write beyond size of field (1st parameter); maybe use struct_group()?");

#define __compiletime_strlen(p)					\
({								\
	char *__p = (char *)(p);				\
	size_t __ret = SIZE_MAX;				\
	const size_t __p_size = __member_size(p);		\
	if (__p_size != SIZE_MAX &&				\
	    __builtin_constant_p(*__p)) {			\
		size_t __p_len = __p_size - 1;			\
		if (__builtin_constant_p(__p[__p_len]) &&	\
		    __p[__p_len] == '\0')			\
			__ret = __builtin_strlen(__p);		\
	}							\
	__ret;							\
})

#if defined(__SANITIZE_ADDRESS__)

#if !defined(CONFIG_CC_HAS_KASAN_MEMINTRINSIC_PREFIX) && !defined(CONFIG_GENERIC_ENTRY)
extern void *__underlying_memset(void *p, int c, __kernel_size_t size) __RENAME(memset);
extern void *__underlying_memmove(void *p, const void *q, __kernel_size_t size) __RENAME(memmove);
extern void *__underlying_memcpy(void *p, const void *q, __kernel_size_t size) __RENAME(memcpy);
#elif defined(CONFIG_KASAN_GENERIC)
extern void *__underlying_memset(void *p, int c, __kernel_size_t size) __RENAME(__asan_memset);
extern void *__underlying_memmove(void *p, const void *q, __kernel_size_t size) __RENAME(__asan_memmove);
extern void *__underlying_memcpy(void *p, const void *q, __kernel_size_t size) __RENAME(__asan_memcpy);
#else 
extern void *__underlying_memset(void *p, int c, __kernel_size_t size) __RENAME(__hwasan_memset);
extern void *__underlying_memmove(void *p, const void *q, __kernel_size_t size) __RENAME(__hwasan_memmove);
extern void *__underlying_memcpy(void *p, const void *q, __kernel_size_t size) __RENAME(__hwasan_memcpy);
#endif

extern void *__underlying_memchr(const void *p, int c, __kernel_size_t size) __RENAME(memchr);
extern int __underlying_memcmp(const void *p, const void *q, __kernel_size_t size) __RENAME(memcmp);
extern char *__underlying_strcat(char *p, const char *q) __RENAME(strcat);
extern char *__underlying_strcpy(char *p, const char *q) __RENAME(strcpy);
extern __kernel_size_t __underlying_strlen(const char *p) __RENAME(strlen);
extern char *__underlying_strncat(char *p, const char *q, __kernel_size_t count) __RENAME(strncat);
extern char *__underlying_strncpy(char *p, const char *q, __kernel_size_t size) __RENAME(strncpy);

#else

#if defined(__SANITIZE_MEMORY__)

#include <linux/kmsan_string.h>
#define __underlying_memcpy	__msan_memcpy
#define __underlying_memmove	__msan_memmove
#define __underlying_memset	__msan_memset
#else
#define __underlying_memcpy	__builtin_memcpy
#define __underlying_memmove	__builtin_memmove
#define __underlying_memset	__builtin_memset
#endif

#define __underlying_memchr	__builtin_memchr
#define __underlying_memcmp	__builtin_memcmp
#define __underlying_strcat	__builtin_strcat
#define __underlying_strcpy	__builtin_strcpy
#define __underlying_strlen	__builtin_strlen
#define __underlying_strncat	__builtin_strncat
#define __underlying_strncpy	__builtin_strncpy

#endif


#define unsafe_memcpy(dst, src, bytes, justification)		\
	__underlying_memcpy(dst, src, bytes)


#if __has_builtin(__builtin_dynamic_object_size)
#define POS			__pass_dynamic_object_size(1)
#define POS0			__pass_dynamic_object_size(0)
#define __struct_size(p)	__builtin_dynamic_object_size(p, 0)
#define __member_size(p)	__builtin_dynamic_object_size(p, 1)
#else
#define POS			__pass_object_size(1)
#define POS0			__pass_object_size(0)
#define __struct_size(p)	__builtin_object_size(p, 0)
#define __member_size(p)	__builtin_object_size(p, 1)
#endif

#define __compiletime_lessthan(bounds, length)	(	\
	__builtin_constant_p((bounds) < (length)) &&	\
	(bounds) < (length)				\
)


__FORTIFY_INLINE __diagnose_as(__builtin_strncpy, 1, 2, 3)
char *strncpy(char * const POS p, const char *q, __kernel_size_t size)
{
	const size_t p_size = __member_size(p);

	if (__compiletime_lessthan(p_size, size))
		__write_overflow();
	if (p_size < size)
		fortify_panic(__func__);
	return __underlying_strncpy(p, q, size);
}

extern __kernel_size_t __real_strnlen(const char *, __kernel_size_t) __RENAME(strnlen);

__FORTIFY_INLINE __kernel_size_t strnlen(const char * const POS p, __kernel_size_t maxlen)
{
	const size_t p_size = __member_size(p);
	const size_t p_len = __compiletime_strlen(p);
	size_t ret;

	
	if (__builtin_constant_p(maxlen) && p_len != SIZE_MAX) {
		
		if (maxlen >= p_size)
			return p_len;
	}

	
	ret = __real_strnlen(p, maxlen < p_size ? maxlen : p_size);
	if (p_size <= ret && maxlen != ret)
		fortify_panic(__func__);
	return ret;
}



#define strlen(p)							\
	__builtin_choose_expr(__is_constexpr(__builtin_strlen(p)),	\
		__builtin_strlen(p), __fortify_strlen(p))
__FORTIFY_INLINE __diagnose_as(__builtin_strlen, 1)
__kernel_size_t __fortify_strlen(const char * const POS p)
{
	const size_t p_size = __member_size(p);
	__kernel_size_t ret;

	
	if (p_size == SIZE_MAX)
		return __underlying_strlen(p);
	ret = strnlen(p, p_size);
	if (p_size <= ret)
		fortify_panic(__func__);
	return ret;
}


extern size_t __real_strlcpy(char *, const char *, size_t) __RENAME(strlcpy);

__FORTIFY_INLINE size_t strlcpy(char * const POS p, const char * const POS q, size_t size)
{
	const size_t p_size = __member_size(p);
	const size_t q_size = __member_size(q);
	size_t q_len;	
	size_t len;	

	if (p_size == SIZE_MAX && q_size == SIZE_MAX)
		return __real_strlcpy(p, q, size);
	q_len = strlen(q);
	len = (q_len >= size) ? size - 1 : q_len;
	if (__builtin_constant_p(size) && __builtin_constant_p(q_len) && size) {
		
		if (len >= p_size)
			__write_overflow();
	}
	if (size) {
		if (len >= p_size)
			fortify_panic(__func__);
		__underlying_memcpy(p, q, len);
		p[len] = '\0';
	}
	return q_len;
}


extern ssize_t __real_strscpy(char *, const char *, size_t) __RENAME(strscpy);

__FORTIFY_INLINE ssize_t strscpy(char * const POS p, const char * const POS q, size_t size)
{
	
	const size_t p_size = __member_size(p);
	const size_t q_size = __member_size(q);
	size_t len;

	
	if (p_size == SIZE_MAX && q_size == SIZE_MAX)
		return __real_strscpy(p, q, size);

	
	if (__compiletime_lessthan(p_size, size))
		__write_overflow();

	
	if (__compiletime_lessthan(p_size, SIZE_MAX)) {
		len = __compiletime_strlen(q);

		if (len < SIZE_MAX && __compiletime_lessthan(len, size)) {
			__underlying_memcpy(p, q, len + 1);
			return len;
		}
	}

	
	len = strnlen(q, size);
	
	len = len == size ? size : len + 1;

	
	if (len > p_size)
		fortify_panic(__func__);

	
	return __real_strscpy(p, q, len);
}


extern size_t __real_strlcat(char *p, const char *q, size_t avail) __RENAME(strlcat);

__FORTIFY_INLINE
size_t strlcat(char * const POS p, const char * const POS q, size_t avail)
{
	const size_t p_size = __member_size(p);
	const size_t q_size = __member_size(q);
	size_t p_len, copy_len;
	size_t actual, wanted;

	
	if (p_size == SIZE_MAX && q_size == SIZE_MAX)
		return __real_strlcat(p, q, avail);

	p_len = strnlen(p, avail);
	copy_len = strlen(q);
	wanted = actual = p_len + copy_len;

	
	if (avail <= p_len)
		return wanted;

	
	if (p_size <= p_len)
		fortify_panic(__func__);

	if (actual >= avail) {
		copy_len = avail - p_len - 1;
		actual = p_len + copy_len;
	}

	
	if (p_size <= actual)
		fortify_panic(__func__);
	__underlying_memcpy(p + p_len, q, copy_len);
	p[actual] = '\0';

	return wanted;
}



__FORTIFY_INLINE __diagnose_as(__builtin_strcat, 1, 2)
char *strcat(char * const POS p, const char *q)
{
	const size_t p_size = __member_size(p);

	if (strlcat(p, q, p_size) >= p_size)
		fortify_panic(__func__);
	return p;
}



__FORTIFY_INLINE __diagnose_as(__builtin_strncat, 1, 2, 3)
char *strncat(char * const POS p, const char * const POS q, __kernel_size_t count)
{
	const size_t p_size = __member_size(p);
	const size_t q_size = __member_size(q);
	size_t p_len, copy_len;

	if (p_size == SIZE_MAX && q_size == SIZE_MAX)
		return __underlying_strncat(p, q, count);
	p_len = strlen(p);
	copy_len = strnlen(q, count);
	if (p_size < p_len + copy_len + 1)
		fortify_panic(__func__);
	__underlying_memcpy(p + p_len, q, copy_len);
	p[p_len + copy_len] = '\0';
	return p;
}

__FORTIFY_INLINE void fortify_memset_chk(__kernel_size_t size,
					 const size_t p_size,
					 const size_t p_size_field)
{
	if (__builtin_constant_p(size)) {
		

		
		if (__compiletime_lessthan(p_size_field, p_size) &&
		    __compiletime_lessthan(p_size, size))
			__write_overflow();

		
		if (__compiletime_lessthan(p_size_field, size))
			__write_overflow_field(p_size_field, size);
	}
	

	
	if (p_size != SIZE_MAX && p_size < size)
		fortify_panic("memset");
}

#define __fortify_memset_chk(p, c, size, p_size, p_size_field) ({	\
	size_t __fortify_size = (size_t)(size);				\
	fortify_memset_chk(__fortify_size, p_size, p_size_field),	\
	__underlying_memset(p, c, __fortify_size);			\
})


#ifndef CONFIG_KMSAN
#define memset(p, c, s) __fortify_memset_chk(p, c, s,			\
		__struct_size(p), __member_size(p))
#endif


__FORTIFY_INLINE bool fortify_memcpy_chk(__kernel_size_t size,
					 const size_t p_size,
					 const size_t q_size,
					 const size_t p_size_field,
					 const size_t q_size_field,
					 const char *func)
{
	if (__builtin_constant_p(size)) {
		

		
		if (__compiletime_lessthan(p_size_field, p_size) &&
		    __compiletime_lessthan(p_size, size))
			__write_overflow();
		if (__compiletime_lessthan(q_size_field, q_size) &&
		    __compiletime_lessthan(q_size, size))
			__read_overflow2();

		
		if (__compiletime_lessthan(p_size_field, size))
			__write_overflow_field(p_size_field, size);
		
		if ((IS_ENABLED(KBUILD_EXTRA_WARN1) ||
		     __compiletime_lessthan(p_size_field, size)) &&
		    __compiletime_lessthan(q_size_field, size))
			__read_overflow2_field(q_size_field, size);
	}
	

	
	if ((p_size != SIZE_MAX && p_size < size) ||
	    (q_size != SIZE_MAX && q_size < size))
		fortify_panic(func);

	
	if (p_size_field != 0 && p_size_field != SIZE_MAX &&
	    p_size != p_size_field && p_size_field < size)
		return true;

	return false;
}

#define __fortify_memcpy_chk(p, q, size, p_size, q_size,		\
			     p_size_field, q_size_field, op) ({		\
	const size_t __fortify_size = (size_t)(size);			\
	const size_t __p_size = (p_size);				\
	const size_t __q_size = (q_size);				\
	const size_t __p_size_field = (p_size_field);			\
	const size_t __q_size_field = (q_size_field);			\
	WARN_ONCE(fortify_memcpy_chk(__fortify_size, __p_size,		\
				     __q_size, __p_size_field,		\
				     __q_size_field, #op),		\
		  #op ": detected field-spanning write (size %zu) of single %s (size %zu)\n", \
		  __fortify_size,					\
		  "field \"" #p "\" at " __FILE__ ":" __stringify(__LINE__), \
		  __p_size_field);					\
	__underlying_##op(p, q, __fortify_size);			\
})




#define memcpy(p, q, s)  __fortify_memcpy_chk(p, q, s,			\
		__struct_size(p), __struct_size(q),			\
		__member_size(p), __member_size(q),			\
		memcpy)
#define memmove(p, q, s)  __fortify_memcpy_chk(p, q, s,			\
		__struct_size(p), __struct_size(q),			\
		__member_size(p), __member_size(q),			\
		memmove)

extern void *__real_memscan(void *, int, __kernel_size_t) __RENAME(memscan);
__FORTIFY_INLINE void *memscan(void * const POS0 p, int c, __kernel_size_t size)
{
	const size_t p_size = __struct_size(p);

	if (__compiletime_lessthan(p_size, size))
		__read_overflow();
	if (p_size < size)
		fortify_panic(__func__);
	return __real_memscan(p, c, size);
}

__FORTIFY_INLINE __diagnose_as(__builtin_memcmp, 1, 2, 3)
int memcmp(const void * const POS0 p, const void * const POS0 q, __kernel_size_t size)
{
	const size_t p_size = __struct_size(p);
	const size_t q_size = __struct_size(q);

	if (__builtin_constant_p(size)) {
		if (__compiletime_lessthan(p_size, size))
			__read_overflow();
		if (__compiletime_lessthan(q_size, size))
			__read_overflow2();
	}
	if (p_size < size || q_size < size)
		fortify_panic(__func__);
	return __underlying_memcmp(p, q, size);
}

__FORTIFY_INLINE __diagnose_as(__builtin_memchr, 1, 2, 3)
void *memchr(const void * const POS0 p, int c, __kernel_size_t size)
{
	const size_t p_size = __struct_size(p);

	if (__compiletime_lessthan(p_size, size))
		__read_overflow();
	if (p_size < size)
		fortify_panic(__func__);
	return __underlying_memchr(p, c, size);
}

void *__real_memchr_inv(const void *s, int c, size_t n) __RENAME(memchr_inv);
__FORTIFY_INLINE void *memchr_inv(const void * const POS0 p, int c, size_t size)
{
	const size_t p_size = __struct_size(p);

	if (__compiletime_lessthan(p_size, size))
		__read_overflow();
	if (p_size < size)
		fortify_panic(__func__);
	return __real_memchr_inv(p, c, size);
}

extern void *__real_kmemdup(const void *src, size_t len, gfp_t gfp) __RENAME(kmemdup)
								    __realloc_size(2);
__FORTIFY_INLINE void *kmemdup(const void * const POS0 p, size_t size, gfp_t gfp)
{
	const size_t p_size = __struct_size(p);

	if (__compiletime_lessthan(p_size, size))
		__read_overflow();
	if (p_size < size)
		fortify_panic(__func__);
	return __real_kmemdup(p, size, gfp);
}



__FORTIFY_INLINE __diagnose_as(__builtin_strcpy, 1, 2)
char *strcpy(char * const POS p, const char * const POS q)
{
	const size_t p_size = __member_size(p);
	const size_t q_size = __member_size(q);
	size_t size;

	
	if (__builtin_constant_p(p_size) &&
	    __builtin_constant_p(q_size) &&
	    p_size == SIZE_MAX && q_size == SIZE_MAX)
		return __underlying_strcpy(p, q);
	size = strlen(q) + 1;
	
	if (__compiletime_lessthan(p_size, size))
		__write_overflow();
	
	if (p_size < size)
		fortify_panic(__func__);
	__underlying_memcpy(p, q, size);
	return p;
}


#undef __underlying_memchr
#undef __underlying_memcmp
#undef __underlying_strcat
#undef __underlying_strcpy
#undef __underlying_strlen
#undef __underlying_strncat
#undef __underlying_strncpy

#undef POS
#undef POS0

#endif 
