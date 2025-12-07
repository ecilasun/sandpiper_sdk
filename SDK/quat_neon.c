// ============================================================================
// quat_t NEON Implementation for ARMv7
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
// Helper: Load/Store quat_t into NEON register
// ============================================================================
#if USE_NEON
static inline float32x4_t quat_load(quat_t q) {
    float __attribute__((aligned(16))) data[4] = { q.x, q.y, q.z, q.w };
    return vld1q_f32(data);
}

static inline quat_t quat_store(float32x4_t v) {
    float __attribute__((aligned(16))) data[4];
    vst1q_f32(data, v);
    quat_t result = { data[0], data[1], data[2], data[3] };
    return result;
}

// Horizontal add for all 4 components
static inline float quat_hadd(float32x4_t v) {
    float32x2_t xy = vget_low_f32(v);
    float32x2_t zw = vget_high_f32(v);
    float32x2_t sum1 = vpadd_f32(xy, zw);
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    return vget_lane_f32(sum2, 0);
}
#endif

// ============================================================================
// Creation Functions
// ============================================================================

quat_t quat_identity(void) {
    quat_t q = { 0.0f, 0.0f, 0.0f, 1.0f };
    return q;
}

quat_t quat_create(float x, float y, float z, float w) {
    quat_t q = { x, y, z, w };
    return q;
}

quat_t quat_from_axis_angle(vec3_t axis, float radians) {
    float half_angle = radians * 0.5f;
    float s = sinf(half_angle);
    float c = cosf(half_angle);
    
    // Normalize axis
    float len_sq = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return quat_identity();
    }
    
#if USE_NEON
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t rsqrt = vrsqrte_f32(v_len_sq);
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    float inv_len = vget_lane_f32(rsqrt, 0);
#else
    float inv_len = 1.0f / sqrtf(len_sq);
#endif
    
    quat_t q = {
        axis.x * inv_len * s,
        axis.y * inv_len * s,
        axis.z * inv_len * s,
        c
    };
    return q;
}

quat_t quat_from_euler(float pitch, float yaw, float roll) {
    // XYZ order: first X (pitch), then Y (yaw), then Z (roll)
    float cx = cosf(pitch * 0.5f);
    float sx = sinf(pitch * 0.5f);
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cz = cosf(roll * 0.5f);
    float sz = sinf(roll * 0.5f);
    
    quat_t q = {
        sx * cy * cz + cx * sy * sz,
        cx * sy * cz - sx * cy * sz,
        cx * cy * sz + sx * sy * cz,
        cx * cy * cz - sx * sy * sz
    };
    return q;
}

