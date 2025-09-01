#pragma once

#include "platform.h"

// VPU program instruction set
#define VCP_HALT			0x00000000
#define VCP_NOOP			0x00000001
#define VCP_WAITLINE		0x00000002
#define VCP_WAITCOLUMN		0x00000003
#define VCP_SETPIXOFF		0x00000004
#define VCP_SETCACHEROFF	0x00000005
#define VCP_SETCACHEWOFF	0x00000006
#define VCP_SETACC			0x00000007
#define VCP_SETPAL			0x00000008
#define VCP_COPYREG			0x00000009
#define VCP_ADD				0x0000000A
#define VCP_COMPARE			0x0000000B
#define VCP_BRANCH			0x0000000C
#define VCP_MUL				0x0000000D
#define VCP_DIV				0x0000000E
#define VCP_MOD				0x0000000F
#define VCP_AND				0x00000010
#define VCP_OR				0x00000011
#define VCP_XOR				0x00000012
#define VCP_NOT				0x00000013
#define VCP_SHL				0x00000014
#define VCP_SHR				0x00000015
#define VCP_LOAD			0x00000016
#define VCP_STORE			0x00000017

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
#define vcp_halt()							(VCP_HALT			|	0					| 0					| 0					)
#define vcp_noop()							(VCP_NOOP			|	0					| 0					| 0					)
#define vcp_waitline(src)					(VCP_WAITLINE		|	0					| 0					| SRCREG(src)		)
#define vcp_waitcolumn(src)					(VCP_WAITCOLUMN		|	0					| 0					| SRCREG(src)		)
#define vcp_setpixoff(src)					(VCP_SETPIXOFF		|	0					| 0					| SRCREG(src)		)
#define vcp_setcacheroff(src)				(VCP_SETCACHEROFF	|	0					| 0					| SRCREG(src)		)
#define vcp_setcachewoff(src)				(VCP_SETCACHEWOFF	|	0					| 0					| SRCREG(src)		)
#define vcp_setpal(index, src)				(VCP_SETPAL			|	INDEX(index)		| 0					| SRCREG(src)		)
#define vcp_setacc(value)					(VCP_SETACC			|	0					| 0					| IMMED24(value)	)
#define vcp_copyreg(dest, src)				(VCP_COPYREG		|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_add(dest, src)					(VCP_ADD			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_compare(dest, src, condition)	(VCP_COMPARE		|	COND(condition)		| DESTREG(dest)		| SRCREG(src)		)
#define vcp_branch(dest)					(VCP_BRANCH			|	0					| DESTREG(dest)		| SRCREG(0)			)
#define vcp_mul(dest, src)					(VCP_MUL			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_div(dest, src)					(VCP_DIV			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_mod(dest, src)					(VCP_MOD			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_and(dest, src)					(VCP_AND			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_or(dest, src)					(VCP_OR				|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_xor(dest, src)					(VCP_XOR			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_not(dest)						(VCP_NOT			|	0					| DESTREG(dest)		| 0					)
#define vcp_shl(dest, src)					(VCP_SHL			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_shr(dest, src)					(VCP_SHR			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_load(dest, src)					(VCP_LOAD			|	0					| DESTREG(dest)		| SRCREG(src)		)
#define vcp_store(src, dest)				(VCP_STORE			|	0					| DESTREG(dest)		| SRCREG(src)		)
