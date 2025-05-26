/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_EXPORT_INTERNAL_H__
#define __LINUX_EXPORT_INTERNAL_H__

#include <linux/compiler.h>
#include <linux/types.h>

#if defined(CONFIG_HAVE_ARCH_PREL32_RELOCATIONS)

#define __KSYM_ALIGN		".balign 4"
#define __KSYM_REF(sym)		".long " #sym "- ."
#elif defined(CONFIG_64BIT)
#define __KSYM_ALIGN		".balign 8"
#define __KSYM_REF(sym)		".quad " #sym
#else
#define __KSYM_ALIGN		".balign 4"
#define __KSYM_REF(sym)		".long " #sym
#endif


#define __KSYMTAB(name, sym, sec, ns)						\
	asm("	.section \"__ksymtab_strings\",\"aMS\",%progbits,1"	"\n"	\
	    "__kstrtab_" #name ":"					"\n"	\
	    "	.asciz \"" #name "\""					"\n"	\
	    "__kstrtabns_" #name ":"					"\n"	\
	    "	.asciz \"" ns "\""					"\n"	\
	    "	.previous"						"\n"	\
	    "	.section \"___ksymtab" sec "+" #name "\", \"a\""	"\n"	\
		__KSYM_ALIGN						"\n"	\
	    "__ksymtab_" #name ":"					"\n"	\
		__KSYM_REF(sym)						"\n"	\
		__KSYM_REF(__kstrtab_ ##name)				"\n"	\
		__KSYM_REF(__kstrtabns_ ##name)				"\n"	\
	    "	.previous"						"\n"	\
	)

#ifdef CONFIG_IA64
#define KSYM_FUNC(name)		@fptr(name)
#elif defined(CONFIG_PARISC) && defined(CONFIG_64BIT)
#define KSYM_FUNC(name)		P%name
#else
#define KSYM_FUNC(name)		name
#endif

#define KSYMTAB_FUNC(name, sec, ns)	__KSYMTAB(name, KSYM_FUNC(name), sec, ns)
#define KSYMTAB_DATA(name, sec, ns)	__KSYMTAB(name, name, sec, ns)

#define SYMBOL_CRC(sym, crc, sec)   \
	asm(".section \"___kcrctab" sec "+" #sym "\",\"a\""	"\n" \
	    ".balign 4"						"\n" \
	    "__crc_" #sym ":"					"\n" \
	    ".long " #crc					"\n" \
	    ".previous"						"\n")

#endif 
