/**
 * \file vpu_layers_demo.c
 * \brief VPU layer B + mix mode demo
 *
 * Demonstrates dual-layer scanout, mix modes, and keycolor transparency.
 * Layer A is a static gradient background; layer B is a moving sprite.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

#define VIDEO_MODE     EVM_320_240
#define VIDEO_COLOR    ECM_16bit_RGB
#define VIDEO_WIDTH    320
#define VIDEO_HEIGHT   240

#define SPRITE_W 40
#define SPRITE_H 40

#define RGB565_CONST(_r, _g, _b) (uint16_t)((((_r) & 0xF8) << 8) | (((_g) & 0xFC) << 3) | (((_b) & 0xF8) >> 3))
#define KEY_COLOR_565 0xF81F
#define BODY_COLOR RGB565_CONST(180, 120, 60)
#define WING_COLOR RGB565_CONST(120, 80, 40)
#define BEAK_COLOR RGB565_CONST(250, 200, 40)
#define EYE_COLOR RGB565_CONST(20, 20, 20)

static struct SPPlatform* s_platform = NULL;
static struct SPSizeAlloc frameBufferA;
static struct SPSizeAlloc frameBufferB0;
static struct SPSizeAlloc frameBufferB1;

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
	return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

static const uint16_t s_sandpiper_sprite[SPRITE_W * SPRITE_H] = {
#define K KEY_COLOR_565
#define B BODY_COLOR
#define W WING_COLOR
#define Q BEAK_COLOR
#define E EYE_COLOR
	// 0-13
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,W,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,
	K,K,K,K,K,K,K,K,K,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,
	K,K,K,K,K,K,K,K,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,
	K,K,K,K,K,K,K,K,K,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,
	K,K,K,K,K,K,K,K,K,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,B,K,K,K,K,K,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,K,K,K,K,K,K,
	K,K,K,K,B,B,B,B,B,B,B,K,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,B,K,K,K,K,K,
	K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,B,B,B,B,K,K,
	K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,B,B,B,B,B,K,K,
	K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,W,W,W,W,W,W,W,W,W,W,W,W,W,W,K,K,B,B,B,B,K,K,
	K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,B,B,B,K,K,
	K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,B,B,K,K,
	K,K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,B,B,B,B,B,B,B,B,B,B,B,B,B,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,B,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,Q,Q,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,Q,Q,Q,Q,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
#undef K
#undef B
#undef W
#undef Q
#undef E
};

static void fill16(uint8_t* base, uint32_t strideBytes, uint32_t width, uint32_t height, uint16_t color)
{
	for (uint32_t y = 0; y < height; ++y)
	{
		uint16_t* row = (uint16_t*)(base + y * strideBytes);
		for (uint32_t x = 0; x < width; ++x)
			row[x] = color;
	}
}

static void blit_sprite16(uint8_t* base, uint32_t strideBytes, uint32_t width, uint32_t height,
	uint32_t x0, uint32_t y0, const uint16_t* sprite, uint16_t keyColor)
{
	if (x0 >= width || y0 >= height)
		return;

	uint32_t w = SPRITE_W;
	uint32_t h = SPRITE_H;
	if (x0 + w > width)
		w = width - x0;
	if (y0 + h > height)
		h = height - y0;

	for (uint32_t y = 0; y < h; ++y)
	{
		uint16_t* row = (uint16_t*)(base + (y0 + y) * strideBytes);
		const uint16_t* src = sprite + y * SPRITE_W;
		for (uint32_t x = 0; x < w; ++x)
		{
			uint16_t c = src[x];
			if (c != keyColor)
				row[x0 + x] = c;
		}
	}
}

static void draw_background(uint8_t* base, uint32_t strideBytes)
{
	for (uint32_t y = 0; y < VIDEO_HEIGHT; ++y)
	{
		uint16_t* row = (uint16_t*)(base + y * strideBytes);
		for (uint32_t x = 0; x < VIDEO_WIDTH; ++x)
		{
			uint8_t r = (uint8_t)((x * 255) / (VIDEO_WIDTH - 1));
			uint8_t g = (uint8_t)((y * 255) / (VIDEO_HEIGHT - 1));
			uint8_t b = 40;
			row[x] = rgb565(r, g, b);
		}
	}
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	s_platform = SPInitPlatform();
	if (!s_platform)
	{
		printf("Failed to initialize platform.\n");
		return 1;
	}

	VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

	uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
	frameBufferA.size = stride * VIDEO_HEIGHT;
	frameBufferB0.size = stride * VIDEO_HEIGHT;
	frameBufferB1.size = stride * VIDEO_HEIGHT;

	SPAllocateBuffer(s_platform, &frameBufferA);
	SPAllocateBuffer(s_platform, &frameBufferB0);
	SPAllocateBuffer(s_platform, &frameBufferB1);

	draw_background(frameBufferA.cpuAddress, stride);

	uint16_t keyColor = KEY_COLOR_565;
	VPUSetScanoutAddress(s_platform->vx, (uint32_t)frameBufferA.dmaAddress);
	VPUSetScanoutAddressB(s_platform->vx, (uint32_t)frameBufferB0.dmaAddress);
	VPUSetScanoutAddress2B(s_platform->vx, (uint32_t)frameBufferB1.dmaAddress);
	VPUSetMixMode(s_platform->vx, 1, 1, keyColor);

	int frame = 0;
	int cycle = 0;
	int lastMixMode = -1;

	printf("vpu_layers_demo: running (CTRL+C to exit)\n");

	while (1)
	{
		while (VPUGetFIFONotEmpty(s_platform->vx)) { }

		int mixMode = (frame / 240) % 5;
		if (mixMode != lastMixMode)
		{
			VPUSetMixMode(s_platform->vx, 1, (uint8_t)mixMode, keyColor);
			lastMixMode = mixMode;
		}

		struct SPSizeAlloc* back = (cycle % 2 == 0) ? &frameBufferB1 : &frameBufferB0;
		uint8_t* layerB = back->cpuAddress;

		if (mixMode == 1)
			fill16(layerB, stride, VIDEO_WIDTH, VIDEO_HEIGHT, keyColor);
		else
			fill16(layerB, stride, VIDEO_WIDTH, VIDEO_HEIGHT, rgb565(0, 80, 160));

		uint32_t x = (uint32_t)((frame * 2) % (VIDEO_WIDTH - SPRITE_W));
		uint32_t y = (uint32_t)((frame) % (VIDEO_HEIGHT - SPRITE_H));
		blit_sprite16(layerB, stride, VIDEO_WIDTH, VIDEO_HEIGHT, x, y, s_sandpiper_sprite, keyColor);

		VPUSyncSwapB(s_platform->vx, 0);
		VPUNoop(s_platform->vx);

		++frame;
		++cycle;
		usleep(16000);
	}

	return 0;
}
