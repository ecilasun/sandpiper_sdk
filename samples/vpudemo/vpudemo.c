/*
	@file vpudemo.c
	@brief Demonstrates how to use the Video Processing Unit (VPU).
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
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();
	VPUInitVideo(s_platform->vx, s_platform);

	// Set up the video output mode
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	// Set a color palette since we're in indexed color mode
	// Here we're using the built-in default, check the VPUSetDefaultPalette() function to roll your own.
	VPUSetDefaultPalette(s_platform->vx);

	// Allocate our two frame buffers
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB);

	// Set up the swap context, double bufferred in this case
	s_platform->sc->cycle = 0;
	s_platform->sc->framebufferA = &frameBufferA;
	s_platform->sc->framebufferB = &frameBufferB;

	// Swap once to make it take effect
	VPUSwapPages(s_platform->vx, &s_platform->sc);

	// Fill both buffers with different test patterns
	for (int y = 0; y < VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < stride; x++)
		{
			uint8_t* pixelA = (uint8_t*)frameBufferA.cpuAddress + (y * stride) + x;
			*pixelA = (x ^ y) % 256;

			uint8_t* pixelB = (uint8_t*)frameBufferB.cpuAddress + (y * stride) + x;
			*pixelB = ((x * y) + 128) % 256;
		}
	}

	do
	{
		// Main loop goes here

		// Wait for vertical sync
		VPUWaitVSync(s_platform->vx);

		// Swap the framebuffers
		VPUSwapPages(s_platform->vx, &s_platform->sc);
	} while(1);

	return 0;
}
