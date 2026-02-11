#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "core.h"
#include "platform.h"
#include "rasterizer.h"
#include "vpu.h"

#define VIDEO_MODE      EVM_320_240
#define VIDEO_COLOR     ECM_16bit_RGB
#define VIDEO_WIDTH     320
#define VIDEO_HEIGHT    240

static struct SPPlatform* s_platform = NULL;
static struct SPSizeAlloc frameBufferA;
static struct SPSizeAlloc frameBufferB;

// Display raw mask data as 16-bit values (not interpreting as bits)
static void DisplayRawRasterBlocks(const raster_block_t *screen_blocks, uint16_t *output, int screen_width, int screen_height)
{
    int num_blocks_x = (screen_width + 3) / 4;
    int num_blocks_y = (screen_height + 3) / 4;
    int total_blocks = num_blocks_x * num_blocks_y;
    
    // Clear output first
    memset(output, 0, screen_width * screen_height * sizeof(uint16_t));
    
    // Display mask data as raw 16-bit values
    int output_idx = 0;
    for (int block_idx = 0; block_idx < total_blocks && output_idx < screen_width * screen_height; block_idx++) {
        const raster_block_t *block = &screen_blocks[block_idx];
        
        // Write the raw mask value directly as a pixel
        output[output_idx++] = block->mask;
    }
}

int main(int argc, char** argv)
{
    // Check for --raw parameter
    int show_raw = 0;
    if (argc > 1 && (strcmp(argv[1], "--raw") == 0 || strcmp(argv[1], "-r") == 0)) {
        show_raw = 1;
        printf("Displaying raw mask data as linear memory dump (16 pixels per block)\n");
    } else {
        printf("Displaying resolved output (pixel-perfect)\n");
        printf("Use --raw or -r to display raw mask memory layout\n");
    }
    
    s_platform = SPInitPlatform();

    uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
    VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

    frameBufferA.size = stride * VIDEO_HEIGHT;
    frameBufferB.size = stride * VIDEO_HEIGHT;
    SPAllocateBuffer(s_platform, &frameBufferA);
    SPAllocateBuffer(s_platform, &frameBufferB);

    memset(frameBufferA.cpuAddress, 0, frameBufferA.size);
    memset(frameBufferB.cpuAddress, 0, frameBufferB.size);

    s_platform->sc->cycle = 1;
    s_platform->sc->framebufferA = &frameBufferA;
    s_platform->sc->framebufferB = &frameBufferB;
    VPUSwapPages(s_platform->vx, s_platform->sc);

    int num_blocks_x = (VIDEO_WIDTH + 3) / 4;
    int num_blocks_y = (VIDEO_HEIGHT + 3) / 4;
    raster_block_t* raster_blocks = (raster_block_t*)malloc(num_blocks_x * num_blocks_y * sizeof(raster_block_t));

    float t = 0.0f;

    do
    {
        float cx = (float)VIDEO_WIDTH * 0.5f + sinf(t) * 80.0f;
        float cy = (float)VIDEO_HEIGHT * 0.5f;
        float size = 60.0f;

        // Prepare all primitives (one in this case)
        triangle_t triangle;
        RPUInitPrimitive(&triangle, cx - size, cy + size * 0.8f, cx + size, cy + size * 0.8f, cx, cy - size);

        // Rasterize all primitives to raster blocks
        RPURasterize(&triangle, raster_blocks, VIDEO_WIDTH, VIDEO_HEIGHT);

        // Display output based on mode
        if (show_raw) {
            // Show raw raster blocks (4x4 block granularity)
            DisplayRawRasterBlocks(raster_blocks, (uint16_t*)s_platform->sc->writepage, VIDEO_WIDTH, VIDEO_HEIGHT);
        } else {
            // Resolve raster blocks to framebuffer format (pixel-perfect)
            RPUResolve(raster_blocks, (uint16_t*)s_platform->sc->writepage, VIDEO_WIDTH, VIDEO_HEIGHT);
        }

        VPUWaitVSync(s_platform->vx);
        VPUSwapPages(s_platform->vx, s_platform->sc);
        t += 0.025f;
    } while (1);

    return 0;
}
