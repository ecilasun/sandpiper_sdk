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

// Small program to change palette color 0
// at start of every scanline by waiting for pixel 0
// The program clocks at approximately one instruction per pixel
static uint32_t s_vcpprogram[] = {
	vcp_setacc(0xFFFFFF),			// White color in ACC (Accumulator Register is R0)
	vcp_copyreg(0x01, 0x00),		// Copy ACC to R1
	vcp_setacc(0x000001),			// Increment value in ACC
	vcp_copyreg(0x02, 0x00),		// Copy ACC to R2
	vcp_setacc(0x000000),			// Zero ACC
// loop:
	vcp_waitcolumn(0x00),			// Wait for pixel 0 of the current scanline
	vcp_setpal(0x00, 0x01),			// Set PAL[0] to R1
	vcp_add(0x01, 0x02),			// Increment R1 by R2
	vcp_compare(0x00, 0x00, COND_EQ),	// Set branch condition to 'R0==R0' in ACC (i.e. COND_ALWAYS)
	vcp_branch(0x14),			// Unconditional branch to byte 20 (start of loop) based on ACC
	vcp_halt(),				// Good practice to pad with a halt instruction
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
	VCPUploadProgram(s_platform, s_vcpprogram, sizeof(s_vcpprogram));

	// Read back and dump the program
	//printf("Reading back VCP program\n");
	//for (uint32_t i = 0; i < sizeof(s_vcpprogram) / sizeof(uint32_t); i++)
	//	printf("%8x:%8x\n", i, vcpread32(s_platform, i * 4));

	// Start the VCP program
	printf("Starting VCP program\n");
	VPUWriteControlRegister(s_platform->vx, 0x0F, 0x0F);

	printf("Entering demo...\n");
	uint32_t color = 0;
	do
	{
		// Vsync barrier
		// Wait for previous frame to finish and swap buffers
		while(VPUGetFIFONotEmpty(s_platform->vx)) { }
		VPUSwapPages(s_platform->vx, s_platform->sc);

		// VPU program demo goes here
		VPUClear(s_platform->vx, color);
		//color++;

		// Queue vsync
		// This will be processed by the VPU asynchronously when the video beam reaches the vertical blanking interval (vblank).
		// It ensures that the buffer swap happens at the correct time to prevent screen tearing.
		VPUSyncSwap(s_platform->vx, 0);
		VPUNoop(s_platform->vx);
	} while(1);

	printf("Done\n");
	return 0;
}
