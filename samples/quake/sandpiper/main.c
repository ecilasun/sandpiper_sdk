/*
 * Copyright (C) 2022 National Cheng Kung University, Taiwan.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <quakembd.h>
#include <quakedef.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <linux/input.h>

#include "core.h"
#include "platform.h"

static struct termios orig_termios;

static void restore_terminal(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

// Map Linux input key codes to printable ASCII (lowercase, no modifiers).
// Returns 0 when the key is non-printable.
static int map_ascii_key(__u16 code)
{
	// Linux input keycodes for letters follow QWERTY keyboard layout, not alphabetical order
	switch (code)
	{
		// Letter keys - must map individually due to QWERTY layout
		case KEY_A: return 'a';
		case KEY_B: return 'b';
		case KEY_C: return 'c';
		case KEY_D: return 'd';
		case KEY_E: return 'e';
		case KEY_F: return 'f';
		case KEY_G: return 'g';
		case KEY_H: return 'h';
		case KEY_I: return 'i';
		case KEY_J: return 'j';
		case KEY_K: return 'k';
		case KEY_L: return 'l';
		case KEY_M: return 'm';
		case KEY_N: return 'n';
		case KEY_O: return 'o';
		case KEY_P: return 'p';
		case KEY_Q: return 'q';
		case KEY_R: return 'r';
		case KEY_S: return 's';
		case KEY_T: return 't';
		case KEY_U: return 'u';
		case KEY_V: return 'v';
		case KEY_W: return 'w';
		case KEY_X: return 'x';
		case KEY_Y: return 'y';
		case KEY_Z: return 'z';
		
		// Number keys
		case KEY_1: return '1';
		case KEY_2: return '2';
		case KEY_3: return '3';
		case KEY_4: return '4';
		case KEY_5: return '5';
		case KEY_6: return '6';
		case KEY_7: return '7';
		case KEY_8: return '8';
		case KEY_9: return '9';
		case KEY_0: return '0';
		
		// Special characters
		case KEY_SPACE: return ' ';
		case KEY_MINUS: return '-';
		case KEY_EQUAL: return '=';
		case KEY_LEFTBRACE: return '[';
		case KEY_RIGHTBRACE: return ']';
		case KEY_BACKSLASH: return '\\';
		case KEY_SEMICOLON: return ';';
		case KEY_APOSTROPHE: return '\'';
		case KEY_GRAVE: return '`';
		case KEY_COMMA: return ',';
		case KEY_DOT: return '.';
		case KEY_SLASH: return '/';
		
		// Keypad
		case KEY_KP0: return '0';
		case KEY_KP1: return '1';
		case KEY_KP2: return '2';
		case KEY_KP3: return '3';
		case KEY_KP4: return '4';
		case KEY_KP5: return '5';
		case KEY_KP6: return '6';
		case KEY_KP7: return '7';
		case KEY_KP8: return '8';
		case KEY_KP9: return '9';
		case KEY_KPDOT: return '.';
		
		default: return 0;
	}
}

enum {
	MOUSE_BUTTON_LEFT = 1,
	MOUSE_BUTTON_MIDDLE = 2,
	MOUSE_BUTTON_RIGHT = 3,
};

enum {
	KEY_EVENT = 0,
	MOUSE_MOTION_EVENT = 1,
	MOUSE_BUTTON_EVENT = 2,
	QUIT_EVENT = 3,
};

typedef struct {
	uint8_t button;
	uint8_t state;
} mouse_button_t;

typedef struct {
	uint32_t type;
	union {
		key_event_t key_event;
		union {
			mouse_motion_t motion;
			mouse_button_t button;
		} mouse;
	};
} event_t;

/*typedef struct {
	event_t *base;
	size_t start;
} event_queue_t;*/

enum {
	RELATIVE_MODE_SUBMISSION = 0,
	WINDOW_TITLE_SUBMISSION = 1,
};

typedef struct {
	uint8_t enabled;
} mouse_submission_t;

typedef struct {
	uint32_t title;
	uint32_t size;
} title_submission_t;

/*typedef struct {
	uint32_t type;
	union {
	   mouse_submission_t mouse;
	   title_submission_t title;
	};
} submission_t;*/

/*typedef struct {
	submission_t *base;
	size_t end;
} submission_queue_t;*/

/*static const int queues_capacity = 128;
static unsigned int event_count;
static event_queue_t event_queue = {
	.base = NULL,
	.start = 0,
};*/
/*static submission_queue_t submission_queue = {
	.base = NULL,
	.end = 0,
};*/
//static event_t event;
static mouse_movement_t mouse_movement;
static int nokeyboard = 0;
static int nomouse = 0;
static struct pollfd fds[2];
static int inited = 0;

uint64_t qembd_get_time()
{
	struct timeval tp;
	struct timezone tzp;
	static int secbase = 0;

	gettimeofday(&tp, &tzp);

	if (secbase == 0)
	{
		secbase = tp.tv_sec;
		return (uint64_t)tp.tv_usec;
	}

	return ((tp.tv_sec - secbase) / 1000000) + tp.tv_usec;
}

/*void qembd_udelay(uint32_t us)
{
	uint64_t start = qembd_get_us_time(), end;
	end = start;
	while (end - start < us)
		end = qembd_get_us_time();
}*/

/*void qembd_set_relative_mode(bool enabled) {
	submission_t submission;
	submission.type = RELATIVE_MODE_SUBMISSION;
	submission.mouse.enabled = enabled;
	submission_queue.base[submission_queue.end++] = submission;
	submission_queue.end &= queues_capacity - 1;
	register int a0 asm("a0") = 1;
	register int a7 asm("a7") = 0xfeed;
	asm volatile("scall" : "+r"(a0) : "r"(a7));
}*/

