#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "../SDK/core.h"
#include "../SDK/video.h"

#define VIDEO_MODE      EVM_320_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    240
static struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
struct EVPUSizeAlloc frameBufferA;
struct EVPUSizeAlloc frameBufferB;

void vpucleanup()
{
	// Turn off video scan-out
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Disable);

	// Release our allocations
	VPUFreeBuffer(&frameBufferB);
	VPUFreeBuffer(&frameBufferA);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();
}

void sigint_handler(int s)
{
	vpucleanup();
	exit(0);
}

int main(int argc, char** argv)
{
	printf("Hello, video\n");

	VPUInitVideo();
	VPUSetDefaultPalette();

	printf("started video system\n");

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);

	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;
	VPUAllocateBuffer(&frameBufferA);
	VPUAllocateBuffer(&frameBufferB);

	atexit(vpucleanup);
	signal(SIGINT, &sigint_handler);

	printf("framebufferA: 0x%08X <- 0x%08X\n", frameBufferA.cpuAddress, frameBufferA.vpuAddress);
	printf("framebufferB: 0x%08X <- 0x%08X\n", frameBufferB.cpuAddress, frameBufferB.vpuAddress);

	// Write random pattern into both buffers
	/*for (uint32_t i=0; i<stride*VIDEO_HEIGHT/4; i++)
	{
		frameBufferA[i] = (i/320) ^ (i%64);
		frameBufferB[i] = (i/320) ^ (i%64);
	}
	DCACHE_FLUSH();
	printf("buffers set to random value\n");*/

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBufferA.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBufferB.vpuAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	printf("video on, set up read(VPU) and write(CPU) addresses\n");

	VPUConsoleSetColors(&s_vctx, CONSOLEDEFAULTFG, CONSOLEDEFAULTBG);
	VPUConsoleClear(&s_vctx);
	VPUConsolePrint(&s_vctx, "ready\n", 16); // Including the new line

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;

	s_vctx.m_caretX = s_vctx.m_cursorX;
	s_vctx.m_caretY = s_vctx.m_cursorY;
	s_vctx.m_caretBlink = 0;
	s_vctx.m_caretType = 0;

	printf("looping a short while...\n");

	int count = 0;
	do
	{
		VPUConsoleResolve(&s_vctx);
		// VPUWaitVSync(); - This and other reads from VPU cause a hardware freeze
		VPUSwapPages(&s_vctx, &s_sctx);
		count++;
	} while(count < 512);

	return 0;
}
