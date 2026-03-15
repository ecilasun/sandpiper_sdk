#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* -------------------------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------------------------- */

static inline float clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

static void normalize3(float* x, float* y, float* z)
{
    float l2 = (*x)*(*x) + (*y)*(*y) + (*z)*(*z);
    if (l2 > 1.0e-12f) {
        float inv = 1.0f / sqrtf(l2);
        *x *= inv;
        *y *= inv;
        *z *= inv;
    } else {
        *x = 0.0f;
        *y = 1.0f;
        *z = 0.0f;
    }
}

/* Parse one face-vertex token ("v", "v/t", "v//n", "v/t/n").
 * Returns 0-based position/uv/normal indices in *pi/*ti/*ni.
 * *ti and *ni are -1 when missing.
 * Negative OBJ indices are resolved against num_pos / num_uv / num_nrm. */
static void parse_face_vert(const char* tok, int num_pos, int num_uv, int num_nrm,
                             int* pi, int* ti, int* ni)
{
    *pi = atoi(tok);
    *ti = -1;
    *ni = -1;

    const char* slash = strchr(tok, '/');
    if (slash) {
        const char* slash2 = strchr(slash + 1, '/');

        if (slash[1] != '/' && slash[1] != '\0' && slash[1] != '\n')
            *ti = atoi(slash + 1);

        if (slash2 && slash2[1] != '\0' && slash2[1] != '\n')
            *ni = atoi(slash2 + 1);
    }

    /* Convert 1-based to 0-based; handle negative (relative) indices */
    *pi = (*pi < 0) ? (num_pos + *pi) : (*pi - 1);
    if (*ti > 0)
        *ti = (*ti < 0) ? (num_uv + *ti) : (*ti - 1);
    else
        *ti = -1;

    if (*ni > 0)
        *ni = (*ni < 0) ? (num_nrm + *ni) : (*ni - 1);
    else
        *ni = -1;
}

/* -------------------------------------------------------------------------
 * Procedural cube geometry
 * ------------------------------------------------------------------------- */

void mesh_create_cube(Mesh* mesh)
{
    /*
     * Unit cube, 6 faces × 4 vertices = 24 vertices, 6 faces × 2 triangles = 12 tris.
     *
     * Each face has its own 4 vertices so UVs can be unique per face.  Winding
     * is CCW when observed from outside the cube (right-hand Y-up convention):
     * the rasterizer will see these as front-facing after the Y-flip to screen
     * space.
     *
     * Layout of the 4 corners on each face (UV mapping):
     *   0 = bottom-left  (0,1)
     *   1 = bottom-right (1,1)
     *   2 = top-right    (1,0)
     *   3 = top-left     (0,0)
     * Two CCW triangles: (0,1,2) and (0,2,3)
     */

    static const MeshVertex verts[24] = {
        /* +Z front */
        {-0.5f,-0.5f, 0.5f,   0.0f,0.0f, 1.0f,  0.0f,1.0f},
        { 0.5f,-0.5f, 0.5f,   0.0f,0.0f, 1.0f,  1.0f,1.0f},
        { 0.5f, 0.5f, 0.5f,   0.0f,0.0f, 1.0f,  1.0f,0.0f},
        {-0.5f, 0.5f, 0.5f,   0.0f,0.0f, 1.0f,  0.0f,0.0f},
        /* -Z back */
        { 0.5f,-0.5f,-0.5f,   0.0f,0.0f,-1.0f,  0.0f,1.0f},
        {-0.5f,-0.5f,-0.5f,   0.0f,0.0f,-1.0f,  1.0f,1.0f},
        {-0.5f, 0.5f,-0.5f,   0.0f,0.0f,-1.0f,  1.0f,0.0f},
        { 0.5f, 0.5f,-0.5f,   0.0f,0.0f,-1.0f,  0.0f,0.0f},
        /* +X right */
        { 0.5f,-0.5f, 0.5f,   1.0f,0.0f, 0.0f,  0.0f,1.0f},
        { 0.5f,-0.5f,-0.5f,   1.0f,0.0f, 0.0f,  1.0f,1.0f},
        { 0.5f, 0.5f,-0.5f,   1.0f,0.0f, 0.0f,  1.0f,0.0f},
        { 0.5f, 0.5f, 0.5f,   1.0f,0.0f, 0.0f,  0.0f,0.0f},
        /* -X left */
        {-0.5f,-0.5f,-0.5f,  -1.0f,0.0f, 0.0f,  0.0f,1.0f},
        {-0.5f,-0.5f, 0.5f,  -1.0f,0.0f, 0.0f,  1.0f,1.0f},
        {-0.5f, 0.5f, 0.5f,  -1.0f,0.0f, 0.0f,  1.0f,0.0f},
        {-0.5f, 0.5f,-0.5f,  -1.0f,0.0f, 0.0f,  0.0f,0.0f},
        /* +Y top */
        {-0.5f, 0.5f, 0.5f,   0.0f,1.0f, 0.0f,  0.0f,1.0f},
        { 0.5f, 0.5f, 0.5f,   0.0f,1.0f, 0.0f,  1.0f,1.0f},
        { 0.5f, 0.5f,-0.5f,   0.0f,1.0f, 0.0f,  1.0f,0.0f},
        {-0.5f, 0.5f,-0.5f,   0.0f,1.0f, 0.0f,  0.0f,0.0f},
        /* -Y bottom */
        {-0.5f,-0.5f,-0.5f,   0.0f,-1.0f,0.0f,  0.0f,1.0f},
        { 0.5f,-0.5f,-0.5f,   0.0f,-1.0f,0.0f,  1.0f,1.0f},
        { 0.5f,-0.5f, 0.5f,   0.0f,-1.0f,0.0f,  1.0f,0.0f},
        {-0.5f,-0.5f, 0.5f,   0.0f,-1.0f,0.0f,  0.0f,0.0f},
    };

    static const MeshTriangle tris[12] = {
        {{0,1,2}},{{0,2,3}},    /* front  */
        {{4,5,6}},{{4,6,7}},    /* back   */
        {{8,9,10}},{{8,10,11}}, /* right  */
        {{12,13,14}},{{12,14,15}},/* left */
        {{16,17,18}},{{16,18,19}},/* top  */
        {{20,21,22}},{{20,22,23}} /* bot  */
    };

    mesh->vertex_count    = 24;
    mesh->triangle_count  = 12;
    mesh->vertices        = (MeshVertex*)  malloc(24 * sizeof(MeshVertex));
    mesh->triangles       = (MeshTriangle*)malloc(12 * sizeof(MeshTriangle));
    memcpy(mesh->vertices,  verts, 24 * sizeof(MeshVertex));
    memcpy(mesh->triangles, tris,  12 * sizeof(MeshTriangle));
}

