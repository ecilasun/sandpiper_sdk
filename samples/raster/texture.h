#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
 * Texture — runtime texture representation used by the raster sample.
 *
 * Only power-of-two dimensions are supported; this allows the NEON rasterizer
 * to use fast bitmask wrapping instead of modulo.  Both width and height must
 * satisfy (n & (n-1)) == 0.
 *
 * Supported on-disk formats (via texture_load_dds):
 *   • Uncompressed DDS with DDPF_RGB flag
 *     – R5G6B5  (16 bpp) — used as-is
 *     – R8G8B8  (24 bpp) — converted to RGB565
 *     – R8G8B8A8 / B8G8R8A8 (32 bpp) — alpha discarded, converted to RGB565
 *   • Block-compressed DDS (DDPF_FOURCC)
 *     – BC1 / DXT1 packed blocks
 *
 * BC1 textures are kept packed in memory and sampled as BC1 at runtime.
 *
 * If the file cannot be opened or the format is unsupported, fall back to
 * texture_create_checkerboard().
 */

enum ETextureFormat {
    ETF_Invalid = 0,
    ETF_RGB565,
    ETF_BC1
};

struct Texture {
    enum ETextureFormat format;
    uint16_t* pixels;       /* RGB565 pixel data (ETF_RGB565) */
    uint8_t*  bc1_blocks;   /* BC1 blocks (ETF_BC1), 8 bytes/block */
    int       bc1_stride_blocks; /* blocks per row for BC1 */
    int       width;    /* must be a power of two        */
    int       height;   /* must be a power of two        */
    int       w_mask;   /* width  - 1 (for fast wrapping) */
    int       h_mask;   /* height - 1 (for fast wrapping) */
};

/* Load a DDS texture and convert to RGB565.
 * Returns true on success; populates *tex.
 * Returns false on failure (format not supported, non-POT, or file error). */
bool texture_load_dds(const char* path, Texture* tex);

/* Create a w×h checkerboard with 8×8 cells using col_a and col_b (RGB565).
 * w and h must be powers of two. */
void texture_create_checkerboard(Texture* tex, int w, int h,
                                 uint16_t col_a, uint16_t col_b);

/* Sample texture at integer texel coordinates with wrapping.
 * Returns RGB565 color for both ETF_RGB565 and ETF_BC1 textures. */
uint16_t texture_sample_rgb565(const Texture* tex, int u, int v);

/* Inline BC1 sampler — u, v must already be wrapped (pre-masked).
 * Skips format dispatch and redundant masking for the rasterizer hot path.
 * Palette interpolation is branchless (ternaries compile to predicated
 * instructions on ARMv7 Cortex-A9). */
static inline uint16_t texture_sample_bc1_direct(const Texture* tex,
                                                 int u, int v)
{
    int bx = u >> 2;
    int by = v >> 2;
    int lx = u & 3;
    int ly = v & 3;
    const uint8_t* blk = tex->bc1_blocks +
                         (by * tex->bc1_stride_blocks + bx) * 8;

    uint16_t c0 = (uint16_t)(blk[0] | (blk[1] << 8));
    uint16_t c1 = (uint16_t)(blk[2] | (blk[3] << 8));
    uint32_t bits = (uint32_t)blk[4]         |
                    ((uint32_t)blk[5] <<  8) |
                    ((uint32_t)blk[6] << 16) |
                    ((uint32_t)blk[7] << 24);

    int sel = (int)((bits >> (2 * (ly * 4 + lx))) & 3u);

    int r0 = (c0 >> 11) & 31, g0 = (c0 >> 5) & 63, b0 = c0 & 31;
    int r1 = (c1 >> 11) & 31, g1 = (c1 >> 5) & 63, b1 = c1 & 31;

    /* Build full palette branchlessly.  The ternaries on simple ints
     * compile to IT-predicated MOV on ARMv7 (no branch). */
    int f = (c0 > c1);  /* 4-color mode flag */

    uint16_t pal[4];
    pal[0] = c0;
    pal[1] = c1;
    pal[2] = (uint16_t)(
        ((f ? (2*r0+r1)/3 : (r0+r1)>>1) << 11) |
        ((f ? (2*g0+g1)/3 : (g0+g1)>>1) <<  5) |
         (f ? (2*b0+b1)/3 : (b0+b1)>>1));
    pal[3] = (uint16_t)(
        (f * ((r0+2*r1)/3) << 11) |
        (f * ((g0+2*g1)/3) <<  5) |
        (f * ((b0+2*b1)/3)));

    return pal[sel];
}

/* Release memory allocated by texture_load_dds or texture_create_checkerboard. */
void texture_free(Texture* tex);
