#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "platform.h"
#include "vec.h"
#include "core.h"
#include "vpu.h"

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define USE_NEON 1
#else
#define USE_NEON 0
#endif

#include "utah_teapot_bezier.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PATCH_SUBDIV 8 // Number of subdivisions per patch side
#define DEPTH_FAR 1.0f

struct SPPlatform* s_platform;
struct SPSizeAlloc framebufferA;
struct SPSizeAlloc framebufferB;
static uint32_t strideInBytes;

// Depth buffer (one float per pixel)
static float *depth_buffer = NULL;

typedef struct {
    float x, y, z;
    float nx, ny, nz; // normal
} Vertex;

#if USE_NEON

// Compute Bernstein basis functions for cubic Bezier
static inline float32x4_t compute_bernstein_basis(float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    float mt = 1.0f - t;
    float mt2 = mt * mt;
    float mt3 = mt2 * mt;
    
    float32x4_t basis = {mt3, 3.0f * t * mt2, 3.0f * t2 * mt, t3};
    return basis;
}

// Compute derivative of Bernstein basis functions
static inline float32x4_t compute_bernstein_deriv(float t)
{
    float t2 = t * t;
    float mt = 1.0f - t;
    float mt2 = mt * mt;
    
    float32x4_t deriv = {
        -3.0f * mt2,
        3.0f * mt2 - 6.0f * t * mt,
        6.0f * t * mt - 3.0f * t2,
        3.0f * t2
    };
    return deriv;
}

// NEON-optimized Bezier surface evaluation
// Evaluates position, du, and dv in one pass
static void bezier_eval_neon(const float cp[4][4][3], float u, float v, 
                              float *pos, float *du, float *dv)
{
    float32x4_t bu = compute_bernstein_basis(u);
    float32x4_t bv = compute_bernstein_basis(v);
    float32x4_t dbu = compute_bernstein_deriv(u);
    float32x4_t dbv = compute_bernstein_deriv(v);
    
    // Extract basis values for easier access
    float bu_arr[4], bv_arr[4], dbu_arr[4], dbv_arr[4];
    vst1q_f32(bu_arr, bu);
    vst1q_f32(bv_arr, bv);
    vst1q_f32(dbu_arr, dbu);
    vst1q_f32(dbv_arr, dbv);
    
    // Accumulate results for x, y, z
    float32x4_t pos_acc = vdupq_n_f32(0.0f);  // x, y, z, unused
    float32x4_t du_acc = vdupq_n_f32(0.0f);
    float32x4_t dv_acc = vdupq_n_f32(0.0f);
    
    for (int i = 0; i < 4; ++i) {
        float bu_i = bu_arr[i];
        float dbu_i = dbu_arr[i];
        
        for (int j = 0; j < 4; ++j) {
            float bv_j = bv_arr[j];
            float dbv_j = dbv_arr[j];
            
            // Load control point (x, y, z)
            float32x4_t cp_vec = {cp[i][j][0], cp[i][j][1], cp[i][j][2], 0.0f};
            
            // Position: sum(cp * bu * bv)
            float weight = bu_i * bv_j;
            pos_acc = vmlaq_n_f32(pos_acc, cp_vec, weight);
            
            // du: sum(cp * dbu * bv)
            float du_weight = dbu_i * bv_j;
            du_acc = vmlaq_n_f32(du_acc, cp_vec, du_weight);
            
            // dv: sum(cp * bu * dbv)
            float dv_weight = bu_i * dbv_j;
            dv_acc = vmlaq_n_f32(dv_acc, cp_vec, dv_weight);
        }
    }
    
    // Store results
    float pos_tmp[4], du_tmp[4], dv_tmp[4];
    vst1q_f32(pos_tmp, pos_acc);
    vst1q_f32(du_tmp, du_acc);
    vst1q_f32(dv_tmp, dv_acc);
    
    pos[0] = pos_tmp[0]; pos[1] = pos_tmp[1]; pos[2] = pos_tmp[2];
    du[0] = du_tmp[0]; du[1] = du_tmp[1]; du[2] = du_tmp[2];
    dv[0] = dv_tmp[0]; dv[1] = dv_tmp[1]; dv[2] = dv_tmp[2];
}

