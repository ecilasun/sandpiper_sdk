// ============================================================================
// vec3_t NEON Implementation for ARMv7
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
// Helper: Load vec3_t into NEON register (loads 4 floats, ignores padding)
// ============================================================================
#if USE_NEON
static inline float32x4_t vec3_load(vec3_t v) {
    float __attribute__((aligned(16))) data[4] = { v.x, v.y, v.z, 0.0f };
    return vld1q_f32(data);
}

static inline vec3_t vec3_store(float32x4_t v) {
    float __attribute__((aligned(16))) data[4];
    vst1q_f32(data, v);
    vec3_t result = { data[0], data[1], data[2], 0.0f };
    return result;
}

// Horizontal add for 3 components: x + y + z
static inline float vec3_hadd3(float32x4_t v) {
    float32x2_t xy = vget_low_f32(v);       // [x, y]
    float32x2_t zw = vget_high_f32(v);      // [z, w]
    float32x2_t sum = vpadd_f32(xy, zw);    // [x+y, z+w]
    return vget_lane_f32(sum, 0) + vget_lane_f32(sum, 1) - vgetq_lane_f32(v, 3);
}

// Alternative: sum only x+y+z components
static inline float vec3_dot_sum(float32x4_t v) {
    // Extract individual lanes and sum
    return vgetq_lane_f32(v, 0) + vgetq_lane_f32(v, 1) + vgetq_lane_f32(v, 2);
}
#endif

// ============================================================================
// Creation Functions
// ============================================================================

vec3_t vec3_create(float x, float y, float z) {
    vec3_t v = { x, y, z, 0.0f };
    return v;
}

vec3_t vec3_zero(void) {
    vec3_t v = { 0.0f, 0.0f, 0.0f, 0.0f };
    return v;
}

vec3_t vec3_one(void) {
    vec3_t v = { 1.0f, 1.0f, 1.0f, 0.0f };
    return v;
}

vec3_t vec3_up(void) {
    vec3_t v = { 0.0f, 1.0f, 0.0f, 0.0f };
    return v;
}

vec3_t vec3_down(void) {
    vec3_t v = { 0.0f, -1.0f, 0.0f, 0.0f };
    return v;
}

vec3_t vec3_left(void) {
    vec3_t v = { -1.0f, 0.0f, 0.0f, 0.0f };
    return v;
}

vec3_t vec3_right(void) {
    vec3_t v = { 1.0f, 0.0f, 0.0f, 0.0f };
    return v;
}

vec3_t vec3_forward(void) {
    vec3_t v = { 0.0f, 0.0f, -1.0f, 0.0f };
    return v;
}

vec3_t vec3_back(void) {
    vec3_t v = { 0.0f, 0.0f, 1.0f, 0.0f };
    return v;
}

// ============================================================================
// Basic Operations
// ============================================================================

vec3_t vec3_add(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t result = vaddq_f32(va, vb);
    return vec3_store(result);
#else
    vec3_t result = { a.x + b.x, a.y + b.y, a.z + b.z, 0.0f };
    return result;
#endif
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t result = vsubq_f32(va, vb);
    return vec3_store(result);
#else
    vec3_t result = { a.x - b.x, a.y - b.y, a.z - b.z, 0.0f };
    return result;
#endif
}

vec3_t vec3_mul(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t result = vmulq_f32(va, vb);
    return vec3_store(result);
#else
    vec3_t result = { a.x * b.x, a.y * b.y, a.z * b.z, 0.0f };
    return result;
#endif
}

vec3_t vec3_div(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    // ARMv7 NEON doesn't have direct divide, use reciprocal estimate + Newton-Raphson
    float32x4_t recip = vrecpeq_f32(vb);
    recip = vmulq_f32(vrecpsq_f32(vb, recip), recip);  // One Newton-Raphson iteration
    recip = vmulq_f32(vrecpsq_f32(vb, recip), recip);  // Second iteration for better precision
    float32x4_t result = vmulq_f32(va, recip);
    return vec3_store(result);
#else
    vec3_t result = { a.x / b.x, a.y / b.y, a.z / b.z, 0.0f };
    return result;
#endif
}

vec3_t vec3_scale(vec3_t v, float s) {
#if USE_NEON
    float32x4_t vv = vec3_load(v);
    float32x4_t vs = vdupq_n_f32(s);
    float32x4_t result = vmulq_f32(vv, vs);
    return vec3_store(result);
#else
    vec3_t result = { v.x * s, v.y * s, v.z * s, 0.0f };
    return result;
#endif
}

vec3_t vec3_negate(vec3_t v) {
#if USE_NEON
    float32x4_t vv = vec3_load(v);
    float32x4_t result = vnegq_f32(vv);
    return vec3_store(result);
#else
    vec3_t result = { -v.x, -v.y, -v.z, 0.0f };
    return result;
#endif
}

// ============================================================================
// Vector Math
// ============================================================================

