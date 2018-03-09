#pragma once

#include <stdint.h>
#include <linux/limits.h>
#include <sys/mman.h>

// Device reserved memory start is after fbcon buffer
#define RESERVED_MEMORY_ADDRESS	0x18096000

// Hardware MMIO addresses
#define AUDIODEVICE_ADDRESS	0x40000000
#define VIDEODEVICE_ADDRESS	0x40001000
#define KEYBDDEVICE_ADDRESS	0x40002000

// 32Mbytes reserved for device access
#define RESERVED_MEMORY_SIZE	0x2000000
// Device region of access
#define DEVICE_MEMORY_SIZE	0x1000

struct SPSizeAlloc
{
	uint8_t* cpuAddress;
	uint8_t* dmaAddress;
	uint32_t size;
};

struct SPPlatform
{
	volatile uint32_t *videoio;
	volatile uint32_t *audioio;
	volatile uint32_t *keyboardio;
	uint8_t* mapped_memory;
	uint32_t alloc_cursor;
	int memfd;
	int ready;
};

int SPInitPlatform(struct SPPlatform* _platform);
void SPShutdownPlatform(struct SPPlatform* _platform);

int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