quat_t quat_from_mat3(mat3_t m) {
    quat_t q;
    float trace = m.m[0] + m.m[4] + m.m[8];
    
    if (trace > 0.0f) {
        float s = sqrtf(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (m.m[5] - m.m[7]) / s;  // (m[1][2] - m[2][1]) / s
        q.y = (m.m[6] - m.m[2]) / s;  // (m[2][0] - m[0][2]) / s
        q.z = (m.m[1] - m.m[3]) / s;  // (m[0][1] - m[1][0]) / s
    } else if (m.m[0] > m.m[4] && m.m[0] > m.m[8]) {
        float s = sqrtf(1.0f + m.m[0] - m.m[4] - m.m[8]) * 2.0f;
        q.w = (m.m[5] - m.m[7]) / s;
        q.x = 0.25f * s;
        q.y = (m.m[1] + m.m[3]) / s;
        q.z = (m.m[6] + m.m[2]) / s;
    } else if (m.m[4] > m.m[8]) {
        float s = sqrtf(1.0f + m.m[4] - m.m[0] - m.m[8]) * 2.0f;
        q.w = (m.m[6] - m.m[2]) / s;
        q.x = (m.m[1] + m.m[3]) / s;
        q.y = 0.25f * s;
        q.z = (m.m[5] + m.m[7]) / s;
    } else {
        float s = sqrtf(1.0f + m.m[8] - m.m[0] - m.m[4]) * 2.0f;
        q.w = (m.m[1] - m.m[3]) / s;
        q.x = (m.m[6] + m.m[2]) / s;
        q.y = (m.m[5] + m.m[7]) / s;
        q.z = 0.25f * s;
    }
    
    return quat_normalize(q);
}

quat_t quat_from_mat4(mat4_t m) {
    // Extract upper-left 3x3 and convert
    mat3_t m3 = {{
        m.m[0], m.m[1], m.m[2],
        m.m[4], m.m[5], m.m[6],
        m.m[8], m.m[9], m.m[10]
    }};
    return quat_from_mat3(m3);
}

// ============================================================================
// Basic Operations
// ============================================================================

quat_t quat_add(quat_t a, quat_t b) {
#if USE_NEON
    float32x4_t va = quat_load(a);
    float32x4_t vb = quat_load(b);
    float32x4_t result = vaddq_f32(va, vb);
    return quat_store(result);
#else
    quat_t result = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    return result;
#endif
}

quat_t quat_sub(quat_t a, quat_t b) {
#if USE_NEON
    float32x4_t va = quat_load(a);
    float32x4_t vb = quat_load(b);
    float32x4_t result = vsubq_f32(va, vb);
    return quat_store(result);
#else
    quat_t result = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    return result;
#endif
}

quat_t quat_mul(quat_t a, quat_t b) {
    // Hamilton product
    // (a.w + a.x*i + a.y*j + a.z*k) * (b.w + b.x*i + b.y*j + b.z*k)
#if USE_NEON
    // Optimized quaternion multiplication using NEON
    float32x4_t qa = quat_load(a);
    float32x4_t qb = quat_load(b);
    
    // Extract components
    float32x4_t a_wwww = vdupq_lane_f32(vget_high_f32(qa), 1);
    float32x4_t a_xxxx = vdupq_lane_f32(vget_low_f32(qa), 0);
    float32x4_t a_yyyy = vdupq_lane_f32(vget_low_f32(qa), 1);
    float32x4_t a_zzzz = vdupq_lane_f32(vget_high_f32(qa), 0);
    
    // b components rearranged for the formula
    float32x4_t b_wzyx = { b.w, b.z, b.y, b.x };
    float32x4_t b_zwxy = { b.z, b.w, b.x, b.y };
    float32x4_t b_yxwz = { b.y, b.x, b.w, b.z };
    
    // Signs for the multiplication
    float32x4_t sign1 = { 1.0f, -1.0f, 1.0f, -1.0f };
    float32x4_t sign2 = { 1.0f, 1.0f, -1.0f, -1.0f };
    float32x4_t sign3 = { -1.0f, 1.0f, 1.0f, -1.0f };
    
    // result = a.w * b + a.x * b_wzyx * sign1 + a.y * b_zwxy * sign2 + a.z * b_yxwz * sign3
    float32x4_t result = vmulq_f32(a_wwww, qb);
    result = vmlaq_f32(result, vmulq_f32(a_xxxx, b_wzyx), sign1);
    result = vmlaq_f32(result, vmulq_f32(a_yyyy, b_zwxy), sign2);
    result = vmlaq_f32(result, vmulq_f32(a_zzzz, b_yxwz), sign3);
    
    return quat_store(result);
#else
    quat_t result = {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
    return result;
#endif
}

quat_t quat_scale(quat_t q, float s) {
#if USE_NEON
    float32x4_t vq = quat_load(q);
    float32x4_t vs = vdupq_n_f32(s);
    float32x4_t result = vmulq_f32(vq, vs);
    return quat_store(result);
#else
    quat_t result = { q.x * s, q.y * s, q.z * s, q.w * s };
    return result;
#endif
}

quat_t quat_conjugate(quat_t q) {
#if USE_NEON
    float32x4_t vq = quat_load(q);
    float32x4_t sign = { -1.0f, -1.0f, -1.0f, 1.0f };
    float32x4_t result = vmulq_f32(vq, sign);
    return quat_store(result);
#else
    quat_t result = { -q.x, -q.y, -q.z, q.w };
    return result;
#endif
}

quat_t quat_inverse(quat_t q) {
    float len_sq = quat_dot(q, q);
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return quat_identity();
    }
    
    quat_t conj = quat_conjugate(q);
    float inv_len_sq = 1.0f / len_sq;
    return quat_scale(conj, inv_len_sq);
}

float quat_dot(quat_t a, quat_t b) {
#if USE_NEON
    float32x4_t va = quat_load(a);
    float32x4_t vb = quat_load(b);
    float32x4_t prod = vmulq_f32(va, vb);
    return quat_hadd(prod);
#else
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
#endif
}

float quat_length(quat_t q) {
#if USE_NEON
    float len_sq = quat_dot(q, q);
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t est = vrsqrte_f32(v_len_sq);
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v_len_sq, est), est));
    est = vmul_f32(est, vrsqrts_f32(vmul_f32(v_len_sq, est), est));
    float32x2_t result = vmul_f32(v_len_sq, est);
    return vget_lane_f32(result, 0);
