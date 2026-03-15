#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * DDS on-disk structures
 * ------------------------------------------------------------------------- */

#pragma pack(push, 1)
struct DDSPixelFormat {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

struct DDSHeader {
    uint32_t       dwSize;
    uint32_t       dwFlags;
    uint32_t       dwHeight;
    uint32_t       dwWidth;
    uint32_t       dwPitchOrLinearSize;
    uint32_t       dwDepth;
    uint32_t       dwMipMapCount;
    uint32_t       dwReserved1[11];
    DDSPixelFormat ddspf;
    uint32_t       dwCaps;
    uint32_t       dwCaps2;
    uint32_t       dwCaps3;
    uint32_t       dwCaps4;
    uint32_t       dwReserved2;
};
#pragma pack(pop)

static const uint32_t DDS_MAGIC        = 0x20534444u; /* "DDS " */
static const uint32_t DDPF_ALPHAPIXELS = 0x00000001u;
static const uint32_t DDPF_FOURCC      = 0x00000004u;
static const uint32_t DDPF_RGB         = 0x00000040u;
static const uint32_t FOURCC_DXT1      = 0x31545844u; /* "DXT1" */

/* -------------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------------- */

static inline bool is_power_of_two(int n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

static inline uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((uint16_t)(r >> 3) << 11) |
                      ((uint16_t)(g >> 2) <<  5) |
                       (uint16_t)(b >> 3));
}

static inline void tex_init_masks(Texture* tex)
{
    tex->w_mask = tex->width  - 1;
    tex->h_mask = tex->height - 1;
}

static inline uint16_t pack565(int r, int g, int b)
{
    return (uint16_t)(((r & 31) << 11) | ((g & 63) << 5) | (b & 31));
}

static inline void unpack565(uint16_t c, int* r, int* g, int* b)
{
    *r = (c >> 11) & 31;
    *g = (c >> 5)  & 63;
    *b = c & 31;
}

