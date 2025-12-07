// ============================================================================
// vec4_t NEON Implementation for ARMv7
// C-style SIMD vector math using ARM NEON intrinsics
// ============================================================================

#include "vec.h"

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define USE_NEON 1
#else
#define USE_NEON 0
#endif

// ============================================================================
// Helper: Load/Store vec4_t into NEON register
// ============================================================================
#if USE_NEON
static inline float32x4_t vec4_load(vec4_t v) {
    float __attribute__((aligned(16))) data[4] = { v.x, v.y, v.z, v.w };
    return vld1q_f32(data);
}

static inline vec4_t vec4_store(float32x4_t v) {
    float __attribute__((aligned(16))) data[4];
    vst1q_f32(data, v);
    vec4_t result = { data[0], data[1], data[2], data[3] };
    return result;
}

// Horizontal add for all 4 components: x + y + z + w
static inline float vec4_hadd(float32x4_t v) {
    float32x2_t xy = vget_low_f32(v);       // [x, y]
    float32x2_t zw = vget_high_f32(v);      // [z, w]
    float32x2_t sum1 = vpadd_f32(xy, zw);   // [x+y, z+w]
    float32x2_t sum2 = vpadd_f32(sum1, sum1); // [x+y+z+w, x+y+z+w]
    return vget_lane_f32(sum2, 0);
}
#endif

// ============================================================================
// Creation Functions
// ============================================================================

vec4_t vec4_create(float x, float y, float z, float w) {
    vec4_t v = { x, y, z, w };
    return v;
}

vec4_t vec4_from_vec3(vec3_t v, float w) {
    vec4_t result = { v.x, v.y, v.z, w };
    return result;
}

vec4_t vec4_zero(void) {
    vec4_t v = { 0.0f, 0.0f, 0.0f, 0.0f };
    return v;
}

vec4_t vec4_one(void) {
    vec4_t v = { 1.0f, 1.0f, 1.0f, 1.0f };
    return v;
}

// ============================================================================
// Basic Operations
// ============================================================================

vec4_t vec4_add(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t result = vaddq_f32(va, vb);
    return vec4_store(result);
#else
    vec4_t result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return result;
#endif
}

vec4_t vec4_sub(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t result = vsubq_f32(va, vb);
    return vec4_store(result);
#else
    vec4_t result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return result;
#endif
}

vec4_t vec4_mul(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t result = vmulq_f32(va, vb);
    return vec4_store(result);
#else
    vec4_t result = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
    return result;
#endif
}

vec4_t vec4_div(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    // ARMv7 NEON doesn't have direct divide, use reciprocal estimate + Newton-Raphson
    float32x4_t recip = vrecpeq_f32(vb);
    recip = vmulq_f32(vrecpsq_f32(vb, recip), recip);  // One Newton-Raphson iteration
    recip = vmulq_f32(vrecpsq_f32(vb, recip), recip);  // Second iteration for better precision
    float32x4_t result = vmulq_f32(va, recip);
    return vec4_store(result);
#else
    vec4_t result = { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
    return result;
#endif
}

vec4_t vec4_scale(vec4_t v, float s) {
#if USE_NEON
    float32x4_t vv = vec4_load(v);
    float32x4_t vs = vdupq_n_f32(s);
    float32x4_t result = vmulq_f32(vv, vs);
    return vec4_store(result);
#else
    vec4_t result = { v.x * s, v.y * s, v.z * s, v.w * s };
    return result;
#endif
}

vec4_t vec4_negate(vec4_t v) {
#if USE_NEON
    float32x4_t vv = vec4_load(v);
    float32x4_t result = vnegq_f32(vv);
    return vec4_store(result);
#else
    vec4_t result = { -v.x, -v.y, -v.z, -v.w };
    return result;
#endif
}

// ============================================================================
// Vector Math
// ============================================================================

float vec4_dot(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t prod = vmulq_f32(va, vb);
    return vec4_hadd(prod);
#else
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
#endif
}

float vec4_length_squared(vec4_t v) {
    return vec4_dot(v, v);
}

float vec4_length(vec4_t v) {
#if USE_NEON
    float len_sq = vec4_length_squared(v);
    // Use NEON rsqrt estimate with Newton-Raphson for inverse sqrt, then multiply
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t est = vrsqrte_f32(v_len_sq);
    // Newton-Raphson iterations
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v_len_sq, est), est));
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v_len_sq, est), est));
    // length = len_sq * rsqrt(len_sq) = sqrt(len_sq)
    float32x2_t result = vmul_f32(v_len_sq, est);
    return vget_lane_f32(result, 0);
#else
    return sqrtf(vec4_length_squared(v));
