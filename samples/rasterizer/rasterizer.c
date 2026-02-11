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

// Display raw raster blocks without resolving
static void DisplayRawRasterBlocks(const raster_block_t *screen_blocks, uint16_t *output, int screen_width, int screen_height)
{
    int num_blocks_x = (screen_width + 3) / 4;
    int num_blocks_y = (screen_height + 3) / 4;
    
    // Process each block
    for (int block_y = 0; block_y < num_blocks_y; block_y++) {
        for (int block_x = 0; block_x < num_blocks_x; block_x++) {
            int block_idx = block_y * num_blocks_x + block_x;
            const raster_block_t *block = &screen_blocks[block_idx];
            
            // Draw entire 4x4 block as a single color based on whether any pixels are set
            int base_x = block_x * 4;
            int base_y = block_y * 4;
            
            // Check if any pixel in mask is non-zero
            uint16_t block_color = 0x0000;  // Black by default
            for (int i = 0; i < 16; i++) {
                if (block->mask[i] != 0) {
                    block_color = 0xFFFF;  // White if any pixel is set
                    break;
                }
            }
            
            // Fill entire 4x4 block with the color
            for (int py = 0; py < 4; py++) {
                for (int px = 0; px < 4; px++) {
                    int screen_x = base_x + px;
                    int screen_y = base_y + py;
                    
                    if (screen_x < screen_width && screen_y < screen_height) {
                        int output_idx = screen_y * screen_width + screen_x;
                        output[output_idx] = block_color;
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    // Check for --raw parameter
    int show_raw = 0;
    if (argc > 1 && (strcmp(argv[1], "--raw") == 0 || strcmp(argv[1], "-r") == 0)) {
        show_raw = 1;
        printf("Displaying raw raster blocks (4x4 block granularity)\n");
    } else {
        printf("Displaying resolved output (pixel-perfect)\n");
        printf("Use --raw or -r to display unresolved raster blocks\n");
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
