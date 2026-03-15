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
 * Pipeline overview
 * -----------------
 * The rendering is split into three stages that mirror a hardware GPU
 * pipeline and run across the Cortex-A9's two cores:
 *
 *   Stage 1 — Vertex shader (main thread)
 *     Projects every mesh vertex through the MVP matrix, evaluates
 *     Gouraud lighting, and writes an array of screen-space SV records.
 *
 *   Stage 2 — Primitive assembly / triangle setup (main thread)
 *     For each triangle: computes edge-function coefficients (A,B,C),
 *     pre-scales perspective-correct UV/depth attributes, and pushes a
 *     TriSetup record into a SPSC ring buffer.
 *     Degenerate and fully off-screen triangles are culled here.
 *
 *   Stage 3 — Fragment shader / rasterizer (worker thread, core 1)
 *     Drains the ring buffer and executes the NEON 4-pixel-wide
 *     half-space fill loop for each TriSetup record:
 *       • Edge-function inside-test      — integer 4-wide compare
 *       • Barycentric weight computation — float 4-wide multiply/accumulate
 *       • Depth test                     — float 4-wide compare/write
 *       • Perspective-correct UV         — NR reciprocal, integer UV wrap
 *       • Texture gather                 — 4 scalar loads (no ARMv7 gather)
 *       • Shade + pixel store            — NEON bsl masked write
 *
 * Synchronisation
 * ---------------
 * A lock-free SPSC ring buffer (power-of-two slots, two atomic indices)
 * connects stages 2 and 3.  The main thread signals end-of-frame by
 * pushing a sentinel TriSetup (is_sentinel = true) and then waiting on
 * a semaphore that the worker posts once it has drained the sentinel.
 * All memory used by each stage (framebuffer, depth buffer, vertex buffer)
 * is disjoint so no other locking is needed.
 */

#include "mesh.h"
#include "texture.h"
#include "core.h"
#include "vpu.h"
#include "vec.h"

#include <arm_neon.h>
#include <pthread.h>
#include <semaphore.h>
#include <atomic>
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
 * Stage 2→3 SPSC ring buffer
 *
 * TriSetup holds everything the fragment stage needs for one triangle.
 * The ring capacity must be a power of two; 512 is ample for a single mesh
 * rendered at 60 Hz (typical scene << 512 triangles per frame).
 * ------------------------------------------------------------------------- */

#define RING_CAPACITY  512u  /* must be power of two */
#define RING_MASK      (RING_CAPACITY - 1u)

/* Per-triangle data produced by primitive assembly (stage 2)
 * and consumed by the rasterizer (stage 3). */
struct TriSetup {
    /* Edge-function coefficients (already sign-correct for CCW fill) */
    int A0, B0, C0;
    int A1, B1, C1;
    int A2, B2, C2;
    float inv_area;

    /* Screen-space bounding box (clamped to screen) */
    int minx, maxx, miny, maxy;

    /* Perspective-correct vertex attributes (differential form):
     *   attrib = base + fw1*(d1) + fw2*(d2)
     * where fw1 = e1*inv_area, fw2 = e2*inv_area. */
    float z0, dz1, dz2;         /* depth */
    float tu0, dtu1, dtu2;      /* u/w * tex_width  */
    float tv0, dtv1, dtv2;      /* v/w * tex_height */
    float iq0, diq1, diq2;      /* 1/w              */
    float s0,  ds1,  ds2;       /* shade            */

    /* Texture parameters */
    int   tw_int;
    int   wmask, hmask;
    bool  use_bc1;

    /* Sentinel: when true the worker thread ends the frame. */
    bool  is_sentinel;
};

/* SPSC ring buffer — one producer (main), one consumer (worker). */
struct TriRing {
    TriSetup   slots[RING_CAPACITY];
    std::atomic<uint32_t> head;  /* written by producer */
    std::atomic<uint32_t> tail;  /* written by consumer */
};

/* -------------------------------------------------------------------------
 * Worker-thread context
 * ------------------------------------------------------------------------- */

struct WorkerCtx {
    TriRing*      ring;
    const Texture* tex;   /* updated each frame before sentinel */
    sem_t         frame_done; /* worker posts when sentinel is processed */
    pthread_t     thread;
}

;

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
 * Stage 3 — Fragment shader
 *
 * Executes one TriSetup record: NEON 4-pixel-wide inner loop + scalar tail.
 * All context comes from the TriSetup; no global read except the framebuffer
 * and depth-buffer pointers (written only by stage 3).
 * ------------------------------------------------------------------------- */

