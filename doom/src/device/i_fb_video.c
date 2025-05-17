#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "i_video.h"
#include "v_video.h"

#include "../SDK/core.h"
#include "../SDK/platform.h"
#include "../SDK/video.h"

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
	SPInitPlatform(&s_platform);

	VPUInitVideo(&s_vctx, &s_platform);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;

	SPAllocateBuffer(&s_platform, &frameBufferA);
	SPAllocateBuffer(&s_platform, &frameBufferB);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBufferA.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBufferB.dmaAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;
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
    byte c;
    // set the X colormap entries
    for (int i=0 ; i<256 ; i++)
		VPUSetPal(&s_vctx, i, palette[0], palette[1], palette[2]);
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

	uint8_t* outmem = s_sctx.writepage;
	for (uint32_t y=0; y<VIDEO_HEIGHT; y++)
	{
		memcpy(outmem, screens[0]+y*SCREENWIDTH, SCREENWIDTH);
		outmem += stride;
	}
}

void I_ReadScreen (byte* scr)
{
    memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}