/**
 * \file mouse.c
 * \brief Mouse input sample
 *
 * Shows a software cursor and prints mouse position/button state
 * using SPFindMouseDevice().
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_320_240
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_WIDTH     320
#define VIDEO_HEIGHT    240

static struct SPPlatform* s_platform = NULL;
static struct SPSizeAlloc frameBuffer;

static inline int clampi(int v, int lo, int hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

static inline void set_pixel(uint8_t* fb, uint32_t stride, int x, int y, uint8_t color)
{
	if (x < 0 || y < 0 || x >= VIDEO_WIDTH || y >= VIDEO_HEIGHT)
		return;
	fb[y * stride + x] = color;
}

static void draw_cursor(uint8_t* fb, uint32_t stride, int x, int y, uint8_t color)
{
	for (int i = -6; i <= 6; ++i)
	{
		set_pixel(fb, stride, x + i, y, color);
		set_pixel(fb, stride, x, y + i, color);
	}
	// small center box
	for (int dy = -1; dy <= 1; ++dy)
	{
		for (int dx = -1; dx <= 1; ++dx)
			set_pixel(fb, stride, x + dx, y + dy, color);
	}
}

static int map_abs_to_screen(int value, const struct input_absinfo* abs, int max_screen)
{
	int range = abs->maximum - abs->minimum;
	if (range <= 0)
		return clampi(value, 0, max_screen);
	int v = value - abs->minimum;
	if (v < 0) v = 0;
	if (v > range) v = range;
	return (v * max_screen) / range;
}

int main(int argc, char** argv)
{
	(void)argc; (void)argv;

	s_platform = SPInitPlatform();
	if (!s_platform)
	{
		fprintf(stderr, "Failed to init platform\n");
		return -1;
	}

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBuffer.size = stride * VIDEO_HEIGHT;
	SPAllocateBuffer(s_platform, &frameBuffer);

	VPUSetWriteAddress(s_platform->vx, (uint32_t)frameBuffer.cpuAddress);
	VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBuffer.dmaAddress);
	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);
	VPUSetDefaultPalette(s_platform->vx);

	// Bright yellow cursor color at palette index 250
	VPUSetPal(s_platform->vx, 250, 255, 255, 0);

	int mouse_fd = SPFindMouseDevice();
	if (mouse_fd < 0)
	{
		printf("Could not find mouse device. Make sure a mouse is connected.\n");
		return 0;
	}

	char dev_name[256] = "Unknown";
	ioctl(mouse_fd, EVIOCGNAME(sizeof(dev_name)), dev_name);

	struct input_absinfo abs_x;
	struct input_absinfo abs_y;
	bool has_abs_x = (ioctl(mouse_fd, EVIOCGABS(ABS_X), &abs_x) == 0);
	bool has_abs_y = (ioctl(mouse_fd, EVIOCGABS(ABS_Y), &abs_y) == 0);
	bool has_abs = has_abs_x && has_abs_y;

	int mouse_x = VIDEO_WIDTH / 2;
	int mouse_y = VIDEO_HEIGHT / 2;
	bool btn_left = false;
	bool btn_middle = false;
	bool btn_right = false;

	struct pollfd pfd;
	pfd.fd = mouse_fd;
	pfd.events = POLLIN;

	char line1[128];
	char line2[128];
	char line3[128];

	while (1)
	{
		int ret = poll(&pfd, 1, 0);
		if (ret > 0)
		{
			struct input_event ev;
			while (read(mouse_fd, &ev, sizeof(ev)) == sizeof(ev))
			{
				if (ev.type == EV_REL)
				{
					if (ev.code == REL_X) mouse_x += ev.value;
					if (ev.code == REL_Y) mouse_y += ev.value;
				}
				else if (ev.type == EV_ABS)
				{
					if (has_abs)
					{
						if (ev.code == ABS_X)
							mouse_x = map_abs_to_screen(ev.value, &abs_x, VIDEO_WIDTH - 1);
						if (ev.code == ABS_Y)
							mouse_y = map_abs_to_screen(ev.value, &abs_y, VIDEO_HEIGHT - 1);
					}
				}
				else if (ev.type == EV_KEY)
				{
					if (ev.code == BTN_LEFT) btn_left = (ev.value != 0);
					if (ev.code == BTN_MIDDLE) btn_middle = (ev.value != 0);
					if (ev.code == BTN_RIGHT) btn_right = (ev.value != 0);
				}
			}
		}

		mouse_x = clampi(mouse_x, 0, VIDEO_WIDTH - 1);
		mouse_y = clampi(mouse_y, 0, VIDEO_HEIGHT - 1);

		memset(frameBuffer.cpuAddress, 0, stride * VIDEO_HEIGHT);

		snprintf(line1, sizeof(line1), "Device: %s", dev_name);
		snprintf(line2, sizeof(line2), "Pos: %d, %d  Mode: %s", mouse_x, mouse_y, has_abs ? "abs" : "rel");
		snprintf(line3, sizeof(line3), "Buttons: L=%d M=%d R=%d", btn_left ? 1 : 0, btn_middle ? 1 : 0, btn_right ? 1 : 0);

		VPUPrintString(s_platform->vx, 15, 0, 4, 4, line1, (int)strlen(line1));
		VPUPrintString(s_platform->vx, 15, 0, 4, 14, line2, (int)strlen(line2));
		VPUPrintString(s_platform->vx, 15, 0, 4, 24, line3, (int)strlen(line3));

		draw_cursor((uint8_t*)frameBuffer.cpuAddress, stride, mouse_x, mouse_y, 250);

		VPUWaitVSync(s_platform->vx);
	}

	return 0;
}
