#pragma once

#include "platform.h"

#define VPUCMD_SETVPAGE		0x00000000
#define VPUCMD_SETPAL		0x00000001
#define VPUCMD_SETVMODE		0x00000002
#define VPUCMD_SHIFTCACHE	0x00000003
#define VPUCMD_SHIFTSCANOUT	0x00000004
#define VPUCMD_SHIFTPIXEL	0x00000005
#define VPUCMD_SETVPAGE2	0x00000006
#define VPUCMD_SYNCSWAP		0x00000007
#define VPUCMD_WCONTROLREG	0x00000008
#define VPUCMD_WPROG		0x00000009
#define VPUCMD_WPROGADDRS	0x0000000A
#define VPUCMD_WPROGDATA	0x0000000B
#define VPUCMD_NOOP			0x000000FF

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
#define VPUINST_JUMP			0x0000000A
#define VPUINST_ADD				0x0000000B
#define VPUINST_COMPARE			0x0000000C
#define VPUINST_BRANCH			0x0000000D
#define VPUINST_MUL				0x0000000E
#define VPUINST_DIV				0x0000000F
#define VPUINST_MOD				0x00000010
#define VPUINST_AND				0x00000011
#define VPUINST_OR				0x00000012
#define VPUINST_XOR				0x00000013
#define VPUINST_NOT				0x00000014
#define VPUINST_SHL				0x00000015
#define VPUINST_SHR				0x00000016
#define VPUINST_LOAD			0x00000017
#define VPUINST_STORE			0x00000018

// Macros that help define VPU instructions with register indices or constants embedded into a single word
// Destination and source registers, as well as immediate values, are always at the same bit positions across different instructions to ensure consistency and simplify decoding.
// The source and destination register indices are 4 bits each, allowing for 16 registers.
#define vinstr_halt()							(VPUINST_HALT)
#define vinstr_noop()							(VPUINST_NOOP)
#define vinstr_waitline(src)					(VPUINST_WAITLINE | ((src & 0xF) << 16))
#define vinstr_waitcolumn(src)					(VPUINST_WAITCOLUMN | ((src & 0xF) << 16))
#define vinstr_setpixoff(src)					(VPUINST_SETPIXOFF | ((src & 0xF) << 16))
#define vinstr_setcacheroff(src)				(VPUINST_SETCACHEROFF | ((src & 0xF) << 16))
#define vinstr_setcachewoff(src)				(VPUINST_SETCACHEWOFF | ((src & 0xF) << 16))
#define vinstr_setpal(index, src)				(VPUINST_SETPAL | ((index & 0xFF) << 24) | ((src & 0xF) << 16))
#define vinstr_setacc(value)					(VPUINST_SETACC | ((value & 0xFFFFFF) << 8))
#define vinstr_copyreg(dest, src)				(VPUINST_COPYREG | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_jump(src)						(VPUINST_JUMP | ((src & 0xF) << 16))
#define vinstr_add(dest, src)					(VPUINST_ADD | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_compare(dest, src)				(VPUINST_COMPARE | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_branch(dest, src, condition)		(VPUINST_BRANCH | ((condition & 0xF) << 20) | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_mul(dest, src)					(VPUINST_MUL | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_div(dest, src)					(VPUINST_DIV | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_mod(dest, src)					(VPUINST_MOD | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_and(dest, src)					(VPUINST_AND | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_or(dest, src)					(VPUINST_OR | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_xor(dest, src)					(VPUINST_XOR | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_not(dest)						(VPUINST_NOT | ((dest & 0xF) << 12))
#define vinstr_shl(dest, src)					(VPUINST_SHL | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_shr(dest, src)					(VPUINST_SHR | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_load(dest, src)					(VPUINST_LOAD | ((dest & 0xF) << 12) | ((src & 0xF) << 16))
#define vinstr_store(src, dest)					(VPUINST_STORE | ((dest & 0xF) << 12) | ((src & 0xF) << 16))

#define VPU_AUTO 0xFFFF

#define DEFAULT_VIDE_SCANOUT_START      0x18000000

#define CONSOLEDIMGRAY 0x00
#define CONSOLEDIMBLUE 0x01
#define CONSOLEDIMGREEN 0x02
#define CONSOLEDIMCYAN 0x03
#define CONSOLEDIMRED 0x04
#define CONSOLEDIMMAGENTA 0x05
#define CONSOLEDIMYELLOW 0x06
#define CONSOLEDIMWHITE 0x07

#define CONSOLEGRAY 0x08
#define CONSOLEBLUE 0x09
#define CONSOLEGREEN 0x0A
#define CONSOLECYAN 0x0B
#define CONSOLERED 0x0C
#define CONSOLEMAGENTA 0x0D
#define CONSOLEYELLOW 0x0E
#define CONSOLEWHITE 0x0F

