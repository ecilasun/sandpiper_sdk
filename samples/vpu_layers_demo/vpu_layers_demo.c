/**
 * \file vpu_layers_demo.c
 * \brief VPU layer B + mix mode demo
 *
 * Demonstrates dual-layer scanout, mix modes, and keycolor transparency.
 * Layer A is a static gradient background; layer B is a moving square.
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

static struct SPPlatform* s_platform = NULL;
static struct SPSizeAlloc frameBufferA;
static struct SPSizeAlloc frameBufferB0;
static struct SPSizeAlloc frameBufferB1;

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
	return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

static void fill16(uint8_t* base, uint32_t strideBytes, uint32_t width, uint32_t height, uint16_t color)
{
	for (uint32_t y = 0; y < height; ++y)
	{
		uint16_t* row = (uint16_t*)(base + y * strideBytes);
		for (uint32_t x = 0; x < width; ++x)
			row[x] = color;
	}
}

static void draw_rect16(uint8_t* base, uint32_t strideBytes, uint32_t width, uint32_t height,
	uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint16_t color)
{
	if (x0 >= width || y0 >= height)
		return;
	if (x0 + w > width)
		w = width - x0;
	if (y0 + h > height)
		h = height - y0;

	for (uint32_t y = 0; y < h; ++y)
	{
		uint16_t* row = (uint16_t*)(base + (y0 + y) * strideBytes);
		for (uint32_t x = 0; x < w; ++x)
			row[x0 + x] = color;
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

	uint16_t keyColor = rgb565(255, 0, 255);
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

		uint32_t squareSize = 40;
		uint32_t x = (uint32_t)((frame * 2) % (VIDEO_WIDTH - squareSize));
		uint32_t y = (uint32_t)((frame) % (VIDEO_HEIGHT - squareSize));
		draw_rect16(layerB, stride, VIDEO_WIDTH, VIDEO_HEIGHT, x, y, squareSize, squareSize, rgb565(255, 220, 0));

		VPUSyncSwapB(s_platform->vx, 0);
		VPUNoop(s_platform->vx);

		++frame;
		++cycle;
		usleep(16000);
	}

	return 0;
}
