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

#include "../doomdef.h"

#include "../i_system.h"
#include "../v_video.h"
#include "../i_video.h"

#include "platform.h"
#include "vpu.h"
#include "apu.h"

extern struct SPPlatform s_platform;
extern struct EVideoContext s_vctx;

struct EVideoSwapContext s_sctx;
struct SPSizeAlloc frameBuffer;

void
I_InitGraphics(void)
{
	usegamma = 1;

	VPUInitVideo(&s_vctx, &s_platform);
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	frameBuffer.size = stride*SCREENHEIGHT;
	SPAllocateBuffer(&s_platform, &frameBuffer);

	VPUSetVideoMode(&s_vctx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);

	s_sctx.cycle = 0;
	s_sctx.framebufferA = &frameBuffer; // No double buffering
	s_sctx.framebufferB = &frameBuffer;
	VPUSwapPages(&s_vctx, &s_sctx);
	VPUClear(&s_vctx, 0x00000000);
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
		r = gammatable[usegamma][*palette++];
		g = gammatable[usegamma][*palette++];
		b = gammatable[usegamma][*palette++];
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
	if (s_sctx.writepage != 0x0)
	{
		uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
		for (uint32_t i=0;i<SCREENHEIGHT;++i)
		{
			uint32_t targetoffset = stride*i;
			uint32_t sourceoffset = SCREENWIDTH*i;
			memcpy(s_sctx.writepage + targetoffset, screens[0] + sourceoffset, SCREENWIDTH);
		}
	}
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
