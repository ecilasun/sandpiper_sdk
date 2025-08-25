#include "core.h"
#include "platform.h"
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "vpu.h"
#include "apu.h"

static struct SPPlatform* g_activePlatform = NULL;

struct SPIoctl
{
	uint32_t offset;
	uint32_t value;
};

// ioctl numbers for sandpiper device
#define SP_IOCTL_GET_VIDEO_CTL		_IOR('k', 0, void*)
#define SP_IOCTL_GET_AUDIO_CTL		_IOR('k', 1, void*)
#define SP_IOCTL_GET_PALETTE_CTL	_IOR('k', 2, void*)
#define SP_IOCTL_AUDIO_READ			_IOR('k', 3, void*)
#define SP_IOCTL_AUDIO_WRITE		_IOW('k', 4, void*)
#define SP_IOCTL_VIDEO_READ			_IOR('k', 5, void*)
#define SP_IOCTL_VIDEO_WRITE		_IOW('k', 6, void*)
#define SP_IOCTL_PALETTE_READ		_IOR('k', 9, void*)
#define SP_IOCTL_PALETTE_WRITE		_IOW('k', 10, void*)

// NOTE: A list of all of the onboard devices can be found under /sys/bus/platform/devices/ including the audio and video devices.
// The file names are annotated with the device addresses, which is useful for MMIO mapping.

void shutdowncleanup()
{
	if (g_activePlatform)
	{
		// Switch to fbcon buffer and shut down video
		if (g_activePlatform->vx)
		{
			// Stop all VPU activity
			VPUWriteControlRegister(g_activePlatform->vx, 0xFF, 0x00);
			// Reset scroll
			VPUShiftCache(g_activePlatform->vx, 0);
			VPUShiftScanout(g_activePlatform->vx, 0);
			VPUShiftPixel(g_activePlatform->vx, 0);
			// Set video scanout to point at linux console framebuffer
			VPUSetScanoutAddress(g_activePlatform->vx, 0x18000000);
			// Back to RGB16 mode
			VPUSetVideoMode(g_activePlatform->vx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);
			// Tear down video system
			VPUShutdownVideo();
		}

		if (g_activePlatform->ac)
		{
			// Tear down audio system, also stops all audio output
			APUShutdownAudio(g_activePlatform->ac);
		}

		// Shutdown platform
		SPShutdownPlatform(g_activePlatform);

		// Do not repeat
		g_activePlatform = NULL;
	}
}

static void signal_handler(int s)
{
	// We don't currently care about which signal was received and simply shut down the platform
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
	platform->paletteio = (uint32_t*)MAP_FAILED;
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

	// Grab the contol registers for palette device
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_PALETTE_CTL, &platform->paletteio) < 0)
	{
		perror("Failed to get palette control");
		close(platform->sandpiperfd);
		err = 1;
	}

	if (!err)
	{
		platform->ready = 1;
		platform->vx = (struct EVideoContext*)malloc(sizeof(struct EVideoContext));
		platform->ac = (struct EAudioContext*)malloc(sizeof(struct EAudioContext));
		platform->sc = (struct EVideoSwapContext*)malloc(sizeof(struct EVideoSwapContext));
		g_activePlatform = platform;

		// Register exit handlers
		atexit(shutdowncleanup);

		struct sigaction sa;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = signal_handler;
		if (sigaction(SIGINT, &sa, NULL) == -1)
		{
			perror("sigaction(SIGINT)");
			err = 1;
		}
		if (sigaction(SIGTERM, &sa, NULL) == -1)
		{
			perror("sigaction(SIGTERM)");
			err = 1;
		}
		if (sigaction(SIGSEGV, &sa, NULL) == -1)
		{
			perror("sigaction(SIGSEGV)");
			err = 1;
		}
	}

	if (err)
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
	_platform->paletteio = 0;
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

void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	// TODO
}

uint32_t audioread32(struct SPPlatform* _platform)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_AUDIO_READ, &ioctlstruct) < 0)
		return 0;
	return ioctlstruct.value;
}

void audiowrite32(struct SPPlatform* _platform, uint32_t value)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = 0;
	ioctlstruct.value = value;
	ioctl(_platform->sandpiperfd, SP_IOCTL_AUDIO_WRITE, &ioctlstruct);
}

uint32_t videoread32(struct SPPlatform* _platform)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_VIDEO_READ, &ioctlstruct) < 0)
		return 0;
	return ioctlstruct.value;
}

void videowrite32(struct SPPlatform* _platform, uint32_t value)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = 0;
	ioctlstruct.value = value;
	ioctl(_platform->sandpiperfd, SP_IOCTL_VIDEO_WRITE, &ioctlstruct);
}

uint32_t paletteread32(struct SPPlatform* _platform)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_PALETTE_READ, &ioctlstruct) < 0)
		return 0;
	return ioctlstruct.value;
}

void palettewrite32(struct SPPlatform* _platform, uint32_t offset, uint32_t value)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = offset;
	ioctlstruct.value = value;
	ioctl(_platform->sandpiperfd, SP_IOCTL_PALETTE_WRITE, &ioctlstruct);
}
