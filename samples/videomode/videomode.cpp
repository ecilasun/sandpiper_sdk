/**
 * \file videomode.cpp
 * \brief Video mode test pattern demo
 *
 * \ingroup examples
 * This example demonstrates the different video modes available on the Sandpiper platform.
 * It draws a test pattern with a circle, lines, and color bars appropriate for the selected mode.
 *
 * Usage: videomode <mode>
 *   mode 0: 320x240 8-bit indexed
 *   mode 1: 640x480 8-bit indexed
 *   mode 2: 320x480 8-bit indexed
 *   mode 3: 640x240 8-bit indexed
 *   mode 4: 320x240 16-bit RGB
 *   mode 5: 640x480 16-bit RGB
 *   mode 6: 320x480 16-bit RGB
 *   mode 7: 640x240 16-bit RGB
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

struct SPPlatform* g_platform = nullptr;
struct SPSizeAlloc g_framebuffer;

// Video mode configuration
struct VideoModeConfig {
    EVideoMode vmode;
    EColorMode cmode;
    uint32_t width;
    uint32_t height;
    const char* name;
};

static const VideoModeConfig g_modes[] = {
    { EVM_320_240, ECM_8bit_Indexed, 320, 240, "320x240 8-bit Indexed" },
    { EVM_640_480, ECM_8bit_Indexed, 640, 480, "640x480 8-bit Indexed" },
    { EVM_320_480, ECM_8bit_Indexed, 320, 480, "320x480 8-bit Indexed" },
    { EVM_640_240, ECM_8bit_Indexed, 640, 240, "640x240 8-bit Indexed" },
    { EVM_320_240, ECM_16bit_RGB,    320, 240, "320x240 16-bit RGB" },
    { EVM_640_480, ECM_16bit_RGB,    640, 480, "640x480 16-bit RGB" },
    { EVM_320_480, ECM_16bit_RGB,    320, 480, "320x480 16-bit RGB" },
    { EVM_640_240, ECM_16bit_RGB,    640, 240, "640x240 16-bit RGB" },
};
static const int g_numModes = sizeof(g_modes) / sizeof(g_modes[0]);

// Helper to set a pixel in 8-bit indexed mode
inline void setPixel8(uint8_t* fb, uint32_t stride, int x, int y, uint8_t color)
{
    fb[y * stride + x] = color;
}

// Helper to set a pixel in 16-bit RGB mode
inline void setPixel16(uint16_t* fb, uint32_t stride, int x, int y, uint16_t color)
{
    fb[y * (stride >> 1) + x] = color;
}

// Draw a horizontal line
void drawHLine8(uint8_t* fb, uint32_t stride, int x0, int x1, int y, uint8_t color)
{
    if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
    for (int x = x0; x <= x1; ++x)
        setPixel8(fb, stride, x, y, color);
}

void drawHLine16(uint16_t* fb, uint32_t stride, int x0, int x1, int y, uint16_t color)
{
    if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
    for (int x = x0; x <= x1; ++x)
        setPixel16(fb, stride, x, y, color);
}

// Draw a vertical line
void drawVLine8(uint8_t* fb, uint32_t stride, int x, int y0, int y1, uint8_t color)
{
    if (y0 > y1) { int t = y0; y0 = y1; y1 = t; }
    for (int y = y0; y <= y1; ++y)
        setPixel8(fb, stride, x, y, color);
}

void drawVLine16(uint16_t* fb, uint32_t stride, int x, int y0, int y1, uint16_t color)
{
    if (y0 > y1) { int t = y0; y0 = y1; y1 = t; }
    for (int y = y0; y <= y1; ++y)
        setPixel16(fb, stride, x, y, color);
}

// Draw a diagonal line using Bresenham's algorithm
void drawLine8(uint8_t* fb, uint32_t stride, int x0, int y0, int x1, int y1, uint8_t color)
{
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        setPixel8(fb, stride, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawLine16(uint16_t* fb, uint32_t stride, int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        setPixel16(fb, stride, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Draw a circle using midpoint algorithm
void drawCircle8(uint8_t* fb, uint32_t stride, int cx, int cy, int radius, uint8_t color)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        setPixel8(fb, stride, cx + x, cy + y, color);
        setPixel8(fb, stride, cx + y, cy + x, color);
        setPixel8(fb, stride, cx - y, cy + x, color);
        setPixel8(fb, stride, cx - x, cy + y, color);
        setPixel8(fb, stride, cx - x, cy - y, color);
        setPixel8(fb, stride, cx - y, cy - x, color);
        setPixel8(fb, stride, cx + y, cy - x, color);
        setPixel8(fb, stride, cx + x, cy - y, color);

        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void drawCircle16(uint16_t* fb, uint32_t stride, int cx, int cy, int radius, uint16_t color)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        setPixel16(fb, stride, cx + x, cy + y, color);
        setPixel16(fb, stride, cx + y, cy + x, color);
        setPixel16(fb, stride, cx - y, cy + x, color);
        setPixel16(fb, stride, cx - x, cy + y, color);
        setPixel16(fb, stride, cx - x, cy - y, color);
        setPixel16(fb, stride, cx - y, cy - x, color);
        setPixel16(fb, stride, cx + y, cy - x, color);
        setPixel16(fb, stride, cx + x, cy - y, color);

        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

// Draw test pattern for 8-bit indexed mode
void drawTestPattern8(uint8_t* fb, uint32_t stride, uint32_t width, uint32_t height)
{
    // Clear to black
    memset(fb, 0, stride * height);

    // Calculate center and radius for circle
    int cx = width / 2;
    int cy = height / 2 - 20;  // Offset up to leave room for color bars
    int radius = (height < width ? height : width) / 4;

    // Draw circle in white (color 15)
    drawCircle8(fb, stride, cx, cy, radius, 15);

    // Draw horizontal line across center in red (color 12)
    drawHLine8(fb, stride, cx - radius - 20, cx + radius + 20, cy, 12);

    // Draw vertical line through center in green (color 10)
    drawVLine8(fb, stride, cx, cy - radius - 20, cy + radius + 20, 10);

    // Draw diagonal lines in blue (color 9) and yellow (color 14)
    drawLine8(fb, stride, cx - radius, cy - radius, cx + radius, cy + radius, 9);
    drawLine8(fb, stride, cx + radius, cy - radius, cx - radius, cy + radius, 14);

    // Draw color bars at the bottom - show first 16 palette colors
    int barHeight = 20;
    int barY = height - barHeight - 10;
    int barWidth = width / 16;

    for (int i = 0; i < 16; ++i) {
        int x0 = i * barWidth;
        int x1 = (i + 1) * barWidth - 1;
        for (int y = barY; y < barY + barHeight; ++y) {
            drawHLine8(fb, stride, x0, x1, y, (uint8_t)i);
        }
    }

    // Draw a second row with more palette colors (16-31)
    barY -= barHeight + 2;
    for (int i = 0; i < 16; ++i) {
        int x0 = i * barWidth;
        int x1 = (i + 1) * barWidth - 1;
        for (int y = barY; y < barY + barHeight; ++y) {
            drawHLine8(fb, stride, x0, x1, y, (uint8_t)(i + 16));
        }
    }
}

// Draw test pattern for 16-bit RGB mode
void drawTestPattern16(uint16_t* fb, uint32_t stride, uint32_t width, uint32_t height)
{
    // Clear to black
    memset(fb, 0, stride * height);

    // Calculate center and radius for circle
    int cx = width / 2;
    int cy = height / 2 - 20;  // Offset up to leave room for color bars
    int radius = (height < width ? height : width) / 4;

    // Colors in RGB565 format
    uint16_t white   = MAKECOLORRGB16(31, 63, 31);
    uint16_t red     = MAKECOLORRGB16(31, 0, 0);
    uint16_t green   = MAKECOLORRGB16(0, 63, 0);
    uint16_t blue    = MAKECOLORRGB16(0, 0, 31);
    uint16_t yellow  = MAKECOLORRGB16(31, 63, 0);
    uint16_t cyan    = MAKECOLORRGB16(0, 63, 31);
    uint16_t magenta = MAKECOLORRGB16(31, 0, 31);

    // Draw circle in white
    drawCircle16(fb, stride, cx, cy, radius, white);

    // Draw horizontal line across center in red
    drawHLine16(fb, stride, cx - radius - 20, cx + radius + 20, cy, red);

    // Draw vertical line through center in green
    drawVLine16(fb, stride, cx, cy - radius - 20, cy + radius + 20, green);

    // Draw diagonal lines in blue and yellow
    drawLine16(fb, stride, cx - radius, cy - radius, cx + radius, cy + radius, blue);
    drawLine16(fb, stride, cx + radius, cy - radius, cx - radius, cy + radius, yellow);

    // Draw RGB color gradient bars at the bottom
    int barHeight = 20;
    int barY = height - barHeight - 10;

    // Red gradient
    for (int x = 0; x < (int)width; ++x) {
        int r = (x * 31) / width;
        uint16_t color = MAKECOLORRGB16(r, 0, 0);
        for (int y = barY; y < barY + barHeight; ++y) {
            setPixel16(fb, stride, x, y, color);
        }
    }

    // Green gradient
    barY -= barHeight + 2;
    for (int x = 0; x < (int)width; ++x) {
        int g = (x * 63) / width;
        uint16_t color = MAKECOLORRGB16(0, g, 0);
        for (int y = barY; y < barY + barHeight; ++y) {
            setPixel16(fb, stride, x, y, color);
        }
    }

    // Blue gradient
    barY -= barHeight + 2;
    for (int x = 0; x < (int)width; ++x) {
        int b = (x * 31) / width;
        uint16_t color = MAKECOLORRGB16(0, 0, b);
        for (int y = barY; y < barY + barHeight; ++y) {
            setPixel16(fb, stride, x, y, color);
        }
    }

    // White (grayscale) gradient
    barY -= barHeight + 2;
    for (int x = 0; x < (int)width; ++x) {
        int r = (x * 31) / width;
        int g = (x * 63) / width;
        int b = (x * 31) / width;
        uint16_t color = MAKECOLORRGB16(r, g, b);
        for (int y = barY; y < barY + barHeight; ++y) {
            setPixel16(fb, stride, x, y, color);
        }
    }
}

void printUsage(const char* progname)
{
    printf("Usage: %s <mode>\n", progname);
    printf("Available modes:\n");
    for (int i = 0; i < g_numModes; ++i) {
        printf("  %d: %s\n", i, g_modes[i].name);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    int modeIndex = atoi(argv[1]);
    if (modeIndex < 0 || modeIndex >= g_numModes) {
        printf("Invalid mode: %d\n", modeIndex);
        printUsage(argv[0]);
        return 1;
    }

    const VideoModeConfig& config = g_modes[modeIndex];
    printf("Selected mode %d: %s\n", modeIndex, config.name);

    // Initialize platform
    printf("Initializing platform...\n");
    g_platform = SPInitPlatform();
    if (!g_platform) {
        printf("Failed to initialize platform\n");
        return 1;
    }

    // Calculate framebuffer size and allocate
    uint32_t stride = VPUGetStride(config.vmode, config.cmode);
    g_framebuffer.size = stride * config.height;
    printf("Allocating framebuffer: %u bytes (stride=%u)\n", g_framebuffer.size, stride);

    if (SPAllocateBuffer(g_platform, &g_framebuffer) != 0) {
        printf("Failed to allocate framebuffer\n");
        SPShutdownPlatform(g_platform);
        return 1;
    }

    // Set up video mode
    printf("Setting video mode...\n");
    VPUSetVideoMode(g_platform->vx, config.vmode, config.cmode, EVS_Enable);

    // For 8-bit mode, ensure default palette is set
    if (config.cmode == ECM_8bit_Indexed) {
        VPUSetDefaultPalette(g_platform->vx);
    }

    // Set scanout address - single buffering, CPU and VPU use same buffer
    VPUSetScanoutAddress(g_platform->vx, (uint32_t)(uintptr_t)g_framebuffer.dmaAddress);

    // Draw test pattern based on color mode
    printf("Drawing test pattern...\n");
    if (config.cmode == ECM_8bit_Indexed) {
        drawTestPattern8((uint8_t*)g_framebuffer.cpuAddress, stride, config.width, config.height);
    } else {
        drawTestPattern16((uint16_t*)g_framebuffer.cpuAddress, stride, config.width, config.height);
    }

    printf("Test pattern displayed. Press Enter to exit...\n");
    getchar();

    // Cleanup
    SPFreeBuffer(g_platform, &g_framebuffer);
    SPShutdownPlatform(g_platform);

    return 0;
}
