// Changes required to run embedded quake on tinysys
// Added by Engin Cilasun

#include <quakembd.h>
#include <string.h>
#include <signal.h>
#include "core.h"
#include "platform.h"
#include "vpu.h"

#define	DISPLAY_WIDTH 320
#define	DISPLAY_HEIGHT 240
//((DISPLAY_WIDTH * 9 + 8) / 16)

struct SPPlatform platform;
struct EVideoContext vx;
struct EVideoSwapContext sc;
struct SPSizeAlloc framebuffer;

void shutdowncleanup()
{
	// Back to fbcon
	VPUSetScanoutAddress(&vx, 0x18000000);
	VPUSetVideoMode(&vx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Release allocations
	SPFreeBuffer(&platform, &framebuffer);

	// Shutdown platform
	SPShutdownPlatform(&platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

int qembd_get_width()
{
	return DISPLAY_WIDTH;
}

int qembd_get_height()
{
	return DISPLAY_HEIGHT;
}

void qembd_vidinit()
{
	// Initialize platform and video system
	SPInitPlatform(&platform);
	VPUInitVideo(&vx, &platform);

	// Grab video buffer
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	framebuffer.size = stride*240;
	SPAllocateBuffer(&platform, &framebuffer);

	// Register exit handlers
	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	// Set up the video mode and frame pointers
	VPUSetVideoMode(&vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);
	sc.cycle = 0;
	sc.framebufferA = &framebuffer; // Not double-buffering
	sc.framebufferB = &framebuffer;
	VPUSwapPages(&vx, &sc);
}

void qembd_fillrect(uint8_t *src, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	uint8_t* pixels = (uint8_t*)sc.writepage;

	for (int py = 0; py < ysize; py++) {
		int offset = (y + py) * DISPLAY_WIDTH + x;
		int poffset = (y + py) * stride + x;
		for (int px = 0; px < xsize; px++)
			pixels[poffset + px] = src[offset + px];
	}
}

void qembd_refresh()
{
	// Not sure what else this is supposed to be doing, swap buffers?
}
