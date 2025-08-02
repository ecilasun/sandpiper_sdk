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

struct SPPlatform s_platform;
struct EVideoContext s_vctx;
struct EAudioContext s_actx;

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

	SPInitPlatform(&s_platform);

	atexit(shutdowncleanup);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigint_handler);
	signal(SIGSEGV, &sigint_handler);

	D_DoomMain();
	return 0;
}
