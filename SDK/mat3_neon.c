// ============================================================================
// mat3_t NEON Implementation for ARMv7
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

mat3_t mat3_identity(void) {
    mat3_t m = {{
        1.0f, 0.0f, 0.0f,  // Column 0
        0.0f, 1.0f, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f   // Column 2
    }};
    return m;
}

mat3_t mat3_zero(void) {
    mat3_t m = {{
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    }};
    return m;
}

mat3_t mat3_from_rows(vec3_t r0, vec3_t r1, vec3_t r2) {
    // Convert from row-major input to column-major storage
    mat3_t m = {{
        r0.x, r1.x, r2.x,  // Column 0
        r0.y, r1.y, r2.y,  // Column 1
        r0.z, r1.z, r2.z   // Column 2
    }};
    return m;
}

mat3_t mat3_from_cols(vec3_t c0, vec3_t c1, vec3_t c2) {
    mat3_t m = {{
        c0.x, c0.y, c0.z,  // Column 0
        c1.x, c1.y, c1.z,  // Column 1
        c2.x, c2.y, c2.z   // Column 2
    }};
    return m;
}

mat3_t mat3_from_mat4(mat4_t src) {
    // Extract upper-left 3x3 from 4x4 matrix
    mat3_t m = {{
        src.m[0], src.m[1], src.m[2],    // Column 0
        src.m[4], src.m[5], src.m[6],    // Column 1
        src.m[8], src.m[9], src.m[10]    // Column 2
    }};
    return m;
}

// ============================================================================
// Basic Operations
// ============================================================================

mat3_t mat3_add(mat3_t a, mat3_t b) {
#if USE_NEON
    mat3_t result;
    // Process 4 elements at a time, then remaining
    float32x4_t va0 = vld1q_f32(&a.m[0]);
    float32x4_t vb0 = vld1q_f32(&b.m[0]);
    float32x4_t vr0 = vaddq_f32(va0, vb0);
    vst1q_f32(&result.m[0], vr0);
    
    float32x4_t va1 = vld1q_f32(&a.m[4]);
    float32x4_t vb1 = vld1q_f32(&b.m[4]);
    float32x4_t vr1 = vaddq_f32(va1, vb1);
    vst1q_f32(&result.m[4], vr1);
    
    // Last element
    result.m[8] = a.m[8] + b.m[8];
    return result;
#else
    mat3_t result;
    for (int i = 0; i < 9; i++) {
        result.m[i] = a.m[i] + b.m[i];
    }
    return result;
#endif
}

mat3_t mat3_sub(mat3_t a, mat3_t b) {
#if USE_NEON
    mat3_t result;
    float32x4_t va0 = vld1q_f32(&a.m[0]);
    float32x4_t vb0 = vld1q_f32(&b.m[0]);
    float32x4_t vr0 = vsubq_f32(va0, vb0);
    vst1q_f32(&result.m[0], vr0);
    
    float32x4_t va1 = vld1q_f32(&a.m[4]);
    float32x4_t vb1 = vld1q_f32(&b.m[4]);
    float32x4_t vr1 = vsubq_f32(va1, vb1);
    vst1q_f32(&result.m[4], vr1);
    
    result.m[8] = a.m[8] - b.m[8];
    return result;
#else
    mat3_t result;
    for (int i = 0; i < 9; i++) {
        result.m[i] = a.m[i] - b.m[i];
    }
    return result;
#endif
}

mat3_t mat3_scale(mat3_t m, float s) {
#if USE_NEON
    mat3_t result;
    float32x4_t vs = vdupq_n_f32(s);
    
    float32x4_t vm0 = vld1q_f32(&m.m[0]);
    float32x4_t vr0 = vmulq_f32(vm0, vs);
    vst1q_f32(&result.m[0], vr0);
    
    float32x4_t vm1 = vld1q_f32(&m.m[4]);
    float32x4_t vr1 = vmulq_f32(vm1, vs);
    vst1q_f32(&result.m[4], vr1);
    
    result.m[8] = m.m[8] * s;
    return result;
#else
    mat3_t result;
    for (int i = 0; i < 9; i++) {
        result.m[i] = m.m[i] * s;
    }
    return result;
#endif
}

