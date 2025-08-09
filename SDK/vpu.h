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
void VPUSetScanoutAddress(struct EVideoContext *_context, const uint32_t _scanOutAddress64ByteAligned);
void VPUSetPal(struct EVideoContext *_context, const uint8_t _paletteIndex, const uint32_t _red, const uint32_t _green, const uint32_t _blue);
void VPUSetVideoMode(struct EVideoContext *_context, const enum EVideoMode _mode, const enum EColorMode _cmode, const enum EVideoScanoutEnable _scanEnable);
void VPUShiftCache(struct EVideoContext *_context, uint8_t _offset);
void VPUShiftScanout(struct EVideoContext *_context, uint8_t _offset);
void VPUShiftPixel(struct EVideoContext *_context, uint8_t _offset);
void VPUSetScanoutAddress2(struct EVideoContext *_context, uint8_t _page);
void VPUSyncSwap(struct EVideoContext *_context, uint8_t _donotwaitforvsync);
uint32_t VPUReadVBlankCounter(struct EVideoContext *_context);
uint32_t VPUGetScanline(struct EVideoContext *_context);
uint32_t VPUGetFIFONotEmpty(struct EVideoContext *_context);

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
