#pragma once

#include <math.h>

// ============================================================================
// NEON Vector Math Library
// A C-style SIMD-friendly vector/matrix math library for 3D graphics
// Please use -mfpu=neon -mfloat-abi=hard for ARMv7 targets to enable NEON
// ============================================================================

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Type Definitions
// ============================================================================

// 3-component vector (with padding for alignment)
typedef struct {
    float x, y, z;
    float _pad;  // Padding for 16-byte alignment
} vec3_t;

// 4-component vector
typedef struct {
    float x, y, z, w;
} vec4_t;

// 3x3 matrix (column-major order)
typedef struct {
    float m[9];  // [col][row] = m[col*3 + row]
} mat3_t;

// 4x4 matrix (column-major order)
typedef struct {
    float m[16];  // [col][row] = m[col*4 + row]
} mat4_t;

// Quaternion for rotations
typedef struct {
    float x, y, z, w;
} quat_t;

// ============================================================================
// Constants
// ============================================================================

#define NEON_PI          3.14159265358979323846f
#define NEON_TWO_PI      6.28318530717958647692f
#define NEON_HALF_PI     1.57079632679489661923f
#define NEON_DEG_TO_RAD  0.01745329251994329577f
#define NEON_RAD_TO_DEG  57.2957795130823208768f
#define NEON_EPSILON     1e-6f

// ============================================================================
// Vector3 Functions
// ============================================================================

// Creation
vec3_t vec3_create(float x, float y, float z);
vec3_t vec3_zero(void);
vec3_t vec3_one(void);
vec3_t vec3_up(void);
vec3_t vec3_down(void);
vec3_t vec3_left(void);
vec3_t vec3_right(void);
vec3_t vec3_forward(void);
vec3_t vec3_back(void);

// Basic operations
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mul(vec3_t a, vec3_t b);
vec3_t vec3_div(vec3_t a, vec3_t b);
vec3_t vec3_scale(vec3_t v, float s);
vec3_t vec3_negate(vec3_t v);

// Vector math
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_length(vec3_t v);
float vec3_length_squared(vec3_t v);
vec3_t vec3_normalize(vec3_t v);
float vec3_distance(vec3_t a, vec3_t b);
float vec3_distance_squared(vec3_t a, vec3_t b);

// Interpolation and utilities
vec3_t vec3_lerp(vec3_t a, vec3_t b, float t);
vec3_t vec3_min(vec3_t a, vec3_t b);
vec3_t vec3_max(vec3_t a, vec3_t b);
vec3_t vec3_clamp(vec3_t v, vec3_t min_val, vec3_t max_val);
vec3_t vec3_reflect(vec3_t incident, vec3_t normal);
vec3_t vec3_refract(vec3_t incident, vec3_t normal, float eta);

// Transform by matrix
vec3_t vec3_transform_mat3(vec3_t v, mat3_t m);
vec3_t vec3_transform_mat4(vec3_t v, mat4_t m);       // Transforms as point (w=1)
vec3_t vec3_transform_mat4_dir(vec3_t v, mat4_t m);  // Transforms as direction (w=0)

// ============================================================================
// Vector4 Functions
// ============================================================================

// Creation
vec4_t vec4_create(float x, float y, float z, float w);
vec4_t vec4_from_vec3(vec3_t v, float w);
vec4_t vec4_zero(void);
vec4_t vec4_one(void);

// Basic operations
vec4_t vec4_add(vec4_t a, vec4_t b);
vec4_t vec4_sub(vec4_t a, vec4_t b);
vec4_t vec4_mul(vec4_t a, vec4_t b);
vec4_t vec4_div(vec4_t a, vec4_t b);
vec4_t vec4_scale(vec4_t v, float s);
vec4_t vec4_negate(vec4_t v);

// Vector math
float vec4_dot(vec4_t a, vec4_t b);
float vec4_length(vec4_t v);
float vec4_length_squared(vec4_t v);
vec4_t vec4_normalize(vec4_t v);

