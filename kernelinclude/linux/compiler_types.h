/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_TYPES_H
#define __LINUX_COMPILER_TYPES_H

#ifndef __ASSEMBLY__


#if defined(CONFIG_DEBUG_INFO_BTF) && defined(CONFIG_PAHOLE_HAS_BTF_TAG) && \
	__has_attribute(btf_type_tag) && !defined(__BINDGEN__)
# define BTF_TYPE_TAG(value) __attribute__((btf_type_tag(#value)))
#else
# define BTF_TYPE_TAG(value) 
#endif


#ifdef __CHECKER__

# define __kernel	__attribute__((address_space(0)))
# define __user		__attribute__((noderef, address_space(__user)))
# define __iomem	__attribute__((noderef, address_space(__iomem)))
# define __percpu	__attribute__((noderef, address_space(__percpu)))
# define __rcu		__attribute__((noderef, address_space(__rcu)))
static inline void __chk_user_ptr(const volatile void __user *ptr) { }
static inline void __chk_io_ptr(const volatile void __iomem *ptr) { }

# define __must_hold(x)	__attribute__((context(x,1,1)))
# define __acquires(x)	__attribute__((context(x,0,1)))
# define __cond_acquires(x) __attribute__((context(x,0,-1)))
# define __releases(x)	__attribute__((context(x,1,0)))
# define __acquire(x)	__context__(x,1)
# define __release(x)	__context__(x,-1)
# define __cond_lock(x,c)	((c) ? ({ __acquire(x); 1; }) : 0)

# define __force	__attribute__((force))
# define __nocast	__attribute__((nocast))
# define __safe		__attribute__((safe))
# define __private	__attribute__((noderef))
# define ACCESS_PRIVATE(p, member) (*((typeof((p)->member) __force *) &(p)->member))
#else 

# define __kernel
# ifdef STRUCTLEAK_PLUGIN
#  define __user	__attribute__((user))
# else
#  define __user	BTF_TYPE_TAG(user)
# endif
# define __iomem
# define __percpu	BTF_TYPE_TAG(percpu)
# define __rcu		BTF_TYPE_TAG(rcu)

# define __chk_user_ptr(x)	(void)0
# define __chk_io_ptr(x)	(void)0

# define __must_hold(x)
# define __acquires(x)
# define __cond_acquires(x)
# define __releases(x)
# define __acquire(x)	(void)0
# define __release(x)	(void)0
# define __cond_lock(x,c) (c)

# define __force
# define __nocast
# define __safe
# define __private
# define ACCESS_PRIVATE(p, member) ((p)->member)
# define __builtin_warning(x, y...) (1)
#endif 


#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

#ifdef __KERNEL__


#include <linux/compiler_attributes.h>

#if CONFIG_FUNCTION_ALIGNMENT > 0
#define __function_aligned		__aligned(CONFIG_FUNCTION_ALIGNMENT)
#else
#define __function_aligned
#endif


#if !defined(CONFIG_CC_IS_GCC) || (CONFIG_FUNCTION_ALIGNMENT == 0)
#define __cold				__attribute__((__cold__))
#else
#define __cold
#endif


#if __has_attribute(__preserve_most__) && (defined(CONFIG_X86_64) || defined(CONFIG_ARM64))
# define __preserve_most notrace __attribute__((__preserve_most__))
#else
# define __preserve_most
#endif




#ifndef __has_builtin
#define __has_builtin(x) (0)
#endif


#ifdef __clang__
#include <linux/compiler-clang.h>
#elif defined(__GNUC__)

#include <linux/compiler-gcc.h>
#else
#error "Unknown compiler"
#endif


#ifdef CONFIG_HAVE_ARCH_COMPILER_H
#include <asm/compiler.h>
#endif

struct ftrace_branch_data {
	const char *func;
	const char *file;
	unsigned line;
	union {
		struct {
			unsigned long correct;
			unsigned long incorrect;
		};
		struct {
			unsigned long miss;
			unsigned long hit;
		};
		unsigned long miss_hit[2];
	};
};

struct ftrace_likely_data {
	struct ftrace_branch_data	data;
	unsigned long			constant;
};

#if defined(CC_USING_HOTPATCH)
#define notrace			__attribute__((hotpatch(0, 0)))
#elif defined(CC_USING_PATCHABLE_FUNCTION_ENTRY)
#define notrace			__attribute__((patchable_function_entry(0, 0)))
#else
#define notrace			__attribute__((__no_instrument_function__))
#endif


#define __naked			__attribute__((__naked__)) notrace


#define inline inline __gnu_inline __inline_maybe_unused notrace


#define __inline__ inline


#ifdef KBUILD_EXTRA_WARN1
#define __inline_maybe_unused
#else
#define __inline_maybe_unused __maybe_unused
#endif


