/*
	@file vpudemo.c
	@brief Demonstrates how to use the Video Processing Unit (VPU).
	@note WARNING! Very rapid flickering, observe with caution.
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // for usleep()

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
	if (argc <= 1)
	{
		printf("vpudemo\nusage: vpudemo [arg]\nargs\ncpu : demo of CPU based vsync\nvpu: demo of VPU based vsync\n");
		return 0;
	}

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

	// Fill both buffers with different colors so we
	// can see the frame swap happening
	for (int y = 0; y < VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < stride; x++)
		{
			uint8_t* pixelA = (uint8_t*)frameBufferA.cpuAddress + (y * stride) + x;
			*pixelA = ((x/4) ^ (y/4)) % 256;

			uint8_t* pixelB = (uint8_t*)frameBufferB.cpuAddress + (y * stride) + x;
			*pixelB = ((x * y) / 128) % 256;
		}
	}

	if (!strcmp(argv[1], "cpu"))
	{
		// Method 1: CPU waits for vsync

		// Set up the swap context, double bufferred in this case
		s_platform->sc->cycle = 0;
		s_platform->sc->framebufferA = &frameBufferA;
		s_platform->sc->framebufferB = &frameBufferB;

		// Swap once to make it take effect
		VPUSwapPages(s_platform->vx, s_platform->sc);

		do
		{
			// TODO: Draw game frame here

			// Wait for vertical sync
			VPUWaitVSync(s_platform->vx);

			// Swap the framebuffers
			VPUSwapPages(s_platform->vx, s_platform->sc);
		} while(1);
	}
	else if (!strcmp(argv[1], "vpu"))
	{
		// Method 2: CPU submits VPU side vsync/buffer swap request

		// For hardware assisted vsync, we need to set this up once at start
		VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBufferA.dmaAddress);
		VPUSetScanoutAddress2(s_platform->vx, (uint32_t)frameBufferB.dmaAddress);

		do
		{
			// First, make sure the VPU has no pending commands
			while(VPUGetFIFONotEmpty(s_platform->vx))
			{
				//usleep(1000);
			}

			// TODO: Draw game frame here

			// Next, submit a syncswap command
			VPUSyncSwap(s_platform->vx, 0);

			// usleep(16667);

			// At this point the CPU is free to do more work on next
			// buffer, since the VPU will handle precise swap timing
		} while(1);
	}

	return 0;
}
