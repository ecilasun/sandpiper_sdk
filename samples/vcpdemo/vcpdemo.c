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

static const char* states[] = {
	"INIT",
	"FETCH",
	"WFETCH",
	"DECODE",
	"EXEC",
	"FREAD",
	"FCOMPARE",
	"HALT",
	"UNKNOWN" };

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

	printf("VCP: PC:0x%X FIFOEmpty:%d Copystate:%d Runstate:%s Execstate:%s Opcode:0x%X\n", pc, fifoempty, copystate, states[runstate], states[execstate], debugopcode);
}

// Some VCP info:
// The VCP clocks at 166MHz, with an instruction retirement rate of 1 instruction every 3 clocks on average.
// Minimum VCP program size is 128 bytes, maximum size is 4KBytes
// Programs must be aligned to 64 byte boundaries in memory.
// There are 16 general purpose registers (R0-R15), R0 is always zero and cannot be modified.
// There is a 256 byte palette RAM, each entry is 32 bits in ARGB format, VCP has direct access to this RAM.

// The video hardware always scans out at 60Hz and has a scan window of 800x525 pixels, out of which 640x480 are visible.
// Given we run at approximately 55.5MIPS, we can afford to run a program that executes around 925925 instructions per frame.
// This is plenty for simple effects like changing palette entries on scanline boundaries, simple raster effects, etc.

// Tiny program to change some palette colors at scanline zero
static uint32_t s_vcpprogram[] = {
	vcp_ldim(0x01, 0x550000),	// Load a color into R1
	vcp_ldim(0x02, 0x000005),	// Load 5 into R2 (increment)
	vcp_ldim(0x03, 0x000002),	// Load 2 into R3 (for PAL[2])
	vcp_ldim(0x04, 0x000010),	// Load 16 into R4 (offset of loop:)
// loop:
	vcp_wscn(0x00),			// Wait for scanline zero (R0==0)
	vcp_wpix(0x00),			// Wait for pixel zero (R0==0)
	vcp_pwrt(0x00, 0x01),		// Set PAL[0] to R1 (R0==0)
	vcp_pwrt(0x03, 0x01),		// Set PAL[2] to R1 (R3==2)
	vcp_radd(0x01, 0x01, 0x02),	// R1 = R1 + R2(5)
	vcp_jump(0x04),			// Unconditional branch to R4 (16, i.e. loop:)
	vcp_noop(),			// Fill the rest with NOOPs
	vcp_noop(),			// (Noops can also be used for timing adjustments, before branches etc)
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

	// Dump program
	/*printf("VCP program:\n");
	for (int i=0;i<16;++i)
		printf("0x%.4X: 0x%.8X\n", i, s_vcpprogram[i]);*/

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
	uint32_t color = 0x04030201; // VCP program updates some of these colors
	do
	{
		// Vsync barrier
		// Wait for previous frame to finish and swap buffers
		while(VPUGetFIFONotEmpty(s_platform->vx)) { }
		VPUSwapPages(s_platform->vx, s_platform->sc);

		// VPU program demo goes here
		VPUClear(s_platform->vx, color);
		color = (color<<8) | ((color&0xFF000000)>>24); // roll

		// Show program debug info
		uint32_t* wordA = (uint32_t*)s_platform->sc->writepage;
		wordA += 8;
		for (int i=0;i<128;++i)
		{
			stat = VCPStatus(s_platform);
			*wordA = stat;
			wordA+=stride/4;
		}

		// Queue vsync
		// This will be processed by the VPU asynchronously when the video beam reaches the vertical blanking interval (vblank).
		// It ensures that the buffer swap happens at the correct time to prevent screen tearing.
		VPUSyncSwap(s_platform->vx, 0);
		VPUNoop(s_platform->vx);
	} while(1);

	printf("Done\n");
	return 0;
}
