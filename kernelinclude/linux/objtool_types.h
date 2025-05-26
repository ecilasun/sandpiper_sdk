/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_OBJTOOL_TYPES_H
#define _LINUX_OBJTOOL_TYPES_H

#ifndef __ASSEMBLY__

#include <linux/types.h>


struct unwind_hint {
	u32		ip;
	s16		sp_offset;
	u8		sp_reg;
	u8		type;
	u8		signal;
};

#endif 


#define UNWIND_HINT_TYPE_UNDEFINED	0
#define UNWIND_HINT_TYPE_END_OF_STACK	1
#define UNWIND_HINT_TYPE_CALL		2
#define UNWIND_HINT_TYPE_REGS		3
#define UNWIND_HINT_TYPE_REGS_PARTIAL	4

#define UNWIND_HINT_TYPE_FUNC		5
#define UNWIND_HINT_TYPE_SAVE		6
#define UNWIND_HINT_TYPE_RESTORE	7

#endif 