// NEON-optimized normalize
static inline void normalize_neon(float *v)
{
    float32x4_t vec = {v[0], v[1], v[2], 0.0f};
    float32x4_t sq = vmulq_f32(vec, vec);
    
    // Horizontal add: x^2 + y^2 + z^2
    float len_sq = vgetq_lane_f32(sq, 0) + vgetq_lane_f32(sq, 1) + vgetq_lane_f32(sq, 2);
    
    if (len_sq > 1e-12f) {
        // Use NEON reciprocal square root estimate with Newton-Raphson refinement
        float32x2_t len_sq_v = vdup_n_f32(len_sq);
        float32x2_t rsqrt = vrsqrte_f32(len_sq_v);
        // One Newton-Raphson iteration for better precision
        rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(len_sq_v, rsqrt), rsqrt));
        float inv_len = vget_lane_f32(rsqrt, 0);
        
        vec = vmulq_n_f32(vec, inv_len);
        v[0] = vgetq_lane_f32(vec, 0);
        v[1] = vgetq_lane_f32(vec, 1);
        v[2] = vgetq_lane_f32(vec, 2);
    }
}

// NEON-optimized cross product
static inline void cross_product_neon(const float *a, const float *b, float *result)
{
    // Cross product: (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

static void subdivide_patch(const float cp[4][4][3], Vertex *verts, int subdiv)
{
    float inv_subdiv = 1.0f / (float)subdiv;
    
    for (int i = 0; i <= subdiv; ++i) {
        float u = (float)i * inv_subdiv;
        
        for (int j = 0; j <= subdiv; ++j) {
            float v = (float)j * inv_subdiv;
            float pos[3], du[3], dv[3], n[3];
            
            // Evaluate position and derivatives in one call
            bezier_eval_neon(cp, u, v, pos, du, dv);
            
            // Normal = cross(du, dv)
            cross_product_neon(du, dv, n);
            normalize_neon(n);
            
            int idx = i * (subdiv + 1) + j;
            verts[idx].x = pos[0];
            verts[idx].y = pos[1];
            verts[idx].z = pos[2];
            verts[idx].nx = n[0];
            verts[idx].ny = n[1];
            verts[idx].nz = n[2];
        }
    }
}

#else
// Scalar fallback implementations

// Evaluate a point on a Bezier surface patch
static void bezier_eval(const float cp[4][4][3], float u, float v, float *out)
{
    float bu[4], bv[4];
    float mt = 1.0f - u, mt2 = mt * mt, mt3 = mt2 * mt;
    float t2 = u * u, t3 = t2 * u;
    bu[0] = mt3;
    bu[1] = 3.0f * u * mt2;
    bu[2] = 3.0f * t2 * mt;
    bu[3] = t3;
    
    mt = 1.0f - v; mt2 = mt * mt; mt3 = mt2 * mt;
    t2 = v * v; t3 = t2 * v;
    bv[0] = mt3;
    bv[1] = 3.0f * v * mt2;
    bv[2] = 3.0f * t2 * mt;
    bv[3] = t3;
    
    out[0] = out[1] = out[2] = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float w = bu[i] * bv[j];
            out[0] += cp[i][j][0] * w;
            out[1] += cp[i][j][1] * w;
            out[2] += cp[i][j][2] * w;
        }
    }
}

