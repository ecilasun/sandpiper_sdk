/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ASM_GLUE_PF_H
#define ASM_GLUE_PF_H

#include <asm/glue.h>



#undef CPU_PABORT_HANDLER
#undef MULTI_PABORT

#ifdef CONFIG_CPU_PABRT_LEGACY
# ifdef CPU_PABORT_HANDLER
#  define MULTI_PABORT 1
# else
#  define CPU_PABORT_HANDLER legacy_pabort
# endif
#endif

#ifdef CONFIG_CPU_PABRT_V6
# ifdef CPU_PABORT_HANDLER
#  define MULTI_PABORT 1
# else
#  define CPU_PABORT_HANDLER v6_pabort
# endif
#endif

#ifdef CONFIG_CPU_PABRT_V7
# ifdef CPU_PABORT_HANDLER
#  define MULTI_PABORT 1
# else
#  define CPU_PABORT_HANDLER v7_pabort
# endif
#endif

#ifndef CPU_PABORT_HANDLER
#error Unknown prefetch abort handler type
#endif

#endif