#endif
}

vec4_t vec4_normalize(vec4_t v) {
#if USE_NEON
    float len_sq = vec4_length_squared(v);
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return vec4_zero();
    }
    
    // Compute inverse sqrt using NEON
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t rsqrt = vrsqrte_f32(v_len_sq);
    // Newton-Raphson iterations for precision
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    
    float inv_len = vget_lane_f32(rsqrt, 0);
    return vec4_scale(v, inv_len);
#else
    float len = vec4_length(v);
    if (len < NEON_EPSILON) {
        return vec4_zero();
    }
    return vec4_scale(v, 1.0f / len);
#endif
}

// ============================================================================
// Interpolation and Utilities
// ============================================================================

vec4_t vec4_lerp(vec4_t a, vec4_t b, float t) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t vt = vdupq_n_f32(t);
    // result = a + t * (b - a)
    float32x4_t diff = vsubq_f32(vb, va);
    float32x4_t result = vmlaq_f32(va, diff, vt);
    return vec4_store(result);
#else
    vec4_t result = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z),
        a.w + t * (b.w - a.w)
    };
    return result;
#endif
}

vec4_t vec4_min(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t result = vminq_f32(va, vb);
    return vec4_store(result);
#else
    vec4_t result = {
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z,
        a.w < b.w ? a.w : b.w
    };
    return result;
#endif
}

vec4_t vec4_max(vec4_t a, vec4_t b) {
#if USE_NEON
    float32x4_t va = vec4_load(a);
    float32x4_t vb = vec4_load(b);
    float32x4_t result = vmaxq_f32(va, vb);
    return vec4_store(result);
#else
    vec4_t result = {
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z,
        a.w > b.w ? a.w : b.w
    };
    return result;
#endif
}

// ============================================================================
// Transform by Matrix
// ============================================================================

vec4_t vec4_transform_mat4(vec4_t v, mat4_t m) {
#if USE_NEON
    // Matrix is column-major: m[col*4 + row]
    // result = v.x * col0 + v.y * col1 + v.z * col2 + v.w * col3
    float32x4_t col0 = vld1q_f32(&m.m[0]);
    float32x4_t col1 = vld1q_f32(&m.m[4]);
    float32x4_t col2 = vld1q_f32(&m.m[8]);
    float32x4_t col3 = vld1q_f32(&m.m[12]);
    
    float32x4_t vx = vdupq_n_f32(v.x);
    float32x4_t vy = vdupq_n_f32(v.y);
    float32x4_t vz = vdupq_n_f32(v.z);
    float32x4_t vw = vdupq_n_f32(v.w);
    
    float32x4_t result = vmulq_f32(col0, vx);
    result = vmlaq_f32(result, col1, vy);
    result = vmlaq_f32(result, col2, vz);
    result = vmlaq_f32(result, col3, vw);
    
    return vec4_store(result);
#else
    vec4_t result = {
        v.x * m.m[0] + v.y * m.m[4] + v.z * m.m[8]  + v.w * m.m[12],
        v.x * m.m[1] + v.y * m.m[5] + v.z * m.m[9]  + v.w * m.m[13],
        v.x * m.m[2] + v.y * m.m[6] + v.z * m.m[10] + v.w * m.m[14],
        v.x * m.m[3] + v.y * m.m[7] + v.z * m.m[11] + v.w * m.m[15]
    };
    return result;
#endif
}

// ============================================================================
// Conversion Functions
// ============================================================================

vec3_t vec4_to_vec3(vec4_t v) {
    vec3_t result = { v.x, v.y, v.z, 0.0f };
    return result;
}

vec3_t vec4_to_vec3_perspective(vec4_t v) {
#if USE_NEON
    // Divide x, y, z by w
    if (fabsf(v.w) < NEON_EPSILON) {
        return vec3_zero();
    }
    
    // Compute 1/w using NEON reciprocal estimate
    float32x2_t vw = vdup_n_f32(v.w);
    float32x2_t recip = vrecpe_f32(vw);
    recip = vmul_f32(vrecps_f32(vw, recip), recip);  // Newton-Raphson iteration
    recip = vmul_f32(vrecps_f32(vw, recip), recip);  // Second iteration
    float inv_w = vget_lane_f32(recip, 0);
    
    vec3_t result = { v.x * inv_w, v.y * inv_w, v.z * inv_w, 0.0f };
    return result;
#else
    if (fabsf(v.w) < NEON_EPSILON) {
        return vec3_zero();
    }
    float inv_w = 1.0f / v.w;
    vec3_t result = { v.x * inv_w, v.y * inv_w, v.z * inv_w, 0.0f };
    return result;
#endif
}
