#pragma once

#include "platform.h"

#define VPUCMD_SETVPAGE		0x00000000
#define VPUCMD_SETPAL		0x00000001
#define VPUCMD_SETVMODE		0x00000002
#define VPUCMD_CTLREGSEL	0x00000003
#define VPUCMD_CTLREGSET	0x00000004
#define VPUCMD_CTLREGCLR	0x00000005

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
#define CONSOLEDEFAULTBG CONSOLEBLUE

// Hardware format is: 12bit R:G:B
#define MAKECOLORRGB12(_r, _g, _b) ((((_r&0xF)<<8) | (_g&0xF)<<4) | (_b&0xF))

enum EVideoMode
{
	EVM_320_Wide,
	EVM_640_Wide,
	EVM_Count
};

enum EColorMode
{
	ECM_8bit_Indexed,
	ECM_16bit_RGB,
	ECM_Count
};

enum EVideoScanoutEnable
{
	EVS_Disable,
	EVS_Enable,
	EVS_Count
};

struct EVideoContext
{
	struct SPPlatform *m_platform;
	enum EVideoMode m_vmode;
	enum EColorMode m_cmode;
	enum EVideoScanoutEnable m_scanEnable;
	uint32_t m_strideInWords;
	uint32_t m_scanoutAddressCacheAligned;
	uint32_t m_cpuWriteAddressCacheAligned;
	uint32_t m_graphicsWidth, m_graphicsHeight;
	uint16_t m_consoleWidth, m_consoleHeight;
	uint16_t m_cursorX, m_cursorY;
	uint16_t m_consoleUpdated;
	uint16_t m_caretX;
	uint16_t m_caretY;
	uint8_t m_consoleColor;
	uint8_t m_caretBlink;
    uint8_t m_caretType;
};

struct EVideoSwapContext
{
	// Swap cycle counter
	uint32_t cycle;
	// Current read and write pages based on cycle
	uint32_t *readpage;	// CPU address
	uint32_t *writepage;	// VPU address
	// Frame buffers to toggle between
	struct SPSizeAlloc *framebufferA;
	struct SPSizeAlloc *framebufferB;
};

void VPUInitVideo(struct EVideoContext* _context, struct SPPlatform* _platform);
void VPUShutdownVideo();

uint32_t VPUGetStride(const enum EVideoMode _mode, const enum EColorMode _cmode);
void VPUGetDimensions(const enum EVideoMode _mode, uint32_t *_width, uint32_t *_height);

void VPUSetDefaultPalette(struct EVideoContext *_context);
void VPUSetVideoMode(struct EVideoContext *_context, const enum EVideoMode _mode, const enum EColorMode _cmode, const enum EVideoScanoutEnable _scanEnable);
void VPUSetScanoutAddress(struct EVideoContext *_context, const uint32_t _scanOutAddress64ByteAligned);
void VPUSetWriteAddress(struct EVideoContext *_context, const uint32_t _cpuWriteAddress64ByteAligned);
void VPUSetPal(struct EVideoContext *_context, const uint8_t _paletteIndex, const uint32_t _red, const uint32_t _green, const uint32_t _blue);
uint32_t VPUReadVBlankCounter(struct EVideoContext *_context);
uint32_t VPUGetScanline(struct EVideoContext *_context);
void VPUSwapPages(struct EVideoContext* _context, struct EVideoSwapContext *_sc);
void VPUWaitVSync(struct EVideoContext *_context);
void VPUPrintString(struct EVideoContext *_context, const uint8_t _foregroundIndex, const uint8_t _backgroundIndex, const uint16_t _x, const uint16_t _y, const char *_message, int _length);

void VPUConsoleResolve(struct EVideoContext *_context);
void VPUConsoleScrollUp(struct EVideoContext *_context);
void VPUConsoleSetColors(struct EVideoContext *_context, const uint8_t _foregroundIndex, const uint8_t _backgroundIndex);
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
