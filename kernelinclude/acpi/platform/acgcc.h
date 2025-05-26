/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __ACGCC_H__
#define __ACGCC_H__

#ifndef va_arg
#ifdef __KERNEL__
#include <linux/stdarg.h>
#else
#include <stdarg.h>
#endif 
#endif 

#define ACPI_INLINE             __inline__



#define ACPI_GET_FUNCTION_NAME          __func__


#define ACPI_PRINTF_LIKE(c) __attribute__ ((__format__ (__printf__, c, c+1)))


#define ACPI_UNUSED_VAR __attribute__ ((unused))



#define COMPILER_VA_MACRO               1



#define ACPI_USE_NATIVE_MATH64



#ifndef __has_attribute
#define __has_attribute(x) 0
#endif



#if __has_attribute(__fallthrough__)
#define ACPI_FALLTHROUGH __attribute__((__fallthrough__))
#endif


#define ACPI_FLEX_ARRAY(TYPE, NAME)             \
        struct {                                \
                struct { } __Empty_ ## NAME;    \
                TYPE NAME[];                    \
        }

#endif				
