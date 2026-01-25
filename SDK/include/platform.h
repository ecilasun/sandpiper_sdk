#pragma once

#include <stdint.h>
#include <linux/limits.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

// Small O(1) allocator parameters
#define SP_ALLOC_ALIGNMENT     128U
#define SP_ALLOC_MAX_ORDER     18U
#define SP_ALLOC_ORDER_COUNT   (SP_ALLOC_MAX_ORDER + 1U)

struct SPFreeBlock;

// Base address of the reserved memory region
#define RESERVED_MEMORY_ADDRESS	0x18000000

// 32Mbytes reserved for device access
#define RESERVED_MEMORY_SIZE	0x2000000

// Device region of access (4Kbytes for each memory-mapped I/O block)
#define DEVICE_MEMORY_SIZE		0x1000

// Shared memory allocation helper with physical and CPU addresses
struct SPSizeAlloc
{
	uint8_t* cpuAddress;
	uint8_t* dmaAddress;
	uint32_t size;
};

// Main platform structure
struct SPPlatform
{
	// Internal state
	volatile uint32_t *videoio;
	volatile uint32_t *audioio;
	volatile uint32_t *paletteio;
	volatile uint32_t *vcpio;
	uint8_t* mapped_memory;
	uint32_t alloc_cursor;
	struct SPFreeBlock* freeLists[SP_ALLOC_ORDER_COUNT];
	int sandpiperfd;

	// Status
	int ready;

	// Device contexts
	struct EVideoContext* vx;
	struct EVideoSwapContext* sc;
	struct EAudioContext* ac;
};

// Audio sample rates
enum EAPUSampleRate
{
	ASR_44_100_Hz = 0,	// 44.1000 KHz
	ASR_22_050_Hz = 1,	// 22.0500 KHz
	ASR_11_025_Hz = 2,	// 11.0250 KHz
	ASR_Halt = 3,		// Halt
};

// Audio buffer sizes
enum EAPUBufferSize
{
	ABS_128Bytes  = 0,	//   32 16bit stereo samples
	ABS_256Bytes  = 1,	//   64 16bit stereo samples
	ABS_512Bytes  = 2,	//  128 16bit stereo samples
	ABS_1024Bytes = 3,	//  256 16bit stereo samples
	ABS_2048Bytes = 4,	//  512 16bit stereo samples
	ABS_4096Bytes = 5,	// 1024 16bit stereo samples
};

// VCP program buffer sizes
enum EVCPBufferSize
{
	PRG_128Bytes  = 0,	//   32 words
	PRG_256Bytes  = 1,	//   64 words
	PRG_512Bytes  = 2,	//  128 words
	PRG_1024Bytes = 3,	//  256 words
	PRG_2048Bytes = 4,	//  512 words
	PRG_4096Bytes = 5,	// 1024 words
};

// Video modes
enum EVideoMode
{
	EVM_320_240,
	EVM_640_480,
	EVM_320_480,
	EVM_640_240,
	EVM_Count
};

// Video color modes
enum EColorMode
{
	ECM_8bit_Indexed,
	ECM_16bit_RGB,
	ECM_Count
};

// Scanline doubling
enum EVideoScanlineDoubling
{
	EVD_Disable,
	EVD_Enable,
	EVD_Count
};

// Video scanout enable/disable
enum EVideoScanoutEnable
{
	EVS_Disable,
	EVS_Enable,
	EVS_Count
};

// Audio context structure
struct EAudioContext
{
	struct SPPlatform *m_platform;
	enum EAPUSampleRate m_sampleRate;
	uint32_t m_bufferSize;
};

// Video context structure
struct EVideoContext
{
	struct SPPlatform *m_platform;
	uint8_t* m_characterBuffer;
	uint8_t* m_colorBuffer;
	enum EVideoMode m_vmode;
	enum EColorMode m_cmode;
	enum EVideoScanoutEnable m_scanEnable;
	enum EVideoScanlineDoubling m_scanlineDoubling;
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

// Video swap context structure
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

// Input device discovery functions
int SPFindKeyboardDevice();
int SPFindMouseDevice();
int SPFindGamepadDevice();

#ifdef __cplusplus
}
#endif