#define CONSOLEDEFAULTFG CONSOLEWHITE
#define CONSOLEDEFAULTBG CONSOLEDIMGRAY

// For setting up palette colors, r8g8b8
#define MAKECOLORRGB24(_r, _g, _b) ((((_r&0xFF)<<16) | (_g&0xFF)<<8) | (_b&0xFF))

// For building colors for 16bit RGB, r5g6b5
#define MAKECOLORRGB16(_r, _g, _b) ((((_r&0x1F)<<11) | (_g&0x3F)<<5) | (_b&0x1F))

void VPUInitVideo(struct EVideoContext* _context, struct SPPlatform* _platform);
void VPUShutdownVideo();

uint32_t VPUGetStride(const enum EVideoMode _mode, const enum EColorMode _cmode);
void VPUGetDimensions(const enum EVideoMode _mode, uint32_t *_width, uint32_t *_height);

// Hardware
void VPUNoop(struct EVideoContext *_context);
void VPUSetScanoutAddress(struct EVideoContext *_context, const uint32_t _scanOutAddress64ByteAligned);
void VPUSetPal(struct EVideoContext *_context, const uint8_t _paletteIndex, const uint32_t _red, const uint32_t _green, const uint32_t _blue);
void VPUSetVideoMode(struct EVideoContext *_context, const enum EVideoMode _mode, const enum EColorMode _cmode, const enum EVideoScanoutEnable _scanEnable);
void VPUShiftCache(struct EVideoContext *_context, uint8_t _offset);
void VPUShiftScanout(struct EVideoContext *_context, uint8_t _offset);
void VPUShiftPixel(struct EVideoContext *_context, uint8_t _offset);
void VPUSetScanoutAddress2(struct EVideoContext *_context, const uint32_t _scanOutAddress64ByteAligned);
void VPUSyncSwap(struct EVideoContext *_context, uint8_t _donotwaitforvsync);
uint32_t VPUReadVBlankCounter(struct EVideoContext *_context);
uint32_t VPUGetScanline(struct EVideoContext *_context);
uint32_t VPUGetFIFONotEmpty(struct EVideoContext *_context);
void VPUWriteControlRegister(struct EVideoContext *_context, uint8_t _setFlag, uint8_t _value);
void VPUProgramWriteMask(struct EVideoContext *_context, uint8_t _mask);
void VPUSetProgramAddress(struct EVideoContext *_context, uint32_t _programAddress);
void VPUWriteProgramWord(struct EVideoContext *_context, uint32_t _word);
uint8_t VPUReadControlRegister(struct EVideoContext *_context);

void VPUClear(struct EVideoContext *_context, const uint32_t _colorWord);
void VPUSetDefaultPalette(struct EVideoContext *_context);
void VPUSetWriteAddress(struct EVideoContext *_context, const uint32_t _cpuWriteAddress64ByteAligned);
void VPUSwapPages(struct EVideoContext* _context, struct EVideoSwapContext *_sc);
void VPUWaitVSync(struct EVideoContext *_context);
void VPUPrintString(struct EVideoContext *_context, const uint8_t _foregroundIndex, const uint8_t _backgroundIndex, const uint16_t _x, const uint16_t _y, const char *_message, int _length);

void VPUConsoleResolve(struct EVideoContext *_context);
void VPUConsoleScrollUp(struct EVideoContext *_context);
void VPUConsoleScrollDown(struct EVideoContext *_context);
void VPUConsoleSetColors(struct EVideoContext *_context, const uint8_t _foregroundIndex, const uint8_t _backgroundIndex);
void VPUConsoleSetForeground(struct EVideoContext *_context, const uint8_t _foregroundIndex);
void VPUConsoleSetBackground(struct EVideoContext *_context, const uint8_t _backgroundIndex);
void VPUConsoleClear(struct EVideoContext *_context);
void VPUConsolePrint(struct EVideoContext *_context, const char *_message, int _length);
void VPUConsolePrintInPlace(struct EVideoContext *_context, const char *_message, int _length);
void VPUConsoleMoveCursor(struct EVideoContext *_context, int dx, int dy);
void VPUConsoleHomeCursor(struct EVideoContext *_context);
void VPUConsoleEndCursor(struct EVideoContext *_context);
void VPUConsoleCopyLine(struct EVideoContext *_context, uint16_t _line, uint16_t _xStart, uint16_t _xEnd, char *_buffer);
void VPUInsertCharacter(struct EVideoContext *_context, uint16_t _line, uint16_t _column, uint8_t _character);
void VPURemoveCharacter(struct EVideoContext *_context, uint16_t _line, uint16_t _column);
int VPUConsoleFillLine(struct EVideoContext *_context, const char _character);
void VPUConsoleSetCursor(struct EVideoContext *_context, uint16_t _x, uint16_t _y);
