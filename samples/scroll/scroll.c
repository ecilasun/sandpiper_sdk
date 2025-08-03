/*
	@file scroll.c
	@brief Demonstrates horizontal scrolling using VPUShiftScanout and VPUShiftPixel.

	@note: Inherently, in 320x240 8 bit mode, screen buffer has a stride of 3*128 bytes,
	@note: which leaves us with 64 pixels of horizontal scrolling space.
	@note: We can write off-screen data to that narrow area, which can then be scrolled into view.
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_320_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    240

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBuffer;

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();
	VPUInitVideo(s_platform->vx, s_platform);
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBuffer.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBuffer);

	VPUSetWriteAddress(s_platform->vx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBuffer.dmaAddress);
	VPUSetDefaultPalette(s_platform->vx);
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	VPUShiftCache(s_platform->vx, 0);
	VPUShiftScanout(s_platform->vx, 0);
	VPUShiftPixel(s_platform->vx, 0);

	int totalscroll = 0;
	int direction = 1;
	int A = 64;

	for (int y = 0; y < VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < stride; x++)
		{
			// Write a test pattern to test scroll
			uint8_t* pixel = (uint8_t*)frameBuffer.cpuAddress + (y * stride) + x;
			*pixel = (x ^ y) % 256;
		}
	}

	do
	{
		totalscroll += direction;
		if (totalscroll > A)
			direction = -1;
		else if (totalscroll <= 0) // NOTE: do not scroll in negative direction
			direction = 1;

		int byteoffset = totalscroll >> 4;		// div 16
		int pixeloffset = totalscroll & 15;		// mod 16

		VPUShiftScanout(s_platform->vx, byteoffset);
		VPUShiftPixel(s_platform->vx, pixeloffset);

		VPUWaitVSync(s_platform->vx);
	} while(1);

	return 0;
}
