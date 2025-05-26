/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_ASSEMBLER_H__
#define __ASM_ASSEMBLER_H__

#ifndef __ASSEMBLY__
#error "Only include this from assembly code"
#endif

#include <asm/ptrace.h>
#include <asm/opcodes-virt.h>
#include <asm/asm-offsets.h>
#include <asm/page.h>
#include <asm/thread_info.h>
#include <asm/uaccess-asm.h>

#define IOMEM(x)	(x)


#ifndef __ARMEB__
#define lspull          lsr
#define lspush          lsl
#define get_byte_0      lsl #0
#define get_byte_1	lsr #8
#define get_byte_2	lsr #16
#define get_byte_3	lsr #24
#define put_byte_0      lsl #0
#define put_byte_1	lsl #8
#define put_byte_2	lsl #16
#define put_byte_3	lsl #24
#else
#define lspull          lsl
#define lspush          lsr
#define get_byte_0	lsr #24
#define get_byte_1	lsr #16
#define get_byte_2	lsr #8
#define get_byte_3      lsl #0
#define put_byte_0	lsl #24
#define put_byte_1	lsl #16
#define put_byte_2	lsl #8
#define put_byte_3      lsl #0
#endif


#ifdef CONFIG_CPU_ENDIAN_BE8
#define ARM_BE8(code...) code
#else
#define ARM_BE8(code...)
#endif


#if __LINUX_ARM_ARCH__ >= 5
#define PLD(code...)	code
#else
#define PLD(code...)
#endif


#ifdef CONFIG_CPU_FEROCEON
#define CALGN(code...) code
#else
#define CALGN(code...)
#endif

#define IMM12_MASK 0xfff


ARM(	fpreg	.req	r11	)
THUMB(	fpreg	.req	r7	)


#if __LINUX_ARM_ARCH__ >= 6
	.macro	disable_irq_notrace
	cpsid	i
	.endm

	.macro	enable_irq_notrace
	cpsie	i
	.endm
#else
	.macro	disable_irq_notrace
	msr	cpsr_c, #PSR_I_BIT | SVC_MODE
	.endm

	.macro	enable_irq_notrace
	msr	cpsr_c, #SVC_MODE
	.endm
#endif

#if __LINUX_ARM_ARCH__ < 7
	.macro	dsb, args
	mcr	p15, 0, r0, c7, c10, 4
	.endm

	.macro	isb, args
	mcr	p15, 0, r0, c7, c5, 4
	.endm
#endif

	.macro asm_trace_hardirqs_off, save=1
#if defined(CONFIG_TRACE_IRQFLAGS)
	.if \save
	stmdb   sp!, {r0-r3, ip, lr}
	.endif
	bl	trace_hardirqs_off
	.if \save
	ldmia	sp!, {r0-r3, ip, lr}
	.endif
#endif
	.endm

	.macro asm_trace_hardirqs_on, cond=al, save=1
#if defined(CONFIG_TRACE_IRQFLAGS)
	
	.if \save
	stmdb   sp!, {r0-r3, ip, lr}
	.endif
	bl\cond	trace_hardirqs_on
	.if \save
	ldmia	sp!, {r0-r3, ip, lr}
	.endif
#endif
	.endm

	.macro disable_irq, save=1
	disable_irq_notrace
	asm_trace_hardirqs_off \save
	.endm

	.macro enable_irq
	asm_trace_hardirqs_on
	enable_irq_notrace
	.endm

	.macro	save_and_disable_irqs, oldcpsr
#ifdef CONFIG_CPU_V7M
	mrs	\oldcpsr, primask
#else
	mrs	\oldcpsr, cpsr
#endif
	disable_irq
	.endm

	.macro	save_and_disable_irqs_notrace, oldcpsr
#ifdef CONFIG_CPU_V7M
	mrs	\oldcpsr, primask
#else
	mrs	\oldcpsr, cpsr
#endif
	disable_irq_notrace
	.endm


	.macro	restore_irqs_notrace, oldcpsr
#ifdef CONFIG_CPU_V7M
	msr	primask, \oldcpsr
