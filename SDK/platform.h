#pragma once

#include <stdint.h>
#include <metal/device.h>
#include <metal/io.h>
#include <sys/mman.h>

#define RESERVED_MEMORY_ADDRESS	0x18000000
#define RESERVED_MEMORY_SIZE	0x8000000

struct SPSizeAlloc
{
	uint32_t* cpuAddress;
	uint32_t* vpuAddress;
	uint32_t size;
};

struct SPPlatform
{
	struct metal_device *videodevice;
	struct metal_io_region *videoio;
	uint32_t* mapped_memory;
	uint32_t alloc_cursor;
	int memfd;
	int ready;
};

bool SPInitPlatform(struct SPPlatform* _platform);
void SPShutdownPlatform(struct SPPlatform* _platform);

void SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