static void fragment_stage(const TriSetup* ts, const Texture* tex)
{
    uint16_t* fb16     = (uint16_t*)s_platform->sc->writepage;
    int       stride16 = (int)(s_stride / 2);

    /* NEON constants — hoisted outside the y-loop */
    float32x4_t vinv   = vdupq_n_f32(ts->inv_area);
    float32x4_t vz0    = vdupq_n_f32(ts->z0);
    float32x4_t vdz1   = vdupq_n_f32(ts->dz1);
    float32x4_t vdz2   = vdupq_n_f32(ts->dz2);
    float32x4_t vtu0   = vdupq_n_f32(ts->tu0);
    float32x4_t vdtu1  = vdupq_n_f32(ts->dtu1);
    float32x4_t vdtu2  = vdupq_n_f32(ts->dtu2);
    float32x4_t vtv0   = vdupq_n_f32(ts->tv0);
    float32x4_t vdtv1  = vdupq_n_f32(ts->dtv1);
    float32x4_t vdtv2  = vdupq_n_f32(ts->dtv2);
    float32x4_t viq0   = vdupq_n_f32(ts->iq0);
    float32x4_t vdiq1  = vdupq_n_f32(ts->diq1);
    float32x4_t vdiq2  = vdupq_n_f32(ts->diq2);
    float32x4_t vs0    = vdupq_n_f32(ts->s0);
    float32x4_t vds1   = vdupq_n_f32(ts->ds1);
    float32x4_t vds2   = vdupq_n_f32(ts->ds2);
    float32x4_t veps   = vdupq_n_f32(1.0e-8f);
    int32x4_t   v_wmask = vdupq_n_s32(ts->wmask);
    int32x4_t   v_hmask = vdupq_n_s32(ts->hmask);
    int32x4_t   v_tw   = vdupq_n_s32(ts->tw_int);
    int32x4_t   vA0x4  = vdupq_n_s32(ts->A0 * 4);
    int32x4_t   vA1x4  = vdupq_n_s32(ts->A1 * 4);
    int32x4_t   vA2x4  = vdupq_n_s32(ts->A2 * 4);
    int32x4_t   vzero  = vdupq_n_s32(0);
    uint16x4_t  v_mask31 = vdup_n_u16(31);
    uint16x4_t  v_mask63 = vdup_n_u16(63);
    static const int32_t k0123[4] = {0, 1, 2, 3};
    int32x4_t   vi0123 = vld1q_s32(k0123);

    const int A0 = ts->A0, A1 = ts->A1, A2 = ts->A2;
    const int B0 = ts->B0, B1 = ts->B1, B2 = ts->B2;
    const int C0 = ts->C0, C1 = ts->C1, C2 = ts->C2;
    const float inv_area = ts->inv_area;
    const float z0=ts->z0, z1=ts->z0+ts->dz1, z2=ts->z0+ts->dz2;
    const float tu0=ts->tu0, tu1=ts->tu0+ts->dtu1, tu2=ts->tu0+ts->dtu2;
    const float tv0=ts->tv0, tv1=ts->tv0+ts->dtv1, tv2=ts->tv0+ts->dtv2;
    const float iq0=ts->iq0, iq1=ts->iq0+ts->diq1, iq2=ts->iq0+ts->diq2;
    const float s0=ts->s0,   s1=ts->s0+ts->ds1,    s2=ts->s0+ts->ds2;
    const int   wmask=ts->wmask, hmask=ts->hmask, tw_int=ts->tw_int;
    (void)z1; (void)z2; (void)tu1; (void)tu2; (void)tv1; (void)tv2;
    (void)iq1; (void)iq2; (void)s1; (void)s2;

    /* Row-start edge values */
    int e0r = A0*ts->minx + B0*ts->miny + C0;
    int e1r = A1*ts->minx + B1*ts->miny + C1;
    int e2r = A2*ts->minx + B2*ts->miny + C2;

    uint16_t* row  = fb16    + ts->miny * stride16;
    float*    drow = s_depth + ts->miny * SCREEN_W;

    for (int y = ts->miny; y <= ts->maxy; ++y) {

        int x = ts->minx;

        /* Initialise NEON edge vectors for x = minx + {0,1,2,3} */
        int32x4_t ve0 = vmlaq_n_s32(vdupq_n_s32(e0r), vi0123, A0);
        int32x4_t ve1 = vmlaq_n_s32(vdupq_n_s32(e1r), vi0123, A1);
        int32x4_t ve2 = vmlaq_n_s32(vdupq_n_s32(e2r), vi0123, A2);

        /* ----------------------------------------------------------------
         * NEON 4-pixel-wide path
         * --------------------------------------------------------------- */
        for (; x + 3 <= ts->maxx; x += 4) {

            uint32x4_t in0    = vcgeq_s32(ve0, vzero);
            uint32x4_t in1    = vcgeq_s32(ve1, vzero);
            uint32x4_t in2    = vcgeq_s32(ve2, vzero);
            uint32x4_t inside = vandq_u32(vandq_u32(in0, in1), in2);

            uint32x2_t ai_fold = vorr_u32(vget_low_u32(inside), vget_high_u32(inside));
            if (vget_lane_u32(ai_fold, 0) | vget_lane_u32(ai_fold, 1)) {

                float32x4_t fw1 = vmulq_f32(vcvtq_f32_s32(ve1), vinv);
                float32x4_t fw2 = vmulq_f32(vcvtq_f32_s32(ve2), vinv);

                float32x4_t zv      = vmlaq_f32(vmlaq_f32(vz0, fw1, vdz1), fw2, vdz2);
                float32x4_t cur_dep = vld1q_f32(drow + x);
                uint32x4_t  dpass   = vcltq_f32(zv, cur_dep);
                uint32x4_t  wmask4  = vandq_u32(inside, dpass);

                uint32x2_t wm_fold = vorr_u32(vget_low_u32(wmask4), vget_high_u32(wmask4));
                if (vget_lane_u32(wm_fold, 0) | vget_lane_u32(wm_fold, 1)) {

                    float32x4_t tuw = vmlaq_f32(vmlaq_f32(vtu0, fw1, vdtu1), fw2, vdtu2);
                    float32x4_t tvw = vmlaq_f32(vmlaq_f32(vtv0, fw1, vdtv1), fw2, vdtv2);
                    float32x4_t iqv = vmlaq_f32(vmlaq_f32(viq0, fw1, vdiq1), fw2, vdiq2);

                    iqv = vmaxq_f32(iqv, veps);
                    float32x4_t rinv = vrecpeq_f32(iqv);
                    rinv = vmulq_f32(rinv, vrecpsq_f32(iqv, rinv));

                    float32x4_t tuv = vmulq_f32(tuw, rinv);
                    float32x4_t tvv = vmulq_f32(tvw, rinv);
                    float32x4_t shv = vmlaq_f32(vmlaq_f32(vs0, fw1, vds1), fw2, vds2);

                    int32x4_t itu = vandq_s32(vcvtq_s32_f32(tuv), v_wmask);
                    int32x4_t itv = vandq_s32(vcvtq_s32_f32(tvv), v_hmask);

                    uint16_t texels[4];
                    if (ts->use_bc1) {
                        int32_t ui[4], vi[4];
                        vst1q_s32(ui, itu);
                        vst1q_s32(vi, itv);
                        texels[0] = texture_sample_bc1_direct(tex, ui[0], vi[0]);
                        texels[1] = texture_sample_bc1_direct(tex, ui[1], vi[1]);
                        texels[2] = texture_sample_bc1_direct(tex, ui[2], vi[2]);
                        texels[3] = texture_sample_bc1_direct(tex, ui[3], vi[3]);
                    } else {
                        int32x4_t tidx = vmlaq_s32(itu, itv, v_tw);
                        int32_t ti[4];
                        vst1q_s32(ti, tidx);
                        texels[0] = tex->pixels[ti[0]];
                        texels[1] = tex->pixels[ti[1]];
                        texels[2] = tex->pixels[ti[2]];
                        texels[3] = tex->pixels[ti[3]];
                    }

                    /* shade in [0,1] → s256 in [0,256]; no channel clamp needed */
                    uint16x4_t vsi = vmovn_u32(vcvtq_u32_f32(vmulq_n_f32(shv, 256.0f)));

                    uint16x4_t vtex = vld1_u16(texels);
                    uint16x4_t vr = vshr_n_u16(vtex, 11);
                    uint16x4_t vg = vand_u16(vshr_n_u16(vtex, 5), v_mask63);
                    uint16x4_t vb = vand_u16(vtex, v_mask31);

                    vr = vshr_n_u16(vmul_u16(vr, vsi), 8);
                    vg = vshr_n_u16(vmul_u16(vg, vsi), 8);
                    vb = vshr_n_u16(vmul_u16(vb, vsi), 8);

                    uint16x4_t new_color = vorr_u16(vorr_u16(vshl_n_u16(vr, 11),
                                                             vshl_n_u16(vg, 5)), vb);

                    uint16x4_t wmask16   = vmovn_u32(wmask4);
                    uint16x4_t old_color = vld1_u16(row + x);
                    vst1_u16(row + x, vbsl_u16(wmask16, new_color, old_color));

                    uint32x4_t new_dep = vbslq_u32(wmask4,
                                                   vreinterpretq_u32_f32(zv),
                                                   vreinterpretq_u32_f32(cur_dep));
                    vst1q_f32(drow + x, vreinterpretq_f32_u32(new_dep));
                }
            }

            ve0 = vaddq_s32(ve0, vA0x4);
            ve1 = vaddq_s32(ve1, vA1x4);
            ve2 = vaddq_s32(ve2, vA2x4);
        }

        /* ----------------------------------------------------------------
         * Scalar tail — remaining 0–3 pixels on the right edge
         * --------------------------------------------------------------- */
        for (; x <= ts->maxx; ++x) {
            int e0 = A0*x + B0*y + C0;
            int e1 = A1*x + B1*y + C1;
            int e2 = A2*x + B2*y + C2;

            if ((e0 | e1 | e2) >= 0) {
                float w0 = (float)e0 * inv_area;
                float w1 = (float)e1 * inv_area;
                float w2 = (float)e2 * inv_area;

                float z = w0*ts->z0 + w1*(ts->z0+ts->dz1) + w2*(ts->z0+ts->dz2);
                if (z < drow[x]) {
                    drow[x] = z;
                    float tuw = w0*tu0 + w1*(tu0+ts->dtu1) + w2*(tu0+ts->dtu2);
                    float tvw = w0*tv0 + w1*(tv0+ts->dtv1) + w2*(tv0+ts->dtv2);
                    float iq  = w0*iq0 + w1*(iq0+ts->diq1) + w2*(iq0+ts->diq2);
                    if (iq < 1.0e-8f) iq = 1.0e-8f;
                    float rcp = 1.0f / iq;
                    float tuf = tuw * rcp;
                    float tvf = tvw * rcp;
                    float shd = w0*s0 + w1*(s0+ts->ds1) + w2*(s0+ts->ds2);
                    int ui = (int)tuf & wmask;
                    int vi = (int)tvf & hmask;
                    uint16_t texel;
                    if (ts->use_bc1)
                        texel = texture_sample_bc1_direct(tex, ui, vi);
                    else
                        texel = tex->pixels[vi * tw_int + ui];
                    row[x] = modulate_rgb565(texel, shd);
                }
            }
        }

        e0r += B0;
        e1r += B1;
        e2r += B2;
        row  += stride16;
        drow += SCREEN_W;
    }
}