// Compute partial derivatives for normal calculation
static void bezier_deriv(const float cp[4][4][3], float u, float v, float *du, float *dv)
{
    float bu[4], dbu[4], bv[4], dbv[4];
    float mt = 1.0f - u, mt2 = mt * mt;
    float t2 = u * u;
    bu[0] = mt2 * mt;
    bu[1] = 3.0f * u * mt2;
    bu[2] = 3.0f * t2 * mt;
    bu[3] = t2 * u;
    dbu[0] = -3.0f * mt2;
    dbu[1] = 3.0f * mt2 - 6.0f * u * mt;
    dbu[2] = 6.0f * u * mt - 3.0f * t2;
    dbu[3] = 3.0f * t2;
    
    mt = 1.0f - v; mt2 = mt * mt;
    t2 = v * v;
    bv[0] = mt2 * mt;
    bv[1] = 3.0f * v * mt2;
    bv[2] = 3.0f * t2 * mt;
    bv[3] = t2 * v;
    dbv[0] = -3.0f * mt2;
    dbv[1] = 3.0f * mt2 - 6.0f * v * mt;
    dbv[2] = 6.0f * v * mt - 3.0f * t2;
    dbv[3] = 3.0f * t2;
    
    du[0] = du[1] = du[2] = 0.0f;
    dv[0] = dv[1] = dv[2] = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float du_w = dbu[i] * bv[j];
            float dv_w = bu[i] * dbv[j];
            du[0] += cp[i][j][0] * du_w;
            du[1] += cp[i][j][1] * du_w;
            du[2] += cp[i][j][2] * du_w;
            dv[0] += cp[i][j][0] * dv_w;
            dv[1] += cp[i][j][1] * dv_w;
            dv[2] += cp[i][j][2] * dv_w;
        }
    }
}

static void normalize(float *v)
{
    float len = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 1e-6f) {
        float inv_len = 1.0f / len;
        v[0] *= inv_len; v[1] *= inv_len; v[2] *= inv_len;
    }
}

static void subdivide_patch(const float cp[4][4][3], Vertex *verts, int subdiv)
{
    float inv_subdiv = 1.0f / (float)subdiv;
    
    for (int i = 0; i <= subdiv; ++i) {
        float u = (float)i * inv_subdiv;
        for (int j = 0; j <= subdiv; ++j) {
            float v = (float)j * inv_subdiv;
            float pos[3], du[3], dv[3], n[3];
            bezier_eval(cp, u, v, pos);
            bezier_deriv(cp, u, v, du, dv);
            // Normal = cross(du, dv)
            n[0] = du[1]*dv[2] - du[2]*dv[1];
            n[1] = du[2]*dv[0] - du[0]*dv[2];
            n[2] = du[0]*dv[1] - du[1]*dv[0];
            normalize(n);
            int idx = i * (subdiv + 1) + j;
            verts[idx].x = pos[0];
            verts[idx].y = pos[1];
            verts[idx].z = pos[2];
            verts[idx].nx = n[0];
            verts[idx].ny = n[1];
            verts[idx].nz = n[2];
        }
    }
}
#endif

// Rasterize triangles (stub)
// Simple directional light
static const float light_dir[3] = {0.5773503f,0.5773503f,0.5773503f};

// Simple orthographic projection

// Camera and transformation
static mat4_t view_mat, proj_mat, model_mat;

// Project vertex using model-view-projection, returns depth in [0,1] range
static void project_vertex(const Vertex *v, int *x, int *y, float *z)
{
    vec4_t p = vec4_create(v->x, v->y, v->z, 1.0f);
    p = vec4_transform_mat4(p, model_mat);
    p = vec4_transform_mat4(p, view_mat);
    p = vec4_transform_mat4(p, proj_mat);
    if (p.w != 0.0f) {
        float inv_w = 1.0f / p.w;
        p.x *= inv_w;
        p.y *= inv_w;
        p.z *= inv_w;
    }
    *x = (int)((p.x * 0.5f + 0.5f) * SCREEN_WIDTH);
    *y = (int)((-p.y * 0.5f + 0.5f) * SCREEN_HEIGHT);
    *z = p.z * 0.5f + 0.5f; // Map from [-1,1] to [0,1]
}

// Clear depth buffer using NEON
#if USE_NEON
static void clear_depth_buffer(void)
{
    float32x4_t far_val = vdupq_n_f32(DEPTH_FAR);
    float *ptr = depth_buffer;
    int count = SCREEN_WIDTH * SCREEN_HEIGHT;
    int vec_count = count / 4;
    
    for (int i = 0; i < vec_count; ++i) {
        vst1q_f32(ptr, far_val);
        ptr += 4;
    }
    // Handle remainder
    for (int i = vec_count * 4; i < count; ++i) {
        depth_buffer[i] = DEPTH_FAR;
    }
}
#else
static void clear_depth_buffer(void)
{
    int count = SCREEN_WIDTH * SCREEN_HEIGHT;
    for (int i = 0; i < count; ++i) {
        depth_buffer[i] = DEPTH_FAR;
    }
}
#endif

