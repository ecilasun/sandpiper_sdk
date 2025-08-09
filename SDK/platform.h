#pragma once

#include <stdint.h>
#include <linux/limits.h>
#include <sys/mman.h>

// Base address of the reserved memory region
#define RESERVED_MEMORY_ADDRESS	0x18000000

// Hardware MMIO addresses
#define AUDIODEVICE_ADDRESS	0x40000000
#define VIDEODEVICE_ADDRESS	0x40001000

// 32Mbytes reserved for device access
#define RESERVED_MEMORY_SIZE	0x2000000
// Device region of access
#define DEVICE_MEMORY_SIZE		0x1000

struct SPSizeAlloc
{
	uint8_t* cpuAddress;
	uint8_t* dmaAddress;
	uint32_t size;
};

struct SPPlatform
{
	// Internal state
	volatile uint32_t *videoio;
	volatile uint32_t *audioio;
	uint8_t* mapped_memory;
	uint32_t alloc_cursor;
	int sandpiperfd;

	// Status
	int ready;

	// Device contexts
	struct EVideoContext* vx;
	struct EVideoSwapContext* sc;
	struct EAudioContext* ac;
};

enum EAPUSampleRate
{
	ASR_44_100_Hz = 0,	// 44.1000 KHz
	ASR_22_050_Hz = 1,	// 22.0500 KHz
	ASR_11_025_Hz = 2,	// 11.0250 KHz
	ASR_Halt = 3,		// Halt
};

enum EAPUBufferSize
{
	ABS_256Bytes  = 0,	//  64 16bit stereo samples
	ABS_512Bytes  = 1,	// 128 16bit stereo samples
	ABS_1024Bytes = 2,	// 256 16bit stereo samples
	ABS_2048Bytes = 3,	// 512 16bit stereo samples
	ABS_4096Bytes = 4,	// 1024 16bit stereo samples
	ABS_128Bytes = 5,	// 32 16bit stereo samples - minimum allowed
};

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

struct EAudioContext
{
	struct SPPlatform *m_platform;
	enum EAPUSampleRate m_sampleRate;
	uint32_t m_bufferSize;
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
	uint8_t *readpage;	// CPU address
	uint8_t *writepage;	// VPU address
	// Frame buffers to toggle between
	struct SPSizeAlloc *framebufferA;
	struct SPSizeAlloc *framebufferB;
};

struct SPPlatform* SPInitPlatform();
void SPShutdownPlatform(struct SPPlatform* _platform);

void SPGetConsoleFramebuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);

uint32_t audioread32(struct SPPlatform* _platform);
uint32_t audioread32hi(struct SPPlatform* _platform);
void audiowrite32(struct SPPlatform* _platform, uint32_t value);
uint32_t videoread32(struct SPPlatform* _platform);
uint32_t videoread32hi(struct SPPlatform* _platform);
void videowrite32(struct SPPlatform* _platform, uint32_t value);

