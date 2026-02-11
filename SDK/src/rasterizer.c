#include "rasterizer.h"
#include <math.h>
#include <string.h>
#include <arm_neon.h>

void RPUInitPrimitive(triangle_t *tri, float x0, float y0, float x1, float y1, float x2, float y2)
{
    tri->x[0] = FLOAT_TO_FIXED16(x0);
    tri->y[0] = FLOAT_TO_FIXED16(y0);
    tri->x[1] = FLOAT_TO_FIXED16(x1);
    tri->y[1] = FLOAT_TO_FIXED16(y1);
    tri->x[2] = FLOAT_TO_FIXED16(x2);
    tri->y[2] = FLOAT_TO_FIXED16(y2);
    
    tri->min_x = tri->x[0];
    tri->max_x = tri->x[0];
    if (tri->x[1] < tri->min_x) tri->min_x = tri->x[1];
    if (tri->x[1] > tri->max_x) tri->max_x = tri->x[1];
    if (tri->x[2] < tri->min_x) tri->min_x = tri->x[2];
    if (tri->x[2] > tri->max_x) tri->max_x = tri->x[2];
    
    tri->min_y = tri->y[0];
    tri->max_y = tri->y[0];
    if (tri->y[1] < tri->min_y) tri->min_y = tri->y[1];
    if (tri->y[1] > tri->max_y) tri->max_y = tri->y[1];
    if (tri->y[2] < tri->min_y) tri->min_y = tri->y[2];
    if (tri->y[2] > tri->max_y) tri->max_y = tri->y[2];
   
    for (int i = 0; i < 3; i++) {
        int next = (i + 1) % 3;
        // Convert to integer space for edge coefficients
        int32_t dx = FIXED16_INT(tri->x[next] - tri->x[i]);
        int32_t dy = FIXED16_INT(tri->y[next] - tri->y[i]);
        
        tri->edges[i].a = dy;   // ddx (integer units per pixel)
        tri->edges[i].b = -dx;  // ddy (integer units per pixel)
        
        // Compute c using integer coordinates
        int32_t x0 = FIXED16_INT(tri->x[i]);
        int32_t y0 = FIXED16_INT(tri->y[i]);
        tri->edges[i].c = -(dy * x0 + tri->edges[i].b * y0);
    }
}

static inline int32_t edge_eval_at(const edge_plane_t *edge,
                                   int32_t x, int32_t y)
{
    // edge = a*x + b*y + c
    return edge->a * (x >> 16) + edge->b * (y >> 16) + edge->c;
}

// This is defined in the .S file
extern void rasterizer_eval_4x4_neon_impl(
    int32_t e0, int32_t e1, int32_t e2,
    int32_t ddx0, int32_t ddx1, int32_t ddx2,
    int32_t ddy0, int32_t ddy1, int32_t ddy2,
    uint16_t *mask_out);

void rasterizer_eval_4x4_neon(const triangle_t *tri,
                              fixed16_t block_x,
                              fixed16_t block_y,
                              raster_block_t *out)
{
    int32_t e0 = edge_eval_at(&tri->edges[0], block_x, block_y);
    int32_t e1 = edge_eval_at(&tri->edges[1], block_x, block_y);
    int32_t e2 = edge_eval_at(&tri->edges[2], block_x, block_y);
    
    /* Store derivatives */
    out->ddx[0] = tri->edges[0].a;
    out->ddx[1] = tri->edges[1].a;
    out->ddx[2] = tri->edges[2].a;
    
    out->ddy[0] = tri->edges[0].b;
    out->ddy[1] = tri->edges[1].b;
    out->ddy[2] = tri->edges[2].b;
    
    out->e0 = e0;
    out->e1 = e1;
    out->e2 = e2;
    
    rasterizer_eval_4x4_neon_impl(
        e0, e1, e2,
        tri->edges[0].a, tri->edges[1].a, tri->edges[2].a,
        tri->edges[0].b, tri->edges[1].b, tri->edges[2].b,
        out->mask);
}

void RPURasterize(const triangle_t *tri, raster_block_t *screen_blocks, int screen_width, int screen_height)
{
    // Clear the screen blocks buffer
    int num_blocks_x = (screen_width + 3) / 4;
    int num_blocks_y = (screen_height + 3) / 4;
    memset(screen_blocks, 0, num_blocks_x * num_blocks_y * sizeof(raster_block_t));
    
    // Get triangle bounds in block coordinates
    int min_block_x = FIXED16_INT(tri->min_x) / 4;
    int max_block_x = (FIXED16_INT(tri->max_x) + 3) / 4;
    int min_block_y = FIXED16_INT(tri->min_y) / 4;
    int max_block_y = (FIXED16_INT(tri->max_y) + 3) / 4;
    
    // Clamp to screen bounds
    if (min_block_x < 0) min_block_x = 0;
    if (max_block_x > num_blocks_x) max_block_x = num_blocks_x;
    if (min_block_y < 0) min_block_y = 0;
    if (max_block_y > num_blocks_y) max_block_y = num_blocks_y;
    
    // Sweep through all blocks in bounding box
    for (int block_y = min_block_y; block_y < max_block_y; block_y++) {
        for (int block_x = min_block_x; block_x < max_block_x; block_x++) {
            // Evaluate at pixel centers: (block * 4 + 0.5) in fixed-point
            fixed16_t px = INT_TO_FIXED16(block_x * 4) + FIXED16_HALF;
            fixed16_t py = INT_TO_FIXED16(block_y * 4) + FIXED16_HALF;
            
            int block_idx = block_y * num_blocks_x + block_x;
            rasterizer_eval_4x4_neon(tri, px, py, &screen_blocks[block_idx]);
        }
    }
}

void RPUResolve(const raster_block_t *screen_blocks, uint16_t *output, int screen_width, int screen_height)
{
    int num_blocks_x = (screen_width + 3) / 4;
    int num_blocks_y = (screen_height + 3) / 4;
    
    // Process each block
    for (int block_y = 0; block_y < num_blocks_y; block_y++) {
        for (int block_x = 0; block_x < num_blocks_x; block_x++) {
            int block_idx = block_y * num_blocks_x + block_x;
            const raster_block_t *block = &screen_blocks[block_idx];
            
            // Write pixels from this block
            int base_x = block_x * 4;
            int base_y = block_y * 4;
            
            for (int py = 0; py < 4; py++) {
                for (int px = 0; px < 4; px++) {
                    int screen_x = base_x + px;
                    int screen_y = base_y + py;
                    
                    // Bounds check
                    if (screen_x >= screen_width || screen_y >= screen_height)
                        continue;
                    
                    int pixel_idx = py * 4 + px;
                    int output_idx = screen_y * screen_width + screen_x;
                    output[output_idx] = block->mask[pixel_idx];
                }
            }
        }
    }
}