#define noinline_for_stack noinline


#ifdef __SANITIZE_ADDRESS__

# define __no_kasan_or_inline __no_sanitize_address notrace __maybe_unused
# define __no_sanitize_or_inline __no_kasan_or_inline
#else
# define __no_kasan_or_inline __always_inline
#endif

#ifdef __SANITIZE_THREAD__

# define __no_kcsan __no_sanitize_thread __disable_sanitizer_instrumentation
# define __no_sanitize_or_inline __no_kcsan notrace __maybe_unused
#else
# define __no_kcsan
#endif

#ifdef __SANITIZE_MEMORY__

# define __no_sanitize_or_inline __no_kmsan_checks notrace __maybe_unused
#endif

#ifndef __no_sanitize_or_inline
#define __no_sanitize_or_inline __always_inline
#endif


#define __noinstr_section(section)					\
	noinline notrace __attribute((__section__(section)))		\
	__no_kcsan __no_sanitize_address __no_profile __no_sanitize_coverage \
	__no_sanitize_memory

#define noinstr __noinstr_section(".noinstr.text")


#define __cpuidle __noinstr_section(".cpuidle.text")

#endif 

#endif 


#ifndef __latent_entropy
# define __latent_entropy
#endif

#if defined(RANDSTRUCT) && !defined(__CHECKER__)
# define __randomize_layout __designated_init __attribute__((randomize_layout))
# define __no_randomize_layout __attribute__((no_randomize_layout))

# define randomized_struct_fields_start	struct {
# define randomized_struct_fields_end	} __randomize_layout;
#else
# define __randomize_layout __designated_init
# define __no_randomize_layout
# define randomized_struct_fields_start
# define randomized_struct_fields_end
#endif

#ifndef __noscs
# define __noscs
#endif

#ifndef __nocfi
# define __nocfi
#endif


#ifdef __alloc_size__
# define __alloc_size(x, ...)	__alloc_size__(x, ## __VA_ARGS__) __malloc
# define __realloc_size(x, ...)	__alloc_size__(x, ## __VA_ARGS__)
#else
# define __alloc_size(x, ...)	__malloc
# define __realloc_size(x, ...)
#endif


#ifndef asm_goto_output
#define asm_goto_output(x...) asm volatile goto(x)
#endif

#ifdef CONFIG_CC_HAS_ASM_INLINE
#define asm_inline asm __inline
#else
#define asm_inline asm
#endif


#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))



#define __scalar_type_to_expr_cases(type)				\
		unsigned type:	(unsigned type)0,			\
		signed type:	(signed type)0

#define __unqual_scalar_typeof(x) typeof(				\
		_Generic((x),						\
			 char:	(char)0,				\
			 __scalar_type_to_expr_cases(char),		\
			 __scalar_type_to_expr_cases(short),		\
			 __scalar_type_to_expr_cases(int),		\
			 __scalar_type_to_expr_cases(long),		\
			 __scalar_type_to_expr_cases(long long),	\
			 default: (x)))


#define __native_word(t) \
	(sizeof(t) == sizeof(char) || sizeof(t) == sizeof(short) || \
	 sizeof(t) == sizeof(int) || sizeof(t) == sizeof(long))

#ifdef __OPTIMIZE__
# define __compiletime_assert(condition, msg, prefix, suffix)		\
	do {								\
									\
		__noreturn extern void prefix ## suffix(void)		\
			__compiletime_error(msg);			\
		if (!(condition))					\
			prefix ## suffix();				\
	} while (0)
#else
# define __compiletime_assert(condition, msg, prefix, suffix) do { } while (0)
#endif

#define _compiletime_assert(condition, msg, prefix, suffix) \
	__compiletime_assert(condition, msg, prefix, suffix)


#define compiletime_assert(condition, msg) \
	_compiletime_assert(condition, msg, __compiletime_assert_, __COUNTER__)

#define compiletime_assert_atomic_type(t)				\
	compiletime_assert(__native_word(t),				\
		"Need native word sized stores/loads for atomicity.")


#ifndef __diag
#define __diag(string)
#endif

#ifndef __diag_GCC
#define __diag_GCC(version, severity, string)
#endif

#define __diag_push()	__diag(push)
#define __diag_pop()	__diag(pop)

#define __diag_ignore(compiler, version, option, comment) \
	__diag_ ## compiler(version, ignore, option)
#define __diag_warn(compiler, version, option, comment) \
	__diag_ ## compiler(version, warn, option)
#define __diag_error(compiler, version, option, comment) \
	__diag_ ## compiler(version, error, option)

#ifndef __diag_ignore_all
#define __diag_ignore_all(option, comment)
#endif

#endif 
