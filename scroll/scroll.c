#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_640_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    480

static struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBuffer;
static struct SPPlatform s_platform;

void shutdowncleanup()
{
	// Reset shifts
	VPUShiftCache(&s_vctx, 0);
	VPUShiftScanout(&s_vctx, 0);
	VPUShiftPixel(&s_vctx, 0);

	// Switch to fbcon buffer
	VPUSetScanoutAddress(&s_vctx, 0x18000000);
	VPUSetVideoMode(&s_vctx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Release allocations
	SPFreeBuffer(&s_platform, &frameBuffer);

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
}

void sigint_handler(int s)
{
	switch(s)
	{
		case SIGINT:
		case SIGTERM:
		case SIGHUP:
			shutdowncleanup();
			exit(0);
		break;
	}
}

int main(int argc, char** argv)
{
	SPInitPlatform(&s_platform);
	VPUInitVideo(&s_vctx, &s_platform);
	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBuffer.size = stride*VIDEO_HEIGHT;
	SPAllocateBuffer(&s_platform, &frameBuffer);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBuffer.dmaAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	VPUShiftCache(&s_vctx, 0);
	VPUShiftScanout(&s_vctx, 0);
	VPUShiftPixel(&s_vctx, 0);

	int totalscroll = 0;
	int direction = 1;
	int A = 64;

	do
	{
		totalscroll += direction;
		if (totalscroll > A)
			direction = -1;
		else if (totalscroll <= 0) // NOTE: do not scroll in negative direction
			direction = 1;
		int byteoffset = (totalscroll / 8);
		int pixeloffset = totalscroll & 7;

		VPUShiftScanout(&s_vctx, byteoffset);
		VPUShiftPixel(&s_vctx, pixeloffset);

		VPUWaitVSync(&s_vctx);
		//VPUSwapPages(&s_vctx, &s_sctx);
	} while(1);

	return 0;
}
