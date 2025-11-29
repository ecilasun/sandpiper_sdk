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
// This is a 128-byte program (32 wwords)
static uint32_t s_vcpprogram[] = {
// start:
	vcp_ldim(0x01, 0x000000),		// scrolloffset = 0
	vcp_ldim(0x02, 0x0000FF),		// colorincrement = 255
	vcp_ldim(0x03, 640),			// endofline = 640
	vcp_ldim(0x04, 0x000020),		// loop: = 32
	vcp_ldim(0x05, 0x00001C),		// reset: = 28
	vcp_ldim(0x0C, 0x00004C),		// idle: = 76
	vcp_ldim(0x08, 0x000001),		// scrollspeed = 1
// reset:
	vcp_radd(0x01, 0x01, 0x08),		// scrolloffset += scrollspeed
// loop:
	vcp_wpix(0x03),					// wait for endofline
	vcp_scanline_read(0x06),		// scanline = $videoscanline
	vcp_ldim(0x09, 0x000080),		// temp = 128
	vcp_cmp(4, 0x07, 0x06, 0x09),	// scanline == 128 ?
	vcp_branch(0x0C, 0x07),			// branch.eq idle:
	vcp_ldim(0x09, 0x000002),		// temp = 2
	vcp_rshl(0x06, 0x06, 0x09),		// scanline = scanline << temp
	vcp_radd(0x06, 0x06, 0x01),		// scanline = scanline + scrolloffset
	vcp_pwrt(0x00, 0x06),			// PAL[0] = scanline
	vcp_radd(0x01, 0x01, 0x02),		// color = color + colorincrement
	vcp_jump(0x04),					// jmp loop:
// idle:
	vcp_pwrt(0x00, 0x00),			// PAL[0] = 0
	vcp_wpix(0x03),					// wait for endofline
	vcp_scanline_read(0x06),		// scanline = $videoscanline
	vcp_cmp(4, 0x07, 0x06, 0x00),	// scanline == 0 ?
	vcp_branch(0x05, 0x07),			// branch.eq reset:
	vcp_jump(0x0C),					// jmp idle:
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),
	vcp_noop(),						// Padding to 128 bytes (TODO: upload code should handle this in the future)
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
