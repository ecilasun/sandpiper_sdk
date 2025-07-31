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
	volatile uint32_t *videoio;
	volatile uint32_t *audioio;
	uint8_t* mapped_memory;
	uint32_t alloc_cursor;
	int sandpiperfd;
	int ready;
};

int SPInitPlatform(struct SPPlatform* _platform);
void SPShutdownPlatform(struct SPPlatform* _platform);

void SPGetConsoleFramebuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);

uint32_t audioread32(struct SPPlatform* _platform);
void audiowrite32(struct SPPlatform* _platform, uint32_t value);
uint32_t videoread32(struct SPPlatform* _platform);
void videowrite32(struct SPPlatform* _platform, uint32_t value);