// Interpolation and utilities
vec4_t vec4_lerp(vec4_t a, vec4_t b, float t);
vec4_t vec4_min(vec4_t a, vec4_t b);
vec4_t vec4_max(vec4_t a, vec4_t b);

// Transform by matrix
vec4_t vec4_transform_mat4(vec4_t v, mat4_t m);

// Conversion
vec3_t vec4_to_vec3(vec4_t v);
vec3_t vec4_to_vec3_perspective(vec4_t v);  // Divides by w

// ============================================================================
// Matrix 3x3 Functions
// ============================================================================

// Creation
mat3_t mat3_identity(void);
mat3_t mat3_zero(void);
mat3_t mat3_from_rows(vec3_t r0, vec3_t r1, vec3_t r2);
mat3_t mat3_from_cols(vec3_t c0, vec3_t c1, vec3_t c2);
mat3_t mat3_from_mat4(mat4_t m);  // Extract upper-left 3x3

// Basic operations
mat3_t mat3_add(mat3_t a, mat3_t b);
mat3_t mat3_sub(mat3_t a, mat3_t b);
mat3_t mat3_scale(mat3_t m, float s);
mat3_t mat3_mul(mat3_t a, mat3_t b);
mat3_t mat3_transpose(mat3_t m);
float mat3_determinant(mat3_t m);
mat3_t mat3_inverse(mat3_t m);

// Rotation matrices
mat3_t mat3_rotation_x(float radians);
mat3_t mat3_rotation_y(float radians);
mat3_t mat3_rotation_z(float radians);
mat3_t mat3_rotation_axis(vec3_t axis, float radians);

// 2D transformation (for 2D graphics with homogeneous coordinates)
mat3_t mat3_translation_2d(float tx, float ty);
mat3_t mat3_scaling_2d(float sx, float sy);
mat3_t mat3_rotation_2d(float radians);

// Normal matrix (for transforming normals)
mat3_t mat3_normal_matrix(mat4_t model_matrix);

// ============================================================================
// Matrix 4x4 Functions
// ============================================================================

// Creation
mat4_t mat4_identity(void);
mat4_t mat4_zero(void);
mat4_t mat4_from_rows(vec4_t r0, vec4_t r1, vec4_t r2, vec4_t r3);
mat4_t mat4_from_cols(vec4_t c0, vec4_t c1, vec4_t c2, vec4_t c3);

// Basic operations
mat4_t mat4_add(mat4_t a, mat4_t b);
mat4_t mat4_sub(mat4_t a, mat4_t b);
mat4_t mat4_scale_scalar(mat4_t m, float s);
mat4_t mat4_mul(mat4_t a, mat4_t b);
mat4_t mat4_transpose(mat4_t m);
float mat4_determinant(mat4_t m);
mat4_t mat4_inverse(mat4_t m);

// Transformation matrices
mat4_t mat4_translation(float tx, float ty, float tz);
mat4_t mat4_translation_vec3(vec3_t t);
mat4_t mat4_scaling(float sx, float sy, float sz);
mat4_t mat4_scaling_uniform(float s);
mat4_t mat4_scaling_vec3(vec3_t s);
mat4_t mat4_rotation_x(float radians);
mat4_t mat4_rotation_y(float radians);
mat4_t mat4_rotation_z(float radians);
mat4_t mat4_rotation_axis(vec3_t axis, float radians);
mat4_t mat4_rotation_euler(float pitch, float yaw, float roll);  // XYZ order

// Compose/decompose
mat4_t mat4_compose(vec3_t translation, quat_t rotation, vec3_t scale);
void mat4_decompose(mat4_t m, vec3_t* translation, quat_t* rotation, vec3_t* scale);
vec3_t mat4_get_translation(mat4_t m);
vec3_t mat4_get_scale(mat4_t m);

// ============================================================================
// Camera / Projection Functions
// ============================================================================

// View matrices
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);
mat4_t mat4_look_at_lh(vec3_t eye, vec3_t target, vec3_t up);  // Left-handed
mat4_t mat4_look_at_rh(vec3_t eye, vec3_t target, vec3_t up);  // Right-handed (default)

