// Changes required to run embedded quake on tinysys
// Added by Engin Cilasun

#include <quakembd.h>
#include <string.h>
#include "core.h"
#include "platform.h"
#include "vpu.h"

#define	DISPLAY_WIDTH 320
#define	DISPLAY_HEIGHT 240
//((DISPLAY_WIDTH * 9 + 8) / 16)

struct SPPlatform* s_platform;
struct SPSizeAlloc framebuffer;

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
	s_platform = SPInitPlatform();
	VPUInitVideo(s_platform->vx, s_platform);

	// Grab video buffer
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	framebuffer.size = stride*240;
	SPAllocateBuffer(s_platform, &framebuffer);

	// Set up the video mode and frame pointers
	VPUSetVideoMode(s_platform->vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);
	s_platform->sc.cycle = 0;
	s_platform->sc.framebufferA = &framebuffer; // Not double-buffering
	s_platform->sc.framebufferB = &framebuffer;
	VPUSwapPages(s_platform->vx, s_platform->sc);
}

void qembd_fillrect(uint8_t *src, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	uint8_t* pixels = (uint8_t*)s_platform->sc.writepage;

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
