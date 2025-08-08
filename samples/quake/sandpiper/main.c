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
#include <linux/input.h>

#include "core.h"

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
static struct pollfd fds[1];
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

		fds[0].fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
		fds[0].events = POLLIN;

		if (fds[0].fd < 0)
		{
			perror("/dev/input/event0: make sure a keyboard is connected");
			nokeyboard = 1;
		}
		inited = 1;
	}

	if (!nokeyboard)
	{
		int ret = poll(fds, 1, 10);
		if (ret > 0)
		{
			struct input_event ev;
			int n = read(fds[0].fd, &ev, sizeof(struct input_event));
			if (n > 0 && ev.type == EV_KEY && (ev.value == 0 || ev.value == 1))
			{
				// We have our scancode and key state here
				switch(ev.code)
				{
					//case KEY_RETURN:	{ e->keycode = K_ENTER; break; }
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
					default:			{ e->keycode = ev.code; break; }
				}
				e->state = ev.value;
				return 0;
			}
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