// Clamp color to 0..31 (5 bits)
static inline uint16_t color_from_gouraud(float c)
{
    int v = (int)(c * 31.0f);
    if (v < 0) v = 0;
    if (v > 31) v = 31;
    // RGB565: grayscale
    return (uint16_t)((v << 11) | (v << 6) | v);
}

#if USE_NEON
// NEON-optimized triangle rasterizer with depth buffer
// Uses fixed-point edge functions and processes 4 pixels at a time
static void rasterize_triangle(uint32_t strideInBytes, const Vertex *v0, const Vertex *v1, const Vertex *v2)
{
    // Project to screen with depth
    int x0, y0, x1, y1, x2, y2;
    float z0, z1, z2;
    project_vertex(v0, &x0, &y0, &z0);
    project_vertex(v1, &x1, &y1, &z1);
    project_vertex(v2, &x2, &y2, &z2);
    
    // Compute vertex colors (diffuse only)
    float c0 = fmaxf(0.0f, v0->nx*light_dir[0] + v0->ny*light_dir[1] + v0->nz*light_dir[2]);
    float c1 = fmaxf(0.0f, v1->nx*light_dir[0] + v1->ny*light_dir[1] + v1->nz*light_dir[2]);
    float c2 = fmaxf(0.0f, v2->nx*light_dir[0] + v2->ny*light_dir[1] + v2->nz*light_dir[2]);

    // Bounding box with early rejection
    int minx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int maxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int miny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int maxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    
    if (maxx < 0 || minx >= SCREEN_WIDTH || maxy < 0 || miny >= SCREEN_HEIGHT) return;
    
    if (minx < 0) minx = 0;
    if (maxx >= SCREEN_WIDTH) maxx = SCREEN_WIDTH - 1;
    if (miny < 0) miny = 0;
    if (maxy >= SCREEN_HEIGHT) maxy = SCREEN_HEIGHT - 1;

    // Edge equation coefficients: E(x,y) = A*x + B*y + C
    int A0 = y1 - y2, B0 = x2 - x1;
    int A1 = y2 - y0, B1 = x0 - x2;
    int A2 = y0 - y1, B2 = x1 - x0;
    
    // Compute area (2x actual area)
    int area2 = A0 * (x0 - x2) + B0 * (y0 - y2);
    if (area2 == 0) return; // Degenerate triangle
    
    float inv_area = 1.0f / (float)area2;
    
    // Precompute NEON constants
    float32x4_t vc0 = vdupq_n_f32(c0);
    float32x4_t vc1 = vdupq_n_f32(c1);
    float32x4_t vc2 = vdupq_n_f32(c2);
    float32x4_t vz0 = vdupq_n_f32(z0);
    float32x4_t vz1 = vdupq_n_f32(z1);
    float32x4_t vz2 = vdupq_n_f32(z2);
    float32x4_t v_inv_area = vdupq_n_f32(inv_area);
    float32x4_t v_31 = vdupq_n_f32(31.0f);
    float32x4_t v_zero = vdupq_n_f32(0.0f);
    
    // X offsets for 4 pixels: {0, 1, 2, 3}
    int32x4_t x_offset = {0, 1, 2, 3};
    int32x4_t vA0 = vdupq_n_s32(A0);
    int32x4_t vA1 = vdupq_n_s32(A1);
    int32x4_t vA2 = vdupq_n_s32(A2);
    int32x4_t vA0_4 = vdupq_n_s32(A0 * 4);
    int32x4_t vA1_4 = vdupq_n_s32(A1 * 4);
    int32x4_t vA2_4 = vdupq_n_s32(A2 * 4);
    
    uint16_t* pixels = (uint16_t*)s_platform->sc->writepage;
    int stride16 = strideInBytes / 2;
    
    for (int y = miny; y <= maxy; ++y) {
        // Edge values at start of row
        int e0_row = A0 * minx + B0 * y + (x1 * y2 - x2 * y1);
        int e1_row = A1 * minx + B1 * y + (x2 * y0 - x0 * y2);
        int e2_row = A2 * minx + B2 * y + (x0 * y1 - x1 * y0);
        
        uint16_t* row = pixels + y * stride16;
        float* depth_row = depth_buffer + y * SCREEN_WIDTH;
        
        // Align minx to 4-pixel boundary for NEON
        int x = minx;
        int aligned_minx = (minx + 3) & ~3;
        
        // Handle unaligned start pixels (scalar with depth test)
        for (; x < aligned_minx && x <= maxx; ++x) {
            if (e0_row >= 0 && e1_row >= 0 && e2_row >= 0) {
                float w0 = (float)e0_row * inv_area;
                float w1 = (float)e1_row * inv_area;
                float w2 = (float)e2_row * inv_area;
                float z = w0*z0 + w1*z1 + w2*z2;
                if (z < depth_row[x]) {
                    depth_row[x] = z;
                    float c = w0*c0 + w1*c1 + w2*c2;
                    row[x] = color_from_gouraud(c);
                }
            }
            e0_row += A0;
            e1_row += A1;
            e2_row += A2;
        }
        
        // Initialize NEON edge values
        int32x4_t ve0 = vaddq_s32(vdupq_n_s32(e0_row), vmulq_s32(vA0, x_offset));
        int32x4_t ve1 = vaddq_s32(vdupq_n_s32(e1_row), vmulq_s32(vA1, x_offset));
        int32x4_t ve2 = vaddq_s32(vdupq_n_s32(e2_row), vmulq_s32(vA2, x_offset));
        
        // Process 4 pixels at a time
        for (; x + 3 <= maxx; x += 4) {
            // Check if all 4 pixels might be inside triangle
            uint32x4_t inside0 = vcgeq_s32(ve0, vdupq_n_s32(0));
            uint32x4_t inside1 = vcgeq_s32(ve1, vdupq_n_s32(0));
            uint32x4_t inside2 = vcgeq_s32(ve2, vdupq_n_s32(0));
            uint32x4_t inside = vandq_u32(vandq_u32(inside0, inside1), inside2);
            
            // Check if any pixel is inside
            uint32_t mask[4];
            vst1q_u32(mask, inside);
            
            if (mask[0] | mask[1] | mask[2] | mask[3]) {
                // Convert edge values to float for interpolation
                float32x4_t fw0 = vmulq_f32(vcvtq_f32_s32(ve0), v_inv_area);
                float32x4_t fw1 = vmulq_f32(vcvtq_f32_s32(ve1), v_inv_area);
                float32x4_t fw2 = vmulq_f32(vcvtq_f32_s32(ve2), v_inv_area);
                
                // Interpolate depth: z = w0*z0 + w1*z1 + w2*z2
                float32x4_t depth = vmlaq_f32(vmlaq_f32(vmulq_f32(fw0, vz0), fw1, vz1), fw2, vz2);
                
                // Load current depth buffer values
                float32x4_t cur_depth = vld1q_f32(&depth_row[x]);
                
                // Depth test: new_depth < cur_depth
                uint32x4_t depth_pass = vcltq_f32(depth, cur_depth);
                
                // Combine triangle inside test with depth test
                uint32x4_t write_mask = vandq_u32(inside, depth_pass);
                
                uint32_t wmask[4];
                vst1q_u32(wmask, write_mask);
                
                if (wmask[0] | wmask[1] | wmask[2] | wmask[3]) {
                    // Interpolate colors: c = w0*c0 + w1*c1 + w2*c2
                    float32x4_t color = vmlaq_f32(vmlaq_f32(vmulq_f32(fw0, vc0), fw1, vc1), fw2, vc2);
                    
                    // Clamp to [0, 31]
                    color = vmaxq_f32(color, v_zero);
                    color = vminq_f32(vmulq_f32(color, v_31), v_31);
                    
                    // Convert to integer
                    int32x4_t icolor = vcvtq_s32_f32(color);
                    
                    // Extract values
                    int32_t cv[4];
                    float dv[4];
                    vst1q_s32(cv, icolor);
                    vst1q_f32(dv, depth);
                    
                    // Write pixels and depth (checking mask)
                    if (wmask[0]) { row[x]   = (uint16_t)((cv[0] << 11) | (cv[0] << 6) | cv[0]); depth_row[x]   = dv[0]; }
                    if (wmask[1]) { row[x+1] = (uint16_t)((cv[1] << 11) | (cv[1] << 6) | cv[1]); depth_row[x+1] = dv[1]; }
                    if (wmask[2]) { row[x+2] = (uint16_t)((cv[2] << 11) | (cv[2] << 6) | cv[2]); depth_row[x+2] = dv[2]; }
                    if (wmask[3]) { row[x+3] = (uint16_t)((cv[3] << 11) | (cv[3] << 6) | cv[3]); depth_row[x+3] = dv[3]; }
                }
            }
            
            // Increment edge values for next 4 pixels
            ve0 = vaddq_s32(ve0, vA0_4);
            ve1 = vaddq_s32(ve1, vA1_4);
            ve2 = vaddq_s32(ve2, vA2_4);
        }
        
        // Update scalar edge values for tail processing
        e0_row += A0 * (x - aligned_minx);
        e1_row += A1 * (x - aligned_minx);
        e2_row += A2 * (x - aligned_minx);
        
        // Handle remaining pixels (scalar with depth test)
        for (; x <= maxx; ++x) {
            if (e0_row >= 0 && e1_row >= 0 && e2_row >= 0) {
                float w0 = (float)e0_row * inv_area;
                float w1 = (float)e1_row * inv_area;
                float w2 = (float)e2_row * inv_area;
                float z = w0*z0 + w1*z1 + w2*z2;
                if (z < depth_row[x]) {
                    depth_row[x] = z;
                    float c = w0*c0 + w1*c1 + w2*c2;
                    row[x] = color_from_gouraud(c);
                }
            }
            e0_row += A0;
            e1_row += A1;
            e2_row += A2;
        }
    }
}

