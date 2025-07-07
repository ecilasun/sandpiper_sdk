#include "platform.h"
#include "locklesspipe.h"
#include "radixtree.h"
#include "objloader.h"

// Folder/file name of the scene to work with
//#define SCENENAME "sibenik"
#define SCENENAME "testscene"

// Define this to get worker tile debug view
//#define SHOW_WORKER_IDS

// Number of worker threads
#define MAX_WORKERS 12

// Define to use Morton curve order instead of scanline order
//#define USE_MORTON_ORDER

static bool g_done = false;
#if defined(USE_MORTON_ORDER)
static const uint32_t tilewidth = 4;
static const uint32_t tileheight = 8;
static const uint32_t width = 512;
static const uint32_t height = 512;
#else
// 320x240 but x2
static const uint32_t tilewidth = 8;
static const uint32_t tileheight = 8;
static const uint32_t width = 512;
static const uint32_t height = 512;
#endif
static const uint32_t tilecountx = width/tilewidth;
static const uint32_t tilecounty = height/tileheight;
static const float cameradistance = 20.f;
static const float raylength = cameradistance + 1000.f;

struct SRenderContext
{
	float rotAng{0.f};
	float aspect{float(height) / float(width)};

	SVec128 rayOrigin;
	SVec128 lookAt;
	SVec128 upVec;
	SMatrix4x4 lookMat;
	SVec128 pzVec;
	SVec128 F;
	SVec128 nil{0.f, 0.f, 0.f, 0.f};
	SVec128 epsilon{0.02f, 0.02f, 0.02f, 0.f};
	SVec128 negepsilon{-0.02f, -0.02f, -0.02f, 0.f};
	uint8_t *pixels;
};

struct SWorkerContext
{
	uint32_t workerID{0};
	HitInfo hitinfo{0};
	uint8_t rasterTile[4*tilewidth*tileheight]; // Internal rasterization tile (in hardware, to avoid arbitration need)
	SRenderContext *rc;
	CLocklessPipe<12> dispatchvector;
};

// TLAS
triangle *sceneGeometry;

// BLAS
struct BLASNode {
	SVec128 aabbMin;
	SVec128 aabbMax;
	triangle *geometry;
	uint32_t numTriangles;
	SRadixTreeNode* BLAS;
	SMatrix4x4 transform;
	SMatrix4x4 invTransform;
	uint32_t leafCount = 0;
};

struct TLASNode {
	SVec128 aabbMin;
	SVec128 aabbMax;
	SRadixTreeNode* TLAS;
	uint32_t leafCount = 0;
};

BLASNode sceneBLASNodes[16];
uint32_t sceneBLASNodeCount = 0;
TLASNode sceneTLASNode;

void TLASBuilder(uint32_t& leafCount, BLASNode* _BLASnodes, uint32_t _numBLASnodes, std::vector<SRadixTreeNode> &_leafnodes, SRadixTreeNode **_lbvh, SVec128 &_worldMin, SVec128 &_worldMax)
{
	_worldMin = EVecConst(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX);
	_worldMax = EVecConst(-FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX);

	for (uint32_t i = 0; i < _numBLASnodes; ++i)
	{
		_worldMin = EVecMin(_worldMin, _BLASnodes[i].aabbMin);
		_worldMax = EVecMax(_worldMax, _BLASnodes[i].aabbMax);
	}

	SVec128 tentwentythree{1023.f, 1023.f, 1023.f, 1.f};
	SVec128 sceneBounds = EVecSub(_worldMax, _worldMin);
	SVec128 gridCellSize = EVecDiv(sceneBounds, tentwentythree);
	SVec128 pointfive{0.5f, 0.5f, 0.5f, 1.f};

	// Generate BLAS pool

	_leafnodes.clear();

	for (uint32_t b = 0; b < _numBLASnodes; ++b)
	{
		SVec128 origin = EVecMul(EVecAdd(_BLASnodes[b].aabbMax, _BLASnodes[b].aabbMin), pointfive);

		uint32_t qXYZ[3];
		EQuantizePosition(origin, qXYZ, _worldMin, gridCellSize);
		uint32_t mortonCode = EMortonEncode(qXYZ[0], qXYZ[1], qXYZ[2]);

		SRadixTreeNode node;
		node.m_bounds.m_Min = _BLASnodes[b].aabbMin;
		node.m_bounds.m_Max = _BLASnodes[b].aabbMax;
		node.m_primitiveIndex = b;
		node.m_spatialKey = mortonCode;
		_leafnodes.emplace_back(node);
	}

	leafCount = (uint32_t)_leafnodes.size();
	*_lbvh = new SRadixTreeNode[2*leafCount-1];

	GenerateLBVH(*_lbvh, _leafnodes, leafCount);
}

