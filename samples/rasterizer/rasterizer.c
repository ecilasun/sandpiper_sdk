#include <stdint.h>
#include <stdlib.h>
#include <math.h>

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

int main(int argc, char** argv)
{
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

        // Resolve raster blocks to framebuffer format
        RPUResolve(raster_blocks, (uint16_t*)s_platform->sc->writepage, VIDEO_WIDTH, VIDEO_HEIGHT);

        VPUWaitVSync(s_platform->vx);
        VPUSwapPages(s_platform->vx, s_platform->sc);
        t += 0.025f;
    } while (1);

    return 0;
}