#else
// Scalar fallback - optimized with incremental edge functions and depth buffer
static void rasterize_triangle(uint32_t strideInBytes, const Vertex *v0, const Vertex *v1, const Vertex *v2)
{
    // Project to screen with depth
    int x0, y0, x1, y1, x2, y2;
    float z0, z1, z2;
    project_vertex(v0, &x0, &y0, &z0);
    project_vertex(v1, &x1, &y1, &z1);
    project_vertex(v2, &x2, &y2, &z2);
    
    // Compute vertex colors (diffuse only)
    float c0 = fmaxf(0.0f, v0->nx*light_dir[0] + v0->ny*light_dir[1] + v0->nz*light_dir[2]);
    float c1 = fmaxf(0.0f, v1->nx*light_dir[0] + v1->ny*light_dir[1] + v1->nz*light_dir[2]);
    float c2 = fmaxf(0.0f, v2->nx*light_dir[0] + v2->ny*light_dir[1] + v2->nz*light_dir[2]);

    // Bounding box
    int minx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int maxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int miny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int maxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    
    if (maxx < 0 || minx >= SCREEN_WIDTH || maxy < 0 || miny >= SCREEN_HEIGHT) return;
    
    if (minx < 0) minx = 0;
    if (maxx >= SCREEN_WIDTH) maxx = SCREEN_WIDTH - 1;
    if (miny < 0) miny = 0;
    if (maxy >= SCREEN_HEIGHT) maxy = SCREEN_HEIGHT - 1;

    // Edge equation coefficients
    int A0 = y1 - y2, B0 = x2 - x1;
    int A1 = y2 - y0, B1 = x0 - x2;
    int A2 = y0 - y1, B2 = x1 - x0;
    
    int area2 = A0 * (x0 - x2) + B0 * (y0 - y2);
    if (area2 == 0) return;
    
    float inv_area = 1.0f / (float)area2;
    
    uint16_t* pixels = (uint16_t*)s_platform->sc->writepage;
    int stride16 = strideInBytes / 2;
    
    for (int y = miny; y <= maxy; ++y) {
        int e0_row = A0 * minx + B0 * y + (x1 * y2 - x2 * y1);
        int e1_row = A1 * minx + B1 * y + (x2 * y0 - x0 * y2);
        int e2_row = A2 * minx + B2 * y + (x0 * y1 - x1 * y0);
        
        uint16_t* row = pixels + y * stride16;
        float* depth_row = depth_buffer + y * SCREEN_WIDTH;
        
        for (int x = minx; x <= maxx; ++x) {
            if (e0_row >= 0 && e1_row >= 0 && e2_row >= 0) {
                float w0 = (float)e0_row * inv_area;
                float w1 = (float)e1_row * inv_area;
                float w2 = (float)e2_row * inv_area;
                float z = w0*z0 + w1*z1 + w2*z2;
                if (z < depth_row[x]) {
                    depth_row[x] = z;
                    float c = w0*c0 + w1*c1 + w2*c2;
                    row[x] = color_from_gouraud(c);
                }
            }
            e0_row += A0;
            e1_row += A1;
            e2_row += A2;
        }
    }
}
#endif