float vec3_dot(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t prod = vmulq_f32(va, vb);
    return vec3_dot_sum(prod);
#else
    return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
#if USE_NEON
    // Cross product: (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    
    // Create shuffled versions: a_yzx, a_zxy, b_yzx, b_zxy
    // ARMv7 doesn't have arbitrary shuffle, so we construct manually
    float a_yzx[4] = { a.y, a.z, a.x, 0.0f };
    float a_zxy[4] = { a.z, a.x, a.y, 0.0f };
    float b_yzx[4] = { b.y, b.z, b.x, 0.0f };
    float b_zxy[4] = { b.z, b.x, b.y, 0.0f };
    
    float32x4_t va_yzx = vld1q_f32(a_yzx);
    float32x4_t va_zxy = vld1q_f32(a_zxy);
    float32x4_t vb_yzx = vld1q_f32(b_yzx);
    float32x4_t vb_zxy = vld1q_f32(b_zxy);
    
    // result = a_yzx * b_zxy - a_zxy * b_yzx
    float32x4_t result = vmlsq_f32(vmulq_f32(va_yzx, vb_zxy), va_zxy, vb_yzx);
    return vec3_store(result);
#else
    vec3_t result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    };
    return result;
#endif
}

float vec3_length_squared(vec3_t v) {
    return vec3_dot(v, v);
}

float vec3_length(vec3_t v) {
#if USE_NEON
    float len_sq = vec3_length_squared(v);
    // Use NEON rsqrt estimate with Newton-Raphson for inverse sqrt, then invert
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t est = vrsqrte_f32(v_len_sq);
    // Newton-Raphson iterations
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v_len_sq, est), est));
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v_len_sq, est), est));
    // length = len_sq * rsqrt(len_sq) = sqrt(len_sq)
    float32x2_t result = vmul_f32(v_len_sq, est);
    return vget_lane_f32(result, 0);
#else
    return sqrtf(vec3_length_squared(v));
#endif
}

vec3_t vec3_normalize(vec3_t v) {
#if USE_NEON
    float len_sq = vec3_length_squared(v);
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return vec3_zero();
    }
    
    // Compute inverse sqrt using NEON
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t rsqrt = vrsqrte_f32(v_len_sq);
    // Newton-Raphson iterations for precision
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    
    float inv_len = vget_lane_f32(rsqrt, 0);
    return vec3_scale(v, inv_len);
#else
    float len = vec3_length(v);
    if (len < NEON_EPSILON) {
        return vec3_zero();
    }
    return vec3_scale(v, 1.0f / len);
#endif
}

float vec3_distance_squared(vec3_t a, vec3_t b) {
    vec3_t diff = vec3_sub(a, b);
    return vec3_length_squared(diff);
}

float vec3_distance(vec3_t a, vec3_t b) {
    vec3_t diff = vec3_sub(a, b);
    return vec3_length(diff);
}

// ============================================================================
// Interpolation and Utilities
// ============================================================================

vec3_t vec3_lerp(vec3_t a, vec3_t b, float t) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t vt = vdupq_n_f32(t);
    // result = a + t * (b - a)
    float32x4_t diff = vsubq_f32(vb, va);
    float32x4_t result = vmlaq_f32(va, diff, vt);
    return vec3_store(result);
#else
    vec3_t result = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z),
        0.0f
    };
    return result;
#endif
}

vec3_t vec3_min(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t result = vminq_f32(va, vb);
    return vec3_store(result);
#else
    vec3_t result = {
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z,
        0.0f
    };
    return result;
#endif
}

vec3_t vec3_max(vec3_t a, vec3_t b) {
#if USE_NEON
    float32x4_t va = vec3_load(a);
    float32x4_t vb = vec3_load(b);
    float32x4_t result = vmaxq_f32(va, vb);
    return vec3_store(result);
#else
    vec3_t result = {
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z,
        0.0f
    };
    return result;
#endif
}

vec3_t vec3_clamp(vec3_t v, vec3_t min_val, vec3_t max_val) {
#if USE_NEON
    float32x4_t vv = vec3_load(v);
    float32x4_t vmin = vec3_load(min_val);
    float32x4_t vmax = vec3_load(max_val);
    float32x4_t result = vmaxq_f32(vminq_f32(vv, vmax), vmin);
    return vec3_store(result);
#else
    vec3_t result = vec3_max(vec3_min(v, max_val), min_val);
    return result;
#endif
}

vec3_t vec3_reflect(vec3_t incident, vec3_t normal) {
    // reflect = incident - 2 * dot(incident, normal) * normal
    float d = vec3_dot(incident, normal);
    vec3_t scaled_normal = vec3_scale(normal, 2.0f * d);
    return vec3_sub(incident, scaled_normal);
}

