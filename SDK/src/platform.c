#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "core.h"
#include "platform.h"
#include <sys/ioctl.h> // For ioctl
#include <stdint.h>
#include <stdio.h> // For perror
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <linux/input.h>

#include "vpu.h"
#include "vcp.h"
#include "apu.h"

static struct SPPlatform* g_activePlatform = NULL;

struct SPFreeBlock
{
	struct SPFreeBlock* next;
};

static inline uint32_t SPAlignSize(uint32_t size)
{
	return size ? E32AlignUp(size, SP_ALLOC_ALIGNMENT) : 0;
}

static int SPOrderForSize(uint32_t size)
{
	if (!size)
		return -1;

	uint32_t blockSize = SP_ALLOC_ALIGNMENT;
	int order = 0;
	while (order <= (int)SP_ALLOC_MAX_ORDER)
	{
		if (blockSize >= size)
			return order;
		blockSize <<= 1;
		order++;
	}

	return -1;
}

static inline uint32_t SPBlockSizeForOrder(uint32_t order)
{
	return SP_ALLOC_ALIGNMENT << order;
}

static volatile uint32_t* SPPointerFromIoctl(uint32_t value)
{
	uintptr_t addr = (uintptr_t)value;
	if (!addr)
		return NULL;
	return (volatile uint32_t*)addr;
}

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
	(void)s;
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
	// The cursor has to stay outside the framebuffer region, which is 640*480*2 bytes in size.
	platform->alloc_cursor = E32AlignUp(0x96000, SP_ALLOC_ALIGNMENT);
	platform->sandpiperfd = -1;
	platform->vx = 0;
	platform->ac = 0;
	platform->sc = 0;
	platform->ready = 0;
	memset(platform->freeLists, 0, sizeof(platform->freeLists));

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
		platform->audioio = SPPointerFromIoctl(ioctlstruct.value);

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
		platform->videoio = SPPointerFromIoctl(ioctlstruct.value);

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
		platform->paletteio = SPPointerFromIoctl(ioctlstruct.value);

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
		platform->vcpio = SPPointerFromIoctl(ioctlstruct.value);

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

	_platform->alloc_cursor = E32AlignUp(0x96000, SP_ALLOC_ALIGNMENT);
	_platform->audioio = 0;
	_platform->videoio = 0;
	_platform->paletteio = 0;
	_platform->vcpio = 0;
	memset(_platform->freeLists, 0, sizeof(_platform->freeLists));
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
	if (!_platform || !_sizealloc || _platform->mapped_memory == (uint8_t*)MAP_FAILED)
	{
		if (_sizealloc)
		{
			_sizealloc->cpuAddress = NULL;
			_sizealloc->dmaAddress = NULL;
		}
		return -1;
	}

	uint32_t alignedSize = SPAlignSize(_sizealloc->size);
	int order = SPOrderForSize(alignedSize);
	if (order < 0)
	{
		_sizealloc->cpuAddress = NULL;
		_sizealloc->dmaAddress = NULL;
		return -1;
	}

	uint32_t bucket = (uint32_t)order;
	uint32_t blockSize = SPBlockSizeForOrder(bucket);
	struct SPFreeBlock* block = _platform->freeLists[bucket];

	if (block)
	{
		_platform->freeLists[bucket] = block->next;
	}
	else
	{
		if (_platform->alloc_cursor + blockSize > RESERVED_MEMORY_SIZE)
		{
			_sizealloc->cpuAddress = NULL;
			_sizealloc->dmaAddress = NULL;
			return -1;
		}

		block = (struct SPFreeBlock*)(_platform->mapped_memory + _platform->alloc_cursor);
		_platform->alloc_cursor += blockSize;
	}

	uint8_t* cpuAddress = (uint8_t*)block;
	uintptr_t offset = (uintptr_t)(cpuAddress - _platform->mapped_memory);
	_sizealloc->cpuAddress = cpuAddress;
	_sizealloc->dmaAddress = (uint8_t*)RESERVED_MEMORY_ADDRESS + offset;
	_sizealloc->size = blockSize;
	return 0;
}

/*
 * Free a previously allocated buffer.
 * Note: This is a placeholder as the current implementation does not support freeing individual allocations.
 */
