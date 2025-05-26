/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_INSTRUMENTATION_H
#define __LINUX_INSTRUMENTATION_H

#ifdef CONFIG_NOINSTR_VALIDATION

#include <linux/stringify.h>


#define __instrumentation_begin(c) ({					\
	asm volatile(__stringify(c) ": nop\n\t"				\
		     ".pushsection .discard.instr_begin\n\t"		\
		     ".long " __stringify(c) "b - .\n\t"		\
		     ".popsection\n\t" : : "i" (c));			\
})
#define instrumentation_begin() __instrumentation_begin(__COUNTER__)


#define __instrumentation_end(c) ({					\
	asm volatile(__stringify(c) ": nop\n\t"				\
		     ".pushsection .discard.instr_end\n\t"		\
		     ".long " __stringify(c) "b - .\n\t"		\
		     ".popsection\n\t" : : "i" (c));			\
})
#define instrumentation_end() __instrumentation_end(__COUNTER__)
#else 
# define instrumentation_begin()	do { } while(0)
# define instrumentation_end()		do { } while(0)
#endif 

#endif 
