#pragma once

#include <stdint.h>
#include <linux/limits.h>
#include <metal/device.h>
#include <metal/io.h>
#include <sys/mman.h>

#define RESERVED_MEMORY_ADDRESS	0x18000000
#define RESERVED_MEMORY_SIZE	0x8000000

struct SPSizeAlloc
{
	uint8_t* cpuAddress;
	uint8_t* dmaAddress;
	uint32_t size;
};

struct SPPlatform
{
	struct metal_device *videodevice;
	struct metal_device *audiodevice;
	struct metal_io_region *videoio;
	struct metal_io_region *audioio;
	uint8_t* mapped_memory;
	uint32_t alloc_cursor;
	int memfd;
	int ready;
};

int SPInitPlatform(struct SPPlatform* _platform);
void SPShutdownPlatform(struct SPPlatform* _platform);

int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
