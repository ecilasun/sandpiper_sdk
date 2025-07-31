#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <pty.h>
#include <unistd.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_640_Wide
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_HEIGHT    480

static struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;
static struct SPPlatform s_platform;

static int32_t masterfd = 0;
static char buf[1024];
static int32_t buflen = 0;
static fd_set fdset;

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
	SPFreeBuffer(&s_platform, &frameBufferB);
	SPFreeBuffer(&s_platform, &frameBufferA);

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
}

void sigint_handler(int /*s*/)
{
	shutdowncleanup();
	exit(0);
}

int32_t utf8decode(const char *s, uint32_t *out_cp) {
	unsigned char c = s[0];
	if (c < 0x80) {
		*out_cp = c;
		return 1;
	} else if ((c>>5) == 0x6) {
		*out_cp=((c & 0x1F) << 6)|(s[1]& 0x3F);
		return 2;
	} else if ((c >> 4) == 0xE) {
		*out_cp=((c &0x0F) << 12)|((s[1]&0x3F) << 6)|(s[2]&0x3F);
		return 3;
	} else if ((c>>3) == 0x1E) {
		*out_cp=((c &0x07) << 18)|((s[1]&0x3F) << 12)|((s[2]&0x3F) << 6)|(s[3]&0x3F);
		return 4;
	}
	return -1; // invalid UTF-8
}

size_t readfrompty()
{
	int32_t nbytes = read(masterfd, buf + buflen, sizeof(buf)-buflen);
	buflen += nbytes;

	int32_t iter = 0;
	while(iter < buflen)
	{
		uint32_t codepoint = 0;
		int32_t len = utf8decode(&buf[iter], &codepoint);
		if (len == -1 || len > buflen)
			break;
		VPUConsolePrint(&s_vctx, (char*)&codepoint, 1);
		iter += len;
	}

	if (iter < buflen)
	{
		memmove(buf, buf + iter, buflen - iter);
	}

	buflen -= iter;

	return nbytes;
}

int main(int /*argc*/, char** /*argv*/)
{
	SPInitPlatform(&s_platform);

	VPUInitVideo(&s_vctx, &s_platform);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);

	frameBufferB.size = frameBufferA.size = stride*VIDEO_HEIGHT;

	SPAllocateBuffer(&s_platform, &frameBufferA);
	SPAllocateBuffer(&s_platform, &frameBufferB);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);

	// Write random pattern into both buffers
	uint32_t* memA = (uint32_t*)frameBufferA.cpuAddress;
	uint32_t* memB = (uint32_t*)frameBufferB.cpuAddress;
	for (uint32_t i=0; i<stride*VIDEO_HEIGHT/4; i++)
	{
		memA[i] = (i/stride) ^ (i%64);
		memB[i] = (i/stride) ^ (i%64);
	}

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBufferA.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBufferB.dmaAddress);
	VPUSetDefaultPalette(&s_vctx);
	VPUSetVideoMode(&s_vctx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	VPUConsoleSetColors(&s_vctx, CONSOLEDEFAULTFG, CONSOLEDEFAULTBG);
	VPUConsoleClear(&s_vctx);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;

	s_vctx.m_caretX = s_vctx.m_cursorX;
	s_vctx.m_caretY = s_vctx.m_cursorY;
	s_vctx.m_caretBlink = 0;
	s_vctx.m_caretType = 0;

	if (forkpty(&masterfd, NULL, NULL, NULL) == 0)
	{
		// Child process
		execlp("/bin/bash", "bash", NULL);
		perror("execlp");
		exit(1);
	}

	int needUpdate = 0;

	do
	{
		FD_ZERO(&fdset);
		FD_SET(masterfd, &fdset);
		select(masterfd+1, &fdset, NULL, NULL, NULL);

		if (FD_ISSET(masterfd, &fdset))
		{
			// Request update if number of bytes read is nonzero
			needUpdate = readfrompty();
		}

		if (s_sctx.cycle % 15 == 0) // When we wait for vysync this makes a quarter second interval
		{
			s_vctx.m_caretBlink ^= 1;
			needUpdate = 1;
		}

		if (needUpdate)
			VPUConsoleResolve(&s_vctx);

		VPUWaitVSync(&s_vctx);
		VPUSwapPages(&s_vctx, &s_sctx);
	} while(1);

	return 0;
}
