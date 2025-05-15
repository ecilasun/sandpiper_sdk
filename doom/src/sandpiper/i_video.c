/*
 * i_video.c
 *
 * Video system support code
 *
 * Copyright (C) 2021 Sylvain Munaut
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "../doomdef.h"

#include "../i_system.h"
#include "../v_video.h"
#include "../i_video.h"

#include "core.h"
#include "video.h"
#include "platform.h"

struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;

struct EVideoContext s_vctx;
static struct EVideoSwapContext s_sctx;
static struct SPPlatform platform;

void shutdowncleanup()
{
	// Turn off video scan-out
	VPUSetVideoMode(&s_vctx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Disable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();

	// Release allocations
	SPFreeBuffer(&platform, &frameBufferB);
	SPFreeBuffer(&platform, &frameBufferA);

	// Shutdown platform
	SPShutdownPlatform(&platform);
}

void sigint_handler(int s)
{
	shutdowncleanup();
	exit(0);
}

void
I_InitGraphics(void)
{
	usegamma = 1;

	SPInitPlatform(&platform);

	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	frameBufferB.size = frameBufferA.size = stride*240;
	SPAllocateBuffer(&platform, &frameBufferA);
	SPAllocateBuffer(&platform, &frameBufferB);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);

	VPUSetWriteAddress(&s_vctx, (uint32_t)frameBufferA.cpuAddress);
	VPUSetScanoutAddress(&s_vctx, (uint32_t)frameBufferA.dmaAddress);

	VPUSetVideoMode(&s_vctx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;
}

void
I_ShutdownGraphics(void)
{
	VPUSetVideoMode(&s_vctx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Disable);
}

void
I_SetPalette(byte* palette)
{
	// Copy palette to G-RAM
	byte r, g, b;
	for (int i=0 ; i<256 ; i++) {
		r = gammatable[usegamma][*palette++]>>4;
		g = gammatable[usegamma][*palette++]>>4;
		b = gammatable[usegamma][*palette++]>>4;
		VPUSetPal(&s_vctx, i, r, g, b);
	}
}


void
I_UpdateNoBlit(void)
{
	// hmm....
}

void
I_FinishUpdate (void)
{
	// Copy screen to framebuffer
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	uint8_t* fb = (uint8_t*)s_vctx.m_cpuWriteAddressCacheAligned;
	for (int i=0;i<SCREENHEIGHT;i++)
		memcpy(fb+i*stride, screens[0], 320);

	// Write pending data to memory to ensure all writes are visible to scan-out
	//DATACACHE_FLUSH();

	VPUSwapPages(&s_vctx, &s_sctx);
}


void
I_WaitVBL(int count)
{
	// Wait until we exit current frame's vbcounter and enter the next one
	VPUWaitVSync(&s_vctx);
}

void
I_ReadScreen(byte* scr)
{
	// Copy what's on screen
	memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

