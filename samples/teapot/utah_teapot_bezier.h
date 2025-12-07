// utah_teapot_bezier.h
// Utah Teapot Bezier patch data
// Source: https://www.cs.cmu.edu/~ph/teapot/teapot.dat
// Format: 32 patches, each with 16 control points (x, y, z)

#ifndef UTAH_TEAPOT_BEZIER_H
#define UTAH_TEAPOT_BEZIER_H

#define TEAPOT_PATCHES 32
#define TEAPOT_PATCH_SIZE 16

// Control points for all patches
extern const float teapot_control_points[306][3];
// Patch indices (32 patches, 16 indices each)
extern const int teapot_patches[TEAPOT_PATCHES][TEAPOT_PATCH_SIZE];

#endif // UTAH_TEAPOT_BEZIER_H
