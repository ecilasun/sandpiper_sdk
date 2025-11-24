/*
 * i_system.c
 *
 * System support code
 *
 * Copyright (C) 1993-1996 by id Software, Inc.
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


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <linux/input.h>

#include "doomdef.h"
#include "doomstat.h"

#include "d_main.h"
#include "g_game.h"
#include "m_misc.h"
#include "i_sound.h"
#include "i_video.h"

#include "i_system.h"

#include "core.h"

static struct pollfd fds[1];
static int s_nokeyboard = 0;
static int s_inited = 0;
static struct termios orig_termios;

static void restore_terminal(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void
I_Init(void)
{
	I_InitSound();
}


byte *
I_ZoneBase(int *size)
{
	/* Give 16M to DOOM */
	*size = 16 * 1024 * 1024;
	return (byte *) malloc (*size);
}

// returns time in 1/TICRATEth second tics
int
I_GetTime (void)
{
	uint64_t cur_time = (uint64_t) clock();
	static int secbase;

	if (!secbase) {
		secbase = cur_time / 1000000;
		return (uint64_t) secbase;
	}

	return ((cur_time/1000)*TICRATE)/1000;
	//return (ClockToMs(E32ReadTime())*TICRATE)/1000;
}

static void
I_GetRemoteEvent(void)
{
	if (!s_inited)
	{
		s_nokeyboard = 0;

		fds[0].fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
		fds[0].events = POLLIN;

		if (fds[0].fd < 0)
		{
			perror("/dev/input/event0: make sure a keyboard is connected");
			s_nokeyboard = 1;
		}

		struct termios raw_termios;
		tcgetattr(STDIN_FILENO, &orig_termios); // Save current settings
		raw_termios = orig_termios;
		raw_termios.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
		tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);
		atexit(restore_terminal);

		s_inited = 1;
	}

	if (!s_nokeyboard)
	{
		int ret = poll(fds, 1, 10);
		if (ret > 0)
		{
			struct input_event ev;
			int n = read(fds[0].fd, &ev, sizeof(struct input_event));
			if (n > 0 && ev.type == EV_KEY)
			{
				event_t event;
				switch(ev.code)
				{
					case KEY_ENTER:			{ event.data1 = DKEY_ENTER; break; }
					case KEY_RIGHT:			{ event.data1 = DKEY_RIGHTARROW; break; }
					case KEY_LEFT:			{ event.data1 = DKEY_LEFTARROW; break; }
					case KEY_DOWN:			{ event.data1 = DKEY_DOWNARROW; break; }
					case KEY_UP:			{ event.data1 = DKEY_UPARROW; break; }
					case KEY_MINUS: 		{ event.data1 = DKEY_MINUS; break; }
					case KEY_EQUAL:			{ event.data1 = DKEY_EQUALS; break; }
					case KEY_ESC:			{ event.data1 = DKEY_ESCAPE; break; }
					case KEY_TAB:			{ event.data1 = DKEY_TAB; break; }
					case KEY_BACKSPACE:		{ event.data1 = DKEY_BACKSPACE; break; }
					case KEY_RIGHTSHIFT:	{ event.data1 = DKEY_RSHIFT; break; }
					case KEY_RIGHTCTRL:		{ event.data1 = DKEY_RCTRL; break; }
					case KEY_RIGHTALT:		{ event.data1 = DKEY_RALT; break; }
					case KEY_LEFTALT:		{ event.data1 = DKEY_LALT; break; }
					case KEY_PAUSE:			{ event.data1 = DKEY_PAUSE; break; }
					case KEY_F1:			{ event.data1 = DKEY_F1; break; }
					case KEY_F2:			{ event.data1 = DKEY_F2; break; }
					case KEY_F3:			{ event.data1 = DKEY_F3; break; }
					case KEY_F4:			{ event.data1 = DKEY_F4; break; }
					case KEY_F5:			{ event.data1 = DKEY_F5; break; }
					case KEY_F6:			{ event.data1 = DKEY_F6; break; }
					case KEY_F7:			{ event.data1 = DKEY_F7; break; }
					case KEY_F8:			{ event.data1 = DKEY_F8; break; }
					case KEY_F9:			{ event.data1 = DKEY_F9; break; }
					case KEY_F10:			{ event.data1 = DKEY_F10; break; }
					case KEY_F11:			{ event.data1 = DKEY_F11; break; }
					case KEY_F12:			{ event.data1 = DKEY_F12; break; }
					default:				{ event.data1 = ev.code; break; }
				}
				event.type = ev.value == 0 ? ev_keyup : ev_keydown;
				D_PostEvent(&event);
			}
		}
	}
}

void
I_StartFrame(void)
{
	/* Nothing to do */
}

void
I_StartTic(void)
{
	I_GetRemoteEvent();
}

ticcmd_t *
I_BaseTiccmd(void)
{
	static ticcmd_t emptycmd;
	return &emptycmd;
}


void
I_Quit(void)
{
	D_QuitNetGame();
	I_ShutdownSound();
	M_SaveDefaults();
	I_ShutdownGraphics();
	exit(0); // NOTE: The environment we're going to return to has been destroyed
}


byte *
I_AllocLow(int length)
{
	byte*	mem;
	mem = (byte *)malloc (length);
	memset (mem,0,length);
	return mem;
}


void
I_Tactile
( int on,
  int off,
  int total )
{
	// UNUSED.
	on = off = total = 0;
}


void
I_Error(char *error, ...)
{
	va_list	argptr;

	// Message first.
	va_start (argptr,error);
	printf ("Error: ");
	vprintf (error, argptr);
	printf ("\n");
	va_end (argptr);

	fflush( stdout );

	// Shutdown. Here might be other errors.
	if (demorecording)
		G_CheckDemoStatus();

	D_QuitNetGame ();
	I_ShutdownGraphics();

	exit(-1);
}
