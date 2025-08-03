#include "core.h"
#include "platform.h"
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static SPPlatform* g_activePlatform = NULL;

// ioctl numbers for sandpiper device
#define SP_IOCTL_GET_VIDEO_CTL	_IOR('k', 0, void*)
#define SP_IOCTL_GET_AUDIO_CTL	_IOR('k', 1, void*)
#define SP_IOCTL_AUDIO_READ		_IOR('k', 2, uint32_t*)
#define SP_IOCTL_AUDIO_WRITE	_IOW('k', 3, uint32_t*)
#define SP_IOCTL_VIDEO_READ		_IOR('k', 4, uint32_t*)
#define SP_IOCTL_VIDEO_WRITE	_IOW('k', 5, uint32_t*)

// NOTE: A list of all of the onboard devices can be found under /sys/bus/platform/devices/ including the audio and video devices.
// The file names are annotated with the device addresses, which is useful for MMIO mapping.

void shutdowncleanup()
{
	if (g_activePlatform)
	{
		// Switch to fbcon buffer and shut down video
		VPUShiftCache(g_activePlatform->vx, 0);
		VPUShiftScanout(g_activePlatform->vx, 0);
		VPUShiftPixel(g_activePlatform->vx, 0);
		VPUSetScanoutAddress(g_activePlatform->vx, 0x18000000);
		VPUSetVideoMode(g_activePlatform->vx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);
		VPUShutdownVideo();

		// TODO: Add audio shutdown here

		// Shutdown platform
		SPShutdownPlatform(g_activePlatform);
	}
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

struct SPPlatform* SPInitPlatform()
{
	struct SPPlatform* platform = (struct SPPlatform*)malloc(sizeof(struct SPPlatform));
	if (!platform)
	{
		fprintf(stderr, "Failed to allocate SPPlatform\n");
		return NULL;
	}

	platform->audioio = (uint32_t*)MAP_FAILED;
	platform->videoio = (uint32_t*)MAP_FAILED;
	platform->mapped_memory = (uint8_t*)MAP_FAILED;
	platform->alloc_cursor = 0x96000; // The cursor has to stay outside the framebuffer region, which is 640*480*2 bytes in size.
	platform->sandpiperfd = -1;
	platform->vx = 0;
	platform->ac = 0;
	platform->sc = 0;
	platform->ready = 0;

	int err = 0;

	platform->sandpiperfd = open("/dev/sandpiper", O_RDWR | O_SYNC);
	if (platform->sandpiperfd < 1)
	{
		perror("Can't access sandpiper device");
		err = 1;
	}

	// Map the 32MByte reserved region for CPU usage
	platform->mapped_memory = (uint8_t*)mmap(NULL, RESERVED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, platform->sandpiperfd, RESERVED_MEMORY_ADDRESS);
	if (platform->mapped_memory == (uint8_t*)MAP_FAILED)
	{
		perror("Can't map reserved region for CPU");
		err = 1;
	}

	// Grab the contol registers for audio device
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_AUDIO_CTL, &platform->audioio) < 0)
	{
		perror("Failed to get audio control");
		close(platform->sandpiperfd);
		err = 1;
	}

	// Grab the contol registers for video device
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_VIDEO_CTL, &platform->videoio) < 0)
	{
		perror("Failed to get video control");
		close(platform->sandpiperfd);
		err = 1;
	}

	if (!err)
	{
		platform->vx = (struct EVideoContext*)malloc(sizeof(struct EVideoContext));
		platform->ac = (struct EAudioContext*)malloc(sizeof(struct EAudioContext));
		platform->sc = (struct EVideoSwapContext*)malloc(sizeof(struct EVideoSwapContext));
		g_activePlatform = platform;

		// Register exit handlers
		atexit(shutdowncleanup);
		signal(SIGINT, &sigint_handler);
		signal(SIGTERM, &sigint_handler);
		signal(SIGSEGV, &sigint_handler);

		platform->ready = 1;
	}
	else
	{
		SPShutdownPlatform(platform);
		return NULL;
	}

	return platform;
}

void SPShutdownPlatform(struct SPPlatform* _platform)
{
	_platform->ready = 0;
	g_activePlatform = NULL;

	if (_platform->mapped_memory != (uint8_t*)MAP_FAILED)
	{
		munmap((void*)_platform->mapped_memory, RESERVED_MEMORY_SIZE);
		_platform->mapped_memory = (uint8_t*)MAP_FAILED;
	}

	if (_platform->sandpiperfd != -1)
	{
		close(_platform->sandpiperfd);
		_platform->sandpiperfd = -1;
	}

	if (_platform->vx)
		free(_platform->vx);
	_platform->vx = 0;

	if (_platform->ac)
		free(_platform->ac);
	_platform->ac = 0;

	if (_platform->sc)
		free(_platform->sc);
	_platform->sc = 0;

	_platform->alloc_cursor = 0x96000;
	_platform->audioio = 0;
	_platform->videoio = 0;
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
