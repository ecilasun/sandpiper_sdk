#include "core.h"
#include "platform.h"
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// ioctl numbers for sandpiper device
#define SP_IOCTL_GET_VIDEO_CTL	_IOR('k', 0, void*)
#define SP_IOCTL_GET_AUDIO_CTL	_IOR('k', 1, void*)
#define SP_IOCTL_AUDIO_READ		_IOR('k', 2, uint32_t*)
#define SP_IOCTL_AUDIO_WRITE	_IOW('k', 3, uint32_t*)
#define SP_IOCTL_VIDEO_READ		_IOR('k', 4, uint32_t*)
#define SP_IOCTL_VIDEO_WRITE	_IOW('k', 5, uint32_t*)

// NOTE: A list of all of the onboard devices can be found under /sys/bus/platform/devices/ including the audio and video devices.
// The file names are annotated with the device addresses, which is useful for MMIO mapping.

int SPInitPlatform(struct SPPlatform* _platform)
{
	_platform->audioio = (uint32_t*)MAP_FAILED;
	_platform->videoio = (uint32_t*)MAP_FAILED;
	_platform->mapped_memory = (uint8_t*)MAP_FAILED;
	_platform->alloc_cursor = 0x96000; // The cursor has to stay outside the framebuffer region, which is 640*480*2 bytes in size.
	_platform->sandpiperfd = -1;
	_platform->ready = 0;
	int err = 0;

	_platform->sandpiperfd = open("/dev/sandpiper", O_RDWR | O_SYNC);
	if (_platform->sandpiperfd < 1)
	{
		perror("Can't access sandpiper device");
		err = 1;
	}

	// Map the 32MByte reserved region for CPU usage
	_platform->mapped_memory = (uint8_t*)mmap(NULL, RESERVED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _platform->sandpiperfd, RESERVED_MEMORY_ADDRESS);
	if (_platform->mapped_memory == (uint8_t*)MAP_FAILED)
	{
		perror("Can't map reserved region for CPU");
		err = 1;
	}

	// Grab the contol registers for audio device
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_GET_AUDIO_CTL, &_platform->audioio) < 0)
	{
		perror("Failed to get audio control");
		close(_platform->sandpiperfd);
		err = 1;
	}

	// Grab the contol registers for video device
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_GET_VIDEO_CTL, &_platform->videoio) < 0)
	{
		perror("Failed to get video control");
		close(_platform->sandpiperfd);
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
	_platform->ready = 0;

	munmap((void*)_platform->audioio, DEVICE_MEMORY_SIZE);
	munmap((void*)_platform->videoio, DEVICE_MEMORY_SIZE);
	munmap((void*)_platform->mapped_memory, RESERVED_MEMORY_SIZE);
	close(_platform->sandpiperfd);

	_platform->alloc_cursor = 0x96000;
	_platform->audioio = 0;
	_platform->videoio = 0;
	_platform->mapped_memory = (uint8_t*)MAP_FAILED;
}

void SPGetConsoleFramebuffer(struct SPPlatform* _platform, struct SPSizeAlloc* _sizealloc)
{
	if (_platform->mapped_memory != (uint8_t*)MAP_FAILED)
	{
		_sizealloc->cpuAddress = _platform->mapped_memory;
		_sizealloc->dmaAddress = (uint8_t*)RESERVED_MEMORY_ADDRESS;
	}
	else
	{
		_sizealloc->cpuAddress = NULL;
		_sizealloc->dmaAddress = NULL;
	}
}

int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc* _sizealloc)
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

void SPFreeBuffer(struct SPPlatform* /*_platform*/, struct SPSizeAlloc */*_sizealloc*/)
{
	// TODO
}

uint32_t audioread32(struct SPPlatform* _platform)
{
	uint32_t value = 0;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_AUDIO_READ, &value) < 0)
		return 0;
	return value;
}

void audiowrite32(struct SPPlatform* _platform, uint32_t value)
{
	ioctl(_platform->sandpiperfd, SP_IOCTL_AUDIO_WRITE, &value);
}

uint32_t videoread32(struct SPPlatform* _platform)
{
	uint32_t value = 0;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_VIDEO_READ, &value) < 0)
		return 0;
	return value;
}

void videowrite32(struct SPPlatform* _platform, uint32_t value)
{
	ioctl(_platform->sandpiperfd, SP_IOCTL_VIDEO_WRITE, &value);
}
