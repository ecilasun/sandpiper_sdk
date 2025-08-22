/*
	@file vsyncdemo.c
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

void printusage()
{
	printf("vsyncdemo\nusage: vsyncdemo [arg]\nargs\ncpu : demo of CPU based vsync\nvpu: demo of VPU based vsync\n");
}

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		printusage();
		return 0;
	}

	s_platform = SPInitPlatform();
	VPUInitVideo(s_platform->vx, s_platform);

	// Set up the video output mode
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	// Set a color palette since we're in indexed color mode
	// Here we're using the built-in default, check the VPUSetDefaultPalette() function to roll your own.
	VPUSetDefaultPalette(s_platform->vx);

	uint8_t regs = VPUReadControlRegister(s_platform->vx);
	printf("VPU control registers: %2X\n", regs);

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
			// 1) Draw game frame here
			// usleep(16667);

			// 2) Wait for vertical sync to occur
			// This ensures that the following swap happens during the vertical blanking interval,
			// preventing screen tearing.
			VPUWaitVSync(s_platform->vx);

			// 3) Swap the framebuffers
			// This makes the currently drawn buffer visible.
			// We can now draw the next frame onto the other buffer.
			// Unlike the VPU rendering, the buffer swap can't be missed, however we might
			// have slightly longer and slightly off-vsync frames this way
			// in case the CPU is late drawing a frame.
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
			// 1) Ensure VPU fifo is empty
			// This means there are no pending commands on the VPU
			// i.e. we have already processed the vsync and the
			// barrier after it.
			while(VPUGetFIFONotEmpty(s_platform->vx)) { }

			// 2) Draw the game frame here
			// usleep(16667);

			// 3) Add a buffer swap commmand
			// This will be processed by the VPU asynchronously when the video beam reaches the vertical blanking interval (vblank).
			// NOTE: This always has to happen after the last write to the video buffer to avoid rendering artifacts.
			// However, it can happen earlier than other CPU side end-of-frame operations.
			VPUSyncSwap(s_platform->vx, 0);

			// 4) Insert a no-operation command (barrier)
			// Any command after the vsync command is not executed until the vsync has occurred.
			// Therefore waiting for the FIFO to be empty after this ensures the vsync has completed.
			// The wait is placed at the start of the next frame's rendering cycle (see the while loop above).
			VPUNoop(s_platform->vx);

			// Important TODO:
			// It is important that we call VPUSwapPages(s_platform->vx, s_platform->sc) here,
			// after the barrier placed by VPUNoop(s_platform->vx) above to ensure proper synchronization.
		} while(1);
	}
	else
		printusage();

	return 0;
}