#else
	msr	cpsr_c, \oldcpsr
#endif
	.endm

	.macro restore_irqs, oldcpsr
	tst	\oldcpsr, #PSR_I_BIT
	asm_trace_hardirqs_on cond=eq
	restore_irqs_notrace \oldcpsr
	.endm


	.irp	c,,eq,ne,cs,cc,mi,pl,vs,vc,hi,ls,ge,lt,gt,le,hs,lo
	.macro	badr\c, rd, sym
#ifdef CONFIG_THUMB2_KERNEL
	adr\c	\rd, \sym + 1
#else
	adr\c	\rd, \sym
#endif
	.endm
	.endr


	.macro	get_thread_info, rd
	
	get_current \rd
	.endm


#ifdef CONFIG_PREEMPT_COUNT
	.macro	inc_preempt_count, ti, tmp
	ldr	\tmp, [\ti, #TI_PREEMPT]	@ get preempt count
	add	\tmp, \tmp, #1			@ increment it
	str	\tmp, [\ti, #TI_PREEMPT]
	.endm

	.macro	dec_preempt_count, ti, tmp
	ldr	\tmp, [\ti, #TI_PREEMPT]	@ get preempt count
	sub	\tmp, \tmp, #1			@ decrement it
	str	\tmp, [\ti, #TI_PREEMPT]
	.endm
#else
	.macro	inc_preempt_count, ti, tmp
	.endm

	.macro	dec_preempt_count, ti, tmp
	.endm
#endif

#define USERL(l, x...)				\
9999:	x;					\
	.pushsection __ex_table,"a";		\
	.align	3;				\
	.long	9999b,l;			\
	.popsection

#define USER(x...)	USERL(9001f, x)

#ifdef CONFIG_SMP
#define ALT_SMP(instr...)					\
9998:	instr

#define ALT_UP(instr...)					\
	.pushsection ".alt.smp.init", "a"			;\
	.align	2						;\
	.long	9998b - .					;\
9997:	instr							;\
	.if . - 9997b == 2					;\
		nop						;\
	.endif							;\
	.if . - 9997b != 4					;\
		.error "ALT_UP() content must assemble to exactly 4 bytes";\
	.endif							;\
	.popsection
#define ALT_UP_B(label)					\
	.pushsection ".alt.smp.init", "a"			;\
	.align	2						;\
	.long	9998b - .					;\
	W(b)	. + (label - 9998b)					;\
	.popsection
#else
#define ALT_SMP(instr...)
#define ALT_UP(instr...) instr
#define ALT_UP_B(label) b label
#endif

	
	.macro		this_cpu_offset, rd:req
#ifdef CONFIG_SMP
ALT_SMP(mrc		p15, 0, \rd, c13, c0, 4)
#ifdef CONFIG_CPU_V6
ALT_UP_B(.L1_\@)
.L0_\@:
	.subsection	1
.L1_\@: ldr_va		\rd, __per_cpu_offset
	b		.L0_\@
	.previous
#endif
#else
	mov		\rd, #0
#endif
	.endm

	
	.macro		set_current, rn:req, tmp:req
#if defined(CONFIG_CURRENT_POINTER_IN_TPIDRURO) || defined(CONFIG_SMP)
9998:	mcr		p15, 0, \rn, c13, c0, 3		@ set TPIDRURO register
#ifdef CONFIG_CPU_V6
ALT_UP_B(.L0_\@)
	.subsection	1
.L0_\@: str_va		\rn, __current, \tmp
	b		.L1_\@
	.previous
.L1_\@:
#endif
#else
	str_va		\rn, __current, \tmp
#endif
	.endm

	
	.macro		get_current, rd:req
#if defined(CONFIG_CURRENT_POINTER_IN_TPIDRURO) || defined(CONFIG_SMP)
9998:	mrc		p15, 0, \rd, c13, c0, 3		@ get TPIDRURO register
#ifdef CONFIG_CPU_V6
ALT_UP_B(.L0_\@)
	.subsection	1
.L0_\@: ldr_va		\rd, __current
	b		.L1_\@
	.previous
.L1_\@:
#endif
#else
	ldr_va		\rd, __current
#endif
	.endm

	
	.macro		reload_current, t1:req, t2:req
#if defined(CONFIG_CURRENT_POINTER_IN_TPIDRURO) || defined(CONFIG_SMP)
#ifdef CONFIG_CPU_V6
ALT_SMP(nop)
ALT_UP_B(.L0_\@)
#endif
	ldr_this_cpu	\t1, __entry_task, \t1, \t2
	mcr		p15, 0, \t1, c13, c0, 3		@ store in TPIDRURO
.L0_\@:
#endif
	.endm


	.macro	instr_sync
#if __LINUX_ARM_ARCH__ >= 7
	isb
#elif __LINUX_ARM_ARCH__ == 6
	mcr	p15, 0, r0, c7, c5, 4
#endif
	.endm


	.macro	smp_dmb mode
#ifdef CONFIG_SMP
#if __LINUX_ARM_ARCH__ >= 7
	.ifeqs "\mode","arm"
	ALT_SMP(dmb	ish)
	.else
	ALT_SMP(W(dmb)	ish)
	.endif
#elif __LINUX_ARM_ARCH__ == 6
	ALT_SMP(mcr	p15, 0, r0, c7, c10, 5)	@ dmb
#else
#error Incompatible SMP platform
#endif
	.ifeqs "\mode","arm"
	ALT_UP(nop)
	.else
	ALT_UP(W(nop))
	.endif
#endif
	.endm


	.macro	__smp_dmb mode
#if __LINUX_ARM_ARCH__ >= 7
	.ifeqs "\mode","arm"
	dmb	ish
	.else
	W(dmb)	ish
	.endif
#elif __LINUX_ARM_ARCH__ == 6
	mcr	p15, 0, r0, c7, c10, 5	@ dmb
#else
	.error "Incompatible SMP platform"
#endif
	.endm

#if defined(CONFIG_CPU_V7M)
	
	.macro	setmode, mode, reg
	.endm
#elif defined(CONFIG_THUMB2_KERNEL)
	.macro	setmode, mode, reg
	mov	\reg, #\mode
	msr	cpsr_c, \reg
	.endm
#else
	.macro	setmode, mode, reg
	msr	cpsr_c, #\mode
	.endm
#endif


.macro safe_svcmode_maskall reg:req
#if __LINUX_ARM_ARCH__ >= 6 && !defined(CONFIG_CPU_V7M)
	mrs	\reg , cpsr
	eor	\reg, \reg, #HYP_MODE
	tst	\reg, #MODE_MASK
	bic	\reg , \reg , #MODE_MASK
	orr	\reg , \reg , #PSR_I_BIT | PSR_F_BIT | SVC_MODE
THUMB(	orr	\reg , \reg , #PSR_T_BIT	)
	bne	1f
	orr	\reg, \reg, #PSR_A_BIT
	badr	lr, 2f
	msr	spsr_cxsf, \reg
	__MSR_ELR_HYP(14)
	__ERET
1:	msr	cpsr_c, \reg
2:
#else

	setmode	PSR_F_BIT | PSR_I_BIT | SVC_MODE, \reg
#endif
.endm


#ifdef CONFIG_THUMB2_KERNEL

	.macro	usraccoff, instr, reg, ptr, inc, off, cond, abort, t=TUSER()
9999:
	.if	\inc == 1
	\instr\()b\t\cond\().w \reg, [\ptr, #\off]
	.elseif	\inc == 4
	\instr\t\cond\().w \reg, [\ptr, #\off]
	.else
	.error	"Unsupported inc macro argument"
	.endif

	.pushsection __ex_table,"a"
	.align	3
	.long	9999b, \abort
	.popsection
	.endm

	.macro	usracc, instr, reg, ptr, inc, cond, rept, abort
	@ explicit IT instruction needed because of the label
	@ introduced by the USER macro
	.ifnc	\cond,al
	.if	\rept == 1
	itt	\cond
	.elseif	\rept == 2
	ittt	\cond
	.else
	.error	"Unsupported rept macro argument"
	.endif
	.endif

	@ Slightly optimised to avoid incrementing the pointer twice
	usraccoff \instr, \reg, \ptr, \inc, 0, \cond, \abort
	.if	\rept == 2
	usraccoff \instr, \reg, \ptr, \inc, \inc, \cond, \abort
	.endif

	add\cond \ptr, #\rept * \inc
	.endm

#else	

	.macro	usracc, instr, reg, ptr, inc, cond, rept, abort, t=TUSER()
	.rept	\rept
9999:
	.if	\inc == 1
	\instr\()b\t\cond \reg, [\ptr], #\inc
	.elseif	\inc == 4
	\instr\t\cond \reg, [\ptr], #\inc
	.else
	.error	"Unsupported inc macro argument"
	.endif

	.pushsection __ex_table,"a"
	.align	3
	.long	9999b, \abort
	.popsection
	.endr
	.endm

#endif	

	.macro	strusr, reg, ptr, inc, cond=al, rept=1, abort=9001f
	usracc	str, \reg, \ptr, \inc, \cond, \rept, \abort
	.endm

	.macro	ldrusr, reg, ptr, inc, cond=al, rept=1, abort=9001f
	usracc	ldr, \reg, \ptr, \inc, \cond, \rept, \abort
	.endm


	.macro	string name:req, string
	.type \name , #object
\name:
	.asciz "\string"
	.size \name , . - \name
	.endm

	.irp	c,,eq,ne,cs,cc,mi,pl,vs,vc,hi,ls,ge,lt,gt,le,hs,lo
	.macro	ret\c, reg
#if __LINUX_ARM_ARCH__ < 6
	mov\c	pc, \reg
#else
	.ifeqs	"\reg", "lr"
	bx\c	\reg
	.else
	mov\c	pc, \reg
	.endif
#endif
	.endm
	.endr

	.macro	ret.w, reg
	ret	\reg
#ifdef CONFIG_THUMB2_KERNEL
	nop
#endif
	.endm

	.macro	bug, msg, line
#ifdef CONFIG_THUMB2_KERNEL
1:	.inst	0xde02
#else
1:	.inst	0xe7f001f2
#endif
#ifdef CONFIG_DEBUG_BUGVERBOSE
	.pushsection .rodata.str, "aMS", %progbits, 1
2:	.asciz	"\msg"
	.popsection
	.pushsection __bug_table, "aw"
	.align	2
	.word	1b, 2b
	.hword	\line
	.popsection
#endif
	.endm

#ifdef CONFIG_KPROBES
#define _ASM_NOKPROBE(entry)				\
	.pushsection "_kprobe_blacklist", "aw" ;	\
	.balign 4 ;					\
	.long entry;					\
	.popsection
#else
#define _ASM_NOKPROBE(entry)
#endif

	.macro		__adldst_l, op, reg, sym, tmp, c
	.if		__LINUX_ARM_ARCH__ < 7
	ldr\c		\tmp, .La\@
	.subsection	1
	.align		2
.La\@:	.long		\sym - .Lpc\@
	.previous
	.else
	.ifnb		\c
 THUMB(	ittt		\c			)
	.endif
	movw\c		\tmp, #:lower16:\sym - .Lpc\@
	movt\c		\tmp, #:upper16:\sym - .Lpc\@
	.endif

#ifndef CONFIG_THUMB2_KERNEL
	.set		.Lpc\@, . + 8			// PC bias
	.ifc		\op, add
	add\c		\reg, \tmp, pc
	.else
	\op\c		\reg, [pc, \tmp]
	.endif
#else
.Lb\@:	add\c		\tmp, \tmp, pc
	
	.set		.Lpc\@, . + (. - .Lb\@)

	.ifnc		\op, add
	\op\c		\reg, [\tmp]
	.endif
#endif
	.endm

	
	.macro		mov_l, dst:req, imm:req, cond
	.if		__LINUX_ARM_ARCH__ < 7
	ldr\cond	\dst, =\imm
	.else
	movw\cond	\dst, #:lower16:\imm
	movt\cond	\dst, #:upper16:\imm
	.endif
	.endm

	
	.macro		adr_l, dst:req, sym:req, cond
	__adldst_l	add, \dst, \sym, \dst, \cond
	.endm

	
	.macro		ldr_l, dst:req, sym:req, cond
	__adldst_l	ldr, \dst, \sym, \dst, \cond
	.endm

	
	.macro		str_l, src:req, sym:req, tmp:req, cond
	__adldst_l	str, \src, \sym, \tmp, \cond
	.endm

	.macro		__ldst_va, op, reg, tmp, sym, cond, offset
#if __LINUX_ARM_ARCH__ >= 7 || \
    !defined(CONFIG_ARM_HAS_GROUP_RELOCS) || \
    (defined(MODULE) && defined(CONFIG_ARM_MODULE_PLTS))
	mov_l		\tmp, \sym, \cond
#else
	
	.globl		\sym
	.reloc		.L0_\@, R_ARM_ALU_PC_G0_NC, \sym
	.reloc		.L1_\@, R_ARM_ALU_PC_G1_NC, \sym
	.reloc		.L2_\@, R_ARM_LDR_PC_G2, \sym
.L0_\@: sub\cond	\tmp, pc, #8 - \offset
.L1_\@: sub\cond	\tmp, \tmp, #4 - \offset
.L2_\@:
#endif
	\op\cond	\reg, [\tmp, #\offset]
	.endm

	
	.macro		ldr_va, rd:req, sym:req, cond, tmp, offset=0
	.ifnb		\tmp
	__ldst_va	ldr, \rd, \tmp, \sym, \cond, \offset
	.else
	__ldst_va	ldr, \rd, \rd, \sym, \cond, \offset
	.endif
	.endm

	
	.macro		str_va, rn:req, sym:req, tmp:req, cond
	__ldst_va	str, \rn, \tmp, \sym, \cond, 0
	.endm

	
	.macro		ldr_this_cpu_armv6, rd:req, sym:req
	this_cpu_offset	\rd
	.globl		\sym
	.reloc		.L0_\@, R_ARM_ALU_PC_G0_NC, \sym
	.reloc		.L1_\@, R_ARM_ALU_PC_G1_NC, \sym
	.reloc		.L2_\@, R_ARM_LDR_PC_G2, \sym
	add		\rd, \rd, pc
.L0_\@: sub		\rd, \rd, #4
.L1_\@: sub		\rd, \rd, #0
.L2_\@: ldr		\rd, [\rd, #4]
	.endm

	
	.macro		ldr_this_cpu, rd:req, sym:req, t1:req, t2:req
#ifndef CONFIG_SMP
	ldr_va		\rd, \sym, tmp=\t1
#elif __LINUX_ARM_ARCH__ >= 7 || \
      !defined(CONFIG_ARM_HAS_GROUP_RELOCS) || \
      (defined(MODULE) && defined(CONFIG_ARM_MODULE_PLTS))
	this_cpu_offset	\t1
	mov_l		\t2, \sym
	ldr		\rd, [\t1, \t2]
#else
	ldr_this_cpu_armv6 \rd, \sym
#endif
	.endm

	
	.macro		rev_l, val:req, tmp:req
	.if		__LINUX_ARM_ARCH__ < 6
	eor		\tmp, \val, \val, ror #16
	bic		\tmp, \tmp, #0x00ff0000
	mov		\val, \val, ror #8
	eor		\val, \val, \tmp, lsr #8
	.else
	rev		\val, \val
	.endif
	.endm

	.if		__LINUX_ARM_ARCH__ < 6
	.set		.Lrev_l_uses_tmp, 1
	.else
	.set		.Lrev_l_uses_tmp, 0
	.endif

	
	.macro		bl_r, dst:req, c
	.if		__LINUX_ARM_ARCH__ < 6
	mov\c		lr, pc
	mov\c		pc, \dst
	.else
	blx\c		\dst
	.endif
	.endm

#endif 
