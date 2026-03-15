#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
 * Mesh — a triangle list with per-vertex 3D position, normal and UV.
 *
 * Vertex positions use a right-handed Y-up coordinate system (the standard
 * OBJ convention).  UVs are expected to be in [0, 1] range; values outside
 * this range are clamped during loading.
 *
 * mesh_load_obj  — loads an .obj file (v / vt / vn / f records).  Faces are
 *                  triangulated via fan decomposition (quads → 2 triangles).
 *                  Vertex normals are consumed when present; otherwise a
 *                  fallback normal is derived from position.  Returns false
 *                  if the file cannot be opened or contains no valid faces.
 *
 * mesh_create_cube — fills *mesh with a unit cube centred on the origin.
 *                    Each face uses the full [0,1]×[0,1] UV range.
 */

struct MeshVertex {
    float x, y, z;  /* position */
    float nx, ny, nz; /* normal */
    float u, v;     /* texture coordinates in [0, 1] */
};

struct MeshTriangle {
    int v[3];       /* indices into the MeshVertex array */
};

struct Mesh {
    MeshVertex*   vertices;
    MeshTriangle* triangles;
    int           vertex_count;
    int           triangle_count;
};

/* Load a Wavefront OBJ file.  Returns true on success. */
bool mesh_load_obj(const char* path, Mesh* mesh);

/* Create a unit cube (side length 1, centred at origin). */
void mesh_create_cube(Mesh* mesh);

/* Free memory allocated by mesh_load_obj or mesh_create_cube. */
void mesh_free(Mesh* mesh);
