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

#include "platform.h"
#include "vpu.h"
#include "apu.h"

struct SPPlatform s_platform;
struct EAudioContext s_actx;
struct EVideoContext s_vctx;
struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBufferA;
struct SPSizeAlloc frameBufferB;

void shutdowncleanup()
{
	// Switch to fbcon buffer
	VPUSetScanoutAddress(&s_vctx, 0x18000000);
	VPUSetVideoMode(&s_vctx, EVM_640_Wide, ECM_16bit_RGB, EVS_Enable);

	// Yield physical memory and reset video routines
	VPUShutdownVideo();
	APUShutdownAudio(&s_actx);

	// Shutdown platform
	SPShutdownPlatform(&s_platform);
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

	SPInitPlatform(&s_platform);
	VPUInitVideo(&s_vctx, &s_platform);
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	frameBufferB.size = frameBufferA.size = stride*SCREENHEIGHT;
	SPAllocateBuffer(&s_platform, &frameBufferA);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);
	signal(SIGSEGV, &sigint_handler);

	VPUSetVideoMode(&s_vctx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBufferA;
	s_sctx.framebufferB = &frameBufferB;
	VPUSwapPages(&s_vctx, &s_sctx);
	VPUClear(&s_vctx, 0x00000000);

	APUInitAudio(&s_actx, &s_platform);
}

void
I_ShutdownGraphics(void)
{
	// shutdowncleanup(); already handled by sigterm/atexit
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
	memcpy(s_sctx.writepage, screens[0], SCREENWIDTH*SCREENHEIGHT);
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
	memcpy(scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}
