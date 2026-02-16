/**
 * \file scroll.c
 * \brief VPU scrolling example
 *
 * \ingroup examples
 * This example demonstrates how to use the VPU scrolling features by
 * rendering a test pattern and smoothly scrolling it both horizontally and vertically.
 * Vertical scrolling is achieved by adjusting the scanout base address.
 * Horizontal scrolling is achieved by using the VPU's pixel and scanout shift features.
 * The background buffer is a square (stride x stride) to allow for
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
#define VIDEO_HEIGHT    240

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBuffer;

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	// Allocate square playfield (stride x stride) for scrolling in both axes
	frameBuffer.size = stride * stride;
	SPAllocateBuffer(s_platform, &frameBuffer);

	uint32_t baseAddress = (uint32_t)frameBuffer.dmaAddress;
	VPUSetWriteAddress(s_platform->vx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(s_platform->vx, baseAddress);
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	VPUShiftCache(s_platform->vx, 0);
	VPUShiftScanout(s_platform->vx, 0);
	VPUShiftPixel(s_platform->vx, 0);

	// Horizontal scroll tracking
	int totalscroll_h = 0;
	int direction_h = 1;
	int maxscroll = 64;

	// Vertical scroll tracking
	int totalscroll_v = 0;
	int direction_v = 2;

	// Fill the square buffer with test pattern
	for (uint32_t y = 0; y < stride; y++)
	{
		for (uint32_t x = 0; x < stride; x++)
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
		if (totalscroll_h > maxscroll)
			direction_h = -1;
		else if (totalscroll_h <= 0)
			direction_h = 1;

		// Update vertical scroll
		totalscroll_v += direction_v;
		if (totalscroll_v > maxscroll)
			direction_v = -1;
		else if (totalscroll_v <= 0)
			direction_v = 1;

		// Apply horizontal scroll via hardware shift
		int byteoffset = totalscroll_h >> 4;		// div 16
		int pixeloffset = totalscroll_h & 15;		// mod 16

		VPUShiftScanout(s_platform->vx, byteoffset);
		VPUShiftPixel(s_platform->vx, pixeloffset);

		// Apply vertical scroll by adjusting scanout base address
		uint32_t scrolledAddress = baseAddress + (totalscroll_v * stride);
		VPUSetScanoutAddress(s_platform->vx, scrolledAddress);

		VPUWaitVSync(s_platform->vx);
	} while(1);

	return 0;
}
