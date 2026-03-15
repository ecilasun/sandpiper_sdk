/*
 * raster.cpp — Textured polygon filling demo (NEON-accelerated)
 *
 * Runs in 320×240 16bpp RGB565 mode on the Sandpiper platform.
 *
 * By default a procedural 64×64 checkerboard texture and a built-in unit
 * cube are used.  Pass real assets on the command line to override:
 *
 *   ./raster [texture.dds [model.obj]]
 *
 * NEON optimisation strategy
 * --------------------------
 * The inner triangle-fill loop processes 4 horizontal pixels per iteration
 * using ARMv7 NEON intrinsics:
 *   • Edge-function inside-test      — integer 4-wide compare
 *   • Barycentric weight computation — float 4-wide multiply/accumulate
 *   • Depth test                     — float 4-wide compare against depth buf
 *   • UV texture-index computation   — integer shift + mask + multiply-add
 *   • Texture gather                 — 4 scalar loads (no ARMv7 gather instr.)
 *   • Pixel store                    — vst1_u16 (4 uint16_t = 8 bytes)
 *
 * Perspective correction: UVs are interpolated as (u/w, v/w, 1/w) and
 * reconstructed per-pixel using u = (u_over_w) / (1/w),
 * v = (v_over_w) / (1/w).
 */

#include "mesh.h"
#include "texture.h"
#include "core.h"
#include "vpu.h"
#include "vec.h"

#include <arm_neon.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* -------------------------------------------------------------------------
 * Screen and video constants
 * ------------------------------------------------------------------------- */

#define SCREEN_W     320
#define SCREEN_H     240
#define VIDEO_MODE   EVM_320_240
#define VIDEO_COLOR  ECM_16bit_RGB

/* Background colour — deep midnight blue in RGB565 */
#define BG_COLOR     MAKECOLORRGB16(2, 4, 12)

/* -------------------------------------------------------------------------
 * Platform globals
 * ------------------------------------------------------------------------- */

static struct SPPlatform* s_platform = nullptr;
static struct SPSizeAlloc s_fbA;
static struct SPSizeAlloc s_fbB;
static uint32_t           s_stride   = 0;   /* bytes per framebuffer row */
static float*             s_depth    = nullptr;

/* -------------------------------------------------------------------------
 * Screen-space projected vertex (intermediate rasterizer input)
 * ------------------------------------------------------------------------- */

struct SV {
    int   x, y;   /* pixel coordinates */
    float z;      /* NDC depth in [0, 1] (smaller = closer) */
    float u_over_w;
    float v_over_w;
    float inv_w;
    float shade;
};

/* -------------------------------------------------------------------------
 * Point lights and Gouraud helpers
 * ------------------------------------------------------------------------- */

struct PointLight {
    vec3_t position;
    vec3_t color;
    float  intensity;
    float  radius;
};

