/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_TYPES_H
#error "Please don't include <linux/compiler-gcc.h> directly, include <linux/compiler.h> instead."
#endif


#define GCC_VERSION (__GNUC__ * 10000		\
		     + __GNUC_MINOR__ * 100	\
		     + __GNUC_PATCHLEVEL__)


#define RELOC_HIDE(ptr, off)						\
({									\
	unsigned long __ptr;						\
	__asm__ ("" : "=r"(__ptr) : "0"(ptr));				\
	(typeof(ptr)) (__ptr + (off));					\
})

#ifdef CONFIG_RETPOLINE
#define __noretpoline __attribute__((__indirect_branch__("keep")))
#endif

#define __UNIQUE_ID(prefix) __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#if defined(LATENT_ENTROPY_PLUGIN) && !defined(__CHECKER__)
#define __latent_entropy __attribute__((latent_entropy))
#endif


#define barrier_before_unreachable() asm volatile("")


#define unreachable() \
	do {					\
		annotate_unreachable();		\
		barrier_before_unreachable();	\
		__builtin_unreachable();	\
	} while (0)


#ifdef CONFIG_GCC_ASM_GOTO_OUTPUT_WORKAROUND
#define asm_goto_output(x...) \
	do { asm volatile goto(x); asm (""); } while (0)
#endif

#if defined(CONFIG_ARCH_USE_BUILTIN_BSWAP)
#define __HAVE_BUILTIN_BSWAP32__
#define __HAVE_BUILTIN_BSWAP64__
#define __HAVE_BUILTIN_BSWAP16__
#endif 

#if GCC_VERSION >= 70000
#define KASAN_ABI_VERSION 5
#else
#define KASAN_ABI_VERSION 4
#endif

#ifdef CONFIG_SHADOW_CALL_STACK
#define __noscs __attribute__((__no_sanitize__("shadow-call-stack")))
#endif

#define __no_sanitize_address __attribute__((__no_sanitize_address__))

#if defined(__SANITIZE_THREAD__)
#define __no_sanitize_thread __attribute__((__no_sanitize_thread__))
#else
#define __no_sanitize_thread
#endif

#define __no_sanitize_undefined __attribute__((__no_sanitize_undefined__))


#if defined(CONFIG_KCOV) && __has_attribute(__no_sanitize_coverage__)
#define __no_sanitize_coverage __attribute__((__no_sanitize_coverage__))
#else
#define __no_sanitize_coverage
#endif


#ifdef __SANITIZE_HWADDRESS__
#define __SANITIZE_ADDRESS__
#endif


#define __no_sanitize_memory
#define __no_kmsan_checks


#define __diag_GCC(version, severity, s) \
	__diag_GCC_ ## version(__diag_GCC_ ## severity s)


#define __diag_GCC_ignore	ignored
#define __diag_GCC_warn		warning
#define __diag_GCC_error	error

#define __diag_str1(s)		#s
#define __diag_str(s)		__diag_str1(s)
#define __diag(s)		_Pragma(__diag_str(GCC diagnostic s))

#if GCC_VERSION >= 80000
#define __diag_GCC_8(s)		__diag(s)
#else
#define __diag_GCC_8(s)
#endif

#define __diag_ignore_all(option, comment) \
	__diag_GCC(8, ignore, option)


#if GCC_VERSION < 90100
#undef __alloc_size__
#endif
