#include "core.h"
#include "video.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int SPInitPlatform(struct SPPlatform* _platform)
{
	_platform->audioio = 0;
	_platform->videoio = 0;
	_platform->keyboardio = 0;
	_platform->mapped_memory = (uint8_t*)MAP_FAILED;
	_platform->alloc_cursor = 0;
	_platform->memfd = -1;
	_platform->ready = 0;
	int err = 0;

	_platform->memfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (_platform->memfd < 1)
	{
		perror("can't open /dev/mem");
		err =  1;
	}

	// Map the 32MByte reserved region for CPU usage
	_platform->mapped_memory = (uint8_t*)mmap(NULL, RESERVED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, _platform->memfd, RESERVED_MEMORY_ADDRESS);
	if (_platform->mapped_memory == (uint8_t*)MAP_FAILED)
	{
		perror("can't map reserved region for CPU");
		err = 1;
	}

	// NOTE: All of our devices are listed under /sys/bus/platform/devices/

	// Gain access to the APU command FIFO
	_platform->audioio = (volatile uint32_t*)mmap(NULL, DEVICE_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, _platform->memfd, AUDIODEVICE_ADDRESS);
	if (_platform->audioio == (uint32_t*)MAP_FAILED)
	{
		perror("can't get audio module io region");
		err = 1;
	}

	// Gain access to the VPU command FIFO
	_platform->videoio = (volatile uint32_t*)mmap(NULL, DEVICE_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, _platform->memfd, VIDEODEVICE_ADDRESS);
	if (_platform->videoio == (uint32_t*)MAP_FAILED)
	{
		perror("can't get video module io region");
		err = 1;
	}

	// Gain access to the KPU command FIFO
	_platform->keyboardio = (volatile uint32_t*)mmap(NULL, DEVICE_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, _platform->memfd, KEYBDDEVICE_ADDRESS);
	if (_platform->keyboardio == (uint32_t*)MAP_FAILED)
	{
		perror("can't get keyboard module io region");
		err = 1;
	}

	if (!err)
		_platform->ready = 1;
	else
	{
		SPShutdownPlatform(_platform);
		return -1;
	}

	return 0;
}

void SPShutdownPlatform(struct SPPlatform* _platform)
{
        munmap((void*)_platform->audioio, DEVICE_MEMORY_SIZE);
        munmap((void*)_platform->videoio, DEVICE_MEMORY_SIZE);
        munmap((void*)_platform->keyboardio, DEVICE_MEMORY_SIZE);
	munmap((void*)_platform->mapped_memory, RESERVED_MEMORY_SIZE);
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
