#pragma once

#include "emath.h"
#include <vector>

struct SRadixTreeNode
{
    SBoundingBox m_bounds;
    uint32_t m_spatialKey{0xFFFFFFFF}, m_primitiveIndex{0xFFFFFFFF};
    uint32_t m_left{0xFFFFFFFF}, m_right{0xFFFFFFFF};
};

struct triangle final {
  SVec128 coords[3];
  SVec128 normals[3];
};

struct HitInfo
{
	SVec128 hitPos;
	uint32_t triIndex;
	triangle *geometryIn;
	triangle *geometryOut;
};

typedef bool(*HitTestFunc)(const SRadixTreeNode &_self, const SVec128 &_rayStart, const SVec128 &_rayEnd, float &_t, const float _tmax, HitInfo &_hitinfo);

void GenerateLBVH(SRadixTreeNode *_nodes, std::vector<SRadixTreeNode> &_leafNodes, const int _numNodes);
void FindClosestHitLBVH(SRadixTreeNode *_nodes, const int _numNodes, const SVec128 &_rayStart, const SVec128 &_rayEnd, float &_t, uint32_t &_hitNode, HitInfo &_hitinfo, HitTestFunc _hitTestFunc);
