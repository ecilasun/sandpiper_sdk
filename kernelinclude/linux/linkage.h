/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_LINKAGE_H
#define _LINUX_LINKAGE_H

#include <linux/compiler_types.h>
#include <linux/stringify.h>
#include <linux/export.h>
#include <asm/linkage.h>


#ifndef ASM_NL
#define ASM_NL		 ;
#endif

#ifdef __cplusplus
#define CPP_ASMLINKAGE extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#ifndef asmlinkage
#define asmlinkage CPP_ASMLINKAGE
#endif

#ifndef cond_syscall
#define cond_syscall(x)	asm(				\
	".weak " __stringify(x) "\n\t"			\
	".set  " __stringify(x) ","			\
		 __stringify(sys_ni_syscall))
#endif

#ifndef SYSCALL_ALIAS
#define SYSCALL_ALIAS(alias, name) asm(			\
	".globl " __stringify(alias) "\n\t"		\
	".set   " __stringify(alias) ","		\
		  __stringify(name))
#endif

#define __page_aligned_data	__section(".data..page_aligned") __aligned(PAGE_SIZE)
#define __page_aligned_bss	__section(".bss..page_aligned") __aligned(PAGE_SIZE)


#define __PAGE_ALIGNED_DATA	.section ".data..page_aligned", "aw"
#define __PAGE_ALIGNED_BSS	.section ".bss..page_aligned", "aw"



#ifndef __ASSEMBLY__
#ifndef asmlinkage_protect
# define asmlinkage_protect(n, ret, args...)	do { } while (0)
#endif
#endif

#ifndef __ALIGN
#define __ALIGN			.balign CONFIG_FUNCTION_ALIGNMENT
#define __ALIGN_STR		__stringify(__ALIGN)
#endif

#ifdef __ASSEMBLY__


#ifndef SYM_T_FUNC
#define SYM_T_FUNC				STT_FUNC
#endif


#ifndef SYM_T_OBJECT
#define SYM_T_OBJECT				STT_OBJECT
#endif


#ifndef SYM_T_NONE
#define SYM_T_NONE				STT_NOTYPE
#endif


#define SYM_A_ALIGN				ALIGN
#define SYM_A_NONE				


#define SYM_L_GLOBAL(name)			.globl name
#define SYM_L_WEAK(name)			.weak name
#define SYM_L_LOCAL(name)			

#ifndef LINKER_SCRIPT
#define ALIGN __ALIGN
#define ALIGN_STR __ALIGN_STR



#ifndef CONFIG_ARCH_USE_SYM_ANNOTATIONS
#ifndef GLOBAL

#define GLOBAL(name) \
	.globl name ASM_NL \
	name:
#endif

#ifndef ENTRY

#define ENTRY(name) \
	SYM_FUNC_START(name)
#endif
#endif 
#endif 

#ifndef CONFIG_ARCH_USE_SYM_ANNOTATIONS
#ifndef WEAK

#define WEAK(name)	   \
	SYM_FUNC_START_WEAK(name)
#endif

#ifndef END

#define END(name) \
	.size name, .-name
#endif


#ifndef ENDPROC

#define ENDPROC(name) \
	SYM_FUNC_END(name)
#endif
#endif 




#ifndef SYM_ENTRY
#define SYM_ENTRY(name, linkage, align...)		\
	linkage(name) ASM_NL				\
	align ASM_NL					\
	name:
#endif


#ifndef SYM_START
#define SYM_START(name, linkage, align...)		\
	SYM_ENTRY(name, linkage, align)
#endif


#ifndef SYM_END
#define SYM_END(name, sym_type)				\
	.type name sym_type ASM_NL			\
	.set .L__sym_size_##name, .-name ASM_NL		\
	.size name, .L__sym_size_##name
#endif


#ifndef SYM_ALIAS
#define SYM_ALIAS(alias, name, linkage)			\
	linkage(alias) ASM_NL				\
	.set alias, name ASM_NL
#endif






#ifndef SYM_INNER_LABEL_ALIGN
#define SYM_INNER_LABEL_ALIGN(name, linkage)	\
	.type name SYM_T_NONE ASM_NL			\
	SYM_ENTRY(name, linkage, SYM_A_ALIGN)
#endif


