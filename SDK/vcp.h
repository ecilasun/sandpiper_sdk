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

#define DESTREG(reg)			((reg & 0xF) << 8)
#define SRCREG1(reg)			((reg & 0xF) << 8)
#define SRCREG2(reg)			((reg & 0xF) << 12)
#define IMMED3(value)			((value & 0x7) << 24)
#define IMMED8(value)			((value & 0xFF) << 24)
#define FLAGS8(value)			((value & 0xFF) << 16)
#define IMMED24(value)			((value & 0xFFFFFFU) << 8)

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
//												[31:24]				[23:16]			[15:12]				[11:8]				[7:0]
#define vcp_halt()							(	0					| 0				| 0					| 0					| VCP_HALT			)
#define vcp_noop()							(	0					| 0				| 0					| 0					| VCP_NOOP			)
#define vcp_waitline(src)					(	0					| 0				| SRCREG2(src)		| 0					| VCP_WAITLINE		)
#define vcp_waitcolumn(src)					(	0					| 0				| SRCREG2(src)		| 0					| VCP_WAITCOLUMN	)
#define vcp_setpixoff(src)					(	0					| 0				| SRCREG2(src)		| 0					| VCP_SETPIXOFF		)
#define vcp_setcacheroff(src)				(	0					| 0				| SRCREG2(src)		| 0					| VCP_SETCACHEROFF	)
#define vcp_setcachewoff(src)				(	0					| 0				| SRCREG2(src)		| 0					| VCP_SETCACHEWOFF	)
#define vcp_setpal(index, src)				(	0					| FLAGS8(index)	| SRCREG2(src)		| 0					| VCP_SETPAL		)
#define vcp_setacc(value)					(								IMMED24(value)									| VCP_SETACC		)
#define vcp_copyreg(dest, src)				(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_COPYREG		)
#define vcp_add(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_ADD			)
#define vcp_compare(dest, src, condition)	(	IMMED3(condition)	| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_COMPARE		)
#define vcp_branch(dest)					(	0					| 0				| SRCREG2(dest)		| SRCREG1(0)		| VCP_BRANCH		)
#define vcp_mul(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_MUL			)
#define vcp_div(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_DIV			)
#define vcp_mod(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_MOD			)
#define vcp_and(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_AND			)
#define vcp_or(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_OR			)
#define vcp_xor(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_XOR			)
#define vcp_not(dest)						(	0					| 0				| 0					| DESTREG(dest)		| VCP_NOT			)
#define vcp_shl(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_SHL			)
#define vcp_shr(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_SHR			)
#define vcp_load(addr, dest)				(	0					| 0				| SRCREG2(addr)		| DESTREG(dest)		| VCP_LOAD			)
#define vcp_store(addr, src, wmask)			(	IMMED8(wmask)		| 0				| SRCREG2(src)		| SRCREG1(addr)		| VCP_STORE			)

void VCPUploadProgram(struct SPPlatform *ctx, const uint32_t *program, size_t size);