// Render teapot
static void render_teapot(uint32_t strideInBytes)
{
    for (int p=0; p<TEAPOT_PATCHES; ++p) {
        float cp[4][4][3];
        for (int i=0; i<4; ++i)
            for (int j=0; j<4; ++j)
                for (int k=0; k<3; ++k)
                    cp[i][j][k] = teapot_control_points[teapot_patches[p][i*4+j]][k];
        Vertex verts[(PATCH_SUBDIV+1)*(PATCH_SUBDIV+1)];
        subdivide_patch(cp, verts, PATCH_SUBDIV);
        // Draw mesh as triangles
        for (int i=0; i<PATCH_SUBDIV; ++i) {
            for (int j=0; j<PATCH_SUBDIV; ++j) {
                Vertex *v00 = &verts[i*(PATCH_SUBDIV+1)+j];
                Vertex *v01 = &verts[i*(PATCH_SUBDIV+1)+(j+1)];
                Vertex *v10 = &verts[(i+1)*(PATCH_SUBDIV+1)+j];
                Vertex *v11 = &verts[(i+1)*(PATCH_SUBDIV+1)+(j+1)];
                rasterize_triangle(strideInBytes, v00, v10, v11);
                rasterize_triangle(strideInBytes, v00, v11, v01);
            }
        }
    }
}

