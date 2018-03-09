#include "core.h"
#include "video.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <metal/sys.h>

int SPInitPlatform(struct SPPlatform* _platform)
{
	_platform->videodevice = NULL;
	_platform->videoio = NULL;
	_platform->mapped_memory = (uint8_t*)MAP_FAILED;
	_platform->alloc_cursor = 0;
	_platform->memfd = -1;
	_platform->ready = 0;

	_platform->memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (_platform->memfd < 1)
	{
		perror("can't open /dev/mem");
		return -1;
	}

	struct metal_init_params init_param = METAL_INIT_DEFAULTS;
	int ret = metal_init(&init_param);
	if (ret)
	{
		perror("libmetal init failed");
		return -1;
	}

	metal_set_log_level(METAL_LOG_CRITICAL);

	// Map the 128 MBytes reserved region for CPU usage
	_platform->mapped_memory = (uint8_t*)mmap(NULL, RESERVED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, _platform->memfd, RESERVED_MEMORY_ADDRESS);
	if (_platform->mapped_memory == (uint8_t*)MAP_FAILED)
	{
		perror("can't map reserved region for CPU");
		return -1;
	}

	// NOTE: All of our devices are listed under /sys/bus/platform/devices/

	// Gain access to the VPU command FIFO
	ret = metal_device_open("platform", "40001000.videomodule", &_platform->videodevice);
	if (ret)
	{
		perror("can't open video device");
		return -1;
	}

	_platform->videoio = metal_device_io_region(_platform->videodevice, 0);
	if (_platform->videoio == NULL)
	{
		perror("can't get video module io region");
		return -1;
	}

	// Gain access to the APU command FIFO
	ret = metal_device_open("platform", "40000000.audiomodule", &_platform->audiodevice);
	if (ret)
	{
		perror("can't open audio device");
		return -1;
	}

	_platform->audioio = metal_device_io_region(_platform->audiodevice, 0);
	if (_platform->audioio == NULL)
	{
		perror("can't get audio module io region");
		return -1;
	}

	_platform->ready = 1;

	return 0;
}

void SPShutdownPlatform(struct SPPlatform* _platform)
{
	// Do we have to do these?
	//metal_io_region_close(_platform->audioio);
	//metal_io_region_close(_platform->videoio);
	//metal_device_close(_platform->audiodevice);
	//metal_device_close(_platform->videodevice);

	metal_finish();

	munmap(_platform->mapped_memory, RESERVED_MEMORY_SIZE);
	close(_platform->memfd);

	_platform->ready = 0;
}

int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	if (_platform->mapped_memory != (uint8_t*)MAP_FAILED)
	{
		uint32_t alignedSize = E32AlignUp(_sizealloc->size, 128);

		// Add bounds checking
		if (_platform->alloc_cursor + alignedSize > RESERVED_MEMORY_SIZE)
		{
			_sizealloc->cpuAddress = NULL;
			_sizealloc->dmaAddress = NULL;
			return -1; // Indicate allocation failure due to out of memory
		}

		_sizealloc->cpuAddress = _platform->mapped_memory + _platform->alloc_cursor;
		_sizealloc->dmaAddress = (uint8_t*)RESERVED_MEMORY_ADDRESS + _platform->alloc_cursor;
		_platform->alloc_cursor += alignedSize;

		return 0;
	}
	else
	{
		_sizealloc->cpuAddress = NULL;
		_sizealloc->dmaAddress = NULL;

		return -1;
	}
}

void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	// TODO
}
