#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "i_video.h"
#include "v_video.h"

#include "core.h"
#include "platform.h"
#include "video.h"

#define VIDEO_MODE      EVM_320_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    240

static struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;
static struct SPPlatform s_platform;

void shutdowncleanup()
{
	// Turn off video scan-out
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Disable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Release allocations
	SPFreeBuffer(&s_platform, &frameBufferB);
	SPFreeBuffer(&s_platform, &frameBufferA);

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

void I_InitGraphics (void)
{
	usegamma = 1;

	SPInitPlatform(&s_platform);

	VPUInitVideo(&s_vctx, &s_platform);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;

	SPAllocateBuffer(&s_platform, &frameBufferA);
	SPAllocateBuffer(&s_platform, &frameBufferB);

	memset((uint8_t*)frameBufferA.cpuAddress, 0, stride*VIDEO_HEIGHT);
	memset((uint8_t*)frameBufferB.cpuAddress, 0, stride*VIDEO_HEIGHT);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	// DEBUG
//	printf("mode:320x240x8bpp stride: %d\n", stride);
//	printf("A: 0x%08X 0x%08X\n", (uint32_t)frameBufferA.cpuAddress, (uint32_t)frameBufferA.dmaAddress);
//	printf("B: 0x%08X 0x%08X\n", (uint32_t)frameBufferB.cpuAddress, (uint32_t)frameBufferB.dmaAddress);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;

	VPUSwapPages(&s_vctx, &s_sctx);
}


void I_ShutdownGraphics(void)
{
}

void I_StartFrame (void)
{

}

// Takes full 8 bit values.
void I_SetPalette (byte* palette)
{
	byte r, g, b;
	// set the X colormap entries
	for (int i=0 ; i<256 ; i++)
	{
		r = gammatable[usegamma][*palette++];
		g = gammatable[usegamma][*palette++];
		b = gammatable[usegamma][*palette++];
		VPUSetPal(&s_vctx, i, r, g, b);
	}
}

void I_UpdateNoBlit (void)
{

}

void I_WaitVBL(int count)
{
	VPUWaitVSync(&s_vctx);
}

void I_FinishUpdate (void)
{
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);

	uint8_t *vramBase = (uint8_t*)s_vctx.m_cpuWriteAddressCacheAligned;
	uint8_t *scr = screens[0];
	for (uint32_t y=0; y<SCREENHEIGHT; y++)
	{
		memcpy(vramBase, scr, SCREENWIDTH);
		vramBase += stride;
		scr += SCREENWIDTH;
	}

	VPUSwapPages(&s_vctx, &s_sctx);
}

void I_ReadScreen (byte* scr)
{
    memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}