#else
    return sqrtf(quat_dot(q, q));
#endif
}

quat_t quat_normalize(quat_t q) {
#if USE_NEON
    float len_sq = quat_dot(q, q);
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return quat_identity();
    }
    
    float32x2_t v_len_sq = vdup_n_f32(len_sq);
    float32x2_t rsqrt = vrsqrte_f32(v_len_sq);
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    rsqrt = vmul_f32(rsqrt, vrsqrts_f32(vmul_f32(v_len_sq, rsqrt), rsqrt));
    
    float inv_len = vget_lane_f32(rsqrt, 0);
    return quat_scale(q, inv_len);
#else
    float len = quat_length(q);
    if (len < NEON_EPSILON) {
        return quat_identity();
    }
    return quat_scale(q, 1.0f / len);
#endif
}

// ============================================================================
// Interpolation
// ============================================================================

quat_t quat_lerp(quat_t a, quat_t b, float t) {
#if USE_NEON
    float32x4_t va = quat_load(a);
    float32x4_t vb = quat_load(b);
    float32x4_t vt = vdupq_n_f32(t);
    
    // result = a + t * (b - a)
    float32x4_t diff = vsubq_f32(vb, va);
    float32x4_t result = vmlaq_f32(va, diff, vt);
    
    return quat_normalize(quat_store(result));
#else
    quat_t result = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z),
        a.w + t * (b.w - a.w)
    };
    return quat_normalize(result);
#endif
}

quat_t quat_slerp(quat_t a, quat_t b, float t) {
    // Compute dot product
    float dot = quat_dot(a, b);
    
    // If dot is negative, negate one quaternion to take shorter path
    quat_t b_adj = b;
    if (dot < 0.0f) {
        b_adj.x = -b.x;
        b_adj.y = -b.y;
        b_adj.z = -b.z;
        b_adj.w = -b.w;
        dot = -dot;
    }
    
    // If quaternions are very close, use linear interpolation
    if (dot > 0.9995f) {
        return quat_lerp(a, b_adj, t);
    }
    
    // Clamp dot to valid range for acos
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;
    
    float theta = acosf(dot);
    float sin_theta = sinf(theta);
    
    if (fabsf(sin_theta) < NEON_EPSILON) {
        return quat_lerp(a, b_adj, t);
    }
    
    float s0 = sinf((1.0f - t) * theta) / sin_theta;
    float s1 = sinf(t * theta) / sin_theta;
    
#if USE_NEON
    float32x4_t va = quat_load(a);
    float32x4_t vb = quat_load(b_adj);
    float32x4_t vs0 = vdupq_n_f32(s0);
    float32x4_t vs1 = vdupq_n_f32(s1);
    
    float32x4_t result = vmlaq_f32(vmulq_f32(va, vs0), vb, vs1);
    return quat_store(result);
#else
    quat_t result = {
        s0 * a.x + s1 * b_adj.x,
        s0 * a.y + s1 * b_adj.y,
        s0 * a.z + s1 * b_adj.z,
        s0 * a.w + s1 * b_adj.w
    };
    return result;
#endif
}

// ============================================================================
// Conversion
// ============================================================================

mat3_t quat_to_mat3(quat_t q) {
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;
    
    float xx = q.x * x2;
    float xy = q.x * y2;
    float xz = q.x * z2;
    float yy = q.y * y2;
    float yz = q.y * z2;
    float zz = q.z * z2;
    float wx = q.w * x2;
    float wy = q.w * y2;
    float wz = q.w * z2;
    
    mat3_t m = {{
        1.0f - (yy + zz),  xy + wz,           xz - wy,           // Column 0
        xy - wz,           1.0f - (xx + zz),  yz + wx,           // Column 1
        xz + wy,           yz - wx,           1.0f - (xx + yy)   // Column 2
    }};
    return m;
}