mat3_t mat3_mul(mat3_t a, mat3_t b) {
#if USE_NEON
    mat3_t result;
    
    // For each column of result: result_col = a * b_col
    // result[col*3 + row] = sum over k of a[k*3 + row] * b[col*3 + k]
    
    for (int col = 0; col < 3; col++) {
        // Load column from B
        float b_col[3] = { b.m[col*3 + 0], b.m[col*3 + 1], b.m[col*3 + 2] };
        
        // Load columns from A and multiply-accumulate
        float32x4_t a_col0 = { a.m[0], a.m[1], a.m[2], 0.0f };
        float32x4_t a_col1 = { a.m[3], a.m[4], a.m[5], 0.0f };
        float32x4_t a_col2 = { a.m[6], a.m[7], a.m[8], 0.0f };
        
        float32x4_t vb0 = vdupq_n_f32(b_col[0]);
        float32x4_t vb1 = vdupq_n_f32(b_col[1]);
        float32x4_t vb2 = vdupq_n_f32(b_col[2]);
        
        float32x4_t res = vmulq_f32(a_col0, vb0);
        res = vmlaq_f32(res, a_col1, vb1);
        res = vmlaq_f32(res, a_col2, vb2);
        
        result.m[col*3 + 0] = vgetq_lane_f32(res, 0);
        result.m[col*3 + 1] = vgetq_lane_f32(res, 1);
        result.m[col*3 + 2] = vgetq_lane_f32(res, 2);
    }
    return result;
#else
    mat3_t result;
    for (int col = 0; col < 3; col++) {
        for (int row = 0; row < 3; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 3; k++) {
                sum += a.m[k*3 + row] * b.m[col*3 + k];
            }
            result.m[col*3 + row] = sum;
        }
    }
    return result;
#endif
}

mat3_t mat3_transpose(mat3_t m) {
    // Swap rows and columns
    mat3_t result = {{
        m.m[0], m.m[3], m.m[6],  // Row 0 -> Column 0
        m.m[1], m.m[4], m.m[7],  // Row 1 -> Column 1
        m.m[2], m.m[5], m.m[8]   // Row 2 -> Column 2
    }};
    return result;
}

float mat3_determinant(mat3_t m) {
    // det = m[0]*(m[4]*m[8] - m[5]*m[7]) 
    //     - m[3]*(m[1]*m[8] - m[2]*m[7]) 
    //     + m[6]*(m[1]*m[5] - m[2]*m[4])
#if USE_NEON
    // Column 0: m[0], m[1], m[2]
    // Column 1: m[3], m[4], m[5]
    // Column 2: m[6], m[7], m[8]
    
    float cofactor0 = m.m[4] * m.m[8] - m.m[5] * m.m[7];
    float cofactor1 = m.m[5] * m.m[6] - m.m[3] * m.m[8];
    float cofactor2 = m.m[3] * m.m[7] - m.m[4] * m.m[6];
    
    return m.m[0] * cofactor0 + m.m[1] * cofactor1 + m.m[2] * cofactor2;
#else
    float cofactor0 = m.m[4] * m.m[8] - m.m[5] * m.m[7];
    float cofactor1 = m.m[5] * m.m[6] - m.m[3] * m.m[8];
    float cofactor2 = m.m[3] * m.m[7] - m.m[4] * m.m[6];
    
    return m.m[0] * cofactor0 + m.m[1] * cofactor1 + m.m[2] * cofactor2;
#endif
}