static inline float saturate(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

static inline uint16_t modulate_rgb565(uint16_t c, float shade)
{
    int s = (int)(shade * 256.0f);
    if (s < 0) s = 0;
    if (s > 512) s = 512;

    int r = ((c >> 11) & 31);
    int g = ((c >> 5)  & 63);
    int b = ( c        & 31);

    r = (r * s) >> 8;
    g = (g * s) >> 8;
    b = (b * s) >> 8;

    if (r > 31) r = 31;
    if (g > 63) g = 63;
    if (b > 31) b = 31;

    return (uint16_t)((r << 11) | (g << 5) | b);
}

static bool file_exists(const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (!fp)
        return false;
    fclose(fp);
    return true;
}

/* Normalize loaded meshes to a stable view-space scale for this demo camera. */
static void fit_mesh_to_unit_bounds(Mesh* mesh)
{
    if (!mesh || mesh->vertex_count <= 0)
        return;

    float minx = mesh->vertices[0].x, maxx = mesh->vertices[0].x;
    float miny = mesh->vertices[0].y, maxy = mesh->vertices[0].y;
    float minz = mesh->vertices[0].z, maxz = mesh->vertices[0].z;

    for (int i = 1; i < mesh->vertex_count; ++i) {
        const MeshVertex& v = mesh->vertices[i];
        if (v.x < minx) minx = v.x; if (v.x > maxx) maxx = v.x;
        if (v.y < miny) miny = v.y; if (v.y > maxy) maxy = v.y;
        if (v.z < minz) minz = v.z; if (v.z > maxz) maxz = v.z;
    }

    float cx = 0.5f * (minx + maxx);
    float cy = 0.5f * (miny + maxy);
    float cz = 0.5f * (minz + maxz);

    float rx = maxx - minx;
    float ry = maxy - miny;
    float rz = maxz - minz;
    float max_extent = rx;
    if (ry > max_extent) max_extent = ry;
    if (rz > max_extent) max_extent = rz;
    if (max_extent < 1.0e-6f)
        max_extent = 1.0f;

    /* Target object max dimension ~2.2 world units for this camera setup. */
    float scale = 2.2f / max_extent;

    for (int i = 0; i < mesh->vertex_count; ++i) {
        MeshVertex& v = mesh->vertices[i];
        v.x = (v.x - cx) * scale;
        v.y = (v.y - cy) * scale;
        v.z = (v.z - cz) * scale;
    }
}

/* -------------------------------------------------------------------------
 * Framebuffer and depth-buffer clear (NEON)
 * ------------------------------------------------------------------------- */

static void clear_fb(uint16_t color)
{
    uint16_t* fb     = (uint16_t*)s_platform->sc->writepage;
    int       total  = (int)(s_stride / 2) * SCREEN_H;
    uint16x8_t cv    = vdupq_n_u16(color);
    int i = 0;
    for (; i + 8 <= total; i += 8)
        vst1q_u16(fb + i, cv);
    for (; i < total; ++i)
        fb[i] = color;
}

static void clear_depth()
{
    float32x4_t one = vdupq_n_f32(1.0f);
    float*       p  = s_depth;
    int n = (SCREEN_W * SCREEN_H) / 4;
    for (int i = 0; i < n; ++i, p += 4)
        vst1q_f32(p, one);
    /* depth buffer size is SCREEN_W * SCREEN_H which is 320*240 = 76800,
     * evenly divisible by 4, so no tail needed here. */
}

/* -------------------------------------------------------------------------
 * Vertex projection
 * Projects a mesh vertex through the MVP matrix into screen space.
 * -------------------------------------------------------------------------
 * The SDK's mat4_perspective produces NDC depth in [-1, 1] (OpenGL style
 * despite the Vulkan note in the header — consistent with the teapot demo).
 * We re-map to [0, 1] with:  sz = ndc_z * 0.5 + 0.5
 * ------------------------------------------------------------------------- */

static void project_vertex(const MeshVertex* mv, const mat4_t* model,
                           const mat4_t* mvp,
                           const PointLight* lights, int light_count,
                           SV* sv)
{
    vec4_t wp4 = vec4_create(mv->x, mv->y, mv->z, 1.0f);
    wp4 = vec4_transform_mat4(wp4, *model);

    vec3_t wn = vec3_create(mv->nx, mv->ny, mv->nz);
    wn = vec3_transform_mat4_dir(wn, *model);
    wn = vec3_normalize(wn);

    /* Gouraud vertex lighting from point lights (ambient + diffuse). */
    float shade = 0.12f; /* ambient */
    vec3_t wp = vec3_create(wp4.x, wp4.y, wp4.z);
    for (int i = 0; i < light_count; ++i) {
        vec3_t lvec = vec3_sub(lights[i].position, wp);
        float dist2 = vec3_length_squared(lvec);
        float r2 = lights[i].radius * lights[i].radius;
        if (dist2 >= r2 || dist2 <= 1.0e-10f)
            continue;

        float dist = sqrtf(dist2);
        vec3_t ldir = vec3_scale(lvec, 1.0f / dist);
        float ndotl = vec3_dot(wn, ldir);
        if (ndotl <= 0.0f)
            continue;

        /* Smooth distance attenuation and soft radius cutoff. */
        float cutoff = 1.0f - dist / lights[i].radius;
        float atten  = lights[i].intensity * cutoff / (1.0f + 0.35f * dist2);
        float lum    = 0.2126f * lights[i].color.x +
                       0.7152f * lights[i].color.y +
                       0.0722f * lights[i].color.z;

        shade += ndotl * atten * lum;
    }

    vec4_t h = vec4_create(mv->x, mv->y, mv->z, 1.0f);
    h = vec4_transform_mat4(h, *mvp);

    float inv_w = 1.0f;

    if (h.w != 0.0f) {
        inv_w = 1.0f / h.w;
        h.x *= inv_w;
        h.y *= inv_w;
        h.z *= inv_w;
    }

    sv->x = (int)((h.x *  0.5f + 0.5f) * (float)SCREEN_W);
    sv->y = (int)((h.y * -0.5f + 0.5f) * (float)SCREEN_H); /* Y flip */
    sv->z =  h.z *  0.5f + 0.5f;
    sv->u_over_w = mv->u * inv_w;
    sv->v_over_w = mv->v * inv_w;
    sv->inv_w    = inv_w;
    sv->shade    = saturate(shade);
}

/* -------------------------------------------------------------------------
 * NEON-accelerated textured triangle rasterizer
 *
 * Uses the half-space (edge-function) method with 4-pixel-wide NEON passes.
 * Both winding orders are handled automatically.  No back-face culling is
 * applied — the depth buffer provides correct occlusion for any mesh.
 *
 * Texture dimensions must be powers of two.
 * ------------------------------------------------------------------------- */

template <bool UseBC1>
static void rasterize_triangle_t(
    const SV* sv0, const SV* sv1, const SV* sv2,
    const Texture* tex)
{
    int x0 = sv0->x, y0 = sv0->y;
    int x1 = sv1->x, y1 = sv1->y;
    int x2 = sv2->x, y2 = sv2->y;

    /* Signed area × 2 (cross product z-component in screen space) */
    int area2 = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
    if (area2 == 0)
        return; /* degenerate */

    /*
     * Edge-function coefficients.
     * E_i(x,y) = A_i*x + B_i*y + C_i
     *
     * A pixel is inside the triangle when E0 >= 0 && E1 >= 0 && E2 >= 0.
     * When area2 < 0 (CCW in screen-Y-down, which is the common case for a
     * right-hand Y-up mesh projected through a right-hand camera) we negate
     * all coefficients so that the standard >=0 test applies uniformly.
     */
    int A0 = y1 - y2, B0 = x2 - x1, C0 = x1*y2 - x2*y1;
    int A1 = y2 - y0, B1 = x0 - x2, C1 = x2*y0 - x0*y2;
    int A2 = y0 - y1, B2 = x1 - x0, C2 = x0*y1 - x1*y0;

    if (area2 < 0) {
        A0=-A0; B0=-B0; C0=-C0;
        A1=-A1; B1=-B1; C1=-C1;
        A2=-A2; B2=-B2; C2=-C2;
        area2 = -area2;
    }
    float inv_area = 1.0f / (float)area2;

    /* Bounding box, clamped to screen */
    int minx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int maxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int miny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int maxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);

    if (maxx < 0 || minx >= SCREEN_W || maxy < 0 || miny >= SCREEN_H)
        return;

    if (minx < 0)        minx = 0;
    if (maxx >= SCREEN_W) maxx = SCREEN_W - 1;
    if (miny < 0)        miny = 0;
    if (maxy >= SCREEN_H) maxy = SCREEN_H - 1;

    /* Perspective-correct interpolation terms (scaled by texture size). */
    float tw  = (float)tex->width;
    float th  = (float)tex->height;
    float tu0 = sv0->u_over_w * tw,  tv0 = sv0->v_over_w * th;
    float tu1 = sv1->u_over_w * tw,  tv1 = sv1->v_over_w * th;
    float tu2 = sv2->u_over_w * tw,  tv2 = sv2->v_over_w * th;
    float iq0 = sv0->inv_w;
    float iq1 = sv1->inv_w;
    float iq2 = sv2->inv_w;
    float s0  = sv0->shade;
    float s1  = sv1->shade;
    float s2  = sv2->shade;
    float z0  = sv0->z, z1 = sv1->z, z2 = sv2->z;

    uint16_t* fb16     = (uint16_t*)s_platform->sc->writepage;
    int       stride16 = (int)(s_stride / 2);
    int       tw_int   = tex->width;
    int       wmask    = tex->w_mask;
    int       hmask    = tex->h_mask;
    /* NEON constants (hoisted outside the y-loop) */
    float32x4_t vinv   = vdupq_n_f32(inv_area);
    float32x4_t vz0    = vdupq_n_f32(z0);
    float32x4_t vz1    = vdupq_n_f32(z1);
    float32x4_t vz2    = vdupq_n_f32(z2);
    float32x4_t vtu0   = vdupq_n_f32(tu0);
    float32x4_t vtu1   = vdupq_n_f32(tu1);
    float32x4_t vtu2   = vdupq_n_f32(tu2);
    float32x4_t vtv0   = vdupq_n_f32(tv0);
    float32x4_t vtv1   = vdupq_n_f32(tv1);
    float32x4_t vtv2   = vdupq_n_f32(tv2);
    float32x4_t viq0   = vdupq_n_f32(iq0);
    float32x4_t viq1   = vdupq_n_f32(iq1);
    float32x4_t viq2   = vdupq_n_f32(iq2);
    float32x4_t vs0    = vdupq_n_f32(s0);
    float32x4_t vs1    = vdupq_n_f32(s1);
    float32x4_t vs2    = vdupq_n_f32(s2);
    float32x4_t veps   = vdupq_n_f32(1.0e-8f);
    int32x4_t   v_wmask = vdupq_n_s32(wmask);
    int32x4_t   v_hmask = vdupq_n_s32(hmask);
    int32x4_t   v_tw   = vdupq_n_s32(tw_int);
    int32x4_t   vA0x4  = vdupq_n_s32(A0 * 4);
    int32x4_t   vA1x4  = vdupq_n_s32(A1 * 4);
    int32x4_t   vA2x4  = vdupq_n_s32(A2 * 4);
    int32x4_t   vzero  = vdupq_n_s32(0);

    /* Row-start edge values at y=miny, then increment per row. */
    int e0r = A0*minx + B0*miny + C0;
    int e1r = A1*minx + B1*miny + C1;
    int e2r = A2*minx + B2*miny + C2;

    for (int y = miny; y <= maxy; ++y) {

        uint16_t* row  = fb16 + y * stride16;
        float*    drow = s_depth + y * SCREEN_W;
        int       x    = minx;

        /* ----------------------------------------------------------------
         * NEON 4-pixel-wide path
         * --------------------------------------------------------------- */

        /* Initialise edge vectors for x = minx + {0,1,2,3} */
        int32_t ei0[4] = {e0r, e0r+A0, e0r+2*A0, e0r+3*A0};
        int32_t ei1[4] = {e1r, e1r+A1, e1r+2*A1, e1r+3*A1};
        int32_t ei2[4] = {e2r, e2r+A2, e2r+2*A2, e2r+3*A2};
        int32x4_t ve0  = vld1q_s32(ei0);
        int32x4_t ve1  = vld1q_s32(ei1);
        int32x4_t ve2  = vld1q_s32(ei2);

        for (; x + 3 <= maxx; x += 4) {

            /* Inside test: all edge functions >= 0 */
            uint32x4_t in0    = vcgeq_s32(ve0, vzero);
            uint32x4_t in1    = vcgeq_s32(ve1, vzero);
            uint32x4_t in2    = vcgeq_s32(ve2, vzero);
            uint32x4_t inside = vandq_u32(vandq_u32(in0, in1), in2);

            uint32_t any_in[4];
            vst1q_u32(any_in, inside);

            if (any_in[0] | any_in[1] | any_in[2] | any_in[3]) {

                /* Barycentric weights */
                float32x4_t fw0 = vmulq_f32(vcvtq_f32_s32(ve0), vinv);
                float32x4_t fw1 = vmulq_f32(vcvtq_f32_s32(ve1), vinv);
                float32x4_t fw2 = vmulq_f32(vcvtq_f32_s32(ve2), vinv);

                /* Interpolated depth */
                float32x4_t zv = vmlaq_f32(
                                     vmlaq_f32(vmulq_f32(fw0, vz0), fw1, vz1),
                                     fw2, vz2);

                /* Depth test against current depth buffer */
                float32x4_t cur_dep = vld1q_f32(drow + x);
                uint32x4_t  dpass   = vcltq_f32(zv, cur_dep);
                uint32x4_t  wmask4  = vandq_u32(inside, dpass);

                uint32_t wm[4];
                vst1q_u32(wm, wmask4);

                if (wm[0] | wm[1] | wm[2] | wm[3]) {

                    /* Interpolated perspective terms: (u/w), (v/w), (1/w). */
                    float32x4_t tuw = vmlaq_f32(
                                         vmlaq_f32(vmulq_f32(fw0, vtu0), fw1, vtu1),
                                         fw2, vtu2);
                    float32x4_t tvw = vmlaq_f32(
                                         vmlaq_f32(vmulq_f32(fw0, vtv0), fw1, vtv1),
                                         fw2, vtv2);
                    float32x4_t iqv = vmlaq_f32(
                                         vmlaq_f32(vmulq_f32(fw0, viq0), fw1, viq1),
                                         fw2, viq2);

                    iqv = vmaxq_f32(iqv, veps);

                    /* u = (u/w)/(1/w), v = (v/w)/(1/w) — single NR step
                     * (~22-bit precision, sufficient for 320×240 texturing). */
                    float32x4_t rinv = vrecpeq_f32(iqv);
                    rinv = vmulq_f32(rinv, vrecpsq_f32(iqv, rinv));

                    float32x4_t tuv = vmulq_f32(tuw, rinv);
                    float32x4_t tvv = vmulq_f32(tvw, rinv);

                    float32x4_t shv = vmlaq_f32(
                                         vmlaq_f32(vmulq_f32(fw0, vs0), fw1, vs1),
                                         fw2, vs2);

                    /* Convert float UV to integer texture indices with wrap */
                    int32x4_t itu = vandq_s32(vcvtq_s32_f32(tuv), v_wmask);
                    int32x4_t itv = vandq_s32(vcvtq_s32_f32(tvv), v_hmask);

                    /* Gather 4 texels (scalar — no ARMv7 gather instr.) */
                    uint16_t texels[4];
                    if constexpr (UseBC1) {
                        int32_t ui[4], vi[4];
                        vst1q_s32(ui, itu);
                        vst1q_s32(vi, itv);
                        texels[0] = texture_sample_rgb565(tex, ui[0], vi[0]);
                        texels[1] = texture_sample_rgb565(tex, ui[1], vi[1]);
                        texels[2] = texture_sample_rgb565(tex, ui[2], vi[2]);
                        texels[3] = texture_sample_rgb565(tex, ui[3], vi[3]);
                    } else {
                        int32x4_t tidx = vmlaq_s32(itu, itv, v_tw);
                        int32_t ti[4];
                        vst1q_s32(ti, tidx);
                        texels[0] = tex->pixels[ti[0]];
                        texels[1] = tex->pixels[ti[1]];
                        texels[2] = tex->pixels[ti[2]];
                        texels[3] = tex->pixels[ti[3]];
                    }

                    /* NEON shade modulation: shade*256 → fixed-point multiply */
                    float32x4_t s256 = vmulq_n_f32(shv, 256.0f);
                    int32x4_t   si32 = vmaxq_s32(vcvtq_s32_f32(s256), vzero);
                    si32 = vminq_s32(si32, vdupq_n_s32(512));
                    uint16x4_t vsi = vmovn_u32(vreinterpretq_u32_s32(si32));

                    uint16x4_t vtex = vld1_u16(texels);
                    uint16x4_t vr = vshr_n_u16(vtex, 11);
                    uint16x4_t vg = vand_u16(vshr_n_u16(vtex, 5), vdup_n_u16(63));
                    uint16x4_t vb = vand_u16(vtex, vdup_n_u16(31));

                    vr = vmin_u16(vshr_n_u16(vmul_u16(vr, vsi), 8), vdup_n_u16(31));
                    vg = vmin_u16(vshr_n_u16(vmul_u16(vg, vsi), 8), vdup_n_u16(63));
                    vb = vmin_u16(vshr_n_u16(vmul_u16(vb, vsi), 8), vdup_n_u16(31));

                    uint16x4_t new_color = vorr_u16(vorr_u16(vshl_n_u16(vr, 11),
                                                             vshl_n_u16(vg, 5)), vb);

                    /* Branchless masked write via NEON bit-select */
                    uint16x4_t wmask16   = vmovn_u32(wmask4);
                    uint16x4_t old_color = vld1_u16(row + x);
                    vst1_u16(row + x, vbsl_u16(wmask16, new_color, old_color));

                    uint32x4_t new_dep = vbslq_u32(wmask4,
                                                   vreinterpretq_u32_f32(zv),
                                                   vreinterpretq_u32_f32(cur_dep));
                    vst1q_f32(drow + x, vreinterpretq_f32_u32(new_dep));
                }
            }

            /* Advance edge functions by 4 pixels */
            ve0 = vaddq_s32(ve0, vA0x4);
            ve1 = vaddq_s32(ve1, vA1x4);
            ve2 = vaddq_s32(ve2, vA2x4);
        }

        /* ----------------------------------------------------------------
         * Scalar tail for the remaining (0–3) pixels
         * --------------------------------------------------------------- */
        for (; x <= maxx; ++x) {
            /* Recompute edge values from scratch for position (x, y) */
            int e0 = A0*x + B0*y + C0;
            int e1 = A1*x + B1*y + C1;
            int e2 = A2*x + B2*y + C2;

            if ((e0 | e1 | e2) >= 0) { /* same as (e0>=0 && e1>=0 && e2>=0) */
                float w0 = (float)e0 * inv_area;
                float w1 = (float)e1 * inv_area;
                float w2 = (float)e2 * inv_area;

                float z = w0*z0 + w1*z1 + w2*z2;
                if (z < drow[x]) {
                    drow[x] = z;
                    float tuw = w0*tu0 + w1*tu1 + w2*tu2;
                    float tvw = w0*tv0 + w1*tv1 + w2*tv2;
                    float iq  = w0*iq0 + w1*iq1 + w2*iq2;
                    if (iq < 1.0e-8f) iq = 1.0e-8f;
                    float rcp = 1.0f / iq;
                    float tuf = tuw * rcp;
                    float tvf = tvw * rcp;
                    float shd = w0*s0 + w1*s1 + w2*s2;
                    int ui = (int)tuf & wmask;
                    int vi = (int)tvf & hmask;
                    uint16_t texel;
                    if constexpr (UseBC1)
                        texel = texture_sample_rgb565(tex, ui, vi);
                    else
                        texel = tex->pixels[vi * tw_int + ui];
                    row[x] = modulate_rgb565(texel, shd);
                }
            }
        }

        e0r += B0;
        e1r += B1;
        e2r += B2;
    } /* end y-loop */
}

