#pragma once

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// VCP command fifo commands
#define VCPSETBUFFERSIZE	0x0
#define VCPSTARTDMA			0x1
#define VCPEXEC				0x2

// VCP program instruction set
#define VCP_NOOP			0x00
#define VCP_LOADIMM			0x01
#define VCP_PALWRITE		0x02
#define VCP_WAITSCANLINE	0x03
#define VCP_WAITPIXEL		0x04
#define VCP_MATHOP			0x05
#define VCP_JUMP			0x06
#define VCP_CMP				0x07
#define VCP_BRANCH			0x08
#define VCP_STORE			0x09
#define VCP_LOAD			0x0A
#define VCP_READSCANINFO	0x0B
#define VCP_LOADPC			0x0C
#define VCP_LOGICOP			0x0D
#define VCP_SYSMEMWRITE		0x0E
#define VCP_SYSMEMREAD		0x0F

#define DESTREG(reg)			((reg & 0xF) << 4)
#define SRCREG1(reg)			((reg & 0xF) << 8)
#define SRCREG2(reg)			((reg & 0xF) << 12)
#define IMMED24(value)			((value & 0xFFFFFFU) << 8)
#define IMMED16(value)			((value & 0xFFFFU) << 16)
#define IMMED8(value)			((value & 0xFFU) << 24)

// Condition codes for VCP_CMP instruction
#define COND_INV		0x08	// invert the condition code (OR with values below)

#define COND_LE			0x01	// or GT if inverted
#define COND_LT			0x02 	// or GE if inverted
#define COND_EQ			0x04 	// or NE if inverted
#define COND_GT			(COND_LE | COND_INV)
#define COND_GE			(COND_LT | COND_INV)
#define COND_NE			(COND_EQ | COND_INV)

// Math operation codes for VCP_MATHOP instruction
#define OP_ADD			0x00 // Addition
#define OP_SUB			0x01 // Subtraction
#define OP_INC			0x02 // Increment
#define OP_DEC			0x03 // Decrement

// Logic operation codes for VCP_LOGICOP instruction
#define OPL_AND			0x00 // Bitwise AND
#define OPL_OR			0x01 // Inclusive OR
#define OPL_XOR			0x02 // Exclusive OR
#define OPL_ASR			0x03 // Arithmetic shift right
#define OPL_SHR			0x04 // Shift right
#define OPL_SHL			0x05 // Shift left
#define OPL_NEG			0x06 // Negate (bitwise NOT)
#define OPL_RCMP		0x07 // Read compare flag
#define OPL_RCTL		0x08 // Read VPU control register

// Register names
#define VREG_ZERO	0x00
#define VREG_1		0x01
#define VREG_2		0x02
#define VREG_3		0x03
#define VREG_4		0x04
#define VREG_5		0x05
#define VREG_6		0x06
#define VREG_7		0x07
#define VREG_8		0x08
#define VREG_9		0x09
#define VREG_A		0x0A
#define VREG_B		0x0B
#define VREG_C		0x0C
#define VREG_D		0x0D
#define VREG_E		0x0E
#define VREG_F		0x0F

// NOTE: JMP is implemented in two instructions as:
// compare(ACC,ACC,EQ) -> result goes to ACC register
// branch(dest) -> jump based on lowest bit of ACC

// NOTE ON IMMEDIATE BRANCH/JUMP:
// vcp_jumpim() / vcp_branchim() encode a signed 16-bit 2's-complement byte offset that is
// added to the PC of the executing instruction (PC-relative). Instruction addresses are multiples of 4.

