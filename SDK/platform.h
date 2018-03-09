#pragma once

#include <stdint.h>
#include <linux/limits.h>
#include <metal/device.h>
#include <metal/io.h>
#include <sys/mman.h>

// Device reserved memory start is after fbcon buffer
#define RESERVED_MEMORY_ADDRESS	0x18096000

// 32Mbytes reserved for device access
#define RESERVED_MEMORY_SIZE	0x2000000

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
	struct metal_device *keyboarddevice;
	struct metal_io_region *videoio;
	struct metal_io_region *audioio;
	struct metal_io_region *keyboardio;
	uint8_t* mapped_memory;
	uint32_t alloc_cursor;
	int memfd;
	int ready;
};

int SPInitPlatform(struct SPPlatform* _platform);
void SPShutdownPlatform(struct SPPlatform* _platform);

int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);
