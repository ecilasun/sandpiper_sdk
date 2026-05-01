/**
 * \file scroll.c
 * \brief VPU scrolling example
 *
 * \ingroup examples
 * This example demonstrates how to use the VPU scrolling features by
 * rendering a test pattern and smoothly scrolling it both horizontally and vertically.
 * Vertical scrolling is achieved by adjusting the scanout base address.
 * Horizontal scrolling is achieved by using the VPU's coarse cache and fine scanout shift features.
 * The background buffer is a 512x512 playfield to allow for
 * smooth scrolling in both axes without visual artifacts.
 * NOTE: This only works in 320-wide 8 bit indexed color modes.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_320_240
#define VIDEO_COLOR     ECM_8bit_Indexed
#define FRAMEBUFFER_WIDTH   512u
#define FRAMEBUFFER_HEIGHT  512u
#define VISIBLE_WIDTH       320u
#define VISIBLE_HEIGHT      240u

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBuffer;

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();

	uint32_t stride = FRAMEBUFFER_WIDTH;
	// Allocate a 512x512 8bpp playfield for scrolling in both axes.
	frameBuffer.size = FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBuffer);

	uint32_t baseAddress = (uint32_t)frameBuffer.dmaAddress;
	VPUSetWriteAddress(s_platform->vx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(s_platform->vx, baseAddress);
	VPUSetVideoModeWithStride(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable, stride);

	VPUShiftCache(s_platform->vx, 0);
	VPUShiftScanout(s_platform->vx, 0);

	// Horizontal scroll tracking
	int totalscroll_h = 0;
	int direction_h = 1;
	int maxscroll_h = (int)(FRAMEBUFFER_WIDTH - VISIBLE_WIDTH);

	// Vertical scroll tracking
	int totalscroll_v = 0;
	int direction_v = 2;
	int maxscroll_v = (int)(FRAMEBUFFER_HEIGHT - VISIBLE_HEIGHT);

	// Fill the full 512x512 buffer with a test pattern.
	for (uint32_t y = 0; y < FRAMEBUFFER_HEIGHT; y++)
	{
		for (uint32_t x = 0; x < FRAMEBUFFER_WIDTH; x++)
		{
			// Write a test pattern to test scroll
			uint8_t* pixel = (uint8_t*)frameBuffer.cpuAddress + (y * stride) + x;
			*pixel = (x ^ y) % 256;
		}
	}

	do
	{
		// Update horizontal scroll
		totalscroll_h += direction_h;
		if (totalscroll_h > maxscroll_h)
			direction_h = -1;
		else if (totalscroll_h <= 0)
			direction_h = 1;

		// Update vertical scroll
		totalscroll_v += direction_v;
		if (totalscroll_v > maxscroll_v)
			direction_v = -1;
		else if (totalscroll_v <= 0)
			direction_v = 1;

		// Apply horizontal scroll via hardware shift.
		// In 320x240x8 mode one pixel is one byte, so the new hardware split is:
		// coarse scroll in 128-byte units plus fine scroll in pixels.
		int coarseoffset = totalscroll_h >> 7;
		int pixeloffset = totalscroll_h & 127;

		VPUShiftCache(s_platform->vx, (uint8_t)coarseoffset);
		VPUShiftScanout(s_platform->vx, (uint8_t)pixeloffset);

		// Apply vertical scroll by adjusting scanout base address
		uint32_t scrolledAddress = baseAddress + (totalscroll_v * stride);
		VPUSetScanoutAddress(s_platform->vx, scrolledAddress);

		VPUWaitVSync(s_platform->vx);
	} while(1);

	return 0;
}
