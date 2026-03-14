/**
 * \file voxelspace.cpp
 * \brief Dual-core VoxelSpace terrain renderer
 *
 * \ingroup examples
 * Comanche-style height-field landscape renderer using Sandpiper's two VPU
 * hardware image planes:
 *
 *   Layer A  — static sky gradient, rendered once at startup, never touched again.
 *   Layer B  — terrain, double-buffered, rendered every frame by two CPU cores.
 *
 * Layer B pixels that were not covered by terrain remain KEY_COLOR (magenta).
 * The VPU hardware mixer (mix-mode 1, keycolor transparency) replaces those
 * pixels with the corresponding Layer A pixel, so the sky gradient shows through
 * automatically without any per-pixel blending in software.
 *
 * Terrain is generated procedurally (sum-of-sines height field) so no external
 * data files are needed.  Core 0 renders screen columns 0..319 and core 1
 * renders columns 320..639 simultaneously using the same barrier pattern as the
 * boids and mandelbrot samples.
 *
 * Algorithm reference: https://github.com/s-macke/VoxelSpace
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <arm_neon.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

// ---------------------------------------------------------------------------
// Video configuration
// ---------------------------------------------------------------------------
#define VIDEO_MODE      EVM_640_480
#define VIDEO_COLOR     ECM_16bit_RGB
#define VIDEO_WIDTH     640
#define VIDEO_HEIGHT    480

// Magenta key-color: Layer B pixels with this color become transparent,
// revealing Layer A (sky) behind them.
#define KEY_COLOR_565   MAKECOLORRGB16(31, 0, 31)   // 0xF81F

// ---------------------------------------------------------------------------
// Terrain maps (1024x1024, power-of-two so wrap is a free bitmask)
// ---------------------------------------------------------------------------
#define MAP_SIZE    1024
#define MAP_MASK    (MAP_SIZE - 1)

static uint8_t  s_heightmap[MAP_SIZE * MAP_SIZE];
static uint16_t s_colormap [MAP_SIZE * MAP_SIZE];

// ---------------------------------------------------------------------------
// Camera state (read-only by worker threads each frame)
// ---------------------------------------------------------------------------
struct Camera
{
    float px, py;       // world position in map units
    float angle;        // yaw in radians
    float height;       // camera altitude (world units)
    float horizon;      // screen-space horizon line (pixels from top)
    float scale;        // vertical projection scale factor
    float farDist;      // maximum render distance (map units)
};

static Camera s_cam;

// ---------------------------------------------------------------------------
// Worker thread barrier (boids-style: mutex+cond, dispatch-and-wait)
// ---------------------------------------------------------------------------
static pthread_mutex_t s_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  s_jobCond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  s_doneCond= PTHREAD_COND_INITIALIZER;
static int  s_frameNum  = 0;    // incremented by main to trigger workers
static int  s_jobsDone  = 0;    // workers increment when finished
static bool s_shutdown  = false;

struct RenderArgs
{
    int       colStart;
    int       colEnd;
    uint16_t* fb;
    uint32_t  stride;   // bytes per row
};

static RenderArgs s_args[2];

// ---------------------------------------------------------------------------
// Procedural terrain generation
// Sum-of-sines with varying frequencies and phases, plus a coarser
// ridge layer — produces varied terrain with peaks, valleys, and plains.
// ---------------------------------------------------------------------------
static void generate_terrain()
{
    printf("Generating terrain...\n");
    for (int y = 0; y < MAP_SIZE; ++y)
    {
        float fy = (float)y;
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            float fx = (float)x;

            // Several overlapping sinusoidal layers at different scales
            float h =  sinf(fx * 0.00813f + fy * 0.00671f) * 0.28f
                     + sinf(fx * 0.02031f - fy * 0.01753f) * 0.18f
                     + sinf(fy * 0.00531f - fx * 0.00413f) * 0.24f
                     + sinf(fx * 0.04217f + fy * 0.03891f) * 0.11f
                     + sinf(fx * 0.08811f - fy * 0.07513f) * 0.06f
                     + cosf(fx * 0.01523f + fy * 0.02031f) * 0.14f
                     // Ridge: fold negative values to create mountain ridges
                     + fabsf(sinf(fx * 0.00913f - fy * 0.01031f)) * 0.22f
                     - 0.15f; // bias slightly downward so water level is common

            // Map [-1..1] → [0..255]
            int ih = (int)((h + 1.0f) * 127.5f);
            if (ih <   0) ih =   0;
            if (ih > 255) ih = 255;

            s_heightmap[y * MAP_SIZE + x] = (uint8_t)ih;

            // Color zones based on elevation
            uint16_t color;
            if (ih < 48)
            {
                // Deep water: dark blue-green
                int b = 16 + ih / 5;
                color = MAKECOLORRGB16(0, 6 + ih / 12, b);
            }
            else if (ih < 62)
            {
                // Shoreline / shallow water: lighter blue
                color = MAKECOLORRGB16(1, 13, 27);
            }
            else if (ih < 75)
            {
                // Beach / sand: warm tan
                color = MAKECOLORRGB16(23, 40, 7);
            }
            else if (ih < 135)
            {
                // Lowland / grassland: green gradient
                int g = 22 + (ih - 75) / 7;
                if (g > 45) g = 45;
                color = MAKECOLORRGB16(5, g, 3);
            }
            else if (ih < 180)
            {
                // Highland / dark forest
                int g = 26 + (ih - 135) / 9;
                if (g > 36) g = 36;
                color = MAKECOLORRGB16(6, g, 4);
            }
            else if (ih < 215)
            {
                // Rock face: brown-grey
                int v = 11 + (ih - 180) / 5;
                color = MAKECOLORRGB16(v, v - 1, v - 3);
            }
            else
            {
                // Snow cap: bright white
                int v = 22 + (ih - 215) / 3;
                if (v > 31) v = 31;
                color = MAKECOLORRGB16(v, v * 2, v);
            }

            s_colormap[y * MAP_SIZE + x] = color;
        }
    }
    printf("Terrain ready.\n");
}

// ---------------------------------------------------------------------------
// Sky layer (Layer A): drawn once to a static buffer
// Gradient: deep midnight blue at top → warm dusk at horizon
// ---------------------------------------------------------------------------
static void draw_sky(uint8_t* base, uint32_t stride)
{
    for (int y = 0; y < VIDEO_HEIGHT; ++y)
    {
        uint16_t* row = (uint16_t*)(base + (uint32_t)y * stride);
        float t = (float)y / (float)(VIDEO_HEIGHT - 1);  // 0 = top, 1 = bottom

        // deep blue → pale cyan-orange haze near horizon
        int r = (int)(1  + t * 10);
        int g = (int)(10 + t * 22);
        int b = (int)(28 - t * 6);
        if (r > 31) r = 31;
        if (g > 63) g = 63;
        if (b <  0) b =  0;

        uint16_t sky = MAKECOLORRGB16(r, g, b);
        for (int x = 0; x < VIDEO_WIDTH; ++x)
            row[x] = sky;
    }
}

// ---------------------------------------------------------------------------
// Fill a framebuffer with the key-color (NEON 8-pixels-at-a-time)
// ---------------------------------------------------------------------------
static void fill_keycolor(uint16_t* fb, uint32_t strideWords, uint32_t height)
{
    uint16x8_t kc8 = vdupq_n_u16((uint16_t)KEY_COLOR_565);
    uint32_t total = strideWords * height;
    uint32_t i = 0;

    for (; i + 8 <= total; i += 8)
        vst1q_u16(fb + i, kc8);

    // Tail (< 8 pixels, unlikely for power-of-two resolution but safe)
    for (; i < total; ++i)
        fb[i] = (uint16_t)KEY_COLOR_565;
}

// ---------------------------------------------------------------------------
// Core renderer: process screen columns [colStart, colEnd)
//
// For each column a ray is stepped from near to far distance.  The projected
// screen row of the terrain sample is compared against the highest row drawn
// so far in this column (starts at screen bottom).  Whenever the new projection
// is above the previous maximum, pixels are filled from the new row down to the
// old maximum — painting terrain from the ground up as depth increases.
// Pixels not covered by terrain retain KEY_COLOR, so the sky shows through.
// ---------------------------------------------------------------------------
static void render_columns(int colStart, int colEnd,
                            uint16_t* fb, uint32_t strideWords)
{
    // Snapshot camera state (both threads read without locking — safe since
    // main writes s_cam before waking workers and does not touch it until
    // the barrier signals both workers have finished).
    const float camPX  = s_cam.px;
    const float camPY  = s_cam.py;
    const float camH   = s_cam.height;
    const float scale  = s_cam.scale;
    const float horiz  = s_cam.horizon;
    const float farD   = s_cam.farDist;

    // 120-degree horizontal FOV split evenly across VIDEO_WIDTH columns.
    // Each column gets a linearly interpolated ray direction, computed by
    // pre-stepping cos/sin across columns (no per-step trig inside the loop).
    const float FOV_HALF = 1.0471975512f;   // π/3 = 60°
    const float angle    = s_cam.angle;

    float cosL = cosf(angle + FOV_HALF);
    float sinL = sinf(angle + FOV_HALF);
    float cosR = cosf(angle - FOV_HALF);
    float sinR = sinf(angle - FOV_HALF);

    float dcos = (cosR - cosL) / (float)VIDEO_WIDTH;
    float dsin = (sinR - sinL) / (float)VIDEO_WIDTH;

    // Start ray direction at first column of this slice
    float rcos = cosL + dcos * (float)colStart;
    float rsin = sinL + dsin * (float)colStart;

    for (int col = colStart; col < colEnd; ++col, rcos += dcos, rsin += dsin)
    {
        // Maximum visible screen row for this column (starts at screen bottom,
        // decreases as farther/higher terrain is found).
        int maxRow = VIDEO_HEIGHT;

        // Step depth from near to far with mildly increasing step size to
        // reduce iterations at distance where detail is sub-pixel anyway.
        for (float z = 1.0f; z < farD; z += 1.0f + z * 0.008f)
        {
            int mx = ((int)(camPX + rcos * z)) & MAP_MASK;
            int my = ((int)(camPY + rsin * z)) & MAP_MASK;

            int    idx    = my * MAP_SIZE + mx;
            float  terrH  = (float)s_heightmap[idx];

            // Project terrain top onto screen (smaller screenRow = higher on screen)
            int screenRow = (int)((camH - terrH) / z * scale + horiz);

            if (screenRow < maxRow)
            {
                if (screenRow < 0) screenRow = 0;

                // Apply depth fog: blend toward the sky haze colour at distance
                uint16_t color = s_colormap[idx];
                float fogT = z / farD;
                if (fogT > 0.6f)
                {
                    float f = (fogT - 0.6f) * 2.5f;  // 0 → 1 over last 40% of range
                    if (f > 1.0f) f = 1.0f;
                    int r  = (color >> 11) & 0x1F;
                    int g  = (color >>  5) & 0x3F;
                    int b  =  color        & 0x1F;
                    // Fog target: pale blue matching sky bottom
                    int fr = 10, fg = 30, fb2 = 22;
                    r = r + (int)((fr - r) * f);
                    g = g + (int)((fg - g) * f);
                    b = b + (int)((fb2 - b) * f);
                    color = MAKECOLORRGB16(r, g, b);
                }

                // Fill vertical line from screenRow down to maxRow (exclusive)
                for (int row = screenRow; row < maxRow; ++row)
                    fb[(uint32_t)row * strideWords + (uint32_t)col] = color;

                maxRow = screenRow;
                if (maxRow <= 0)
                    break;  // entire column is filled, stop depth stepping
            }
        }
        // Pixels fb[0..maxRow-1][col] remain KEY_COLOR → sky shows through
    }
}

// ---------------------------------------------------------------------------
// Worker thread (pinned to a specific CPU core by the main thread)
// ---------------------------------------------------------------------------
static void* worker_main(void* arg)
{
    RenderArgs* ra = (RenderArgs*)arg;
    int localFrame = 0;

    while (1)
    {
        // Wait for the main thread to publish a new frame
        pthread_mutex_lock(&s_mutex);
        while (!s_shutdown && s_frameNum == localFrame)
            pthread_cond_wait(&s_jobCond, &s_mutex);

        if (s_shutdown)
        {
            pthread_mutex_unlock(&s_mutex);
            break;
        }
        localFrame = s_frameNum;

        // Snapshot render parameters (written by main before broadcast)
        uint16_t* fb          = ra->fb;
        uint32_t  strideWords = ra->stride >> 1;
        int       colStart    = ra->colStart;
        int       colEnd      = ra->colEnd;
        pthread_mutex_unlock(&s_mutex);

        render_columns(colStart, colEnd, fb, strideWords);

        // Signal main that this worker is done
        pthread_mutex_lock(&s_mutex);
        ++s_jobsDone;
        if (s_jobsDone == 2)
            pthread_cond_signal(&s_doneCond);
        pthread_mutex_unlock(&s_mutex);
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    generate_terrain();

    // Initial camera: start in the middle of the map, high enough to see terrain
    s_cam.px      = (float)(MAP_SIZE / 2);
    s_cam.py      = (float)(MAP_SIZE / 2);
    s_cam.angle   = 0.0f;
    s_cam.horizon = (float)(VIDEO_HEIGHT / 2) + 30.0f;  // horizon slightly below centre
    s_cam.scale   = 240.0f;                             // tuned for 640x480
    s_cam.farDist = 280.0f;

    // Initial height: hover above starting terrain + 50 world units
    {
        int cx = (int)s_cam.px & MAP_MASK;
        int cy = (int)s_cam.py & MAP_MASK;
        s_cam.height = (float)s_heightmap[cy * MAP_SIZE + cx] + 50.0f;
    }

    // -----------------------------------------------------------------------
    // Platform + VPU init
    // -----------------------------------------------------------------------
    struct SPPlatform* platform = SPInitPlatform();
    if (!platform)
    {
        fprintf(stderr, "Failed to init platform\n");
        return -1;
    }

    VPUSetVideoMode(platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);
    uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);    // bytes per row

    // -----------------------------------------------------------------------
    // Allocate buffers
    // Layer A: one static sky buffer
    // Layer B: two terrain buffers for double-buffering
    // -----------------------------------------------------------------------
    struct SPSizeAlloc fbSky, fbB0, fbB1;
    fbSky.size = stride * VIDEO_HEIGHT;
    fbB0.size  = stride * VIDEO_HEIGHT;
    fbB1.size  = stride * VIDEO_HEIGHT;

    SPAllocateBuffer(platform, &fbSky);
    SPAllocateBuffer(platform, &fbB0);
    SPAllocateBuffer(platform, &fbB1);

    // Draw sky into Layer A (written once, never changed)
    draw_sky(fbSky.cpuAddress, stride);

    // -----------------------------------------------------------------------
    // Configure VPU dual-plane output
    // Layer A = static sky (VPUSetScanoutAddress)
    // Layer B = terrain, double-buffered (VPUSetScanoutAddressB / Address2B)
    // Mix mode 1 = keycolor transparency: KEY_COLOR on Layer B → Layer A shows
    // -----------------------------------------------------------------------
    VPUSetScanoutAddress  (platform->vx, (uint32_t)fbSky.dmaAddress);
    VPUSetScanoutAddressB (platform->vx, (uint32_t)fbB0.dmaAddress);
    VPUSetScanoutAddress2B(platform->vx, (uint32_t)fbB1.dmaAddress);
    VPUSetMixMode(platform->vx, 1, 1, (uint16_t)KEY_COLOR_565);

    // -----------------------------------------------------------------------
    // Spawn two worker threads, one per CPU core
    // -----------------------------------------------------------------------
    pthread_t      workers[2];
    pthread_attr_t attrs[2];
    cpu_set_t      cpusets[2];

    s_args[0].colStart = 0;
    s_args[0].colEnd   = VIDEO_WIDTH / 2;
    s_args[1].colStart = VIDEO_WIDTH / 2;
    s_args[1].colEnd   = VIDEO_WIDTH;

    for (int t = 0; t < 2; ++t)
    {
        CPU_ZERO(&cpusets[t]);
        CPU_SET(t, &cpusets[t]);
        pthread_attr_init(&attrs[t]);
        pthread_attr_setaffinity_np(&attrs[t], sizeof(cpu_set_t), &cpusets[t]);
        pthread_create(&workers[t], &attrs[t], worker_main, &s_args[t]);
        pthread_attr_destroy(&attrs[t]);
    }

    printf("VoxelSpace running (CTRL+C to exit)\n");

    int cycle = 0;

    while (1)
    {
        // Wait until the VPU FIFO has drained — ensures the previous VPUSyncSwapB
        // vsync event has actually fired and the front/back assignment is settled
        // before we decide which buffer is safe to write into.
        while (VPUGetFIFONotEmpty(platform->vx)) { }

        // Pick the back Layer B buffer for writing.
        // Initially fbB0 is the front (VPUSetScanoutAddressB) and fbB1 is the back
        // (VPUSetScanoutAddress2B), so cycle 0 must write to fbB1 — matching the
        // same selection used by vpu_layers_demo.
        struct SPSizeAlloc* back  = (cycle % 2 == 0) ? &fbB1 : &fbB0;
        uint16_t*           fb   = (uint16_t*)back->cpuAddress;
        uint32_t       strideW   = stride >> 1;

        // Fill terrain buffer with key-color (sky punches through undrawn areas)
        fill_keycolor(fb, strideW, VIDEO_HEIGHT);

        // Publish back-buffer pointer and dispatch workers
        pthread_mutex_lock(&s_mutex);
        s_args[0].fb     = fb;
        s_args[0].stride = stride;
        s_args[1].fb     = fb;
        s_args[1].stride = stride;
        s_jobsDone = 0;
        ++s_frameNum;
        pthread_cond_broadcast(&s_jobCond);
        while (s_jobsDone < 2)
            pthread_cond_wait(&s_doneCond, &s_mutex);
        pthread_mutex_unlock(&s_mutex);

        // Swap Layer B to the freshly rendered terrain buffer
        VPUSyncSwapB(platform->vx, 0);
        VPUNoop(platform->vx);   // barrier: wait until the command is consumed

        // -----------------------------------------------------------------------
        // Advance camera: slow auto-fly with gentle banking turns
        // -----------------------------------------------------------------------
        // Gradually turn to follow a curved path (direction slowly oscillates)
        float turnRate = 0.0008f * sinf((float)cycle * 0.0031f);
        s_cam.angle += turnRate;
        if (s_cam.angle >  6.28318f) s_cam.angle -= 6.28318f;
        if (s_cam.angle <  0.0f    ) s_cam.angle += 6.28318f;

        // Advance forward
        float speed = 1.1f;
        s_cam.px += cosf(s_cam.angle) * speed;
        s_cam.py += sinf(s_cam.angle) * speed;

        // Wrap position within the tiling map
        s_cam.px = fmodf(s_cam.px + (float)MAP_SIZE, (float)MAP_SIZE);
        s_cam.py = fmodf(s_cam.py + (float)MAP_SIZE, (float)MAP_SIZE);

        // Float camera height above local terrain with gentle damping
        int camMX = (int)s_cam.px & MAP_MASK;
        int camMY = (int)s_cam.py & MAP_MASK;
        float groundH  = (float)s_heightmap[camMY * MAP_SIZE + camMX];
        float targetH  = groundH + 55.0f;
        s_cam.height  += (targetH - s_cam.height) * 0.04f;

        // Tilt horizon slightly to simulate banking (cosmetic only)
        float bankHorizonBias = turnRate * 3000.0f;
        s_cam.horizon = (float)(VIDEO_HEIGHT / 2) + 30.0f + bankHorizonBias;

        ++cycle;
    }

    return 0;
}