void BLASBuilder(uint32_t& leafCount, triangle* _triangles, uint32_t _numTriangles, std::vector<SRadixTreeNode> &_leafnodes, SRadixTreeNode **_lbvh, SVec128 &_blasMin, SVec128 &_blasMax)
{
	// Generate bounds AABB
	_blasMin = EVecConst(FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX);
	_blasMax = EVecConst(-FLT_MAX,-FLT_MAX,-FLT_MAX,-FLT_MAX);
	for (uint32_t i = 0; i < _numTriangles; ++i)
	{
		_blasMin = EVecMin(_blasMin, _triangles[i].coords[0]);
		_blasMin = EVecMin(_blasMin, _triangles[i].coords[1]);
		_blasMin = EVecMin(_blasMin, _triangles[i].coords[2]);

		_blasMax = EVecMax(_blasMax, _triangles[i].coords[0]);
		_blasMax = EVecMax(_blasMax, _triangles[i].coords[1]);
		_blasMax = EVecMax(_blasMax, _triangles[i].coords[2]);
	}

	// Grab the center point and build a simple offset transform
	//SVec128 boundCenter = EVecMul(EVecAdd(_blasMax, _blasMin), EVecConst(0.5f,0.5f,0.5f,1.f));
	//transform = ...;
	// Build inverse transform for ray tests
	//invTransform = ...;

	SVec128 tentwentythree{1023.f, 1023.f, 1023.f, 1.f};
	SVec128 sceneBounds = EVecSub(_blasMax, _blasMin);
	SVec128 gridCellSize = EVecDiv(sceneBounds, tentwentythree);

	// Generate geometry pool

	_leafnodes.clear();
	SVec128 onethird = EVecConst(0.3333333f, 0.3333333f, 0.3333333f, 0.f);

	for (uint32_t tri = 0; tri < _numTriangles; ++tri)
	{
		SBoundingBox primitiveAABB;
		EResetBounds(primitiveAABB);

		SVec128 v0 = EVecSetW(_triangles[tri].coords[0], 1.f);
		SVec128 v1 = EVecSetW(_triangles[tri].coords[1], 1.f);
		SVec128 v2 = EVecSetW(_triangles[tri].coords[2], 1.f);

		EExpandBounds(primitiveAABB, v0);
		EExpandBounds(primitiveAABB, v1);
		EExpandBounds(primitiveAABB, v2);

		SVec128 origin = EVecMul(EVecAdd(v0, EVecAdd(v1, v2)), onethird);

		uint32_t qXYZ[3];
		EQuantizePosition(origin, qXYZ, _blasMin, gridCellSize);
		uint32_t mortonCode = EMortonEncode(qXYZ[0], qXYZ[1], qXYZ[2]);

		SRadixTreeNode node;
		node.m_bounds.m_Min = primitiveAABB.m_Min;
		node.m_bounds.m_Max = primitiveAABB.m_Max;
		node.m_primitiveIndex = tri;
		node.m_spatialKey = mortonCode;
		_leafnodes.emplace_back(node);
	}

	leafCount = (uint32_t)_leafnodes.size();
	*_lbvh = new SRadixTreeNode[2*leafCount-1];

	GenerateLBVH(*_lbvh, _leafnodes, leafCount);
}

