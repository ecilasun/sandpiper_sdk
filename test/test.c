#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "../SDK/core.h"
#include "../SDK/platform.h"
#include "../SDK/video.h"

#define VIDEO_MODE      EVM_640_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    480

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

int main(int argc, char** argv)
{
	printf("Hello, video\n");

	SPInitPlatform(&s_platform);

	printf("started platform\n");

	VPUInitVideo(&s_vctx, &s_platform);

	printf("started video system\n");

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	printf("stride: %d, height: %d\n", stride, VIDEO_HEIGHT);

	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;

	SPAllocateBuffer(&s_platform, &frameBufferA);
	printf("acrs:%d framebufferA: 0x%08X <- 0x%08X - %dbytes\n", s_platform.alloc_cursor, frameBufferA.cpuAddress, frameBufferA.dmaAddress, frameBufferA.size);

	SPAllocateBuffer(&s_platform, &frameBufferB);
	printf("acrs:%d framebufferB: 0x%08X <- 0x%08X - %dbytes\n", s_platform.alloc_cursor, frameBufferB.cpuAddress, frameBufferB.dmaAddress, frameBufferB.size);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);

	// Write random pattern into both buffers
	uint32_t* memA = (uint32_t*)frameBufferA.cpuAddress;
	uint32_t* memB = (uint32_t*)frameBufferB.cpuAddress;
	for (uint32_t i=0; i<stride*VIDEO_HEIGHT/4; i++)
	{
		memA[i] = (i/640) ^ (i%64);
		memB[i] = (i/640) ^ (i%64);
	}
	DCACHE_FLUSH();

	printf("buffers set to random values\n");

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBufferA.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBufferB.dmaAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	printf("video on, set up read(VPU) and write(CPU) addresses\n");

	VPUConsoleSetColors(&s_vctx, CONSOLEDEFAULTFG, CONSOLEDEFAULTBG);
	VPUConsoleClear(&s_vctx);
	VPUConsolePrint(&s_vctx, "sandpiper ready\n", VPU_AUTO);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;

	s_vctx.m_caretX = s_vctx.m_cursorX;
	s_vctx.m_caretY = s_vctx.m_cursorY;
	s_vctx.m_caretBlink = 0;
	s_vctx.m_caretType = 0;

	printf("looping a short while...\n");

	do
	{
		VPUConsoleResolve(&s_vctx);

		if (s_sctx.cycle % 30 == 0)
			s_vctx.m_caretBlink ^= 1;

		VPUWaitVSync(&s_vctx); // This and other reads from VPU cause a hardware freeze, figure out why
		VPUSwapPages(&s_vctx, &s_sctx);
	} while(s_sctx.cycle < 4096);

	return 0;
}
