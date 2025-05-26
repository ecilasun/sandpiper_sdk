/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_UNIFIED_H
#define __ASM_UNIFIED_H

#if defined(__ASSEMBLY__)
	.syntax unified
#else
__asm__(".syntax unified");
#endif

#ifdef CONFIG_CPU_V7M
#define AR_CLASS(x...)
#define M_CLASS(x...)	x
#else
#define AR_CLASS(x...)	x
#define M_CLASS(x...)
#endif

#ifdef CONFIG_THUMB2_KERNEL


#define PSR_ISETSTATE	PSR_T_BIT

#define ARM(x...)
#define THUMB(x...)	x
#ifdef __ASSEMBLY__
#define W(instr)	instr.w
#else
#define WASM(instr)	#instr ".w"
#endif

#else	


#define PSR_ISETSTATE	0

#define ARM(x...)	x
#define THUMB(x...)
#ifdef __ASSEMBLY__
#define W(instr)	instr
#else
#define WASM(instr)	#instr
#endif

#endif	

#endif	
