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

void SPInitPlatform(struct SPPlatform* _platform)
{
	_platform->videodevice = NULL;
	_platform->videoio = NULL;
	_platform->mapped_memory = MAP_FAILED;
	_platform->alloc_cursor = 0;
	_platform->memfd = -1;
	_platform->ready = 0;

	_platform->memfd = open("/dev/mem", O_RDWR);
	if (_platform->memfd < 1)
	{
			perror("can't open /dev/mem");
			return;
	}

	struct metal_init_params init_param = METAL_INIT_DEFAULTS;
	int ret = metal_init(&init_param);
	if (ret)
	{
		perror("libmetal init failed");
		return;
	}

	// Map the 128 MBytes reserved region for CPU usage
	_platform->mapped_memory = (uint32_t*)mmap(NULL, RESERVED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, _platform->memfd, RESERVED_MEMORY_ADDRESS);
	if (_platform->mapped_memory == MAP_FAILED)
	{
		perror("can't map reserved region for CPU");
		return;
	}

	// Gain access to the VPU command FIFO
	// We use metal to talk to video device's FIFO
	// NOTE: devices are listed under /sys/bus/platform/devices/
	ret = metal_device_open("platform", "40001000.videomodule", &_platform->videodevice);
	if (ret)
	{
		perror("can't open device");
		return;
	}

	_platform->videoio = metal_device_io_region(_platform->videodevice, 0);
	if (_platform->videoio == NULL)
	{
		perror("can't get video module io region");
		return;
	}

	_platform->ready = 1;
}

void SPShutdownPlatform(struct SPPlatform* _platform)
{
	// Do we need to do these?
	//metal_io_region_close(_platform->videoio);
	//metal_device_close(_platform->videodevice);

	metal_finish();

	munmap(_platform->mapped_memory, RESERVED_MEMORY_SIZE);
	close(_platform->memfd);

	_platform->ready = 0;
}

void SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	if (_platform->mapped_memory != MAP_FAILED)
	{
		_sizealloc->cpuAddress = _platform->mapped_memory + _platform->alloc_cursor;
		_sizealloc->vpuAddress = (uint32_t*)RESERVED_MEMORY_ADDRESS + _platform->alloc_cursor;
		_platform->alloc_cursor += _sizealloc->size;
	}
	else
	{
		_sizealloc->cpuAddress = NULL;
		_sizealloc->vpuAddress = NULL;
	}
}

void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	// TODO
}
