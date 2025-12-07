// ============================================================================
// mat4_t NEON Implementation for ARMv7
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
// Creation Functions
// ============================================================================

mat4_t mat4_identity(void) {
    mat4_t m = {{
        1.0f, 0.0f, 0.0f, 0.0f,  // Column 0
        0.0f, 1.0f, 0.0f, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f, 0.0f,  // Column 2
        0.0f, 0.0f, 0.0f, 1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_zero(void) {
    mat4_t m = {{
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    }};
    return m;
}

mat4_t mat4_from_rows(vec4_t r0, vec4_t r1, vec4_t r2, vec4_t r3) {
    // Convert from row-major input to column-major storage
    mat4_t m = {{
        r0.x, r1.x, r2.x, r3.x,  // Column 0
        r0.y, r1.y, r2.y, r3.y,  // Column 1
        r0.z, r1.z, r2.z, r3.z,  // Column 2
        r0.w, r1.w, r2.w, r3.w   // Column 3
    }};
    return m;
}

mat4_t mat4_from_cols(vec4_t c0, vec4_t c1, vec4_t c2, vec4_t c3) {
    mat4_t m = {{
        c0.x, c0.y, c0.z, c0.w,  // Column 0
        c1.x, c1.y, c1.z, c1.w,  // Column 1
        c2.x, c2.y, c2.z, c2.w,  // Column 2
        c3.x, c3.y, c3.z, c3.w   // Column 3
    }};
    return m;
}

// ============================================================================
// Basic Operations
// ============================================================================

mat4_t mat4_add(mat4_t a, mat4_t b) {
#if USE_NEON
    mat4_t result;
    for (int i = 0; i < 4; i++) {
        float32x4_t va = vld1q_f32(&a.m[i * 4]);
        float32x4_t vb = vld1q_f32(&b.m[i * 4]);
        float32x4_t vr = vaddq_f32(va, vb);
        vst1q_f32(&result.m[i * 4], vr);
    }
    return result;
#else
    mat4_t result;
    for (int i = 0; i < 16; i++) {
        result.m[i] = a.m[i] + b.m[i];
    }
    return result;
#endif
}

mat4_t mat4_sub(mat4_t a, mat4_t b) {
#if USE_NEON
    mat4_t result;
    for (int i = 0; i < 4; i++) {
        float32x4_t va = vld1q_f32(&a.m[i * 4]);
        float32x4_t vb = vld1q_f32(&b.m[i * 4]);
        float32x4_t vr = vsubq_f32(va, vb);
        vst1q_f32(&result.m[i * 4], vr);
    }
    return result;
#else
    mat4_t result;
    for (int i = 0; i < 16; i++) {
        result.m[i] = a.m[i] - b.m[i];
    }
    return result;
#endif
}

mat4_t mat4_scale_scalar(mat4_t m, float s) {
#if USE_NEON
    mat4_t result;
    float32x4_t vs = vdupq_n_f32(s);
    for (int i = 0; i < 4; i++) {
        float32x4_t vm = vld1q_f32(&m.m[i * 4]);
        float32x4_t vr = vmulq_f32(vm, vs);
        vst1q_f32(&result.m[i * 4], vr);
    }
    return result;
#else
    mat4_t result;
    for (int i = 0; i < 16; i++) {
        result.m[i] = m.m[i] * s;
    }
    return result;
#endif
}

mat4_t mat4_mul(mat4_t a, mat4_t b) {
#if USE_NEON
    mat4_t result;
    
    // Load all columns of A
    float32x4_t a_col0 = vld1q_f32(&a.m[0]);
    float32x4_t a_col1 = vld1q_f32(&a.m[4]);
    float32x4_t a_col2 = vld1q_f32(&a.m[8]);
    float32x4_t a_col3 = vld1q_f32(&a.m[12]);
    
    // For each column of result
    for (int col = 0; col < 4; col++) {
        float32x4_t b_col = vld1q_f32(&b.m[col * 4]);
        
        // Broadcast each element of b_col
        float32x4_t b0 = vdupq_lane_f32(vget_low_f32(b_col), 0);
        float32x4_t b1 = vdupq_lane_f32(vget_low_f32(b_col), 1);
        float32x4_t b2 = vdupq_lane_f32(vget_high_f32(b_col), 0);
        float32x4_t b3 = vdupq_lane_f32(vget_high_f32(b_col), 1);
        
        // result_col = a_col0 * b[0] + a_col1 * b[1] + a_col2 * b[2] + a_col3 * b[3]
        float32x4_t res = vmulq_f32(a_col0, b0);
        res = vmlaq_f32(res, a_col1, b1);
        res = vmlaq_f32(res, a_col2, b2);
        res = vmlaq_f32(res, a_col3, b3);
        
        vst1q_f32(&result.m[col * 4], res);
    }
    return result;
#else
    mat4_t result;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
            result.m[col * 4 + row] = sum;
        }
    }
    return result;
#endif
}

mat4_t mat4_transpose(mat4_t m) {
#if USE_NEON
    mat4_t result;
    
    // Load all columns
    float32x4_t col0 = vld1q_f32(&m.m[0]);
    float32x4_t col1 = vld1q_f32(&m.m[4]);
    float32x4_t col2 = vld1q_f32(&m.m[8]);
    float32x4_t col3 = vld1q_f32(&m.m[12]);
    
    // Transpose using NEON intrinsics
    // First, interleave pairs
    float32x4x2_t tmp01 = vtrnq_f32(col0, col1);  // {a0,b0,a2,b2}, {a1,b1,a3,b3}
    float32x4x2_t tmp23 = vtrnq_f32(col2, col3);  // {c0,d0,c2,d2}, {c1,d1,c3,d3}
    
    // Now combine low and high parts
    float32x4_t row0 = vcombine_f32(vget_low_f32(tmp01.val[0]), vget_low_f32(tmp23.val[0]));
    float32x4_t row1 = vcombine_f32(vget_low_f32(tmp01.val[1]), vget_low_f32(tmp23.val[1]));
    float32x4_t row2 = vcombine_f32(vget_high_f32(tmp01.val[0]), vget_high_f32(tmp23.val[0]));
    float32x4_t row3 = vcombine_f32(vget_high_f32(tmp01.val[1]), vget_high_f32(tmp23.val[1]));
    
    vst1q_f32(&result.m[0], row0);
    vst1q_f32(&result.m[4], row1);
    vst1q_f32(&result.m[8], row2);
    vst1q_f32(&result.m[12], row3);
    
    return result;
#else
    mat4_t result = {{
        m.m[0], m.m[4], m.m[8],  m.m[12],
        m.m[1], m.m[5], m.m[9],  m.m[13],
        m.m[2], m.m[6], m.m[10], m.m[14],
        m.m[3], m.m[7], m.m[11], m.m[15]
    }};
    return result;
#endif
}

float mat4_determinant(mat4_t m) {
    // Compute 2x2 determinants for bottom rows
    float s0 = m.m[0] * m.m[5] - m.m[4] * m.m[1];
    float s1 = m.m[0] * m.m[9] - m.m[8] * m.m[1];
    float s2 = m.m[0] * m.m[13] - m.m[12] * m.m[1];
    float s3 = m.m[4] * m.m[9] - m.m[8] * m.m[5];
    float s4 = m.m[4] * m.m[13] - m.m[12] * m.m[5];
    float s5 = m.m[8] * m.m[13] - m.m[12] * m.m[9];
    
    float c5 = m.m[10] * m.m[15] - m.m[14] * m.m[11];
    float c4 = m.m[6] * m.m[15] - m.m[14] * m.m[7];
    float c3 = m.m[6] * m.m[11] - m.m[10] * m.m[7];
    float c2 = m.m[2] * m.m[15] - m.m[14] * m.m[3];
    float c1 = m.m[2] * m.m[11] - m.m[10] * m.m[3];
    float c0 = m.m[2] * m.m[7] - m.m[6] * m.m[3];
    
    return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
}

mat4_t mat4_inverse(mat4_t m) {
    // Compute 2x2 determinants
    float s0 = m.m[0] * m.m[5] - m.m[4] * m.m[1];
    float s1 = m.m[0] * m.m[9] - m.m[8] * m.m[1];
    float s2 = m.m[0] * m.m[13] - m.m[12] * m.m[1];
    float s3 = m.m[4] * m.m[9] - m.m[8] * m.m[5];
    float s4 = m.m[4] * m.m[13] - m.m[12] * m.m[5];
    float s5 = m.m[8] * m.m[13] - m.m[12] * m.m[9];
    
    float c5 = m.m[10] * m.m[15] - m.m[14] * m.m[11];
    float c4 = m.m[6] * m.m[15] - m.m[14] * m.m[7];
    float c3 = m.m[6] * m.m[11] - m.m[10] * m.m[7];
    float c2 = m.m[2] * m.m[15] - m.m[14] * m.m[3];
    float c1 = m.m[2] * m.m[11] - m.m[10] * m.m[3];
    float c0 = m.m[2] * m.m[7] - m.m[6] * m.m[3];
    
    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    
    if (fabsf(det) < NEON_EPSILON) {
        return mat4_identity();  // Return identity for singular matrix
    }
    
    float inv_det = 1.0f / det;
    
    mat4_t result;
    
    result.m[0]  = ( m.m[5] * c5 - m.m[9] * c4 + m.m[13] * c3) * inv_det;
    result.m[1]  = (-m.m[1] * c5 + m.m[9] * c2 - m.m[13] * c1) * inv_det;
    result.m[2]  = ( m.m[1] * c4 - m.m[5] * c2 + m.m[13] * c0) * inv_det;
    result.m[3]  = (-m.m[1] * c3 + m.m[5] * c1 - m.m[9] * c0) * inv_det;
    
    result.m[4]  = (-m.m[4] * c5 + m.m[8] * c4 - m.m[12] * c3) * inv_det;
    result.m[5]  = ( m.m[0] * c5 - m.m[8] * c2 + m.m[12] * c1) * inv_det;
    result.m[6]  = (-m.m[0] * c4 + m.m[4] * c2 - m.m[12] * c0) * inv_det;
    result.m[7]  = ( m.m[0] * c3 - m.m[4] * c1 + m.m[8] * c0) * inv_det;
    
    result.m[8]  = ( m.m[4] * s5 - m.m[8] * s4 + m.m[12] * s3) * inv_det;
    result.m[9]  = (-m.m[0] * s5 + m.m[8] * s2 - m.m[12] * s1) * inv_det;
    result.m[10] = ( m.m[0] * s4 - m.m[4] * s2 + m.m[12] * s0) * inv_det;
    result.m[11] = (-m.m[0] * s3 + m.m[4] * s1 - m.m[8] * s0) * inv_det;
    
    result.m[12] = (-m.m[7] * s5 + m.m[11] * s4 - m.m[15] * s3) * inv_det;
    result.m[13] = ( m.m[3] * s5 - m.m[11] * s2 + m.m[15] * s1) * inv_det;
    result.m[14] = (-m.m[3] * s4 + m.m[7] * s2 - m.m[15] * s0) * inv_det;
    result.m[15] = ( m.m[3] * s3 - m.m[7] * s1 + m.m[11] * s0) * inv_det;
    
    return result;
}

// ============================================================================
// Transformation Matrices
// ============================================================================

mat4_t mat4_translation(float tx, float ty, float tz) {
    mat4_t m = {{
        1.0f, 0.0f, 0.0f, 0.0f,  // Column 0
        0.0f, 1.0f, 0.0f, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f, 0.0f,  // Column 2
          tx,   ty,   tz, 1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_translation_vec3(vec3_t t) {
    return mat4_translation(t.x, t.y, t.z);
}

mat4_t mat4_scaling(float sx, float sy, float sz) {
    mat4_t m = {{
          sx, 0.0f, 0.0f, 0.0f,  // Column 0
        0.0f,   sy, 0.0f, 0.0f,  // Column 1
        0.0f, 0.0f,   sz, 0.0f,  // Column 2
        0.0f, 0.0f, 0.0f, 1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_scaling_uniform(float s) {
    return mat4_scaling(s, s, s);
}

mat4_t mat4_scaling_vec3(vec3_t s) {
    return mat4_scaling(s.x, s.y, s.z);
}

mat4_t mat4_rotation_x(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat4_t m = {{
        1.0f, 0.0f, 0.0f, 0.0f,  // Column 0
        0.0f,    c,    s, 0.0f,  // Column 1
        0.0f,   -s,    c, 0.0f,  // Column 2
        0.0f, 0.0f, 0.0f, 1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_rotation_y(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat4_t m = {{
           c, 0.0f,   -s, 0.0f,  // Column 0
        0.0f, 1.0f, 0.0f, 0.0f,  // Column 1
           s, 0.0f,    c, 0.0f,  // Column 2
        0.0f, 0.0f, 0.0f, 1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_rotation_z(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat4_t m = {{
           c,    s, 0.0f, 0.0f,  // Column 0
          -s,    c, 0.0f, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f, 0.0f,  // Column 2
        0.0f, 0.0f, 0.0f, 1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_rotation_axis(vec3_t axis, float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    float t = 1.0f - c;
    
    // Normalize axis
    float len_sq = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return mat4_identity();
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
    
    float x = axis.x * inv_len;
    float y = axis.y * inv_len;
    float z = axis.z * inv_len;
    
    mat4_t m = {{
        t*x*x + c,     t*x*y + s*z,   t*x*z - s*y,   0.0f,  // Column 0
        t*x*y - s*z,   t*y*y + c,     t*y*z + s*x,   0.0f,  // Column 1
        t*x*z + s*y,   t*y*z - s*x,   t*z*z + c,     0.0f,  // Column 2
        0.0f,          0.0f,          0.0f,          1.0f   // Column 3
    }};
    return m;
}

mat4_t mat4_rotation_euler(float pitch, float yaw, float roll) {
    // XYZ order: first X (pitch), then Y (yaw), then Z (roll)
    float cx = cosf(pitch);
    float sx = sinf(pitch);
    float cy = cosf(yaw);
    float sy = sinf(yaw);
    float cz = cosf(roll);
    float sz = sinf(roll);
    
    mat4_t m = {{
        cy*cz,                  cy*sz,                 -sy,    0.0f,  // Column 0
        sx*sy*cz - cx*sz,       sx*sy*sz + cx*cz,      sx*cy,  0.0f,  // Column 1
        cx*sy*cz + sx*sz,       cx*sy*sz - sx*cz,      cx*cy,  0.0f,  // Column 2
        0.0f,                   0.0f,                  0.0f,   1.0f   // Column 3
    }};
    return m;
}

// ============================================================================
// Compose / Decompose
// ============================================================================

mat4_t mat4_compose(vec3_t translation, quat_t rotation, vec3_t scale) {
    // Build rotation matrix from quaternion
    float x2 = rotation.x + rotation.x;
    float y2 = rotation.y + rotation.y;
    float z2 = rotation.z + rotation.z;
    
    float xx = rotation.x * x2;
    float xy = rotation.x * y2;
    float xz = rotation.x * z2;
    float yy = rotation.y * y2;
    float yz = rotation.y * z2;
    float zz = rotation.z * z2;
    float wx = rotation.w * x2;
    float wy = rotation.w * y2;
    float wz = rotation.w * z2;
    
    mat4_t m = {{
        (1.0f - (yy + zz)) * scale.x,  (xy + wz) * scale.x,           (xz - wy) * scale.x,           0.0f,
        (xy - wz) * scale.y,           (1.0f - (xx + zz)) * scale.y,  (yz + wx) * scale.y,           0.0f,
        (xz + wy) * scale.z,           (yz - wx) * scale.z,           (1.0f - (xx + yy)) * scale.z,  0.0f,
        translation.x,                  translation.y,                  translation.z,                  1.0f
    }};
    return m;
}

void mat4_decompose(mat4_t m, vec3_t* translation, quat_t* rotation, vec3_t* scale) {
    // Extract translation
    if (translation) {
        translation->x = m.m[12];
        translation->y = m.m[13];
        translation->z = m.m[14];
        translation->_pad = 0.0f;
    }
    
    // Extract scale (length of each column)
    float sx = sqrtf(m.m[0]*m.m[0] + m.m[1]*m.m[1] + m.m[2]*m.m[2]);
    float sy = sqrtf(m.m[4]*m.m[4] + m.m[5]*m.m[5] + m.m[6]*m.m[6]);
    float sz = sqrtf(m.m[8]*m.m[8] + m.m[9]*m.m[9] + m.m[10]*m.m[10]);
    
    if (scale) {
        scale->x = sx;
        scale->y = sy;
        scale->z = sz;
        scale->_pad = 0.0f;
    }
    
    // Extract rotation (normalize the rotation part)
    if (rotation) {
        float inv_sx = (sx > NEON_EPSILON) ? 1.0f / sx : 0.0f;
        float inv_sy = (sy > NEON_EPSILON) ? 1.0f / sy : 0.0f;
        float inv_sz = (sz > NEON_EPSILON) ? 1.0f / sz : 0.0f;
        
        float r00 = m.m[0] * inv_sx;
        float r01 = m.m[1] * inv_sx;
        float r02 = m.m[2] * inv_sx;
        float r10 = m.m[4] * inv_sy;
        float r11 = m.m[5] * inv_sy;
        float r12 = m.m[6] * inv_sy;
        float r20 = m.m[8] * inv_sz;
        float r21 = m.m[9] * inv_sz;
        float r22 = m.m[10] * inv_sz;
        
        // Convert rotation matrix to quaternion
        float trace = r00 + r11 + r22;
        
        if (trace > 0.0f) {
            float s = sqrtf(trace + 1.0f) * 2.0f;
            rotation->w = 0.25f * s;
            rotation->x = (r12 - r21) / s;
            rotation->y = (r20 - r02) / s;
            rotation->z = (r01 - r10) / s;
        } else if (r00 > r11 && r00 > r22) {
            float s = sqrtf(1.0f + r00 - r11 - r22) * 2.0f;
            rotation->w = (r12 - r21) / s;
            rotation->x = 0.25f * s;
            rotation->y = (r01 + r10) / s;
            rotation->z = (r20 + r02) / s;
        } else if (r11 > r22) {
            float s = sqrtf(1.0f + r11 - r00 - r22) * 2.0f;
            rotation->w = (r20 - r02) / s;
            rotation->x = (r01 + r10) / s;
            rotation->y = 0.25f * s;
            rotation->z = (r12 + r21) / s;
        } else {
            float s = sqrtf(1.0f + r22 - r00 - r11) * 2.0f;
            rotation->w = (r01 - r10) / s;
            rotation->x = (r20 + r02) / s;
            rotation->y = (r12 + r21) / s;
            rotation->z = 0.25f * s;
        }
    }
}

vec3_t mat4_get_translation(mat4_t m) {
    vec3_t t = { m.m[12], m.m[13], m.m[14], 0.0f };
    return t;
}

vec3_t mat4_get_scale(mat4_t m) {
    float sx = sqrtf(m.m[0]*m.m[0] + m.m[1]*m.m[1] + m.m[2]*m.m[2]);
    float sy = sqrtf(m.m[4]*m.m[4] + m.m[5]*m.m[5] + m.m[6]*m.m[6]);
    float sz = sqrtf(m.m[8]*m.m[8] + m.m[9]*m.m[9] + m.m[10]*m.m[10]);
    vec3_t s = { sx, sy, sz, 0.0f };
    return s;
}

// ============================================================================
// Camera / View Matrices
// ============================================================================

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
    return mat4_look_at_rh(eye, target, up);
}

mat4_t mat4_look_at_rh(vec3_t eye, vec3_t target, vec3_t up) {
    // Forward (z-axis points toward viewer in RH)
    vec3_t f = vec3_normalize(vec3_sub(target, eye));
    vec3_t r = vec3_normalize(vec3_cross(f, up));
    vec3_t u = vec3_cross(r, f);
    
    mat4_t m = {{
        r.x,           u.x,          -f.x,          0.0f,
        r.y,           u.y,          -f.y,          0.0f,
        r.z,           u.z,          -f.z,          0.0f,
        -vec3_dot(r, eye), -vec3_dot(u, eye), vec3_dot(f, eye), 1.0f
    }};
    return m;
}

mat4_t mat4_look_at_lh(vec3_t eye, vec3_t target, vec3_t up) {
    vec3_t f = vec3_normalize(vec3_sub(target, eye));
    vec3_t r = vec3_normalize(vec3_cross(up, f));
    vec3_t u = vec3_cross(f, r);
    
    mat4_t m = {{
        r.x,           u.x,           f.x,          0.0f,
        r.y,           u.y,           f.y,          0.0f,
        r.z,           u.z,           f.z,          0.0f,
        -vec3_dot(r, eye), -vec3_dot(u, eye), -vec3_dot(f, eye), 1.0f
    }};
    return m;
}

// ============================================================================
// Projection Matrices (Right-handed, depth [0,1])
// ============================================================================

mat4_t mat4_perspective(float fov_radians, float aspect, float near_plane, float far_plane) {
    float tan_half_fov = tanf(fov_radians * 0.5f);
    float range = far_plane - near_plane;
    
    mat4_t m = mat4_zero();
    m.m[0]  = 1.0f / (aspect * tan_half_fov);
    m.m[5]  = 1.0f / tan_half_fov;
    m.m[10] = -far_plane / range;
    m.m[11] = -1.0f;
    m.m[14] = -(far_plane * near_plane) / range;
    return m;
}

mat4_t mat4_perspective_fov(float fov_y_radians, float width, float height, float near_plane, float far_plane) {
    float aspect = width / height;
    return mat4_perspective(fov_y_radians, aspect, near_plane, far_plane);
}

mat4_t mat4_perspective_infinite(float fov_radians, float aspect, float near_plane) {
    float tan_half_fov = tanf(fov_radians * 0.5f);
    
    mat4_t m = mat4_zero();
    m.m[0]  = 1.0f / (aspect * tan_half_fov);
    m.m[5]  = 1.0f / tan_half_fov;
    m.m[10] = -1.0f;
    m.m[11] = -1.0f;
    m.m[14] = -near_plane;
    return m;
}

mat4_t mat4_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane) {
    float rl = right - left;
    float tb = top - bottom;
    float fn = far_plane - near_plane;
    
    mat4_t m = mat4_zero();
    m.m[0]  = 2.0f / rl;
    m.m[5]  = 2.0f / tb;
    m.m[10] = -1.0f / fn;
    m.m[12] = -(right + left) / rl;
    m.m[13] = -(top + bottom) / tb;
    m.m[14] = -near_plane / fn;
    m.m[15] = 1.0f;
    return m;
}

mat4_t mat4_ortho_symmetric(float width, float height, float near_plane, float far_plane) {
    return mat4_ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, near_plane, far_plane);
}

// ============================================================================
// Projection Matrices (OpenGL style, depth [-1,1])
// ============================================================================

mat4_t mat4_perspective_gl(float fov_radians, float aspect, float near_plane, float far_plane) {
    float tan_half_fov = tanf(fov_radians * 0.5f);
    float range = far_plane - near_plane;
    
    mat4_t m = mat4_zero();
    m.m[0]  = 1.0f / (aspect * tan_half_fov);
    m.m[5]  = 1.0f / tan_half_fov;
    m.m[10] = -(far_plane + near_plane) / range;
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * far_plane * near_plane) / range;
    return m;
}

mat4_t mat4_ortho_gl(float left, float right, float bottom, float top, float near_plane, float far_plane) {
    float rl = right - left;
    float tb = top - bottom;
    float fn = far_plane - near_plane;
    
    mat4_t m = mat4_zero();
    m.m[0]  = 2.0f / rl;
    m.m[5]  = 2.0f / tb;
    m.m[10] = -2.0f / fn;
    m.m[12] = -(right + left) / rl;
    m.m[13] = -(top + bottom) / tb;
    m.m[14] = -(far_plane + near_plane) / fn;
    m.m[15] = 1.0f;
    return m;
}

// ============================================================================
// Projection Matrices (Left-handed)
// ============================================================================

mat4_t mat4_perspective_lh(float fov_radians, float aspect, float near_plane, float far_plane) {
    float tan_half_fov = tanf(fov_radians * 0.5f);
    float range = far_plane - near_plane;
    
    mat4_t m = mat4_zero();
    m.m[0]  = 1.0f / (aspect * tan_half_fov);
    m.m[5]  = 1.0f / tan_half_fov;
    m.m[10] = far_plane / range;
    m.m[11] = 1.0f;
    m.m[14] = -(far_plane * near_plane) / range;
    return m;
}

mat4_t mat4_ortho_lh(float left, float right, float bottom, float top, float near_plane, float far_plane) {
    float rl = right - left;
    float tb = top - bottom;
    float fn = far_plane - near_plane;
    
    mat4_t m = mat4_zero();
    m.m[0]  = 2.0f / rl;
    m.m[5]  = 2.0f / tb;
    m.m[10] = 1.0f / fn;
    m.m[12] = -(right + left) / rl;
    m.m[13] = -(top + bottom) / tb;
    m.m[14] = -near_plane / fn;
    m.m[15] = 1.0f;
    return m;
}

// ============================================================================
// Camera Utilities
// ============================================================================

vec3_t camera_project(vec3_t world_pos, mat4_t view_projection, float viewport_x, float viewport_y, float viewport_width, float viewport_height) {
    // Transform to clip space
    vec4_t clip = vec4_transform_mat4(vec4_from_vec3(world_pos, 1.0f), view_projection);
    
    // Perspective divide
    if (fabsf(clip.w) < NEON_EPSILON) {
        return vec3_zero();
    }
    
    float inv_w = 1.0f / clip.w;
    vec3_t ndc = { clip.x * inv_w, clip.y * inv_w, clip.z * inv_w, 0.0f };
    
    // Map to viewport
    vec3_t screen = {
        viewport_x + (ndc.x * 0.5f + 0.5f) * viewport_width,
        viewport_y + (ndc.y * 0.5f + 0.5f) * viewport_height,
        ndc.z,
        0.0f
    };
    return screen;
}

vec3_t camera_unproject(vec3_t screen_pos, mat4_t view_projection_inverse, float viewport_x, float viewport_y, float viewport_width, float viewport_height) {
    // Map from viewport to NDC
    vec3_t ndc = {
        (screen_pos.x - viewport_x) / viewport_width * 2.0f - 1.0f,
        (screen_pos.y - viewport_y) / viewport_height * 2.0f - 1.0f,
        screen_pos.z,
        0.0f
    };
    
    // Transform by inverse view-projection
    vec4_t world = vec4_transform_mat4(vec4_create(ndc.x, ndc.y, ndc.z, 1.0f), view_projection_inverse);
    
    // Perspective divide
    if (fabsf(world.w) < NEON_EPSILON) {
        return vec3_zero();
    }
    
    float inv_w = 1.0f / world.w;
    vec3_t result = { world.x * inv_w, world.y * inv_w, world.z * inv_w, 0.0f };
    return result;
}

void camera_ray_from_screen(vec3_t screen_pos, mat4_t view_projection_inverse, float viewport_x, float viewport_y, float viewport_width, float viewport_height, vec3_t* ray_origin, vec3_t* ray_direction) {
    // Near plane point
    vec3_t near_pos = screen_pos;
    near_pos.z = 0.0f;
    vec3_t near_world = camera_unproject(near_pos, view_projection_inverse, viewport_x, viewport_y, viewport_width, viewport_height);
    
    // Far plane point
    vec3_t far_pos = screen_pos;
    far_pos.z = 1.0f;
    vec3_t far_world = camera_unproject(far_pos, view_projection_inverse, viewport_x, viewport_y, viewport_width, viewport_height);
    
    if (ray_origin) {
        *ray_origin = near_world;
    }
    if (ray_direction) {
        *ray_direction = vec3_normalize(vec3_sub(far_world, near_world));
    }
}

// ============================================================================
// Frustum Functions
// ============================================================================

void camera_extract_frustum_planes(mat4_t vp, vec4_t planes[6]) {
    // Extract frustum planes from view-projection matrix
    // Planes are: Left, Right, Bottom, Top, Near, Far
    // Each plane: ax + by + cz + d = 0, stored as (a, b, c, d)
    
    // Left plane
    planes[0].x = vp.m[3] + vp.m[0];
    planes[0].y = vp.m[7] + vp.m[4];
    planes[0].z = vp.m[11] + vp.m[8];
    planes[0].w = vp.m[15] + vp.m[12];
    
    // Right plane
    planes[1].x = vp.m[3] - vp.m[0];
    planes[1].y = vp.m[7] - vp.m[4];
    planes[1].z = vp.m[11] - vp.m[8];
    planes[1].w = vp.m[15] - vp.m[12];
    
    // Bottom plane
    planes[2].x = vp.m[3] + vp.m[1];
    planes[2].y = vp.m[7] + vp.m[5];
    planes[2].z = vp.m[11] + vp.m[9];
    planes[2].w = vp.m[15] + vp.m[13];
    
    // Top plane
    planes[3].x = vp.m[3] - vp.m[1];
    planes[3].y = vp.m[7] - vp.m[5];
    planes[3].z = vp.m[11] - vp.m[9];
    planes[3].w = vp.m[15] - vp.m[13];
    
    // Near plane
    planes[4].x = vp.m[3] + vp.m[2];
    planes[4].y = vp.m[7] + vp.m[6];
    planes[4].z = vp.m[11] + vp.m[10];
    planes[4].w = vp.m[15] + vp.m[14];
    
    // Far plane
    planes[5].x = vp.m[3] - vp.m[2];
    planes[5].y = vp.m[7] - vp.m[6];
    planes[5].z = vp.m[11] - vp.m[10];
    planes[5].w = vp.m[15] - vp.m[14];
    
    // Normalize all planes
    for (int i = 0; i < 6; i++) {
        float len = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
        if (len > NEON_EPSILON) {
            float inv_len = 1.0f / len;
            planes[i].x *= inv_len;
            planes[i].y *= inv_len;
            planes[i].z *= inv_len;
            planes[i].w *= inv_len;
        }
    }
}

int camera_point_in_frustum(vec4_t planes[6], vec3_t point) {
    for (int i = 0; i < 6; i++) {
        float dist = planes[i].x * point.x + planes[i].y * point.y + planes[i].z * point.z + planes[i].w;
        if (dist < 0.0f) {
            return 0;  // Outside
        }
    }
    return 1;  // Inside
}

int camera_sphere_in_frustum(vec4_t planes[6], vec3_t center, float radius) {
    for (int i = 0; i < 6; i++) {
        float dist = planes[i].x * center.x + planes[i].y * center.y + planes[i].z * center.z + planes[i].w;
        if (dist < -radius) {
            return 0;  // Outside
        }
    }
    return 1;  // Inside or intersecting
}

int camera_aabb_in_frustum(vec4_t planes[6], vec3_t min_point, vec3_t max_point) {
    for (int i = 0; i < 6; i++) {
        // Find the positive vertex (furthest along plane normal)
        vec3_t p = min_point;
        if (planes[i].x >= 0.0f) p.x = max_point.x;
        if (planes[i].y >= 0.0f) p.y = max_point.y;
        if (planes[i].z >= 0.0f) p.z = max_point.z;
        
        float dist = planes[i].x * p.x + planes[i].y * p.y + planes[i].z * p.z + planes[i].w;
        if (dist < 0.0f) {
            return 0;  // Outside
        }
    }
    return 1;  // Inside or intersecting
}