/* -------------------------------------------------------------------------
 * Worker thread — drains the ring buffer and runs the fragment stage
 * ------------------------------------------------------------------------- */

static void* raster_worker(void* arg)
{
    WorkerCtx* ctx = (WorkerCtx*)arg;
    TriRing*   ring = ctx->ring;

    for (;;) {
        /* Spin-wait for a slot (SPSC — no mutex needed) */
        uint32_t tail;
        do {
            tail = ring->tail.load(std::memory_order_relaxed);
        } while (ring->head.load(std::memory_order_acquire) == tail);

        const TriSetup* ts = &ring->slots[tail & RING_MASK];

        if (ts->is_sentinel) {
            /* Advance tail past sentinel, signal main thread, then loop */
            ring->tail.store(tail + 1u, std::memory_order_release);
            sem_post(&ctx->frame_done);
            continue;
        }

        fragment_stage(ts, ctx->tex);

        ring->tail.store(tail + 1u, std::memory_order_release);
    }
    return nullptr;
}

/* -------------------------------------------------------------------------
 * Stage 2 — Primitive assembly + triangle setup
 *
 * Computes edge-function coefficients and attribute differentials for one
 * triangle and pushes a TriSetup into the ring buffer.  Culls degenerate
 * and fully off-screen triangles.
 * ------------------------------------------------------------------------- */

