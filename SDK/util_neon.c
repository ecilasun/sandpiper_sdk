// ============================================================================
// Utility Functions NEON Implementation for ARMv7
// Common math utilities using ARM NEON intrinsics
// ============================================================================

#include "vec.h"

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define USE_NEON 1
#else
#define USE_NEON 0
#endif

// ============================================================================
// Angle Conversions
// ============================================================================

float deg_to_rad(float degrees) {
    return degrees * NEON_DEG_TO_RAD;
}

float rad_to_deg(float radians) {
    return radians * NEON_RAD_TO_DEG;
}

// ============================================================================
// Common Math
// ============================================================================

float clampf(float value, float min_val, float max_val) {
#if USE_NEON
    float32x2_t v = vdup_n_f32(value);
    float32x2_t vmin = vdup_n_f32(min_val);
    float32x2_t vmax = vdup_n_f32(max_val);
    v = vmax_f32(vmin_f32(v, vmax), vmin);
    return vget_lane_f32(v, 0);
#else
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
#endif
}

float lerpf(float a, float b, float t) {
    return a + t * (b - a);
}

float smoothstep(float edge0, float edge1, float x) {
    // Clamp x to [0, 1] range
    float t = clampf((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Smooth Hermite interpolation
    return t * t * (3.0f - 2.0f * t);
}

float inversesqrt(float x) {
#if USE_NEON
    float32x2_t v = vdup_n_f32(x);
    float32x2_t est = vrsqrte_f32(v);
    // Newton-Raphson iterations for precision
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v, est), est));
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v, est), est));
    return vget_lane_f32(est, 0);
#else
    return 1.0f / sqrtf(x);
#endif
}

// ============================================================================
// Fast Approximate Math Functions
// ============================================================================

// Fast sine approximation using polynomial
// Valid for x in [-PI, PI], uses minimax polynomial
float fast_sin(float x) {
    // Reduce x to [-PI, PI] range
    while (x > NEON_PI) x -= NEON_TWO_PI;
    while (x < -NEON_PI) x += NEON_TWO_PI;
    
    // Polynomial coefficients (minimax approximation)
    const float c1 =  0.99997937679290771484375f;
    const float c2 = -0.166624367237091064453125f;
    const float c3 =  0.00830444693565368652344f;
    const float c4 = -0.000184535203129053115844f;
    
    float x2 = x * x;
    return x * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)));
}

// Fast cosine using fast_sin
float fast_cos(float x) {
    return fast_sin(x + NEON_HALF_PI);
}

// Fast tangent approximation
float fast_tan(float x) {
    float s = fast_sin(x);
    float c = fast_cos(x);
    if (fabsf(c) < NEON_EPSILON) {
        return (s >= 0.0f) ? 1e10f : -1e10f;
    }
    return s / c;
}

// Fast inverse square root (Quake-style with NEON refinement)
float fast_inversesqrt(float x) {
#if USE_NEON
    // Use NEON estimate (single iteration, less precise but faster)
    float32x2_t v = vdup_n_f32(x);
    float32x2_t est = vrsqrte_f32(v);
    // Single Newton-Raphson iteration
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v, est), est));
    return vget_lane_f32(est, 0);
#else
    // Classic Quake fast inverse sqrt
    union {
        float f;
        uint32_t i;
    } conv;
    
    float x2 = x * 0.5f;
    conv.f = x;
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f = conv.f * (1.5f - (x2 * conv.f * conv.f));  // Newton iteration
    return conv.f;
#endif
}
