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

static const char* states[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

void decodeStatus(uint32_t stat)
{
// FEDC BA98 7654 3210 FEDC BA98 7654 3210
// ---- OOOO -CFP PPPP PPPP PPPP RRRR EEEE
//assign vcpstate = {4'd0, debugopcode, 1'b0, copystate, ~vcpfifoempty, debug_pc, runstate, execstate};

	uint32_t execstate = stat & 0xF;
	uint32_t runstate = (stat >> 4) & 0xF;
	uint32_t pc = (stat >> 8) & 0x1FFF;
	uint32_t fifoempty = (stat >> 21) & 0x1;
	uint32_t copystate = (stat >> 22) & 0x1;
	uint32_t debugopcode = (stat >> 24) & 0xF;

	printf("PC:0x%X ~FIFO:%d copy:%d run:%s exec:%s opcode:0x%X\n", pc, fifoempty, copystate, states[runstate], states[execstate], debugopcode);
}

// Tiny program to change some palette colors at pixel zero of each scanline
static uint32_t s_vcpprogram[] = {
	vcp_ldim(0x02, 0x040404),	// Load 1 into R2 (increment)
	vcp_ldim(0x03, 640),		// Load 640 into R3 (end of line)
	vcp_ldim(0x04, 0x000010),	// Load 16 into R4 (offset of loop:)
	vcp_ldim(0x01, 0x55AADD),	// Load a color into R1
// loop:
	vcp_wpix(0x03),			// Wait for pixel R3 (R3==640) i.e. end of scanline
	vcp_pwrt(0x00, 0x01),		// Set PAL[R0] to R1 (R0==0)
	vcp_radd(0x01, 0x01, 0x02),	// R1 = R1 + R2(1)
	vcp_jump(0x04),			// Unconditional branch to R4 (16, i.e. loop:)
	vcp_noop(),			// Fill the rest with NOOPs
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
		for (int x = 0; x < stride/4; x++)
		{
			uint32_t* pixelA = (uint32_t*)frameBufferA.cpuAddress + (y * stride/4) + x;
			*pixelA = 0x0000FFFF;

			uint32_t* pixelB = (uint32_t*)frameBufferB.cpuAddress + (y * stride/4) + x;
			*pixelB = 0xFFFF0000;
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

	// Dump program bytecode
	printf("VCP program:\n");
	for (int i=0;i<16;++i)
		printf("0x%.4X: 0x%.8X\n", i, s_vcpprogram[i]);

	uint32_t stat;

	// Stop all running programs by clearing all control registers
	printf("Stopping programs...");
	VPUWriteControlRegister(s_platform->vx, 0x0F, 0x00);
	stat = VCPStatus(s_platform);
	decodeStatus(stat);

	printf("Uploading VCP program...");
	VCPUploadProgram(s_platform, s_vcpprogram, PRG_128Bytes);
	stat = VCPStatus(s_platform);
	decodeStatus(stat);

	// Start the VCP program
	printf("Starting VCP program...");
	VCPExecProgram(s_platform, 0x1); // b0001
	stat = VCPStatus(s_platform);
	decodeStatus(stat);

	printf("Entering demo...\n");
	uint32_t color = 0x03020000; // VCP program updates some of these colors
	do
	{
		// Vsync barrier
		// Wait for previous frame to finish and swap buffers
		while(VPUGetFIFONotEmpty(s_platform->vx)) { }
		VPUSwapPages(s_platform->vx, s_platform->sc);

		// VPU program demo goes here
		//VPUClear(s_platform->vx, color);
		//color = (color<<8) | ((color&0xFF000000)>>24); // roll colors right

		// Queue vsync
		// This will be processed by the VPU asynchronously when the video beam reaches the vertical blanking interval (vblank).
		// It ensures that the buffer swap happens at the correct time to prevent screen tearing.
		VPUSyncSwap(s_platform->vx, 0);
		VPUNoop(s_platform->vx);
	} while(1);

	printf("Done\n");
	return 0;
}
