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
#include <signal.h>

#include "platform.h"
#include "vpu.h"
#include "apu.h"

// Platform context
struct SPPlatform s_platform;

// Device contexts
struct EVideoContext s_vctx;
struct EAudioContext s_actx;
struct EVideoSwapContext s_sctx;

// Video and audio buffers
struct SPSizeAlloc frameBuffer;
struct SPSizeAlloc mixbufferA;
struct SPSizeAlloc mixbufferB;

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

int main(int argc, char *argv[])
{
    myargc = argc;
    myargv = argv;

	// Initialize platform and subsystems
	SPInitPlatform(&s_platform);
	APUInitAudio(&s_actx, &s_platform);
	mixbufferB.size = mixbufferA.size = 512*2*2;
	SPAllocateBuffer(&s_platform, &mixbufferA);
	SPAllocateBuffer(&s_platform, &mixbufferB);
	APUSetBufferSize(&s_actx, ABS_2048Bytes); // Number of 16 bit stereo samples
	APUSetSampleRate(&s_actx, ASR_11_025_Hz);

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

	// Setup exit handlers
	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);
	signal(SIGSEGV, &sigint_handler);

	// Enter Doom main loop
	D_DoomMain();
	return 0;
}
