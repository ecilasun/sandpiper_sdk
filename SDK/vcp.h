#pragma once

#include "platform.h"

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
#define VCP_ADD				0x05
#define VCP_JUMP			0x06
#define VCP_CMP				0x07
#define VCP_BRANCH			0x08
#define VCP_STORE			0x09
#define VCP_LOAD			0x0A
#define VCP_READSCANLINE	0x0B
#define VCP_READSCANPIXEL	0x0C
#define VCP_AND				0x0D
#define VCP_OR				0x0E
#define VCP_XOR				0x0F

#define DESTREG(reg)			((reg & 0xF) << 4)
#define SRCREG1(reg)			((reg & 0xF) << 8)
#define SRCREG2(reg)			((reg & 0xF) << 12)
#define IMMED24(value)			((value & 0xFFFFFFU) << 8)
#define IMMED8(value)			((value & 0xFFU) << 24)

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
//												[31:24]				[15:12]				[11:8]				[7:4]				[3:0]
#define vcp_noop()					(	0					| 0					| 0					| 0					| VCP_NOOP			)
#define vcp_ldim(dest, immed)				(	IMMED24(immed)												| DESTREG(dest)		| VCP_LOADIMM		)
#define vcp_pwrt(addrs, src)				(	0					| SRCREG2(src)		| SRCREG1(addrs)	| 0					| VCP_PALWRITE		)
#define vcp_wscn(line)					(	0					| 0					| SRCREG1(line)		| 0					| VCP_WAITSCANLINE	)
#define vcp_wpix(pixel)					(	0					| 0					| SRCREG1(pixel)	| 0					| VCP_WAITPIXEL		)
#define vcp_radd(dest, src1, src2)			(	0					| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_ADD			)
#define vcp_jump(addrs)					(	0					| 0					| SRCREG1(addrs)	| 0					| VCP_JUMP			)
#define vcp_cmp(cmpflags, dest, src1, src2)		(	IMMED8(cmpflags)	| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_CMP			)
#define vcp_branch(addrs, src)				(	0					| SRCREG2(src)		| SRCREG1(addrs)	| 0					| VCP_BRANCH		)
#define vcp_store(addrs, src)				(	0					| SRCREG2(src)		| SRCREG1(addrs)	| 0					| VCP_STORE			)
#define vcp_load(addrs, dest)				(	0					| 0					| SRCREG1(addrs)	| DESTREG(dest)		| VCP_LOAD			)
#define vcp_scanline_read(dest)				(	0					| 0					| 0					| DESTREG(dest)		| VCP_READSCANLINE	)
#define vcp_scanpixel_read(dest)			(	0					| 0					| 0					| DESTREG(dest)		| VCP_READSCANPIXEL	)
#define vcp_and(dest, src1, src2)			(	0					| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_AND			)
#define vcp_or(dest, src1, src2)			(	0					| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_OR			)
#define vcp_xor(dest, src1, src2)			(	0					| SRCREG2(src2)		| SRCREG1(src1)		| DESTREG(dest)		| VCP_XOR			)

void VCPUploadProgram(struct SPPlatform *ctx, const uint32_t* _program, enum EVCPBufferSize size);
void VCPExecProgram(struct SPPlatform *ctx, const uint8_t _execFlags);
uint32_t VCPStatus(struct SPPlatform *ctx);