void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc)
{
	if (!_platform || !_sizealloc || !_sizealloc->cpuAddress || !_sizealloc->size)
		return;

	if (_platform->mapped_memory == (uint8_t*)MAP_FAILED)
		return;

	uint8_t* cpuAddress = _sizealloc->cpuAddress;
	uint8_t* regionStart = _platform->mapped_memory;
	uint8_t* regionEnd = regionStart + RESERVED_MEMORY_SIZE;
	if (cpuAddress < regionStart || cpuAddress >= regionEnd)
		return;

	uintptr_t offset = (uintptr_t)(cpuAddress - regionStart);
	if ((offset & (SP_ALLOC_ALIGNMENT - 1U)) != 0)
		return;

	uint32_t alignedSize = SPAlignSize(_sizealloc->size);
	int order = SPOrderForSize(alignedSize);
	if (order < 0)
		return;

	struct SPFreeBlock* block = (struct SPFreeBlock*)cpuAddress;
	block->next = _platform->freeLists[order];
	_platform->freeLists[order] = block;

	_sizealloc->cpuAddress = NULL;
	_sizealloc->dmaAddress = NULL;
	_sizealloc->size = 0;
}

// Helper macros for input device detection
#define NBITS(x) (((x) + 7) / 8)
#define test_bit(bit, array) (array[bit/8] & (1<<(bit%8)))

int SPFindKeyboardDevice()
{
	char name[256] = "Unknown";
	unsigned char bit[EV_MAX][NBITS(KEY_MAX)];
	
	for (int i = 0; i < 32; i++)
	{
		char device_path[64];
		snprintf(device_path, sizeof(device_path), "/dev/input/event%d", i);
		
		int fd = open(device_path, O_RDONLY | O_NONBLOCK);
		if (fd < 0)
			continue;
		
		// Get device name
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);
		
		// Check if device supports EV_KEY events
		memset(bit, 0, sizeof(bit));
		ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
		
		if (test_bit(EV_KEY, bit[0]))
		{
			// Check if it has typical keyboard keys
			ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX), bit[EV_KEY]);
			if (test_bit(KEY_ENTER, bit[EV_KEY]) && test_bit(KEY_SPACE, bit[EV_KEY]))
			{
				printf("Found keyboard: %s at %s\n", name, device_path);
				return fd;
			}
		}
		close(fd);
	}
	return -1;
}

int SPFindMouseDevice()
{
	char name[256] = "Unknown";
	unsigned char bit[EV_MAX][NBITS(KEY_MAX)];
	
	for (int i = 0; i < 32; i++)
	{
		char device_path[64];
		snprintf(device_path, sizeof(device_path), "/dev/input/event%d", i);
		
		int fd = open(device_path, O_RDONLY | O_NONBLOCK);
		if (fd < 0)
			continue;
		
		// Get device name
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);
		
		// Check if device supports mouse events
		memset(bit, 0, sizeof(bit));
		ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
		
		// Look for relative movement (mouse) or absolute position (touchpad)
		int has_rel = test_bit(EV_REL, bit[0]);
		int has_abs = test_bit(EV_ABS, bit[0]);
		int has_key = test_bit(EV_KEY, bit[0]);
		
		if ((has_rel || has_abs) && has_key)
		{
			// Check for mouse buttons
			ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX), bit[EV_KEY]);
			if (test_bit(BTN_MOUSE, bit[EV_KEY]) || test_bit(BTN_LEFT, bit[EV_KEY]))
			{
				printf("Found mouse: %s at %s\n", name, device_path);
				return fd;
			}
		}
		close(fd);
	}
	return -1;
}

int SPFindGamepadDevice()
{
	char name[256] = "Unknown";
	unsigned char bit[EV_MAX][NBITS(KEY_MAX)];
	
	for (int i = 0; i < 32; i++)
	{
		char device_path[64];
		snprintf(device_path, sizeof(device_path), "/dev/input/event%d", i);
		
		int fd = open(device_path, O_RDONLY | O_NONBLOCK);
		if (fd < 0)
			continue;
		
		// Get device name
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);
		
		// Check if device supports gamepad events
		memset(bit, 0, sizeof(bit));
		ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
		
		// Gamepads typically have both absolute axes (joysticks) and buttons
		if (test_bit(EV_ABS, bit[0]) && test_bit(EV_KEY, bit[0]))
		{
			// Check for gamepad buttons
			ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX), bit[EV_KEY]);
			if (test_bit(BTN_GAMEPAD, bit[EV_KEY]) || 
			    test_bit(BTN_SOUTH, bit[EV_KEY]) ||
			    test_bit(BTN_A, bit[EV_KEY]))
			{
				printf("Found gamepad: %s at %s\n", name, device_path);
				return fd;
			}
		}
		close(fd);
	}
	return -1;
}