int main(int c, char **v)
{
	return qembd_main(c, v);
}

void *qembd_allocmain(size_t size)
{
	return malloc(size);
}

static int poll_event()
{
	/*if (event_count <= 0)
		return 0;
	event = event_queue.base[event_queue.start++];
	event_queue.start &= queues_capacity - 1;
	--event_count;

	if (event.type == MOUSE_MOTION_EVENT) {
		mouse_movement.x += event.mouse.motion.xrel;
		mouse_movement.y += event.mouse.motion.yrel;
	}*/

	return 0;
}

int qembd_dequeue_key_event(key_event_t *e)
{
	if (!inited)
	{
		nokeyboard = 0;
		nomouse = 0;

		fds[0].fd = SPFindKeyboardDevice();
		fds[0].events = POLLIN;

		if (fds[0].fd < 0)
		{
			printf("Could not find keyboard device. Make sure a keyboard is connected.\n");
			nokeyboard = 1;
		}

		fds[1].fd = SPFindMouseDevice();
		fds[1].events = POLLIN;
		if (fds[1].fd < 0)
		{
			printf("Could not find mouse device.\n");
			nomouse = 1;
		}
		else
		{
			int flags = fcntl(fds[1].fd, F_GETFL, 0);
			if (flags != -1) fcntl(fds[1].fd, F_SETFL, flags | O_NONBLOCK);
		}

		struct termios raw_termios;
		tcgetattr(STDIN_FILENO, &orig_termios); // Save current settings
		raw_termios = orig_termios;
		raw_termios.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
		tcsetattr(STDIN_FILENO, TCSANOW, &raw_termios);
		atexit(restore_terminal);

		inited = 1;
	}

	int ret = poll(fds, 2, 10);
	if (ret <= 0) return -1;

	if (!nomouse && (fds[1].revents & POLLIN))
	{
		struct input_event ev;
		while (read(fds[1].fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event))
		{
			if (ev.type == EV_REL)
			{
				if (ev.code == REL_X) mouse_movement.x += ev.value;
				if (ev.code == REL_Y) mouse_movement.y += ev.value;
			}
			else if (ev.type == EV_KEY)
			{
				int mask = 0;
				if (ev.code == BTN_LEFT) mask = 1;
				else if (ev.code == BTN_RIGHT) mask = 2;
				else if (ev.code == BTN_MIDDLE) mask = 4;

				if (mask) {
					if (ev.value) mouse_movement.buttons |= mask;
					else mouse_movement.buttons &= ~mask;
				}
			}
		}
	}

	if (!nokeyboard && (fds[0].revents & POLLIN))
	{
		struct input_event ev;
		int n = read(fds[0].fd, &ev, sizeof(struct input_event));
		if (n > 0 && ev.type == EV_KEY)
		{
			// We have our scancode and key state here
			switch(ev.code)
			{
				case KEY_ENTER:		{ e->keycode = K_ENTER; break; }
				case KEY_RIGHT:		{ e->keycode = K_RIGHTARROW; break; }
				case KEY_LEFT:		{ e->keycode = K_LEFTARROW; break; }
				case KEY_DOWN:		{ e->keycode = K_DOWNARROW; break; }
				case KEY_UP:		{ e->keycode = K_UPARROW; break; }
				case KEY_ESC:		{ e->keycode = K_ESCAPE; break; }
				case KEY_TAB:		{ e->keycode = K_TAB; break; }
				case KEY_BACKSPACE:	{ e->keycode = K_BACKSPACE; break; }
				case KEY_LEFTSHIFT:	{ e->keycode = K_SHIFT; break; }
				case KEY_LEFTCTRL:	{ e->keycode = K_CTRL; break; }
				case KEY_RIGHTALT:	{ e->keycode = K_ALT; break; }
				case KEY_LEFTALT:	{ e->keycode = K_ALT; break; }
				case KEY_PAUSE:		{ e->keycode = K_PAUSE; break; }
				case KEY_F1:		{ e->keycode = K_F1; break; }
				case KEY_F2:		{ e->keycode = K_F2; break; }
				case KEY_F3:		{ e->keycode = K_F3; break; }
				case KEY_F4:		{ e->keycode = K_F4; break; }
				case KEY_F5:		{ e->keycode = K_F5; break; }
				case KEY_F6:		{ e->keycode = K_F6; break; }
				case KEY_F7:		{ e->keycode = K_F7; break; }
				case KEY_F8:		{ e->keycode = K_F8; break; }
				case KEY_F9:		{ e->keycode = K_F9; break; }
				case KEY_F10:		{ e->keycode = K_F10; break; }
				case KEY_F11:		{ e->keycode = K_F11; break; }
				case KEY_F12:		{ e->keycode = K_F12; break; }
				case KEY_HOME:		{ e->keycode = K_HOME; break; }
				case KEY_END:		{ e->keycode = K_END; break; }
				case KEY_PAGEUP:	{ e->keycode = K_PGUP; break; }
				case KEY_PAGEDOWN:	{ e->keycode = K_PGDN; break; }
				case KEY_INSERT:	{ e->keycode = K_INS; break; }
				case KEY_DELETE:	{ e->keycode = K_DEL; break; }
				default:			{
					int ascii = map_ascii_key(ev.code);
					e->keycode = ascii ? ascii : ev.code;
					break;
				}
			}
			e->state = ev.value == 0 ? 0 : 1; // 1: key down, 0: key up, 2: autorepeat
			return 0;
		}
	}

	return -1;
}

int qembd_get_mouse_movement(mouse_movement_t *movement)
{
	*movement = mouse_movement;
	mouse_movement.x = 0;
	mouse_movement.y = 0;
	return 0;
}