bool ClosestHitTriangle(const SRadixTreeNode &_self, const SVec128 &_rayStart, const SVec128 &_rayEnd, float &_t, const float _tmax, HitInfo &_hitinfo)
{
	uint32_t tri = _self.m_primitiveIndex;
	if (tri == 0xFFFFFFFF)
		return false;
	
	SVec128 deltaRay = EVecSub(_rayEnd, _rayStart);
	bool isHit = HitTriangle(
		_hitinfo.geometryIn[tri].coords[0],
		_hitinfo.geometryIn[tri].coords[1],
		_hitinfo.geometryIn[tri].coords[2],
		_rayStart, deltaRay,
		_t);

	if (isHit && _t < _tmax)
	{
		_hitinfo.hitPos = EVecAdd(_rayStart, EVecMul(deltaRay, EVecConst(_t,_t,_t,0.f)));
		_hitinfo.triIndex = tri;
		_hitinfo.geometryOut = _hitinfo.geometryIn;
	}

	return isHit;
}

bool ClosestHitBLAS(const SRadixTreeNode &_self, const SVec128 &_rayStart, const SVec128 &_rayEnd, float &_t, const float _tmax, HitInfo &_hitinfo)
{
	uint32_t blas = _self.m_primitiveIndex;
	if (blas == 0xFFFFFFFF)
		return false;

	if (_t < _tmax)
	{
		// This node is closer, test it
		uint32_t hitNode = 0xFFFFFFFF;
		_hitinfo.geometryIn = sceneBLASNodes[blas].geometry;
		float tHit;
		FindClosestHitLBVH(sceneBLASNodes[blas].BLAS, sceneBLASNodes[blas].leafCount, _rayStart, _rayEnd, tHit, hitNode, _hitinfo, ClosestHitTriangle);
		if (hitNode != 0xFFFFFFFF && _hitinfo.geometryOut)
		{
			//_t = tHit;
			return true;
		}
	}

	return false;
}

static int DispatcherThread(void *data)
{
	SWorkerContext *vec = (SWorkerContext *)(data);
	while(!g_done)
	{
		uint32_t workItem = 0xFFFFFFFF;
		if (vec->dispatchvector.Read(&workItem, sizeof(uint32_t)))
		{
			// x/y tile indices
			uint32_t tx = workItem%tilecountx;
			uint32_t ty = workItem/tilecountx;

			// Upper left pixel position
			uint32_t ox = tx*tilewidth;
			uint32_t oy = ty*tileheight;

			uint8_t *p = vec->rasterTile;
			for (uint32_t iy = 0; iy<tileheight; ++iy)
			{
				float py = vec->rc->aspect * (float(height)/2.f-float(iy+oy))/float(height);
				for (uint32_t ix = 0; ix<tilewidth; ++ix)
				{
					float px = (float(ix+ox) - float(width)/2.f)/float(width);

					float tHit = FLT_MAX;

					SVec128 pyVec{py,py,py,0.f};
					SVec128 U = EVecMul(vec->rc->lookMat.r[1], pyVec);
					SVec128 raylenvec{raylength,raylength,raylength,0.f};
					SVec128 pxVec{px,px,px,0.f};
					SVec128 L = EVecMul(vec->rc->lookMat.r[0], pxVec);
					SVec128 rayDir = EVecMul(EVecAdd(EVecAdd(L, U), vec->rc->F), raylenvec);
					SVec128 rayEnd = EVecAdd(vec->rc->rayOrigin, rayDir);

					uint32_t hitNode = 0xFFFFFFFF;

					// TODO: trace sceneTLASNode.TLAS which then traces the inner BLAS nodes
					vec->hitinfo.geometryOut = nullptr;
					FindClosestHitLBVH(sceneTLASNode.TLAS, sceneTLASNode.leafCount, vec->rc->rayOrigin, rayEnd, tHit, hitNode, vec->hitinfo, ClosestHitBLAS);

					float final = 0.f;

					if (hitNode != 0xFFFFFFFF && vec->hitinfo.geometryOut)
					{
						// The hit leaf node of the TLAS points at the BLAS to use
						auto &self = sceneTLASNode.TLAS[hitNode];

						// The BLAS gives us the geometry to draw
						triangle *model = vec->hitinfo.geometryOut;

						uint32_t tri = vec->hitinfo.triIndex;

						SVec128 sunPos{20.f,35.f,20.f,1.f};
						SVec128 sunRay = EVecSub(sunPos, vec->hitinfo.hitPos);

						// Global + NdotL
						{
							SVec128 uvw;
							float fuvw[3];
							CalculateBarycentrics(
								vec->hitinfo.hitPos,
								model[tri].coords[0],
								model[tri].coords[1],
								model[tri].coords[2], fuvw);
							SVec128 uvwx = EVecSplatX(EVecConst(fuvw[0]));
							SVec128 uvwy = EVecSplatX(EVecConst(fuvw[1]));
							SVec128 uvwz = EVecSplatX(EVecConst(fuvw[2]));
							SVec128 uvwzA = EVecMul(uvwz, model[tri].normals[0]); // A*uvw.zzz
							SVec128 uvwyB = EVecMul(uvwy, model[tri].normals[1]); // B*uvw.yyy
							SVec128 uvwxC = EVecMul(uvwx, model[tri].normals[2]); // C*uvw.xxx
							SVec128 nrm = EVecNorm3(EVecAdd(uvwzA, EVecAdd(uvwyB, uvwxC))); // A*uvw.zzz + B*uvw.yyy + C*uvw.xxx
							float L = fabs(EVecGetFloatX(EVecDot3(nrm, EVecNorm3(sunRay))));
							final += L;
						}
					}

					uint8_t C = uint8_t(final*255.f);
#if defined(SHOW_WORKER_IDS)
					p[(ix+iy*tilewidth)*4+0] = (vec->workerID&4) ? 128:C; // B
					p[(ix+iy*tilewidth)*4+1] = (vec->workerID&2) ? 128:C; // G
					p[(ix+iy*tilewidth)*4+2] = (vec->workerID&1) ? 128:C; // R
					p[(ix+iy*tilewidth)*4+3] = 0xFF;
#else
					p[(ix+iy*tilewidth)*4+0] = C; // B
					p[(ix+iy*tilewidth)*4+1] = C; // G
					p[(ix+iy*tilewidth)*4+2] = C; // R
					p[(ix+iy*tilewidth)*4+3] = 0xFF;
#endif // SHOW_WORKER_IDS
				}
			}

			// Copy to final surface
			// In hardware, this is 32 bytes of data, so it's much less than a
			// cache line size (which is 512 bits, i.e. 64 bytes)
			for (uint32_t h=0;h<tileheight;++h)
				memcpy(&vec->rc->pixels[4*(ox + (oy+h)*width)], &vec->rasterTile[h*tilewidth*4], tilewidth*4);
		}
	}

	return 0;
}