#ifndef SYM_INNER_LABEL
#define SYM_INNER_LABEL(name, linkage)		\
	.type name SYM_T_NONE ASM_NL			\
	SYM_ENTRY(name, linkage, SYM_A_NONE)
#endif


#ifndef SYM_FUNC_START
#define SYM_FUNC_START(name)				\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_ALIGN)
#endif


#ifndef SYM_FUNC_START_NOALIGN
#define SYM_FUNC_START_NOALIGN(name)			\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_NONE)
#endif


#ifndef SYM_FUNC_START_LOCAL
#define SYM_FUNC_START_LOCAL(name)			\
	SYM_START(name, SYM_L_LOCAL, SYM_A_ALIGN)
#endif


#ifndef SYM_FUNC_START_LOCAL_NOALIGN
#define SYM_FUNC_START_LOCAL_NOALIGN(name)		\
	SYM_START(name, SYM_L_LOCAL, SYM_A_NONE)
#endif


#ifndef SYM_FUNC_START_WEAK
#define SYM_FUNC_START_WEAK(name)			\
	SYM_START(name, SYM_L_WEAK, SYM_A_ALIGN)
#endif


#ifndef SYM_FUNC_START_WEAK_NOALIGN
#define SYM_FUNC_START_WEAK_NOALIGN(name)		\
	SYM_START(name, SYM_L_WEAK, SYM_A_NONE)
#endif


#ifndef SYM_FUNC_END
#define SYM_FUNC_END(name)				\
	SYM_END(name, SYM_T_FUNC)
#endif


#ifndef SYM_FUNC_ALIAS
#define SYM_FUNC_ALIAS(alias, name)					\
	SYM_ALIAS(alias, name, SYM_L_GLOBAL)
#endif


#ifndef SYM_FUNC_ALIAS_LOCAL
#define SYM_FUNC_ALIAS_LOCAL(alias, name)				\
	SYM_ALIAS(alias, name, SYM_L_LOCAL)
#endif


#ifndef SYM_FUNC_ALIAS_WEAK
#define SYM_FUNC_ALIAS_WEAK(alias, name)				\
	SYM_ALIAS(alias, name, SYM_L_WEAK)
#endif


#ifndef SYM_CODE_START
#define SYM_CODE_START(name)				\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_ALIGN)
#endif


#ifndef SYM_CODE_START_NOALIGN
#define SYM_CODE_START_NOALIGN(name)			\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_NONE)
#endif


#ifndef SYM_CODE_START_LOCAL
#define SYM_CODE_START_LOCAL(name)			\
	SYM_START(name, SYM_L_LOCAL, SYM_A_ALIGN)
#endif


#ifndef SYM_CODE_START_LOCAL_NOALIGN
#define SYM_CODE_START_LOCAL_NOALIGN(name)		\
	SYM_START(name, SYM_L_LOCAL, SYM_A_NONE)
#endif


#ifndef SYM_CODE_END
#define SYM_CODE_END(name)				\
	SYM_END(name, SYM_T_NONE)
#endif




#ifndef SYM_DATA_START
#define SYM_DATA_START(name)				\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_NONE)
#endif


#ifndef SYM_DATA_START_LOCAL
#define SYM_DATA_START_LOCAL(name)			\
	SYM_START(name, SYM_L_LOCAL, SYM_A_NONE)
#endif


#ifndef SYM_DATA_END
#define SYM_DATA_END(name)				\
	SYM_END(name, SYM_T_OBJECT)
#endif


#ifndef SYM_DATA_END_LABEL
#define SYM_DATA_END_LABEL(name, linkage, label)	\
	linkage(label) ASM_NL				\
	.type label SYM_T_OBJECT ASM_NL			\
	label:						\
	SYM_END(name, SYM_T_OBJECT)
#endif


#ifndef SYM_DATA
#define SYM_DATA(name, data...)				\
	SYM_DATA_START(name) ASM_NL				\
	data ASM_NL						\
	SYM_DATA_END(name)
#endif


#ifndef SYM_DATA_LOCAL
#define SYM_DATA_LOCAL(name, data...)			\
	SYM_DATA_START_LOCAL(name) ASM_NL			\
	data ASM_NL						\
	SYM_DATA_END(name)
#endif

#endif 

#endif 