mat4_t quat_to_mat4(quat_t q) {
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;
    
    float xx = q.x * x2;
    float xy = q.x * y2;
    float xz = q.x * z2;
    float yy = q.y * y2;
    float yz = q.y * z2;
    float zz = q.z * z2;
    float wx = q.w * x2;
    float wy = q.w * y2;
    float wz = q.w * z2;
    
    mat4_t m = {{
        1.0f - (yy + zz),  xy + wz,           xz - wy,           0.0f,  // Column 0
        xy - wz,           1.0f - (xx + zz),  yz + wx,           0.0f,  // Column 1
        xz + wy,           yz - wx,           1.0f - (xx + yy),  0.0f,  // Column 2
        0.0f,              0.0f,              0.0f,              1.0f   // Column 3
    }};
    return m;
}

void quat_to_axis_angle(quat_t q, vec3_t* axis, float* angle) {
    quat_t qn = quat_normalize(q);
    
    // Ensure w is in valid range for acos
    float w = qn.w;
    if (w > 1.0f) w = 1.0f;
    if (w < -1.0f) w = -1.0f;
    
    float half_angle = acosf(w);
    float sin_half = sinf(half_angle);
    
    if (angle) {
        *angle = half_angle * 2.0f;
    }
    
    if (axis) {
        if (fabsf(sin_half) > NEON_EPSILON) {
            float inv_sin = 1.0f / sin_half;
            axis->x = qn.x * inv_sin;
            axis->y = qn.y * inv_sin;
            axis->z = qn.z * inv_sin;
            axis->_pad = 0.0f;
        } else {
            // Angle is close to 0, return arbitrary axis
            axis->x = 1.0f;
            axis->y = 0.0f;
            axis->z = 0.0f;
            axis->_pad = 0.0f;
        }
    }
}

vec3_t quat_to_euler(quat_t q) {
    // Convert to Euler angles (XYZ order)
    vec3_t euler;
    
    // Roll (x-axis rotation)
    float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    euler.x = atan2f(sinr_cosp, cosr_cosp);
    
    // Pitch (y-axis rotation)
    float sinp = 2.0f * (q.w * q.y - q.z * q.x);
    if (fabsf(sinp) >= 1.0f) {
        euler.y = copysignf(NEON_HALF_PI, sinp);  // Use 90 degrees if out of range
    } else {
        euler.y = asinf(sinp);
    }
    
    // Yaw (z-axis rotation)
    float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    euler.z = atan2f(siny_cosp, cosy_cosp);
    
    euler._pad = 0.0f;
    return euler;
}

// ============================================================================
// Rotation
// ============================================================================

vec3_t quat_rotate_vec3(quat_t q, vec3_t v) {
    // Optimized quaternion-vector rotation: v' = q * v * q^-1
    // Using the formula: v' = v + 2 * q.w * (q.xyz x v) + 2 * (q.xyz x (q.xyz x v))
    
#if USE_NEON
    vec3_t qv = { q.x, q.y, q.z, 0.0f };
    
    // t = 2 * cross(q.xyz, v)
    vec3_t t = vec3_cross(qv, v);
    t = vec3_scale(t, 2.0f);
    
    // result = v + q.w * t + cross(q.xyz, t)
    vec3_t wt = vec3_scale(t, q.w);
    vec3_t qt = vec3_cross(qv, t);
    
    return vec3_add(vec3_add(v, wt), qt);
#else
    vec3_t qv = { q.x, q.y, q.z, 0.0f };
    
    vec3_t t = vec3_cross(qv, v);
    t = vec3_scale(t, 2.0f);
    
    vec3_t wt = vec3_scale(t, q.w);
    vec3_t qt = vec3_cross(qv, t);
    
    return vec3_add(vec3_add(v, wt), qt);
#endif
}

// ============================================================================
// Utilities
// ============================================================================

quat_t quat_look_rotation(vec3_t forward, vec3_t up) {
    // Create quaternion that rotates from (0,0,-1) to forward direction
    vec3_t f = vec3_normalize(forward);
    vec3_t r = vec3_normalize(vec3_cross(up, f));
    vec3_t u = vec3_cross(f, r);
    
    // Build rotation matrix and convert to quaternion
    mat3_t m = {{
        r.x, u.x, -f.x,
        r.y, u.y, -f.y,
        r.z, u.z, -f.z
    }};
    
    return quat_from_mat3(m);
}

float quat_angle(quat_t a, quat_t b) {
    float dot = quat_dot(a, b);
    
    // Clamp to valid range
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;
    
    // angle = 2 * acos(|dot|)
    return 2.0f * acosf(fabsf(dot));
}
