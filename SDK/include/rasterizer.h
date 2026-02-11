#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Fixed-point types: 16.16 format
typedef int32_t fixed16_t;

#define FLOAT_TO_FIXED16(f) ((fixed16_t)((f) * 65536.0f))
#define FIXED16_TO_FLOAT(f) ((float)(f) / 65536.0f)
#define FIXED16_INT(f) ((f) >> 16)
#define FIXED16_FRAC(f) ((f) & 0xFFFF)

// Edge plane equation: e = a*x + b*y + c
typedef struct {
    int32_t a;      // ddx: change in e per pixel x
    int32_t b;      // ddy: change in e per pixel y
    int32_t c;      // base value at (0,0)
} edge_plane_t;

typedef struct {
    fixed16_t x[3];
    fixed16_t y[3];
    
    // TODO: add more vertex attributes here
    uint32_t color[3];
    
    edge_plane_t edges[3];

    // Bounds
    fixed16_t min_x, max_x;
    fixed16_t min_y, max_y;
} triangle_t;

// Tile output from rasterizer: 4x4 pixels laid out in linear format
typedef struct {
    // Mask per pixel (1: inside, 0: outside)
    uint16_t mask[16];
    
    int32_t ddx[3];
    int32_t ddy[3];
    
    int32_t e0, e1, e2;
} raster_block_t;

void RPUInitPrimitive(triangle_t *tri, float x0, float y0, float x1, float y1, float x2, float y2);
void RPURasterize(const triangle_t *tri, raster_block_t *screen_blocks, int screen_width, int screen_height);
void RPUResolve(const raster_block_t *screen_blocks, uint16_t *output, int screen_width, int screen_height);

#ifdef __cplusplus
}
#endif