mat3_t mat3_inverse(mat3_t m) {
    float det = mat3_determinant(m);
    
    if (fabsf(det) < NEON_EPSILON) {
        return mat3_identity();  // Return identity for singular matrix
    }
    
    float inv_det = 1.0f / det;
    
    // Compute adjugate matrix (cofactor matrix transposed)
    mat3_t result;
    
    // Cofactors for row 0 of adjugate (column 0 of cofactor)
    result.m[0] = (m.m[4] * m.m[8] - m.m[5] * m.m[7]) * inv_det;
    result.m[1] = (m.m[2] * m.m[7] - m.m[1] * m.m[8]) * inv_det;
    result.m[2] = (m.m[1] * m.m[5] - m.m[2] * m.m[4]) * inv_det;
    
    // Cofactors for row 1 of adjugate (column 1 of cofactor)
    result.m[3] = (m.m[5] * m.m[6] - m.m[3] * m.m[8]) * inv_det;
    result.m[4] = (m.m[0] * m.m[8] - m.m[2] * m.m[6]) * inv_det;
    result.m[5] = (m.m[2] * m.m[3] - m.m[0] * m.m[5]) * inv_det;
    
    // Cofactors for row 2 of adjugate (column 2 of cofactor)
    result.m[6] = (m.m[3] * m.m[7] - m.m[4] * m.m[6]) * inv_det;
    result.m[7] = (m.m[1] * m.m[6] - m.m[0] * m.m[7]) * inv_det;
    result.m[8] = (m.m[0] * m.m[4] - m.m[1] * m.m[3]) * inv_det;
    
    return result;
}

// ============================================================================
// Rotation Matrices
// ============================================================================

mat3_t mat3_rotation_x(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat3_t m = {{
        1.0f, 0.0f, 0.0f,  // Column 0
        0.0f,    c,    s,  // Column 1
        0.0f,   -s,    c   // Column 2
    }};
    return m;
}

mat3_t mat3_rotation_y(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat3_t m = {{
           c, 0.0f,   -s,  // Column 0
        0.0f, 1.0f, 0.0f,  // Column 1
           s, 0.0f,    c   // Column 2
    }};
    return m;
}

mat3_t mat3_rotation_z(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat3_t m = {{
           c,    s, 0.0f,  // Column 0
          -s,    c, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f   // Column 2
    }};
    return m;
}

mat3_t mat3_rotation_axis(vec3_t axis, float radians) {
    // Rodrigues' rotation formula
    float c = cosf(radians);
    float s = sinf(radians);
    float t = 1.0f - c;
    
    // Normalize axis
    float len_sq = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
    if (len_sq < NEON_EPSILON * NEON_EPSILON) {
        return mat3_identity();
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
    
    mat3_t m = {{
        t*x*x + c,     t*x*y + s*z,   t*x*z - s*y,   // Column 0
        t*x*y - s*z,   t*y*y + c,     t*y*z + s*x,   // Column 1
        t*x*z + s*y,   t*y*z - s*x,   t*z*z + c      // Column 2
    }};
    return m;
}

// ============================================================================
// 2D Transformation (Homogeneous Coordinates)
// ============================================================================

mat3_t mat3_translation_2d(float tx, float ty) {
    mat3_t m = {{
        1.0f, 0.0f, 0.0f,  // Column 0
        0.0f, 1.0f, 0.0f,  // Column 1
          tx,   ty, 1.0f   // Column 2
    }};
    return m;
}

mat3_t mat3_scaling_2d(float sx, float sy) {
    mat3_t m = {{
          sx, 0.0f, 0.0f,  // Column 0
        0.0f,   sy, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f   // Column 2
    }};
    return m;
}

mat3_t mat3_rotation_2d(float radians) {
    float c = cosf(radians);
    float s = sinf(radians);
    
    mat3_t m = {{
           c,    s, 0.0f,  // Column 0
          -s,    c, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f   // Column 2
    }};
    return m;
}

// ============================================================================
// Normal Matrix
// ============================================================================

mat3_t mat3_normal_matrix(mat4_t model_matrix) {
    // Normal matrix = transpose(inverse(upper-left 3x3 of model matrix))
    // For orthogonal matrices (no non-uniform scaling), this is just the upper-left 3x3
    
    mat3_t upper3x3 = mat3_from_mat4(model_matrix);
    mat3_t inverse = mat3_inverse(upper3x3);
    return mat3_transpose(inverse);
}
