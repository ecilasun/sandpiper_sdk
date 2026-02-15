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

#define SPRITE_W 32
#define SPRITE_H 32

#define RGB565_CONST(_r, _g, _b) (uint16_t)((((_r) & 0xF8) << 8) | (((_g) & 0xFC) << 3) | (((_b) & 0xF8) >> 3))
#define KEY_COLOR_565 0xF81F
#define METAL_COLOR RGB565_CONST(180, 180, 200)
#define DARK_METAL RGB565_CONST(80, 80, 100)
#define LED_RED RGB565_CONST(255, 0, 0)
#define LED_BLUE RGB565_CONST(0, 150, 255)

// Font data from SDK (subset of resident font)
extern const uint8_t residentfont[];

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
#define M METAL_COLOR
#define D DARK_METAL
#define R LED_RED
#define B LED_BLUE
	// Robot sprite 32x32
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,M,M,M,M,M,M,M,M,M,M,M,M,M,M,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,K,K,K,K,K,K,K,
	K,K,K,K,K,K,M,M,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,M,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,R,R,R,D,D,D,D,D,D,D,D,R,R,R,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,R,R,R,D,D,D,D,D,D,D,D,R,R,R,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,R,R,R,D,D,D,D,D,D,D,D,R,R,R,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,D,D,D,D,D,M,M,M,M,D,D,D,D,D,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,D,D,D,D,D,D,M,M,M,M,M,M,D,D,D,D,D,D,M,K,K,K,K,K,K,
	K,K,K,K,K,K,M,M,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,M,M,K,K,K,K,K,K,
	K,K,K,K,K,K,K,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,D,D,D,M,M,M,M,M,M,M,M,M,M,M,M,D,D,D,K,K,K,K,K,K,K,
	K,K,K,K,K,K,D,D,D,D,M,M,M,M,M,M,M,M,M,M,M,M,D,D,D,D,K,K,K,K,K,K,
	K,K,K,K,K,K,D,D,D,D,M,M,M,M,M,M,M,M,M,M,M,M,D,D,D,D,K,K,K,K,K,K,
	K,K,K,K,K,K,K,D,D,D,M,M,M,M,M,M,M,M,M,M,M,M,D,D,D,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,M,M,M,M,M,M,M,M,M,M,M,M,M,M,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,M,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,M,M,M,M,M,D,D,D,D,D,D,D,D,D,D,M,M,M,M,M,K,K,K,K,K,K,
	K,K,K,K,K,M,M,M,M,M,D,D,D,D,D,D,D,D,D,D,D,D,M,M,M,M,M,K,K,K,K,K,
	K,K,K,K,K,M,M,M,M,M,D,D,M,M,K,K,K,K,M,M,D,D,M,M,M,M,M,K,K,K,K,K,
	K,K,K,K,K,M,M,M,M,M,D,D,M,M,K,K,K,K,M,M,D,D,M,M,M,M,M,K,K,K,K,K,
	K,K,K,K,K,M,M,M,M,M,D,D,M,M,K,K,K,K,M,M,D,D,M,M,M,M,M,K,K,K,K,K,
	K,K,K,K,K,K,M,M,M,D,D,D,M,M,K,K,K,K,M,M,D,D,D,M,M,M,K,K,K,K,K,K,
	K,K,K,K,K,K,K,M,D,D,D,D,M,M,M,M,M,M,M,M,D,D,D,D,M,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,D,D,D,D,D,M,M,M,M,M,M,D,D,D,D,D,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,D,D,D,D,D,D,D,D,D,D,D,D,D,D,K,K,K,K,K,K,K,K,K,
	K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,K,
#undef K
#undef M
#undef D
#undef R
#undef B
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

static void print_string_rgb565(uint8_t* base, uint32_t strideBytes, uint32_t width, uint32_t height,
	uint16_t x, uint16_t y, const char* text, uint16_t fgColor, uint16_t bgColor)
{
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
	{
		int ch = text[i];
		if (ch < 32) continue;
		
		int charRow = (ch >> 4) * 8;
		int charCol = ch % 16;
		
		for (int cy = 0; cy < 8; ++cy)
		{
			uint32_t yPos = y + cy;
			if (yPos >= height) break;
			
			uint16_t* row = (uint16_t*)(base + yPos * strideBytes);
			uint8_t charData = residentfont[charCol + ((charRow + cy) * 16)];
			
			for (int cx = 0; cx < 8; ++cx)
			{
				uint32_t xPos = x + i * 8 + cx;
				if (xPos >= width) break;
				
				int bitPos = (cx < 4) ? (3 - cx) : (11 - cx);
				uint8_t bit = (charData >> bitPos) & 1;
				row[xPos] = bit ? fgColor : bgColor;
			}
		}
	}
}