int main()
{
    printf("starting platform code\n");
    s_platform = SPInitPlatform();

    printf("Allocating video buffers\n");
    strideInBytes = VPUGetStride(EVM_640_480, ECM_16bit_RGB);
    framebufferA.size = strideInBytes*SCREEN_HEIGHT;
    framebufferB.size = strideInBytes*SCREEN_HEIGHT;
    SPAllocateBuffer(s_platform, &framebufferA);
    SPAllocateBuffer(s_platform, &framebufferB);
    
    // Allocate depth buffer
    printf("Allocating depth buffer\n");
    depth_buffer = (float*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float));

    printf("setting video mode\n");
    VPUSetVideoMode(s_platform->vx, EVM_640_480, ECM_16bit_RGB, EVS_Enable);

    struct EVideoSwapContext* sc = s_platform->sc;
    sc->cycle = 0;
    sc->framebufferA = &framebufferA;
    sc->framebufferB = &framebufferB;
	VPUSwapPages(s_platform->vx, s_platform->sc);

    // Camera setup - looking at the center of the teapot
    view_mat = mat4_translation(0.0f, -2.0f, -10.0f);
    proj_mat = mat4_perspective(45.0f * NEON_DEG_TO_RAD, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    // Pre-rotation to orient teapot upright (Z-up to Y-up)
    mat4_t orient_mat = mat4_rotation_x(-1.5707963f); // -90 degrees

    float angle = 0.0f;
    while (true)
	{
		// Clear framebuffer and depth buffer
		VPUClear(s_platform->vx, 0x00000000);
		clear_depth_buffer();

		// Animate teapot rotation around its vertical axis (Z in model space)
        mat4_t rot_mat = mat4_rotation_z(angle);
        model_mat = mat4_mul(orient_mat, rot_mat);
        angle += 0.02f;
        if (angle > 6.2831853f) angle -= 6.2831853f;
        render_teapot(strideInBytes);

		VPUWaitVSync(s_platform->vx);
		VPUSwapPages(s_platform->vx, s_platform->sc);
	}
    return 0;
}
