/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_OBJTOOL_H
#define _LINUX_OBJTOOL_H

#include <linux/objtool_types.h>

#ifdef CONFIG_OBJTOOL

#include <asm/asm.h>

#ifndef __ASSEMBLY__

#define UNWIND_HINT(type, sp_reg, sp_offset, signal)	\
	"987: \n\t"						\
	".pushsection .discard.unwind_hints\n\t"		\
					\
	".long 987b - .\n\t"					\
	".short " __stringify(sp_offset) "\n\t"			\
	".byte " __stringify(sp_reg) "\n\t"			\
	".byte " __stringify(type) "\n\t"			\
	".byte " __stringify(signal) "\n\t"			\
	".balign 4 \n\t"					\
	".popsection\n\t"


#define STACK_FRAME_NON_STANDARD(func) \
	static void __used __section(".discard.func_stack_frame_non_standard") \
		*__func_stack_frame_non_standard_##func = func


#ifdef CONFIG_FRAME_POINTER
#define STACK_FRAME_NON_STANDARD_FP(func) STACK_FRAME_NON_STANDARD(func)
#else
#define STACK_FRAME_NON_STANDARD_FP(func)
#endif

#define ANNOTATE_NOENDBR					\
	"986: \n\t"						\
	".pushsection .discard.noendbr\n\t"			\
	".long 986b\n\t"					\
	".popsection\n\t"

#define ASM_REACHABLE							\
	"998:\n\t"							\
	".pushsection .discard.reachable\n\t"				\
	".long 998b\n\t"						\
	".popsection\n\t"

#else 


#define ANNOTATE_INTRA_FUNCTION_CALL				\
	999:							\
	.pushsection .discard.intra_function_calls;		\
	.long 999b;						\
	.popsection;


.macro UNWIND_HINT type:req sp_reg=0 sp_offset=0 signal=0
.Lhere_\@:
	.pushsection .discard.unwind_hints
		
		.long .Lhere_\@ - .
		.short \sp_offset
		.byte \sp_reg
		.byte \type
		.byte \signal
		.balign 4
	.popsection
.endm

.macro STACK_FRAME_NON_STANDARD func:req
	.pushsection .discard.func_stack_frame_non_standard, "aw"
	.long \func - .
	.popsection
.endm

.macro STACK_FRAME_NON_STANDARD_FP func:req
#ifdef CONFIG_FRAME_POINTER
	STACK_FRAME_NON_STANDARD \func
#endif
.endm

.macro ANNOTATE_NOENDBR
.Lhere_\@:
	.pushsection .discard.noendbr
	.long	.Lhere_\@
	.popsection
.endm


.macro VALIDATE_UNRET_BEGIN
#if defined(CONFIG_NOINSTR_VALIDATION) && \
	(defined(CONFIG_CPU_UNRET_ENTRY) || defined(CONFIG_CPU_SRSO))
.Lhere_\@:
	.pushsection .discard.validate_unret
	.long	.Lhere_\@ - .
	.popsection
#endif
.endm

.macro REACHABLE
.Lhere_\@:
	.pushsection .discard.reachable
	.long	.Lhere_\@
	.popsection
.endm

#endif 

#else 

#ifndef __ASSEMBLY__

#define UNWIND_HINT(type, sp_reg, sp_offset, signal) "\n\t"
#define STACK_FRAME_NON_STANDARD(func)
#define STACK_FRAME_NON_STANDARD_FP(func)
#define ANNOTATE_NOENDBR
#define ASM_REACHABLE
#else
#define ANNOTATE_INTRA_FUNCTION_CALL
.macro UNWIND_HINT type:req sp_reg=0 sp_offset=0 signal=0
.endm
.macro STACK_FRAME_NON_STANDARD func:req
.endm
.macro ANNOTATE_NOENDBR
.endm
.macro REACHABLE
.endm
#endif

#endif 

#endif 