static void assemble_triangle(TriRing* ring,
                               const SV* sv0, const SV* sv1, const SV* sv2,
                               const Texture* tex)
{
    int x0 = sv0->x, y0 = sv0->y;
    int x1 = sv1->x, y1 = sv1->y;
    int x2 = sv2->x, y2 = sv2->y;

    int area2 = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
    if (area2 == 0)
        return;

    int A0 = y1 - y2, B0 = x2 - x1, C0 = x1*y2 - x2*y1;
    int A1 = y2 - y0, B1 = x0 - x2, C1 = x2*y0 - x0*y2;
    int A2 = y0 - y1, B2 = x1 - x0, C2 = x0*y1 - x1*y0;

    if (area2 < 0) {
        A0=-A0; B0=-B0; C0=-C0;
        A1=-A1; B1=-B1; C1=-C1;
        A2=-A2; B2=-B2; C2=-C2;
        area2 = -area2;
    }

    int minx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int maxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int miny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int maxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);

    if (maxx < 0 || minx >= SCREEN_W || maxy < 0 || miny >= SCREEN_H)
        return;

    if (minx < 0)         minx = 0;
    if (maxx >= SCREEN_W) maxx = SCREEN_W - 1;
    if (miny < 0)         miny = 0;
    if (maxy >= SCREEN_H) maxy = SCREEN_H - 1;

    float inv_area = 1.0f / (float)area2;
    float tw = (float)tex->width;
    float th = (float)tex->height;

    float tu0 = sv0->u_over_w * tw;
    float tu1 = sv1->u_over_w * tw;
    float tu2 = sv2->u_over_w * tw;
    float tv0 = sv0->v_over_w * th;
    float tv1 = sv1->v_over_w * th;
    float tv2 = sv2->v_over_w * th;

    /* Spin-wait for a free slot (ring is not full under normal operation) */
    uint32_t head;
    TriRing* r = ring;
    do {
        head = r->head.load(std::memory_order_relaxed);
    } while ((head - r->tail.load(std::memory_order_acquire)) >= RING_CAPACITY);

    TriSetup* ts = &r->slots[head & RING_MASK];

    ts->A0=A0; ts->B0=B0; ts->C0=C0;
    ts->A1=A1; ts->B1=B1; ts->C1=C1;
    ts->A2=A2; ts->B2=B2; ts->C2=C2;
    ts->inv_area = inv_area;
    ts->minx=minx; ts->maxx=maxx; ts->miny=miny; ts->maxy=maxy;

    /* Differential form: base = sv0 value, d1 = sv1-sv0, d2 = sv2-sv0 */
    ts->z0=sv0->z;   ts->dz1=sv1->z-sv0->z;     ts->dz2=sv2->z-sv0->z;
    ts->tu0=tu0;     ts->dtu1=tu1-tu0;           ts->dtu2=tu2-tu0;
    ts->tv0=tv0;     ts->dtv1=tv1-tv0;           ts->dtv2=tv2-tv0;
    ts->iq0=sv0->inv_w; ts->diq1=sv1->inv_w-sv0->inv_w; ts->diq2=sv2->inv_w-sv0->inv_w;
    ts->s0=sv0->shade;  ts->ds1=sv1->shade-sv0->shade;   ts->ds2=sv2->shade-sv0->shade;

    ts->tw_int  = tex->width;
    ts->wmask   = tex->w_mask;
    ts->hmask   = tex->h_mask;
    ts->use_bc1 = (tex->format == ETF_BC1);
    ts->is_sentinel = false;

    r->head.store(head + 1u, std::memory_order_release);
}

