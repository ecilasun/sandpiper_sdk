#pragma once

#include "platform.h"

// VPU program instruction set
#define VCP_HALT			0x00
#define VCP_NOOP			0x01
#define VCP_WAITLINE		0x02
#define VCP_WAITCOLUMN		0x03
#define VCP_SETPIXOFF		0x04
#define VCP_SETCACHEROFF	0x05
#define VCP_SETCACHEWOFF	0x06
#define VCP_SETACC			0x07
#define VCP_SETPAL			0x08
#define VCP_COPYREG			0x09
#define VCP_ADD				0x0A
#define VCP_COMPARE			0x0B
#define VCP_BRANCH			0x0C
#define VCP_JUMP			0x0D
#define VCP_RESERVED1		0x0E
#define VCP_RESERVED2		0x0F
#define VCP_AND				0x10
#define VCP_OR				0x11
#define VCP_XOR				0x12
#define VCP_NOT				0x13
#define VCP_SHL				0x14
#define VCP_SHR				0x15
#define VCP_LOAD			0x16
#define VCP_STORE			0x17

#define DESTREG(reg)			((reg & 0xF) << 8)
#define SRCREG1(reg)			((reg & 0xF) << 8)
#define SRCREG2(reg)			((reg & 0xF) << 12)
#define FLAGS8(value)			((value & 0xFF) << 16)
#define IMMED8(value)			((value & 0xFF) << 24)
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
#define vcp_setpal(index, src)				(	IMMED8(wmask)		| FLAGS8(index)	| SRCREG2(src)		| 0					| VCP_SETPAL		)
#define vcp_setacc(value)					(	IMMED24(value)																| VCP_SETACC		)
#define vcp_copyreg(dest, src)				(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_COPYREG		)
#define vcp_add(dest, src)					(	0					| 0				| SRCREG2(src)		| DESTREG(dest)		| VCP_ADD			)
#define vcp_compare(dest, src, cond)		(	0					| FLAGS8(cond)	| SRCREG2(src)		| DESTREG(dest)		| VCP_COMPARE		)
#define vcp_branch(dest)					(	0					| 0				| SRCREG2(dest)		| SRCREG1(0)		| VCP_BRANCH		)
#define vcp_jump(dest)						(	0					| 0				| SRCREG2(dest)		| 0					| VCP_JUMP			)
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