static void draw_background(uint8_t* base, uint32_t strideBytes)
{
	// Sky, mountains, and grass background
	for (uint32_t y = 0; y < VIDEO_HEIGHT; ++y)
	{
		uint16_t* row = (uint16_t*)(base + y * strideBytes);
		
		// Sky gradient (top 60% of screen)
		if (y < VIDEO_HEIGHT * 60 / 100)
		{
			uint8_t skyB = 200 + (55 * y / (VIDEO_HEIGHT * 60 / 100));
			uint16_t skyColor = rgb565(100, 150, skyB);
			for (uint32_t x = 0; x < VIDEO_WIDTH; ++x)
				row[x] = skyColor;
		}
		// Grass (bottom 40% of screen)
		else
		{
			uint8_t grassG = 120 + (30 * (y - VIDEO_HEIGHT * 60 / 100)) / (VIDEO_HEIGHT * 40 / 100);
			uint16_t grassColor = rgb565(50, grassG, 40);
			for (uint32_t x = 0; x < VIDEO_WIDTH; ++x)
				row[x] = grassColor;
		}
	}
	
	// Draw mountains
	uint16_t mountainColor = rgb565(80, 80, 90);
	uint16_t mountainDark = rgb565(60, 60, 70);
	
	// Left mountain
	uint32_t leftPeakY = VIDEO_HEIGHT * 35 / 100;
	uint32_t horizonY = VIDEO_HEIGHT * 60 / 100;
	for (uint32_t y = leftPeakY; y < horizonY; ++y)
	{
		uint16_t* row = (uint16_t*)(base + y * strideBytes);
		uint32_t distFromPeak = y - leftPeakY;
		uint32_t mountainHeight = horizonY - leftPeakY;
		uint32_t centerX = 60;
		uint32_t halfWidth = (distFromPeak * 50) / mountainHeight;
		
		uint32_t leftEdge = centerX - halfWidth;
		uint32_t rightEdge = centerX + halfWidth;
		
		for (uint32_t x = leftEdge; x < rightEdge && x < VIDEO_WIDTH; ++x)
		{
			uint16_t color = (x < centerX) ? mountainDark : mountainColor;
			row[x] = color;
		}
	}
	
	// Right mountain
	uint32_t rightPeakY = VIDEO_HEIGHT * 40 / 100;
	for (uint32_t y = rightPeakY; y < horizonY; ++y)
	{
		uint16_t* row = (uint16_t*)(base + y * strideBytes);
		uint32_t distFromPeak = y - rightPeakY;
		uint32_t mountainHeight = horizonY - rightPeakY;
		uint32_t centerX = 220;
		uint32_t halfWidth = (distFromPeak * 60) / mountainHeight;
		
		uint32_t leftEdge = centerX - halfWidth;
		uint32_t rightEdge = centerX + halfWidth;
		
		for (uint32_t x = leftEdge; x < rightEdge && x < VIDEO_WIDTH; ++x)
		{
			uint16_t color = (x < centerX) ? mountainDark : mountainColor;
			row[x] = color;
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

	// Print "Background layer" on layer A
	print_string_rgb565(frameBufferA.cpuAddress, stride, VIDEO_WIDTH, VIDEO_HEIGHT,
		8, 8, "Background layer", rgb565(255, 255, 255), rgb565(0, 0, 0));

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

		// Print "Foreground layer" at bottom left
		print_string_rgb565(layerB, stride, VIDEO_WIDTH, VIDEO_HEIGHT,
			8, VIDEO_HEIGHT - 16, "Foreground layer", rgb565(255, 255, 0), keyColor);

		// Print "Blend Mode: X" at bottom right
		char modeText[20];
		snprintf(modeText, sizeof(modeText), "Blend Mode: %d", mixMode);
		int modeTextLen = strlen(modeText);
		print_string_rgb565(layerB, stride, VIDEO_WIDTH, VIDEO_HEIGHT,
			VIDEO_WIDTH - (modeTextLen * 8) - 8, VIDEO_HEIGHT - 16, modeText, rgb565(255, 255, 0), keyColor);

		VPUSyncSwapB(s_platform->vx, 0);
		VPUNoop(s_platform->vx);

		++frame;
		++cycle;
		usleep(16000);
	}

	return 0;
}