vec3_t vec3_refract(vec3_t incident, vec3_t normal, float eta) {
    float n_dot_i = vec3_dot(normal, incident);
    float k = 1.0f - eta * eta * (1.0f - n_dot_i * n_dot_i);
    
    if (k < 0.0f) {
        return vec3_zero();  // Total internal reflection
    }
    
    // refract = eta * incident - (eta * n_dot_i + sqrt(k)) * normal
    vec3_t scaled_incident = vec3_scale(incident, eta);
    vec3_t scaled_normal = vec3_scale(normal, eta * n_dot_i + sqrtf(k));
    return vec3_sub(scaled_incident, scaled_normal);
}

// ============================================================================
// Transform by Matrix
// ============================================================================

vec3_t vec3_transform_mat3(vec3_t v, mat3_t m) {
#if USE_NEON
    // Matrix is column-major: m[col*3 + row]
    // result.x = v.x * m[0] + v.y * m[3] + v.z * m[6]
    // result.y = v.x * m[1] + v.y * m[4] + v.z * m[7]
    // result.z = v.x * m[2] + v.y * m[5] + v.z * m[8]
    
    float32x4_t col0 = { m.m[0], m.m[1], m.m[2], 0.0f };
    float32x4_t col1 = { m.m[3], m.m[4], m.m[5], 0.0f };
    float32x4_t col2 = { m.m[6], m.m[7], m.m[8], 0.0f };
    
    float32x4_t vx = vdupq_n_f32(v.x);
    float32x4_t vy = vdupq_n_f32(v.y);
    float32x4_t vz = vdupq_n_f32(v.z);
    
    float32x4_t result = vmulq_f32(col0, vx);
    result = vmlaq_f32(result, col1, vy);
    result = vmlaq_f32(result, col2, vz);
    
    return vec3_store(result);
#else
    vec3_t result = {
        v.x * m.m[0] + v.y * m.m[3] + v.z * m.m[6],
        v.x * m.m[1] + v.y * m.m[4] + v.z * m.m[7],
        v.x * m.m[2] + v.y * m.m[5] + v.z * m.m[8],
        0.0f
    };
    return result;
#endif
}

vec3_t vec3_transform_mat4(vec3_t v, mat4_t m) {
#if USE_NEON
    // Transform as point (w=1), includes translation
    // Matrix is column-major: m[col*4 + row]
    float32x4_t col0 = vld1q_f32(&m.m[0]);
    float32x4_t col1 = vld1q_f32(&m.m[4]);
    float32x4_t col2 = vld1q_f32(&m.m[8]);
    float32x4_t col3 = vld1q_f32(&m.m[12]);
    
    float32x4_t vx = vdupq_n_f32(v.x);
    float32x4_t vy = vdupq_n_f32(v.y);
    float32x4_t vz = vdupq_n_f32(v.z);
    
    float32x4_t result = vmulq_f32(col0, vx);
    result = vmlaq_f32(result, col1, vy);
    result = vmlaq_f32(result, col2, vz);
    result = vaddq_f32(result, col3);  // Add col3 for w=1
    
    // Extract x, y, z (ignore w)
    vec3_t out;
    out.x = vgetq_lane_f32(result, 0);
    out.y = vgetq_lane_f32(result, 1);
    out.z = vgetq_lane_f32(result, 2);
    out._pad = 0.0f;
    return out;
#else
    vec3_t result = {
        v.x * m.m[0] + v.y * m.m[4] + v.z * m.m[8]  + m.m[12],
        v.x * m.m[1] + v.y * m.m[5] + v.z * m.m[9]  + m.m[13],
        v.x * m.m[2] + v.y * m.m[6] + v.z * m.m[10] + m.m[14],
        0.0f
    };
    return result;
#endif
}

vec3_t vec3_transform_mat4_dir(vec3_t v, mat4_t m) {
#if USE_NEON
    // Transform as direction (w=0), no translation
    float32x4_t col0 = vld1q_f32(&m.m[0]);
    float32x4_t col1 = vld1q_f32(&m.m[4]);
    float32x4_t col2 = vld1q_f32(&m.m[8]);
    
    float32x4_t vx = vdupq_n_f32(v.x);
    float32x4_t vy = vdupq_n_f32(v.y);
    float32x4_t vz = vdupq_n_f32(v.z);
    
    float32x4_t result = vmulq_f32(col0, vx);
    result = vmlaq_f32(result, col1, vy);
    result = vmlaq_f32(result, col2, vz);
    
    vec3_t out;
    out.x = vgetq_lane_f32(result, 0);
    out.y = vgetq_lane_f32(result, 1);
    out.z = vgetq_lane_f32(result, 2);
    out._pad = 0.0f;
    return out;
#else
    vec3_t result = {
        v.x * m.m[0] + v.y * m.m[4] + v.z * m.m[8],
        v.x * m.m[1] + v.y * m.m[5] + v.z * m.m[9],
        v.x * m.m[2] + v.y * m.m[6] + v.z * m.m[10],
        0.0f
    };
    return result;
#endif
}