static void rasterize_triangle(
    const SV* sv0, const SV* sv1, const SV* sv2,
    const Texture* tex)
{
    if (tex->format == ETF_BC1)
        rasterize_triangle_t<true>(sv0, sv1, sv2, tex);
    else
        rasterize_triangle_t<false>(sv0, sv1, sv2, tex);
}

/* -------------------------------------------------------------------------
 * main
 * ------------------------------------------------------------------------- */

int main(int argc, char** argv)
{
    const char* default_tex_path  = "test.dds";
    const char* default_mesh_path = "test.obj";

    bool tex_from_cli  = (argc >= 2);
    bool mesh_from_cli = (argc >= 3);

    const char* tex_path  = tex_from_cli  ? argv[1] : default_tex_path;
    const char* mesh_path = mesh_from_cli ? argv[2] : default_mesh_path;

    /* --- Load (or generate) texture ------------------------------------ */
    Texture tex = {};
    bool tex_loaded = false;

    if (file_exists(tex_path)) {
        tex_loaded = texture_load_dds(tex_path, &tex);
        if (!tex_loaded) {
            fprintf(stderr, "raster: could not load '%s', using checkerboard\n",
                    tex_path);
        }
    } else if (tex_from_cli) {
        fprintf(stderr, "raster: '%s' not found, using checkerboard\n", tex_path);
    }

    if (!tex_loaded) {
        /* Two contrasting RGB565 colours: bright orange and dark teal */
        uint16_t col_a = MAKECOLORRGB16(28, 14,  2);  /* warm orange */
        uint16_t col_b = MAKECOLORRGB16( 2,  8, 18);  /* cool teal   */
        texture_create_checkerboard(&tex, 64, 64, col_a, col_b);
    }

    bool tex_valid =
        (tex.format == ETF_RGB565 && tex.pixels != nullptr) ||
        (tex.format == ETF_BC1 && tex.bc1_blocks != nullptr);

    if (!tex_valid) {
        fprintf(stderr, "raster: failed to create texture\n");
        return -1;
    }

    /* --- Load (or generate) mesh --------------------------------------- */
    Mesh mesh = {};
    bool mesh_loaded = false;

    if (file_exists(mesh_path)) {
        mesh_loaded = mesh_load_obj(mesh_path, &mesh);
        if (!mesh_loaded) {
            fprintf(stderr, "raster: could not load '%s', using built-in cube\n",
                    mesh_path);
        }
    } else if (mesh_from_cli) {
        fprintf(stderr, "raster: '%s' not found, using built-in cube\n", mesh_path);
    }

    if (!mesh_loaded)
        mesh_create_cube(&mesh);
    else
        fit_mesh_to_unit_bounds(&mesh);

    /* --- Platform init ------------------------------------------------- */
    s_platform = SPInitPlatform();
    if (!s_platform) {
        fprintf(stderr, "raster: platform init failed\n");
        return -1;
    }

    s_stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
    s_fbA.size = s_fbB.size = s_stride * SCREEN_H;
    SPAllocateBuffer(s_platform, &s_fbA);
    SPAllocateBuffer(s_platform, &s_fbB);

    VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

    s_platform->sc->cycle      = 0;
    s_platform->sc->framebufferA = &s_fbA;
    s_platform->sc->framebufferB = &s_fbB;
    VPUSwapPages(s_platform->vx, s_platform->sc);
    VPUClear(s_platform->vx, 0x00000000);
    VPUSwapPages(s_platform->vx, s_platform->sc);
    VPUClear(s_platform->vx, 0x00000000);

    VPUSetScanoutAddress(s_platform->vx,  (uint32_t)s_fbA.dmaAddress);
    VPUSetScanoutAddress2(s_platform->vx, (uint32_t)s_fbB.dmaAddress);

    /* --- Depth buffer (CPU memory, not DMA) ---------------------------- */
    s_depth = (float*)malloc((size_t)(SCREEN_W * SCREEN_H) * sizeof(float));
    if (!s_depth) {
        fprintf(stderr, "raster: out of memory for depth buffer\n");
        return -1;
    }

    /* --- Pre-project vertex buffer (reused every frame) ---------------- */
    SV* sv = (SV*)malloc((size_t)mesh.vertex_count * sizeof(SV));
    if (!sv) {
        fprintf(stderr, "raster: out of memory for screen vertices\n");
        return -1;
    }

    /* --- Camera (fixed) ------------------------------------------------ */
    mat4_t view = mat4_look_at(
        vec3_create(0.0f, 0.8f, -3.5f),   /* eye    */
        vec3_create(0.0f, 0.0f,  0.0f),   /* target */
        vec3_create(0.0f, 1.0f,  0.0f)); /* up     */

    mat4_t proj = mat4_perspective(
        65.0f * NEON_DEG_TO_RAD,
        (float)SCREEN_W / (float)SCREEN_H,
        0.1f, 100.0f);

    /* --- Main render loop ---------------------------------------------- */
    float angle_y = 0.0f;
    float angle_x = 0.0f;

    for (;;) {
        /* Scene point lights. The second light orbits to show dynamic shading. */
        PointLight lights[2];
        lights[0].position  = vec3_create( 2.2f,  2.0f, -2.0f);
        lights[0].color     = vec3_create( 1.0f,  0.96f, 0.85f);
        lights[0].intensity = 3.4f;
        lights[0].radius    = 7.5f;

        lights[1].position  = vec3_create(2.2f * cosf(angle_y * 1.7f),
                                          0.8f + 1.1f * sinf(angle_x * 1.4f),
                                          2.2f * sinf(angle_y * 1.7f));
        lights[1].color     = vec3_create(0.50f, 0.68f, 1.00f);
        lights[1].intensity = 2.4f;
        lights[1].radius    = 6.0f;

        /* Build model matrix: gentle tumble */
        mat4_t model = mat4_mul(mat4_rotation_y(angle_y),
                                mat4_rotation_x(angle_x));
        mat4_t mvp   = mat4_mul(proj, mat4_mul(view, model));

        /* Project all mesh vertices */
        for (int i = 0; i < mesh.vertex_count; ++i)
            project_vertex(&mesh.vertices[i], &model, &mvp, lights, 2, &sv[i]);

        /* Clear framebuffer and depth buffer */
        clear_fb(BG_COLOR);
        clear_depth();

        /* Rasterize all triangles (cheap pre-cull before entering hot rasterizer). */
        const MeshTriangle* tris = mesh.triangles;
        int tri_count = mesh.triangle_count;
        for (int t = 0; t < tri_count; ++t) {
            const MeshTriangle* tri = &tris[t];
            const SV* a = &sv[tri->v[0]];
            const SV* b = &sv[tri->v[1]];
            const SV* c = &sv[tri->v[2]];

            /*
             * Coarse rejection to prevent giant projected triangles when geometry
             * is behind or crossing the camera with no clipping.
             */
            if (a->inv_w <= 0.0f || b->inv_w <= 0.0f || c->inv_w <= 0.0f)
                continue;
            if ((a->z <= 0.0f && b->z <= 0.0f && c->z <= 0.0f) ||
                (a->z >= 1.0f && b->z >= 1.0f && c->z >= 1.0f))
                continue;

            /* Degenerate or fully offscreen: skip function-call and setup cost. */
            int area2 = (b->x - a->x) * (c->y - a->y) - (c->x - a->x) * (b->y - a->y);
            if (area2 == 0)
                continue;

            int minx = a->x < b->x ? (a->x < c->x ? a->x : c->x) : (b->x < c->x ? b->x : c->x);
            int maxx = a->x > b->x ? (a->x > c->x ? a->x : c->x) : (b->x > c->x ? b->x : c->x);
            int miny = a->y < b->y ? (a->y < c->y ? a->y : c->y) : (b->y < c->y ? b->y : c->y);
            int maxy = a->y > b->y ? (a->y > c->y ? a->y : c->y) : (b->y > c->y ? b->y : c->y);
            if (maxx < 0 || minx >= SCREEN_W || maxy < 0 || miny >= SCREEN_H)
                continue;

            rasterize_triangle(a, b, c, &tex);
        }

        /* Wait for vsync, then swap display/render pages */
        VPUWaitVSync(s_platform->vx);
        VPUSwapPages(s_platform->vx, s_platform->sc);

        /* Advance rotation angles (degrees per frame at ~60 Hz) */
        angle_y += 0.058f; /* ~1.03 deg/frame */
        angle_x += 0.027f; /* ~0.40 deg/frame */
        if (angle_y > NEON_TWO_PI) angle_y -= NEON_TWO_PI;
        if (angle_x > NEON_TWO_PI) angle_x -= NEON_TWO_PI;
    }

    /* (unreachable — here for completeness) */
    free(sv);
    free(s_depth);
    mesh_free(&mesh);
    texture_free(&tex);
    return 0;
}