// Macros that help define VPU instructions with register indices or constants embedded into a single word
// Destination and source registers, as well as immediate values, are always at the same bit positions across different instructions to ensure consistency and simplify decoding.
// The source and destination register indices are 4 bits each, allowing for 16 registers.
//												[31:24]					[15:12]				[11:8]				[7:4]				[3:0]
#define vcp_noop()								(	0					| 0					| 0					| 0					| VCP_NOOP			)
#define vcp_ldim(dest, immed)					(	IMMED24(immed)												| DESTREG(dest)		| VCP_LOADIMM		)
#define vcp_pwrt(addrs, src)					(	0					| SRCREG2(src)		| SRCREG1(addrs)	| 0					| VCP_PALWRITE		)
#define vcp_wscn(line)							(	0					| 0					| SRCREG1(line)		| 0					| VCP_WAITSCANLINE	)
#define vcp_wpix(pixel)							(	0					| 0					| SRCREG1(pixel)	| 0					| VCP_WAITPIXEL		)
#define vcp_radd(dest, src1, src2)				(	IMMED8(OP_ADD)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_MATHOP		)
#define vcp_rsub(dest, src1, src2)				(	IMMED8(OP_SUB)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_MATHOP		)
#define vcp_rinc(dest, src1)					(	IMMED8(OP_INC)		| 0					| SRCREG1(src1)		| DESTREG(dest)		| VCP_MATHOP		)
#define vcp_rdec(dest, src1)					(	IMMED8(OP_DEC)		| 0					| SRCREG1(src1)		| DESTREG(dest)		| VCP_MATHOP		)
#define vcp_jump(addrs)							(	0					| 0					| SRCREG1(addrs)	| 0x0				| VCP_JUMP			)
#define vcp_jumpim(addrs)						(	IMMED16(addrs)							| 0					| DESTREG(1)		| VCP_JUMP			)
#define vcp_cmp(cmpflags, src1, src2)			(	IMMED8(cmpflags)	| SRCREG2(src2)		| SRCREG1(src1)		| 0					| VCP_CMP			)
#define vcp_branch(addrs)						(	0					| 0					| SRCREG1(addrs)	| 0					| VCP_BRANCH		)
#define vcp_branchim(addrs)						(	IMMED16(addrs)							| 0					| DESTREG(1)		| VCP_BRANCH		)
#define vcp_store(addrs, src)					(	0					| SRCREG2(src)		| SRCREG1(addrs)	| 0					| VCP_STORE			)
#define vcp_load(addrs, dest)					(	0					| 0					| SRCREG1(addrs)	| DESTREG(dest)		| VCP_LOAD			)
#define vcp_scanline_read(dest)					(	0					| 0					| 0					| DESTREG(dest)		| VCP_READSCANINFO	)
#define vcp_scanpixel_read(dest)				(	0					| 0					| SRCREG1(1)		| DESTREG(dest)		| VCP_READSCANINFO	)
#define vcp_loadpc(dest)						(	0					| 0					| 0					| DESTREG(dest)		| VCP_LOADPC		)
#define vcp_rand(dest, src1, src2)				(	IMMED8(OPL_AND)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_ror(dest, src1, src2)				(	IMMED8(OPL_OR)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rxor(dest, src1, src2)				(	IMMED8(OPL_XOR)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rasr(dest, src1, src2)				(	IMMED8(OPL_ASR)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rshr(dest, src1, src2)				(	IMMED8(OPL_SHR)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rshl(dest, src1, src2)				(	IMMED8(OPL_SHL)		| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rneg(dest, src1)					(	IMMED8(OPL_NEG)		| 0					| SRCREG1(src1)		| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rcmp(dest)							(	IMMED8(OPL_RCMP)	| 0					| 0					| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_rctl(dest)							(	IMMED8(OPL_RCTL)	| 0					| 0					| DESTREG(dest)		| VCP_LOGICOP		)
#define vcp_sysmemwrite(addrs, src)				(	0					| SRCREG2(src)		| SRCREG1(addrs)	| 0					| VCP_SYSMEMWRITE	)
#define vcp_sysmemread(addrs, dest)				(	0					| 0					| SRCREG1(addrs)	| DESTREG(dest)		| VCP_SYSMEMREAD	)

// Pseudo instructions
#define vcp_mv(dest, src)		vcp_radd(dest, src, VREG_ZERO)
#define vcp_mvim(dest, imm)		vcp_ldim(dest, imm)
#define vcp_clr(dest)			vcp_mv(dest, VREG_ZERO)

void VCPUploadProgram(struct SPPlatform *ctx, const uint32_t* _program, enum EVCPBufferSize size);
void VCPExecProgram(struct SPPlatform *ctx, const uint8_t _execFlags);
uint32_t VCPStatus(struct SPPlatform *ctx);

#ifdef __cplusplus
}
#endif