#if defined(USE_MORTON_ORDER)
void EMorton2DDecode(const uint32_t morton, uint32_t &x, uint32_t &y)
{
  uint32_t res = morton&0x5555555555555555;
  res=(res|(res>>1)) & 0x3333333333333333;
  res=(res|(res>>2)) & 0x0f0f0f0f0f0f0f0f;
  res=(res|(res>>4)) & 0x00ff00ff00ff00ff;
  res=res|(res>>8);
  x = res;
  res = (morton>>1)&0x5555555555555555;
  res=(res|(res>>1)) & 0x3333333333333333;
  res=(res|(res>>2)) & 0x0f0f0f0f0f0f0f0f;
  res=(res|(res>>4)) & 0x00ff00ff00ff00ff;
  res=res|(res>>8);
  y = res;
}
#endif

#if defined(PLATFORM_LINUX)
int main(int _argc, char** _argv)
#else
int SDL_main(int _argc, char** _argv)
#endif
{
#if defined(PLATFORM_LINUX)
	// Console always there for Linux
#else
	HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_SHOW ); // In case it's not shown at startup
#endif

	objl::Loader objloader;
#if defined(PLATFORM_WINDOWS)
	if (!objloader.LoadFile(SCENENAME "\\" SCENENAME ".obj"))
#else
	if (!objloader.LoadFile(SCENENAME "/" SCENENAME ".obj"))
