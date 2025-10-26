/*
	@file vsyncdemo.c
	@brief Demonstrates how to use the Video Processing Unit (VPU).
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"
#include "vcp.h"

#define VIDEO_MODE      EVM_320_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    240

static struct SPPlatform* s_platform = NULL;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;

// Some VCP info:
// The VCP clocks at 166MHz, with an instruction retirement rate of 1 instruction every 3 clocks on average.
// Minimum VCP program size is 128 bytes, maximum size is 4KBytes
// Programs must be aligned to 64 byte boundaries in memory.
// There are 16 general purpose registers (R0-R15), R0 is always zero and cannot be modified.
// There is a 256 byte palette RAM, each entry is 32 bits in ARGB format, VCP has direct access to this RAM.

// The video hardware always scans out at 60Hz and has a scan window of 800x525 pixels, out of which 640x480 are visible.
// Given we run at approximately 55.5MIPS, we can afford to run a program that executes around 925925 instructions per frame.
// This is plenty for simple effects like changing palette entries on scanline boundaries, simple raster effects, etc.

// Tiny program to change palette color 0
// at start of every scanline by waiting for pixel 0
static uint32_t s_vcpprogram[] = {
	vcp_ldim(0x01, 0xFFFFFFFF),		// Load white color into R1
	vcp_ldim(0x02, 0x000005),		// Load 5 into R2 (increment)
	vcp_ldim(0x03, 0x00000C),		// Load 12 into R3 (offset of loop:)
// loop:
	vcp_wscn(0x00, 0x00),			// Wait for first pixel of first scanline
	vcp_pwrt(0x00, 0x01),			// Set PAL[0] to R1
	vcp_pwrt(0x01, 0x01),			// Set PAL[1] to R1
	vcp_radd(0x01, 0x01, 0x02),		// R1 = R1 + R2(5)
	vcp_jump(0x3),					// Unconditional branch to R3 (12, i.e. loop:)
	vcp_noop(),						// Fill the rest with NOOPs
	vcp_noop(),						// (Noops can also be used for timing adjustments, before branches etc)
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
};

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();

	// Set up the video output mode
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	// Allocate our two frame buffers
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB);

	// Fill both buffers with different patterns so we
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

	printf("Setting up VPU assisted swap address\n");
	// For hardware assisted vsync, we need to set this up once at start
	VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBufferA.dmaAddress);
	VPUSetScanoutAddress2(s_platform->vx, (uint32_t)frameBufferB.dmaAddress);
	// This one is for the CPU side so it can keep up with the hardware flips
	s_platform->sc->cycle = 0;
	s_platform->sc->framebufferA = &frameBufferA;
	s_platform->sc->framebufferB = &frameBufferB;

	// Stop all running programs by clearing all control registers
	printf("Stopping VCP programs\n");
	VPUWriteControlRegister(s_platform->vx, 0x0F, 0x00);

	printf("Uploading VCP program\n");
	VCPUploadProgram(s_platform, &s_vcpprogram, PRG_128Bytes);

	// Start the VCP program
	printf("Starting VCP program\n");
	VPUWriteControlRegister(s_platform->vx, 0x0F, 0x0F);

	printf("Ensuring VCP program started\n");
	printf("status: %d\n", VPUReadControlRegister(s_platform->vx));

	printf("Entering demo...\n");
	uint32_t color = 0x00040201; // RED(x04) entryshould change by the program
	do
	{
		// Vsync barrier
		// Wait for previous frame to finish and swap buffers
		while(VPUGetFIFONotEmpty(s_platform->vx)) { }
		VPUSwapPages(s_platform->vx, s_platform->sc);

		// VPU program demo goes here
		VPUClear(s_platform->vx, color);
		color = (color<<8) | ((color&0xFF000000)>>24); // roll

		// Queue vsync
		// This will be processed by the VPU asynchronously when the video beam reaches the vertical blanking interval (vblank).
		// It ensures that the buffer swap happens at the correct time to prevent screen tearing.
		VPUSyncSwap(s_platform->vx, 0);
		VPUNoop(s_platform->vx);
	} while(1);

	printf("Done\n");
	return 0;
}
