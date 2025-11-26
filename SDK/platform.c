#include "core.h"
#include "platform.h"
#include <sys/ioctl.h> // For ioctl
#include <stdint.h>
#include <stdio.h> // For perror
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "vpu.h"
#include "vcp.h"
#include "apu.h"

static struct SPPlatform* g_activePlatform = NULL;

// ioctl numbers for sandpiper device
#define SP_IOCTL_GET_VIDEO_CTL		_IOR('k', 0, void*)
#define SP_IOCTL_GET_AUDIO_CTL		_IOR('k', 1, void*)
#define SP_IOCTL_GET_PALETTE_CTL	_IOR('k', 2, void*)
#define SP_IOCTL_GET_VCP_CTL		_IOR('k', 11, void*)

// NOTE: A list of all of the onboard devices can be found under /sys/bus/platform/devices/ including the audio and video devices.
// The file names are annotated with the device addresses, which is useful for MMIO mapping.

/*
 * This function is called at program exit to ensure that the Sandpiper platform is cleanly shut down.
 */
void shutdowncleanup()
{
	if (g_activePlatform)
	{
		// Switch to fbcon buffer and shut down video
		if (g_activePlatform->vx)
		{
			// NOTE: The sandpiper device driver takes care of the following:
			// - Restore vide scanout address to linux console framebuffer (0x18000000)
			// - Restore video mode to RGB16 640x480
			// - Stop all VCP program activity
			// - Reset VPU control registers
			// - Reset scroll registers
			// - Stop all audio output

			// We only need to make sure we free memory and tear down API side here
			
			free(g_activePlatform->vx);
			g_activePlatform->vx = 0;
		}

		if (g_activePlatform->ac)
		{
			free(g_activePlatform->ac);
			g_activePlatform->ac = 0;
		}

		// Shutdown platform
		SPShutdownPlatform(g_activePlatform);

		// Do not repeat
		g_activePlatform = NULL;
	}
}

/*
 * Signal handler to catch termination signals and ensure clean shutdown.
 */
static void signal_handler(int s)
{
	// We don't currently care about which signal was received and simply shut down the platform
	shutdowncleanup();
	exit(0);
}

/*
 * Initialize the Sandpiper platform, mapping necessary resources and setting up device contexts.
 */
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
	platform->vcpio = (uint32_t*)MAP_FAILED;
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

	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;

	// Grab the contol registers for audio device
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_AUDIO_CTL, &ioctlstruct) < 0)
	{
		perror("Failed to get audio control");
		close(platform->sandpiperfd);
		err = 1;
	}
	else
		platform->audioio = (volatile uint32_t*)ioctlstruct.value;

	// Grab the contol registers for video device
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_VIDEO_CTL, &ioctlstruct) < 0)
	{
		perror("Failed to get video control");
		close(platform->sandpiperfd);
		err = 1;
	}
	else
		platform->videoio = (volatile uint32_t*)ioctlstruct.value;

	// Grab the contol registers for palette device
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_PALETTE_CTL, &ioctlstruct) < 0)
	{
		perror("Failed to get palette control");
		close(platform->sandpiperfd);
		err = 1;
	}
	else
		platform->paletteio = (volatile uint32_t*)ioctlstruct.value;

	// Grab the contol registers for VCP (this is inside VPU for now)
	ioctlstruct.offset = 0;
	ioctlstruct.value = 0;
	if (ioctl(platform->sandpiperfd, SP_IOCTL_GET_VCP_CTL, &ioctlstruct) < 0)
	{
		perror("Failed to get coprocessor control");
		close(platform->sandpiperfd);
		err = 1;
	}
	else
		platform->vcpio = (volatile uint32_t*)ioctlstruct.value;

	if (!err)
	{
		platform->ready = 1;
		platform->vx = (struct EVideoContext*)malloc(sizeof(struct EVideoContext));
		platform->ac = (struct EAudioContext*)malloc(sizeof(struct EAudioContext));
		platform->sc = (struct EVideoSwapContext*)malloc(sizeof(struct EVideoSwapContext));
		g_activePlatform = platform;

		// Start up main video and audio systems
		VPUInitVideo(g_activePlatform->vx, g_activePlatform);
		APUInitAudio(g_activePlatform->ac, g_activePlatform);

		// Register exit handlers
		atexit(shutdowncleanup);

		// We handle termination, segmentation fault, abort, and illegal instruction signals to ensure clean shutdown
		// NOTE: If we hang at exit, we may need to add more signals here.
		struct sigaction sa;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = signal_handler;
		if (sigaction(SIGINT, &sa, NULL) == -1)
		{
			// Interrupt signal
			perror("sigaction(SIGINT)");
			err = 1;
		}
		if (sigaction(SIGTERM, &sa, NULL) == -1)
		{
			// Termination signal
			perror("sigaction(SIGTERM)");
			err = 1;
		}
		if (sigaction(SIGSEGV, &sa, NULL) == -1)
		{
			// Segmentation fault
			perror("sigaction(SIGSEGV)");
			err = 1;
		}
		if (sigaction(SIGABRT, &sa, NULL) == -1)
		{
			// Abort signal from abort()
			perror("sigaction(SIGABRT)");
			err = 1;
		}
		if (sigaction(SIGILL, &sa, NULL) == -1)
		{
			// Illegal instruction
			perror("sigaction(SIGILL)");
			err = 1;
		}
		if (sigaction(SIGFPE, &sa, NULL) == -1)
		{
			// Floating point exception
			perror("sigaction(SIGFPE)");
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

/*
 * Shutdown the Sandpiper platform, unmapping resources and closing device handles.
 */
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
	_platform->vcpio = 0;
}

/*
 * Retrieve the console framebuffer addresses for CPU and DMA access.
 */
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

/*
 * Allocate a buffer from the reserved memory region.
 */
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

/*
 * Free a previously allocated buffer.
 * Note: This is a placeholder as the current implementation does not support freeing individual allocations.
 */
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	// TODO
}
