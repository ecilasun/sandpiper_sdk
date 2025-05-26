/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_COMPILER_ATTRIBUTES_H
#define __LINUX_COMPILER_ATTRIBUTES_H




#define __alias(symbol)                 __attribute__((__alias__(#symbol)))


#define __aligned(x)                    __attribute__((__aligned__(x)))
#define __aligned_largest               __attribute__((__aligned__))


#define __alloc_size__(x, ...)		__attribute__((__alloc_size__(x, ## __VA_ARGS__)))


#define __always_inline                 inline __attribute__((__always_inline__))


#define __assume_aligned(a, ...)        __attribute__((__assume_aligned__(a, ## __VA_ARGS__)))


#define __cleanup(func)			__attribute__((__cleanup__(func)))


#define __attribute_const__             __attribute__((__const__))


#if __has_attribute(__copy__)
# define __copy(symbol)                 __attribute__((__copy__(symbol)))
#else
# define __copy(symbol)
#endif


#if __has_attribute(__counted_by__)
# define __counted_by(member)		__attribute__((__counted_by__(member)))
#else
# define __counted_by(member)
#endif


#if __has_attribute(__diagnose_as_builtin__)
# define __diagnose_as(builtin...)	__attribute__((__diagnose_as_builtin__(builtin)))
#else
# define __diagnose_as(builtin...)
#endif


#define __deprecated


#if __has_attribute(__designated_init__)
# define __designated_init              __attribute__((__designated_init__))
#else
# define __designated_init
#endif


#if __has_attribute(__error__)
# define __compiletime_error(msg)       __attribute__((__error__(msg)))
#else
# define __compiletime_error(msg)
#endif


#if __has_attribute(__externally_visible__)
# define __visible                      __attribute__((__externally_visible__))
#else
# define __visible
#endif


#define __printf(a, b)                  __attribute__((__format__(printf, a, b)))
#define __scanf(a, b)                   __attribute__((__format__(scanf, a, b)))


#define __gnu_inline                    __attribute__((__gnu_inline__))


#define __malloc                        __attribute__((__malloc__))


#define __mode(x)                       __attribute__((__mode__(x)))


#if __has_attribute(__no_caller_saved_registers__)
# define __no_caller_saved_registers	__attribute__((__no_caller_saved_registers__))
#else
# define __no_caller_saved_registers
#endif


#if __has_attribute(__noclone__)
# define __noclone                      __attribute__((__noclone__))
#else
# define __noclone
#endif


#if __has_attribute(__fallthrough__)
# define fallthrough                    __attribute__((__fallthrough__))
#else
# define fallthrough                    do {} while (0)  
#endif


# define __flatten			__attribute__((flatten))


#define   noinline                      __attribute__((__noinline__))


#if __has_attribute(__nonstring__)
# define __nonstring                    __attribute__((__nonstring__))
#else
# define __nonstring
#endif


#if __has_attribute(__no_profile_instrument_function__)
# define __no_profile                  __attribute__((__no_profile_instrument_function__))
#else
# define __no_profile
#endif


#define __noreturn                      __attribute__((__noreturn__))


#if __has_attribute(__no_stack_protector__)
# define __no_stack_protector		__attribute__((__no_stack_protector__))
#else
# define __no_stack_protector
#endif


#if __has_attribute(__overloadable__)
# define __overloadable			__attribute__((__overloadable__))
#else
# define __overloadable
#endif


#define __packed                        __attribute__((__packed__))


#if __has_attribute(__pass_dynamic_object_size__)
# define __pass_dynamic_object_size(type)	__attribute__((__pass_dynamic_object_size__(type)))
#else
# define __pass_dynamic_object_size(type)
#endif
#if __has_attribute(__pass_object_size__)
# define __pass_object_size(type)	__attribute__((__pass_object_size__(type)))
#else
# define __pass_object_size(type)
#endif


#define __pure                          __attribute__((__pure__))


#define __section(section)              __attribute__((__section__(section)))


#define __always_unused                 __attribute__((__unused__))
#define __maybe_unused                  __attribute__((__unused__))


#define __used                          __attribute__((__used__))


#define __must_check                    __attribute__((__warn_unused_result__))


#if __has_attribute(__warning__)
# define __compiletime_warning(msg)     __attribute__((__warning__(msg)))
#else
# define __compiletime_warning(msg)
#endif


#if __has_attribute(disable_sanitizer_instrumentation)
# define __disable_sanitizer_instrumentation \
	 __attribute__((disable_sanitizer_instrumentation))
#else
# define __disable_sanitizer_instrumentation
#endif


#define __weak                          __attribute__((__weak__))


#define __fix_address noinline __noclone

#endif 
