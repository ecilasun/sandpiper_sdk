/*
 * i_main.c
 *
 * Main entry point
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

// Modified to work on E32E by Engin Cilasun

#include "../doomdef.h"
#include "../m_argv.h"
#include "../d_main.h"
#include <stdlib.h>

#include "platform.h"
#include "vpu.h"
#include "apu.h"

// Platform context
struct SPPlatform* s_platform = NULL;

// Video and audio buffers
struct SPSizeAlloc frameBuffer;
struct SPSizeAlloc mixbufferA;
struct SPSizeAlloc mixbufferB;

int main(int argc, char *argv[])
{
    myargc = argc;
    myargv = argv;

	// Initialize platform and subsystems
	s_platform = SPInitPlatform();
	APUInitAudio(s_platform->ac, s_platform);
	mixbufferB.size = mixbufferA.size = 512*2*2;
	SPAllocateBuffer(s_platform, &mixbufferA);
	SPAllocateBuffer(s_platform, &mixbufferB);
	APUSetBufferSize(s_platform->ac, ABS_2048Bytes); // Number of 16 bit stereo samples
	APUSetSampleRate(s_platform->ac, ASR_11_025_Hz);

	VPUInitVideo(s_platform->vx, s_platform);
	uint32_t stride = VPUGetStride(EVM_320_Wide, ECM_8bit_Indexed);
	frameBuffer.size = stride*SCREENHEIGHT;
	SPAllocateBuffer(s_platform, &frameBuffer);
	VPUSetVideoMode(s_platform->vx, EVM_320_Wide, ECM_8bit_Indexed, EVS_Enable);
	s_platform->sc->cycle = 0;
	s_platform->sc->framebufferA = &frameBuffer; // No double buffering
	s_platform->sc->framebufferB = &frameBuffer;
	VPUSwapPages(s_platform->vx, s_platform->sc);
	VPUClear(s_platform->vx, 0x00000000);

	// Enter Doom main loop
	D_DoomMain();
	return 0;
}