// Projection matrices (right-handed, depth [0,1] for Vulkan/Metal/DX12)
mat4_t mat4_perspective(float fov_radians, float aspect, float near_plane, float far_plane);
mat4_t mat4_perspective_fov(float fov_y_radians, float width, float height, float near_plane, float far_plane);
mat4_t mat4_perspective_infinite(float fov_radians, float aspect, float near_plane);
mat4_t mat4_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane);
mat4_t mat4_ortho_symmetric(float width, float height, float near_plane, float far_plane);

// Projection matrices (OpenGL style, depth [-1,1])
mat4_t mat4_perspective_gl(float fov_radians, float aspect, float near_plane, float far_plane);
mat4_t mat4_ortho_gl(float left, float right, float bottom, float top, float near_plane, float far_plane);

// Left-handed variants
mat4_t mat4_perspective_lh(float fov_radians, float aspect, float near_plane, float far_plane);
mat4_t mat4_ortho_lh(float left, float right, float bottom, float top, float near_plane, float far_plane);

// Camera utilities
vec3_t camera_unproject(vec3_t screen_pos, mat4_t view_projection_inverse, float viewport_x, float viewport_y, float viewport_width, float viewport_height);
vec3_t camera_project(vec3_t world_pos, mat4_t view_projection, float viewport_x, float viewport_y, float viewport_width, float viewport_height);
void camera_ray_from_screen(vec3_t screen_pos, mat4_t view_projection_inverse, float viewport_x, float viewport_y, float viewport_width, float viewport_height, vec3_t* ray_origin, vec3_t* ray_direction);

// Frustum planes (plane equation: ax + by + cz + d = 0, stored as vec4(a,b,c,d))
void camera_extract_frustum_planes(mat4_t view_projection, vec4_t planes[6]);
int camera_point_in_frustum(vec4_t planes[6], vec3_t point);
int camera_sphere_in_frustum(vec4_t planes[6], vec3_t center, float radius);
int camera_aabb_in_frustum(vec4_t planes[6], vec3_t min_point, vec3_t max_point);

// ============================================================================
// Quaternion Functions
// ============================================================================

// Creation
quat_t quat_identity(void);
quat_t quat_create(float x, float y, float z, float w);
quat_t quat_from_axis_angle(vec3_t axis, float radians);
quat_t quat_from_euler(float pitch, float yaw, float roll);  // XYZ order
quat_t quat_from_mat3(mat3_t m);
quat_t quat_from_mat4(mat4_t m);

// Basic operations
quat_t quat_add(quat_t a, quat_t b);
quat_t quat_sub(quat_t a, quat_t b);
quat_t quat_mul(quat_t a, quat_t b);
quat_t quat_scale(quat_t q, float s);
quat_t quat_conjugate(quat_t q);
quat_t quat_inverse(quat_t q);
float quat_length(quat_t q);
quat_t quat_normalize(quat_t q);
float quat_dot(quat_t a, quat_t b);

// Interpolation
quat_t quat_lerp(quat_t a, quat_t b, float t);
quat_t quat_slerp(quat_t a, quat_t b, float t);

// Conversion
mat3_t quat_to_mat3(quat_t q);
mat4_t quat_to_mat4(quat_t q);
void quat_to_axis_angle(quat_t q, vec3_t* axis, float* angle);
vec3_t quat_to_euler(quat_t q);

// Rotation
vec3_t quat_rotate_vec3(quat_t q, vec3_t v);

// Utilities
quat_t quat_look_rotation(vec3_t forward, vec3_t up);
float quat_angle(quat_t a, quat_t b);

// ============================================================================
// Utility Functions
// ============================================================================

// Angle conversions
float deg_to_rad(float degrees);
float rad_to_deg(float radians);

// Common math
float clampf(float value, float min_val, float max_val);
float lerpf(float a, float b, float t);
float smoothstep(float edge0, float edge1, float x);
float inversesqrt(float x);

// Approximate fast math (less accurate but faster)
float fast_sin(float x);
float fast_cos(float x);
float fast_tan(float x);
float fast_inversesqrt(float x);

#ifdef __cplusplus
}
#endif