#endif
	{
		printf("Failed to load OBJ file\n");
		return 1;
	}

	// Set up triangle data
	int t=0;
	int totaltriangles = 0;

	for (auto &mesh : objloader.LoadedMeshes)
		totaltriangles += int(mesh.Indices.size()/3);

	if (totaltriangles==0)
	{
		printf("No triangles in model\n");
		return 1;
	}

	sceneGeometry = new triangle[totaltriangles];

	// Build BLAS array
	for (auto &mesh : objloader.LoadedMeshes) // ..or, the entire scene
	{
		int triCount = mesh.Indices.size()/3;
		// Start of model
		triangle *model = &sceneGeometry[t];
		uint32_t modelTriCount = 0;
		for (int i=0; i<triCount; ++i)
		{
			int tri = i*3;
			unsigned int i0 = mesh.Indices[tri+0];
			unsigned int i1 = mesh.Indices[tri+1];
			unsigned int i2 = mesh.Indices[tri+2];

			sceneGeometry[t].coords[0] = EVecConst( mesh.Vertices[i0].Position.X, mesh.Vertices[i0].Position.Y, mesh.Vertices[i0].Position.Z, 0.f);
			sceneGeometry[t].coords[1] = EVecConst( mesh.Vertices[i1].Position.X, mesh.Vertices[i1].Position.Y, mesh.Vertices[i1].Position.Z, 0.f);
			sceneGeometry[t].coords[2] = EVecConst( mesh.Vertices[i2].Position.X, mesh.Vertices[i2].Position.Y, mesh.Vertices[i2].Position.Z, 0.f);

			sceneGeometry[t].normals[0] = EVecConst( mesh.Vertices[i0].Normal.X, mesh.Vertices[i0].Normal.Y, mesh.Vertices[i0].Normal.Z, 0.f);
			sceneGeometry[t].normals[1] = EVecConst( mesh.Vertices[i1].Normal.X, mesh.Vertices[i1].Normal.Y, mesh.Vertices[i1].Normal.Z, 0.f);
			sceneGeometry[t].normals[2] = EVecConst( mesh.Vertices[i2].Normal.X, mesh.Vertices[i2].Normal.Y, mesh.Vertices[i2].Normal.Z, 0.f);

			++t;
			++modelTriCount;
		}

		// Build BLAS
		if (modelTriCount)
		{
			std::vector<SRadixTreeNode> tmpNodes;
			sceneBLASNodes[sceneBLASNodeCount].geometry = model;
			sceneBLASNodes[sceneBLASNodeCount].numTriangles = modelTriCount;
			BLASBuilder(sceneBLASNodes[sceneBLASNodeCount].leafCount,
				model, modelTriCount, tmpNodes,
				&sceneBLASNodes[sceneBLASNodeCount].BLAS,
				sceneBLASNodes[sceneBLASNodeCount].aabbMin,
				sceneBLASNodes[sceneBLASNodeCount].aabbMax);
			//EMatIdentity(sceneBLASNodes[sceneBLASNodeCount].transform);

			fprintf(stderr, "BLAS %d numTri:%d bounds: (%f,%f,%f)-(%f,%f,%f)\n",
				sceneBLASNodeCount,
				modelTriCount,
				EVecGetFloatX(sceneBLASNodes[sceneBLASNodeCount].aabbMin),
				EVecGetFloatY(sceneBLASNodes[sceneBLASNodeCount].aabbMin),
				EVecGetFloatZ(sceneBLASNodes[sceneBLASNodeCount].aabbMin),
				EVecGetFloatX(sceneBLASNodes[sceneBLASNodeCount].aabbMax),
				EVecGetFloatY(sceneBLASNodes[sceneBLASNodeCount].aabbMax),
				EVecGetFloatZ(sceneBLASNodes[sceneBLASNodeCount].aabbMax));

			++sceneBLASNodeCount;
		}
	}

	// Build TLAS
	if (sceneBLASNodeCount)
	{
		std::vector<SRadixTreeNode> tmpNodes;
		TLASBuilder(sceneTLASNode.leafCount,
			sceneBLASNodes, sceneBLASNodeCount, tmpNodes,
			&sceneTLASNode.TLAS,
			sceneTLASNode.aabbMin,
			sceneTLASNode.aabbMax);
	}

	// Trace the BVH
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Could not init SDL2: %s\n", SDL_GetError());
		return 1;
	}

    SDL_Window *screen = SDL_CreateWindow("BVH8Tool",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width, height, 0);

    if(!screen)
	{
        fprintf(stderr, "Could not create window\n");
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer)
	{
        fprintf(stderr, "Could not create renderer\n");
        return 1;
    }

	SDL_Surface *surface = SDL_GetWindowSurface(screen);

    SDL_SetRenderDrawColor(renderer, 32, 32, 128, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

	bool done = false;

	SRenderContext rc;
	SWorkerContext wc[MAX_WORKERS];
	SDL_Thread *thrd[MAX_WORKERS];
	for (uint32_t i=0; i<MAX_WORKERS; ++i)
	{
		wc[i].workerID = i;
		wc[i].rc = &rc;
		thrd[i] = SDL_CreateThread(DispatcherThread, "DispatcherThread", (void*)&wc[i]);
	}

	rc.rotAng = 3.141592f;
	rc.aspect = float(height) / float(width);
	rc.nil = SVec128{0.f, 0.f, 0.f, 0.f};
	rc.negepsilon = SVec128{-0.02f, -0.02f, -0.02f, 0.f};
	rc.epsilon = SVec128{0.02f, 0.02f, 0.02f, 0.f};

	do{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
				done = true;
		}

		if (SDL_MUSTLOCK(surface))
			SDL_LockSurface(surface);

		// Set up camera data
		rc.rayOrigin = SVec128{sinf(rc.rotAng)*cameradistance, sinf(rc.rotAng*0.1f)*12.f, cosf(rc.rotAng)*cameradistance, 1.f};
		rc.lookAt = SVec128{0.f,0.f,0.f,1.f};
		rc.upVec = SVec128{0.f,1.f,0.f,0.f};
		rc.lookMat = EMatLookAtRightHanded(rc.rayOrigin, rc.lookAt, rc.upVec);
		rc.pzVec = SVec128{-1.f,-1.f,-1.f,0.f};
		rc.F = EVecMul(rc.lookMat.r[2], rc.pzVec);
		rc.pixels = (uint8_t*)surface->pixels;

		int distributedAll = 0;
		uint32_t workunit = 0;
		do {
			// Distribute all tiles across all work queues
			for (uint32_t i=0; i<MAX_WORKERS; ++i)
			{
				if (wc[i].dispatchvector.FreeSpace() != 0) // We have space in this worker's queue
				{
#if defined(USE_MORTON_ORDER)
					uint32_t x, y;
					EMorton2DDecode(workunit, x, y);
					uint32_t mortonindex = x+y*tilecountx;
					if (mortonindex < tilecountx*tilecounty) // Ran out of tiles yet?
						wc[i].dispatchvector.Write(&mortonindex, sizeof(uint32_t));
					else
						distributedAll = 1; // Done with all tiles
#else
					if (workunit < tilecountx*tilecounty) // Ran out of tiles yet?
						wc[i].dispatchvector.Write(&workunit, sizeof(uint32_t));
					else
						distributedAll = 1; // Done with all tiles
#endif
					// Next tile
					workunit++;
				}
			}
		} while (!distributedAll); // We're done handing out jobs

		// Rotate
		rc.rotAng += 0.01f;

		// Wait for all threads to be done with locked image pointer before updating window image
		int tdone;
		do
		{
			tdone = 0;
			for (uint32_t i=0; i<MAX_WORKERS; ++i)
				tdone += wc[i].dispatchvector.BytesAvailable() ? 0 : 1;
		} while(tdone != MAX_WORKERS);

		// TODO: Copy tiles to their respective positions
		//(uint8_t*)surface->pixels;

		if (SDL_MUSTLOCK(surface))
			SDL_UnlockSurface(surface);
		SDL_UpdateWindowSurface(screen);

	} while (!done);

	for (uint32_t i=0; i<sceneBLASNodeCount; ++i)
		delete [] sceneBLASNodes[i].BLAS;
	delete [] sceneTLASNode.TLAS;
	delete [] sceneGeometry;

	g_done = true;

	for (uint32_t i=0; i<MAX_WORKERS; ++i)
	{
		int threadReturnValue;
		SDL_WaitThread(thrd[i], &threadReturnValue);
	}

	// Done
	SDL_FreeSurface(surface);
    SDL_DestroyWindow(screen);
	SDL_Quit();

	return 0;
}
