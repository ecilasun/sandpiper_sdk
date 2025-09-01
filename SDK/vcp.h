#pragma once

#include "platform.h"

// VPU program instruction set
#define VPUINST_HALT			0x00000000
#define VPUINST_NOOP			0x00000001
#define VPUINST_WAITLINE		0x00000002
#define VPUINST_WAITCOLUMN		0x00000003
#define VPUINST_SETPIXOFF		0x00000004
#define VPUINST_SETCACHEROFF	0x00000005
#define VPUINST_SETCACHEWOFF	0x00000006
#define VPUINST_SETACC			0x00000007
#define VPUINST_SETPAL			0x00000008
#define VPUINST_COPYREG			0x00000009
#define VPUINST_ADD				0x0000000A
#define VPUINST_COMPARE			0x0000000B
#define VPUINST_BRANCH			0x0000000C
#define VPUINST_MUL				0x0000000D
#define VPUINST_DIV				0x0000000E
#define VPUINST_MOD				0x0000000F
#define VPUINST_AND				0x00000010
#define VPUINST_OR				0x00000011
#define VPUINST_XOR				0x00000012
#define VPUINST_NOT				0x00000013
#define VPUINST_SHL				0x00000014
#define VPUINST_SHR				0x00000015
#define VPUINST_LOAD			0x00000016
#define VPUINST_STORE			0x00000017

#define SRCREG(src)				((src & 0xF) << 12)
#define DESTREG(dest)			((dest & 0xF) << 8)
#define INDEX(index)			((index & 0xFF) << 20)
#define IMMED24(value)			((value & 0xFFFFFFU) << 8)
#define COND(conditioncode)		((conditioncode & 0x7) << 20)

#define COND_EQ					0x01	// or NE if inverted
#define COND_LT					0x02 	// or GE if inverted
#define COND_LE					0x04 	// or GT if inverted
#define COND_ZERO				0x08	// or NZ if inverted
#define COND_INV				0x10	// invert the condition code

// NOTE: JMP is implemented in two instructions as:
// compare(ACC,ACC,EQ) -> result goes to ACC register
// branch(dest) -> jump based on lowest bit of ACC

// Macros that help define VPU instructions with register indices or constants embedded into a single word
// Destination and source registers, as well as immediate values, are always at the same bit positions across different instructions to ensure consistency and simplify decoding.
// The source and destination register indices are 4 bits each, allowing for 16 registers.
#define vinstr_halt()							(VPUINST_HALT			|	0					| 0					| 0					)
#define vinstr_noop()							(VPUINST_NOOP			|	0					| 0					| 0					)
#define vinstr_waitline(src)					(VPUINST_WAITLINE		|	0					| 0					| SRCREG(src)		)
#define vinstr_waitcolumn(src)					(VPUINST_WAITCOLUMN		|	0					| 0					| SRCREG(src)		)
#define vinstr_setpixoff(src)					(VPUINST_SETPIXOFF		|	0					| 0					| SRCREG(src)		)
#define vinstr_setcacheroff(src)				(VPUINST_SETCACHEROFF	|	0					| 0					| SRCREG(src)		)
#define vinstr_setcachewoff(src)				(VPUINST_SETCACHEWOFF	|	0					| 0					| SRCREG(src)		)
#define vinstr_setpal(index, src)				(VPUINST_SETPAL			|	INDEX(index)		| 0					| SRCREG(src)		)
#define vinstr_setacc(value)					(VPUINST_SETACC			|	0					| 0					| IMMED24(value)	)
#define vinstr_copyreg(dest, src)				(VPUINST_COPYREG		|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_add(dest, src)					(VPUINST_ADD			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_compare(dest, src, condition)	(VPUINST_COMPARE		|	COND(condition)		| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_branch(dest)						(VPUINST_BRANCH			|	0					| DESTREG(dest)		| SRCREG(0)			)
#define vinstr_mul(dest, src)					(VPUINST_MUL			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_div(dest, src)					(VPUINST_DIV			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_mod(dest, src)					(VPUINST_MOD			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_and(dest, src)					(VPUINST_AND			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_or(dest, src)					(VPUINST_OR				|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_xor(dest, src)					(VPUINST_XOR			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_not(dest)						(VPUINST_NOT			|	0					| DESTREG(dest)		| 0					)
#define vinstr_shl(dest, src)					(VPUINST_SHL			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_shr(dest, src)					(VPUINST_SHR			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_load(dest, src)					(VPUINST_LOAD			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vinstr_store(src, dest)					(VPUINST_STORE			|	0					| DESTREG(dest)		| SRCREG(src)		)