/* Push the end-of-frame sentinel and block until the worker drains it. */
static void flush_pipeline(TriRing* ring, WorkerCtx* ctx)
{
    uint32_t head;
    do {
        head = ring->head.load(std::memory_order_relaxed);
    } while ((head - ring->tail.load(std::memory_order_acquire)) >= RING_CAPACITY);

    TriSetup* ts = &ring->slots[head & RING_MASK];
    memset(ts, 0, sizeof(*ts));
    ts->is_sentinel = true;

    ring->head.store(head + 1u, std::memory_order_release);
    sem_wait(&ctx->frame_done);
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
        uint16_t col_a = MAKECOLORRGB16(28, 14,  2);
        uint16_t col_b = MAKECOLORRGB16( 2,  8, 18);
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

    s_platform->sc->cycle        = 0;
    s_platform->sc->framebufferA = &s_fbA;
    s_platform->sc->framebufferB = &s_fbB;
    VPUSwapPages(s_platform->vx, s_platform->sc);
    VPUClear(s_platform->vx, 0x00000000);
    VPUSwapPages(s_platform->vx, s_platform->sc);
    VPUClear(s_platform->vx, 0x00000000);

    VPUSetScanoutAddress(s_platform->vx,  (uint32_t)s_fbA.dmaAddress);
    VPUSetScanoutAddress2(s_platform->vx, (uint32_t)s_fbB.dmaAddress);

    /* --- Depth buffer -------------------------------------------------- */
    s_depth = (float*)malloc((size_t)(SCREEN_W * SCREEN_H) * sizeof(float));
    if (!s_depth) {
        fprintf(stderr, "raster: out of memory for depth buffer\n");
        return -1;
    }

    /* --- Pre-project vertex buffer ------------------------------------- */
    SV* sv = (SV*)malloc((size_t)mesh.vertex_count * sizeof(SV));
    if (!sv) {
        fprintf(stderr, "raster: out of memory for screen vertices\n");
        return -1;
    }

    /* --- Pipeline ring buffer ------------------------------------------ */
    TriRing* ring = (TriRing*)malloc(sizeof(TriRing));
    if (!ring) {
        fprintf(stderr, "raster: out of memory for ring buffer\n");
        return -1;
    }
    ring->head.store(0u);
    ring->tail.store(0u);

    /* --- Worker context and thread ------------------------------------- */
    WorkerCtx wctx;
    wctx.ring = ring;
    wctx.tex  = &tex;
    sem_init(&wctx.frame_done, 0, 0);
    pthread_create(&wctx.thread, nullptr, raster_worker, &wctx);

    /* --- Camera (fixed) ------------------------------------------------ */
    mat4_t view = mat4_look_at(
        vec3_create(0.0f, 0.8f, -3.5f),
        vec3_create(0.0f, 0.0f,  0.0f),
        vec3_create(0.0f, 1.0f,  0.0f));

    mat4_t proj = mat4_perspective(
        65.0f * NEON_DEG_TO_RAD,
        (float)SCREEN_W / (float)SCREEN_H,
        0.1f, 100.0f);

    /* --- Main render loop ---------------------------------------------- */
    float angle_y = 0.0f;
    float angle_x = 0.0f;

    for (;;) {
        /* Scene point lights */
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

        mat4_t model = mat4_mul(mat4_rotation_y(angle_y),
                                mat4_rotation_x(angle_x));
        mat4_t mvp   = mat4_mul(proj, mat4_mul(view, model));

        /* ---- Stage 1: Vertex shader ----------------------------------- */
        for (int i = 0; i < mesh.vertex_count; ++i)
            project_vertex(&mesh.vertices[i], &model, &mvp, lights, 2, &sv[i]);

        /* Clear framebuffer and depth buffer before feeding rasterizer */
        clear_fb(BG_COLOR);
        clear_depth();

        /* ---- Stage 2: Primitive assembly ------------------------------ */
        /* Update the worker's texture pointer before pushing any triangles */
        wctx.tex = &tex;
        const MeshTriangle* tris = mesh.triangles;
        int tri_count = mesh.triangle_count;
        for (int t = 0; t < tri_count; ++t) {
            const MeshTriangle* tri = &tris[t];
            assemble_triangle(ring,
                              &sv[tri->v[0]],
                              &sv[tri->v[1]],
                              &sv[tri->v[2]],
                              &tex);
        }

        /* ---- Stage 2→3 sync: push sentinel, wait for worker ----------- */
        flush_pipeline(ring, &wctx);

        /* Wait for vsync, then swap display/render pages */
        VPUWaitVSync(s_platform->vx);
        VPUSwapPages(s_platform->vx, s_platform->sc);

        angle_y += 0.058f;
        angle_x += 0.027f;
        if (angle_y > NEON_TWO_PI) angle_y -= NEON_TWO_PI;
        if (angle_x > NEON_TWO_PI) angle_x -= NEON_TWO_PI;
    }

    /* (unreachable) */
    free(sv);
    free(s_depth);
    free(ring);
    mesh_free(&mesh);
    texture_free(&tex);
    sem_destroy(&wctx.frame_done);
    return 0;
}