static uint16_t bc1_sample_block_texel(const uint8_t* blk, int lx, int ly)
{
    uint16_t c0 = (uint16_t)(blk[0] | (blk[1] << 8));
    uint16_t c1 = (uint16_t)(blk[2] | (blk[3] << 8));
    uint32_t bits = (uint32_t)blk[4] |
                    ((uint32_t)blk[5] << 8) |
                    ((uint32_t)blk[6] << 16) |
                    ((uint32_t)blk[7] << 24);

    int r0, g0, b0, r1, g1, b1;
    unpack565(c0, &r0, &g0, &b0);
    unpack565(c1, &r1, &g1, &b1);

    uint16_t pal[4];
    pal[0] = c0;
    pal[1] = c1;

    if (c0 > c1) {
        pal[2] = pack565((2*r0 + r1) / 3, (2*g0 + g1) / 3, (2*b0 + b1) / 3);
        pal[3] = pack565((r0 + 2*r1) / 3, (g0 + 2*g1) / 3, (b0 + 2*b1) / 3);
    } else {
        pal[2] = pack565((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2);
        pal[3] = 0; /* DXT1 transparent color; treated as black in RGB565 */
    }

    int sel = (int)((bits >> (2 * (ly * 4 + lx))) & 0x3u);
    return pal[sel];
}

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */

bool texture_load_dds(const char* path, Texture* tex)
{
    tex->format = ETF_Invalid;
    tex->pixels = nullptr;
    tex->bc1_blocks = nullptr;
    tex->bc1_stride_blocks = 0;

    FILE* fp = fopen(path, "rb");
    if (!fp)
        return false;

    uint32_t magic = 0;
    if (fread(&magic, 4, 1, fp) != 1 || magic != DDS_MAGIC) {
        fclose(fp);
        return false;
    }

    DDSHeader hdr;
    if (fread(&hdr, sizeof(hdr), 1, fp) != 1) {
        fclose(fp);
        return false;
    }

    int w = (int)hdr.dwWidth;
    int h = (int)hdr.dwHeight;

    /* Validate dimensions */
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096 ||
        !is_power_of_two(w) || !is_power_of_two(h)) {
        fprintf(stderr, "raster: DDS %s: non-power-of-two or invalid size %dx%d\n",
                path, w, h);
        fclose(fp);
        return false;
    }

    if (hdr.ddspf.dwFlags & DDPF_RGB) {
        uint32_t bpp = hdr.ddspf.dwRGBBitCount;

        uint16_t* pixels = (uint16_t*)malloc((size_t)(w * h) * sizeof(uint16_t));
        if (!pixels) {
            fclose(fp);
            return false;
        }

        bool ok = true;

        if (bpp == 16) {
            /* Assume R5G6B5 — read directly */
            ok = (fread(pixels, sizeof(uint16_t), (size_t)(w * h), fp) == (size_t)(w * h));

        } else if (bpp == 24) {
            /* R8G8B8 → RGB565 */
            for (int i = 0; i < w * h && ok; ++i) {
                uint8_t rgb[3];
                ok = (fread(rgb, 1, 3, fp) == 3);
                if (ok)
                    pixels[i] = rgb888_to_rgb565(rgb[0], rgb[1], rgb[2]);
            }

        } else if (bpp == 32) {
            /* R8G8B8A8 or B8G8R8A8 — determine order from red bitmask */
            bool bgr = (hdr.ddspf.dwRBitMask == 0x00FF0000u);
            for (int i = 0; i < w * h && ok; ++i) {
                uint8_t rgba[4];
                ok = (fread(rgba, 1, 4, fp) == 4);
                if (ok) {
                    pixels[i] = bgr ? rgb888_to_rgb565(rgba[2], rgba[1], rgba[0])
                                    : rgb888_to_rgb565(rgba[0], rgba[1], rgba[2]);
                }
            }

        } else {
            fprintf(stderr, "raster: DDS %s: unsupported bit depth %u\n", path, bpp);
            ok = false;
        }

        fclose(fp);

        if (!ok) {
            free(pixels);
            return false;
        }

        tex->format = ETF_RGB565;
        tex->pixels = pixels;
        tex->bc1_blocks = nullptr;
        tex->bc1_stride_blocks = 0;
        tex->width  = w;
        tex->height = h;
        tex_init_masks(tex);
        return true;
    }

    if ((hdr.ddspf.dwFlags & DDPF_FOURCC) && hdr.ddspf.dwFourCC == FOURCC_DXT1) {
        int bw = (w + 3) >> 2;
        int bh = (h + 3) >> 2;
        size_t bc1_size = (size_t)bw * (size_t)bh * 8u;

        uint8_t* blocks = (uint8_t*)malloc(bc1_size);
        if (!blocks) {
            fclose(fp);
            return false;
        }

        bool ok = (fread(blocks, 1, bc1_size, fp) == bc1_size);
        fclose(fp);

        if (!ok) {
            free(blocks);
            return false;
        }

        tex->format = ETF_BC1;
        tex->pixels = nullptr;
        tex->bc1_blocks = blocks;
        tex->bc1_stride_blocks = bw;
        tex->width = w;
        tex->height = h;
        tex_init_masks(tex);
        return true;
    }

    fprintf(stderr, "raster: DDS %s: unsupported compressed format (only BC1/DXT1 supported)\n", path);
    fclose(fp);
    return false;
}

void texture_create_checkerboard(Texture* tex, int w, int h,
                                 uint16_t col_a, uint16_t col_b)
{
    uint16_t* pixels = (uint16_t*)malloc((size_t)(w * h) * sizeof(uint16_t));
    if (!pixels) {
        tex->pixels = nullptr;
        tex->width  = 0;
        tex->height = 0;
        tex->w_mask = 0;
        tex->h_mask = 0;
        return;
    }

    /* 8×8 checker cells */
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int cell = ((x >> 3) ^ (y >> 3)) & 1;
            pixels[y * w + x] = cell ? col_a : col_b;
        }
    }

    tex->pixels = pixels;
    tex->format = ETF_RGB565;
    tex->bc1_blocks = nullptr;
    tex->bc1_stride_blocks = 0;
    tex->width  = w;
    tex->height = h;
    tex_init_masks(tex);
}

uint16_t texture_sample_rgb565(const Texture* tex, int u, int v)
{
    int uu = u & tex->w_mask;
    int vv = v & tex->h_mask;

    if (tex->format == ETF_RGB565)
        return tex->pixels[vv * tex->width + uu];

    if (tex->format == ETF_BC1) {
        int bx = uu >> 2;
        int by = vv >> 2;
        int lx = uu & 3;
        int ly = vv & 3;
        const uint8_t* blk = tex->bc1_blocks + ((by * tex->bc1_stride_blocks + bx) * 8);
        return bc1_sample_block_texel(blk, lx, ly);
    }

    return 0;
}

void texture_free(Texture* tex)
{
    free(tex->pixels);
    free(tex->bc1_blocks);
    tex->format = ETF_Invalid;
    tex->pixels = nullptr;
    tex->bc1_blocks = nullptr;
    tex->bc1_stride_blocks = 0;
    tex->width  = 0;
    tex->height = 0;
    tex->w_mask = 0;
    tex->h_mask = 0;
}
