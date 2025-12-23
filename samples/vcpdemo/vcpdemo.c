/**
 * \file vcpdemo.c
 * \brief VCP demo program
 * 
 * \ingroup examples
 * 
 * This example demonstrates how to use the VCP (Video Co-Processor) to run a small program
 * that modifies the palette colors at the start of each scanline.
 * It also demonstrates how to handle control flow in a VCP program.
 * 
 * It sets up a palletted video mode, allocates two frame buffers, uploads a VCP program,
 * and runs a loop that updates the frame buffers with a simple pattern while the VCP
 * program modifies the palette colors. The even and odd lines are filled with different colors
 * to create a striped effect.
 * Both colors contain palette index 0 which is the one modified by the VCP program.
 * 
 * The VCP program changes the color at palette index 0x00 based on the current scanline,
 * creating a scrolling color effect. It stops at scanline 128, sets the update color to black
 * and idles until the next frame.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"
#include "vcp.h"

#define VIDEO_MODE      EVM_320_240
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
// This is a 128-byte program (32 wwords)
static const uint32_t s_vcpprogram[32] = {
// start:
	vcp_ldim(VREG_1, 0x000000),						// scrolloffset = 0
	vcp_ldim(VREG_2, 0x0000FF),						// mask = 0xFF
	vcp_ldim(VREG_3, 640),							// endofline = 640
	vcp_ldim(VREG_8, 0x000002),						// scrollspeed = 2
	vcp_ldim(VREG_9, 0x000003),						// shift3 = 3
	vcp_ldim(VREG_4, 0x000006),						// shift6 = 6
	vcp_ldim(VREG_5, 0x000008),						// shift8 = 8
	vcp_ldim(VREG_C, 0x000010),						// shift16 = 16
	vcp_ldim(VREG_B, 0x000080),						// stopline = 128
// reset:
	vcp_radd(VREG_1, VREG_1, VREG_8),				// scrolloffset += scrollspeed
// loop:
	vcp_wpix(VREG_3),								// wait for endofline
	vcp_scanline_read(VREG_6),						// scanline = $videoscanline
	vcp_cmp(COND_EQ, VREG_6, VREG_B),				// scanline == 128 ?
	vcp_branchim(0x34),								// branch.eq idle: +0x34 bytes from this instruction
	vcp_radd(VREG_7, VREG_6, VREG_1),				// t = scanline + scrolloffset
	vcp_rand(VREG_A, VREG_7, VREG_2),				// b = (t >> 0) & 0xFF
	vcp_rshl(VREG_D, VREG_7, VREG_9),				// g = (t << 3) & 0xFF
	vcp_rand(VREG_D, VREG_D, VREG_2),
	vcp_rshl(VREG_D, VREG_D, VREG_5),				// g <<= 8
	vcp_ror(VREG_A, VREG_A, VREG_D),				// color |= g
	vcp_rshl(VREG_D, VREG_7, VREG_4),				// r = (t << 6) & 0xFF
	vcp_rand(VREG_D, VREG_D, VREG_2),
	vcp_rshl(VREG_D, VREG_D, VREG_C),				// r <<= 16
	vcp_ror(VREG_A, VREG_A, VREG_D),				// color |= r
	vcp_pwrt(VREG_ZERO, VREG_A),					// PAL[0] = color
	vcp_jumpim(-0x3C),								// jmp loop: -0x3C bytes from this instruction
// idle:
	vcp_pwrt(VREG_ZERO, VREG_ZERO),					// PAL[0] = 0
	vcp_wpix(VREG_3),								// wait for endofline
	vcp_scanline_read(VREG_6),						// scanline = $videoscanline
	vcp_cmp(COND_EQ, VREG_6, VREG_ZERO),			// scanline == 0 ?
	vcp_branchim(-0x54),							// branch.eq reset: -0x54 bytes from this instruction
	vcp_jumpim(-0x14),								// jmp idle: -0x14 bytes from this instruction
};

int main(int argc, char** argv)
{
	s_platform = SPInitPlatform();

	// Set up the video output mode
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	// Allocate our two frame buffers
	printf("Allocating framebuffers\n");
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB);

	printf("Setting up VPU assisted swap address\n");
	// For hardware assisted vsync, we need to set this up once at start
	VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBufferA.dmaAddress);
	VPUSetScanoutAddress2(s_platform->vx, (uint32_t)frameBufferB.dmaAddress);
	// This one is for the CPU side so it can keep up with the hardware flips
	s_platform->sc->cycle = 0;
	s_platform->sc->framebufferA = &frameBufferA;
	s_platform->sc->framebufferB = &frameBufferB;

	uint32_t stat;

	// Stop all running programs by clearing all control registers
	printf("Stopping existing programs...");
	VPUWriteControlRegister(s_platform->vx, 0x0F, 0x00);
	stat = VCPStatus(s_platform);
	decodeStatus(stat);

	printf("Uploading new VCP program...");
	VCPUploadProgram(s_platform, s_vcpprogram, PRG_128Bytes);
	stat = VCPStatus(s_platform);
	decodeStatus(stat);

	// Start the VCP program
	printf("Starting VCP program...");
	VCPExecProgram(s_platform, 0x1); // b0001
	stat = VCPStatus(s_platform);
	decodeStatus(stat);

	printf("Starting demo...\n");

	// VCP program updates color at palette index 0x00
	uint32_t colorEven = 0x0000FFFF; // Even/odd pixels, scrolling at 60Hz
	uint32_t colorOdd = 0x00000000; // Solid line

	uint32_t frame = 0;
	do
	{
		// Vsync barrier
		// Wait for previous frame (if any) to consume swap command + barrier, then swap buffers
		while(VPUGetFIFONotEmpty(s_platform->vx)) { }
		VPUSwapPages(s_platform->vx, s_platform->sc);

		// VPU program demo goes here
		{
			++frame;
			if ((frame%10) == 0)
				colorEven = ((colorEven&0xFF)<<24) | ((colorEven>>8)&0x00FFFFFF);
			uint32_t H = s_platform->vx->m_graphicsHeight;
			uint32_t W = s_platform->vx->m_strideInWords;
			// Read this every frame since it flips between buffers
			uint32_t *vramBase = (uint32_t*)s_platform->vx->m_cpuWriteAddressCacheAligned;
			for (uint32_t y=0; y<H; ++y)
			{
				uint32_t row = y*W;
				for (uint32_t x=0; x<W; ++x)
					vramBase[row+x] = y&1 ? colorOdd : colorEven;
			}
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