/* -------------------------------------------------------------------------
 * OBJ loader
 * ------------------------------------------------------------------------- */

bool mesh_load_obj(const char* path, Mesh* mesh)
{
    FILE* fp = fopen(path, "r");
    if (!fp)
        return false;

    /* Stage 1: collect raw positions and UVs */
    int cap_pos = 1024, num_pos = 0;
    float* pos = (float*)malloc((size_t)cap_pos * 3 * sizeof(float));

    int cap_uv = 1024, num_uv = 0;
    float* uv = (float*)malloc((size_t)cap_uv * 2 * sizeof(float));

    int cap_nrm = 1024, num_nrm = 0;
    float* nrm = (float*)malloc((size_t)cap_nrm * 3 * sizeof(float));

    /* Stage 2: unified vertex + index list */
    int cap_verts = 4096, num_verts = 0;
    MeshVertex* verts = (MeshVertex*)malloc((size_t)cap_verts * sizeof(MeshVertex));

    int cap_tris = 4096, num_tris = 0;
    MeshTriangle* tris = (MeshTriangle*)malloc((size_t)cap_tris * sizeof(MeshTriangle));

    if (!pos || !uv || !nrm || !verts || !tris) {
        free(pos); free(uv); free(nrm); free(verts); free(tris);
        fclose(fp);
        return false;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {

        if (line[0] == 'v' && line[1] == ' ') {
            /* Vertex position */
            if (num_pos >= cap_pos) {
                cap_pos *= 2;
                pos = (float*)realloc(pos, (size_t)cap_pos * 3 * sizeof(float));
            }
            float x = 0, y = 0, z = 0;
            sscanf(line + 2, "%f %f %f", &x, &y, &z);
            pos[num_pos*3+0] = x;
            pos[num_pos*3+1] = y;
            pos[num_pos*3+2] = z;
            num_pos++;

        } else if (line[0] == 'v' && line[1] == 't') {
            /* Texture coordinate */
            if (num_uv >= cap_uv) {
                cap_uv *= 2;
                uv = (float*)realloc(uv, (size_t)cap_uv * 2 * sizeof(float));
            }
            float u = 0, v = 0;
            sscanf(line + 3, "%f %f", &u, &v);
            uv[num_uv*2+0] = u;
            uv[num_uv*2+1] = v;
            num_uv++;

        } else if (line[0] == 'v' && line[1] == 'n') {
            /* Vertex normal */
            if (num_nrm >= cap_nrm) {
                cap_nrm *= 2;
                nrm = (float*)realloc(nrm, (size_t)cap_nrm * 3 * sizeof(float));
            }
            float nx = 0, ny = 1, nz = 0;
            sscanf(line + 3, "%f %f %f", &nx, &ny, &nz);
            normalize3(&nx, &ny, &nz);
            nrm[num_nrm*3+0] = nx;
            nrm[num_nrm*3+1] = ny;
            nrm[num_nrm*3+2] = nz;
            num_nrm++;

        } else if (line[0] == 'f' && line[1] == ' ') {
            /* Face — up to 8 vertices (quads + n-gons) */
            int fv_pi[8], fv_ti[8], fv_ni[8];
            int nfv = 0;

            const char* p = line + 2;
            while (*p && nfv < 8) {
                /* Skip whitespace */
                while (*p == ' ' || *p == '\t') ++p;
                if (*p == '\r' || *p == '\n' || *p == '\0') break;

                /* Find end of token */
                const char* tok_start = p;
                while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') ++p;

                /* Null-terminate a local copy */
                char tok[64];
                int tok_len = (int)(p - tok_start);
                if (tok_len >= (int)sizeof(tok)) tok_len = (int)sizeof(tok) - 1;
                memcpy(tok, tok_start, (size_t)tok_len);
                tok[tok_len] = '\0';

                parse_face_vert(tok, num_pos, num_uv, num_nrm,
                                &fv_pi[nfv], &fv_ti[nfv], &fv_ni[nfv]);
                nfv++;
            }

            if (nfv < 3)
                continue;

            /* Build per-face vertices (no sharing between faces).
             * Fan triangulation: (0,1,2), (0,2,3), ... */
            int vi_start = num_verts;

            for (int i = 0; i < nfv; ++i) {
                if (num_verts >= cap_verts) {
                    cap_verts *= 2;
                    verts = (MeshVertex*)realloc(verts, (size_t)cap_verts * sizeof(MeshVertex));
                }
                int pi = fv_pi[i];
                int ti = fv_ti[i];
                int ni = fv_ni[i];
                MeshVertex mv;
                mv.x = (pi >= 0 && pi < num_pos) ? pos[pi*3+0] : 0.0f;
                mv.y = (pi >= 0 && pi < num_pos) ? pos[pi*3+1] : 0.0f;
                mv.z = (pi >= 0 && pi < num_pos) ? pos[pi*3+2] : 0.0f;
                mv.u = (ti >= 0 && ti < num_uv)  ? clamp01(uv[ti*2+0]) : 0.0f;
                mv.v = (ti >= 0 && ti < num_uv)  ? clamp01(uv[ti*2+1]) : 0.0f;

                if (ni >= 0 && ni < num_nrm) {
                    mv.nx = nrm[ni*3+0];
                    mv.ny = nrm[ni*3+1];
                    mv.nz = nrm[ni*3+2];
                } else {
                    /* Fallback: radial normal from position. */
                    mv.nx = mv.x;
                    mv.ny = mv.y;
                    mv.nz = mv.z;
                    normalize3(&mv.nx, &mv.ny, &mv.nz);
                }
                verts[num_verts++] = mv;
            }

            for (int i = 1; i < nfv - 1; ++i) {
                if (num_tris >= cap_tris) {
                    cap_tris *= 2;
                    tris = (MeshTriangle*)realloc(tris, (size_t)cap_tris * sizeof(MeshTriangle));
                }
                tris[num_tris].v[0] = vi_start;
                tris[num_tris].v[1] = vi_start + i;
                tris[num_tris].v[2] = vi_start + i + 1;
                num_tris++;
            }
        }
    }

    fclose(fp);
    free(pos);
    free(uv);
    free(nrm);

    if (num_tris == 0) {
        free(verts);
        free(tris);
        return false;
    }

    mesh->vertices       = verts;
    mesh->triangles      = tris;
    mesh->vertex_count   = num_verts;
    mesh->triangle_count = num_tris;
    return true;
}

void mesh_free(Mesh* mesh)
{
    free(mesh->vertices);
    free(mesh->triangles);
    mesh->vertices       = nullptr;
    mesh->triangles      = nullptr;
    mesh->vertex_count   = 0;
    mesh->triangle_count = 0;
}
