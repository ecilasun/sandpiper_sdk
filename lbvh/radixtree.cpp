#include "radixtree.h"

// Based on "Maximizing Parallelism in the Construction of BVHs, Octrees, and k-d Trees" [2012] by Tero Karras
// and "Fast Parallel Construction of High-Quality Bounding Volume Hierarchies" [2013] by Tero Karras and Timo Aila

void ERadixSortClear(unsigned int count[256])
{
	// Clear the count table
	for (int t = 0; t < 256; ++t)
		count[t] = 0;
}

void ERadixSortScanAscending(const unsigned int _phase, SRadixTreeNode *_sortArray, const size_t _numElements, unsigned int count[256])
{
	// Scan count each element's existance in the input array
	unsigned int shiftCount = _phase * 8;
	for (size_t elem = 0; elem < _numElements; ++elem)
	{
		unsigned int rdx = (_sortArray[elem].m_spatialKey >> shiftCount) & 0xFF;
		count[rdx]++;
	}
}

void ERadixSortGenerateOffsets(unsigned int count[256], unsigned int offsets[256])
{
	// Generate prefix sum
	offsets[0] = 0;
	for (int t = 1; t < 256; ++t)
		offsets[t] = offsets[t-1] + count[t-1];
}

void ERadixSortRemapAscending(const unsigned int _phase, SRadixTreeNode *_sortArray, SRadixTreeNode *_tmpArray, const size_t _numElements, unsigned int offsets[256])
{
	// Remap elements to new indices
	unsigned int shiftCount = _phase * 8;
	for (int elem = 0; elem <_numElements; ++elem)
	{
		unsigned int rdx = (_sortArray[elem].m_spatialKey >> shiftCount) & 0xFF;
		int newEntry = offsets[rdx];
		_tmpArray[newEntry] = _sortArray[elem];
		offsets[rdx]++;
	}
}

void ERadixSortRemapDescending(const unsigned int _phase, SRadixTreeNode *_sortArray, SRadixTreeNode *_tmpArray, const size_t _numElements, unsigned int offsets[256])
{
	// Remap elements to new indices
	unsigned int shiftCount = _phase * 8;
	for (int elem = 0; elem <_numElements; ++elem)
	{
		unsigned int rdx = (_sortArray[elem].m_spatialKey >> shiftCount) & 0xFF;
		rdx = rdx ^ 0xFF;
		int newEntry = offsets[rdx];
		_tmpArray[newEntry] = _sortArray[elem];
		offsets[rdx]++;
	}
}

void ERadixSortSpatialDatabaseAscending(SRadixTreeNode *_sortArray, size_t _numElements)
{
	SRadixTreeNode *tmpArray = new SRadixTreeNode[_numElements];

	// Count table (per-byte sort, therefore only 2^8 elements long)
	unsigned int offsets[256];
	unsigned int count[256];

	// Start with src=source and tgt=target, swapping at each phase.
	SRadixTreeNode *srce = _sortArray;
	SRadixTreeNode *trgt = tmpArray;

	// There are 4 phases in our sort (using 4 bytes of the unsiged integer being sorted)
	for (int phase = 0; phase < 4; ++phase)
	{
		// 0 - Reset count table (dispatch("ERadixSortClear.cp",1,1,1), only 256 entries)
		ERadixSortClear(count);

		// 1 - Scan left to right, grab current byte, increment its counter (dispatch("radixSortScan.cp",roundup(_numElements/64),1,1))
		ERadixSortScanAscending(phase, srce, _numElements, count);

		// 2 - Generate placement offset table (dispatch("radixSortGenerateOffsets.cp",1,1,1), only 256 entries)
		ERadixSortGenerateOffsets(count, offsets);

		// 3 - Reorder using count table (dispatch("radixSortRemap.cp",roundup(_numElements/64),1,1))
		ERadixSortRemapAscending(phase, srce, trgt, _numElements, offsets);

		// Swap structure pointers (on 4th byte, this will be back to original 'src')
		SRadixTreeNode *temp = srce;
		srce = trgt;
		trgt = temp;
	}

	delete [] tmpArray;
}

int GetLongestCommonPrefix(SRadixTreeNode *_nodes, const int A, const int B, const int _numNodes)
{
	// GetLongestCommonPrefix(A,B) is -1 when A/B are out of array range
    if (A < 0 || B < 0 || A >= _numNodes || B >= _numNodes)
        return -1;

    uint32_t mortonCodeA = _nodes[A].m_spatialKey;
    uint32_t mortonCodeB = _nodes[B].m_spatialKey;
    if (mortonCodeA != mortonCodeB)
        return ECountLeadingZeros( mortonCodeA ^ mortonCodeB);
    else
        return ECountLeadingZeros(A ^ B) + 31;
}

void DetermineRange(const int idx, SRadixTreeNode *_nodes, int &first, int &last, const int _numNodes)
{
	int d = GetLongestCommonPrefix(_nodes, idx, idx+1, _numNodes) - GetLongestCommonPrefix(_nodes, idx, idx-1, _numNodes);
	d = EMaximum(EMinimum(d, 1),-1);
	int minPrefix = GetLongestCommonPrefix(_nodes, idx, idx-d, _numNodes);

	int maxLength = 2;
	while(GetLongestCommonPrefix(_nodes, idx, idx + maxLength*d, _numNodes) > minPrefix)
		maxLength *= 2;

	int length = 0;
	for (int t=maxLength/2; t>0; t/=2)
		if (GetLongestCommonPrefix(_nodes, idx, idx+(length+t)*d, _numNodes) > minPrefix)
			length = length + t;

	int j = idx + length*d;

	first = EMinimum(idx, j);
	last = EMaximum(idx, j);
}

int FindSplit(SRadixTreeNode *_nodes, int first, int last, const int _numNodes)
{
	int commonPrefix = GetLongestCommonPrefix(_nodes, first, last, _numNodes);
	int split = first;
	int step = last - first;

	do
	{
		step = (step + 1) >> 1;
		int newSplit = split + step;

		if (newSplit < last)
		{
			int splitPrefix = GetLongestCommonPrefix(_nodes, first, newSplit, _numNodes);
			if (splitPrefix > commonPrefix)
				split = newSplit;
		}
	} while (step > 1);

	return split;
}

void GenerateHierarchy(int _idx, SRadixTreeNode *_nodes, const int _numNodes)
{
	int first, last;
	DetermineRange(_idx, _nodes, first, last, _numNodes);
	int split = FindSplit(_nodes, first, last, _numNodes);

	uint32_t leafNodeOffset = _numNodes - 1;

	uint32_t childAIndex;
	if (split == first)
		childAIndex = leafNodeOffset + split;
	else
		childAIndex = split;

	uint32_t childBIndex;
	if (split+1 == last)
		childBIndex = leafNodeOffset + split+1;
	else
		childBIndex = split+1;

	// Write parent node
	_nodes[_idx].m_left = childAIndex;
	_nodes[_idx].m_right = childBIndex;
}

SBoundingBox ComputeAABBs(SRadixTreeNode *_nodes, const int _idx, const int _numNodes)
{
	SRadixTreeNode &self = _nodes[_idx];

	SBoundingBox leftBound = self.m_bounds;
	SBoundingBox rightBound = self.m_bounds;

	if (self.m_left != 0xFFFFFFFF)// && self.m_right != 0xFFFFFFFF)
	{
		leftBound = ComputeAABBs(_nodes, self.m_left, _numNodes);
		rightBound = ComputeAABBs(_nodes, self.m_right, _numNodes);
	}

	SBoundingBox selfBound;
	EJoinBounds(selfBound, leftBound, rightBound);

	self.m_bounds = selfBound;
	return self.m_bounds;
}

void GenerateLBVH(SRadixTreeNode *_nodes, std::vector<SRadixTreeNode> &_leafNodes, const int _numNodes)
{
	if (_numNodes == 0)
		return;

	int rootNode = 0;
	int firstLeafNode = _numNodes - 1;

	// Radix sort leaf nodes
	ERadixSortSpatialDatabaseAscending(_leafNodes.data(), _numNodes);

	// Copy sorted leaf nodes
	for (uint32_t n=firstLeafNode; n<firstLeafNode+_numNodes; ++n)
	{
		// Keys live on top level nodes (NOTE: This is to be discarded later)
		_nodes[n-firstLeafNode].m_spatialKey = _leafNodes[n-firstLeafNode].m_spatialKey;
		// Bound and geometry data lives on leaf nodes
		_nodes[n].m_bounds = _leafNodes[n-firstLeafNode].m_bounds;
		_nodes[n].m_primitiveIndex = _leafNodes[n-firstLeafNode].m_primitiveIndex;
	}

	// Build tree hierarchy
	for (int idx=rootNode; idx<firstLeafNode; ++idx)
		GenerateHierarchy(idx, _nodes, _numNodes);

	// Generate AABBs
	ComputeAABBs(_nodes, rootNode, _numNodes);
}

void FindClosestHitLBVH(SRadixTreeNode *_nodes, const int _numNodes, const SVec128 &_rayStart, const SVec128 &_rayEnd, float &_t, uint32_t &_hitNode, HitInfo &_hitInfo, HitTestFunc _hitTestFunc)
{
	// For cases where divide by zero throws exceptions, use the following approach
	SVec128 rayDelta = EVecSub(_rayEnd, _rayStart);
	// Generate select mask
	SVec128 zeroMask = EVecCmpEQ(rayDelta, EVecZero());
	// Use mask to select max float in place of 1s dividend, and 1s in place of 0s for divisor
	SVec128 dividend = EVecSel(g_XMMaxFloat, g_XMOne, zeroMask);
	SVec128 divisor = EVecSel(g_XMOne, rayDelta, zeroMask);
	// Reciprocal by division instead of rcp instruction
	SVec128 invDelta = EVecDiv(dividend, divisor);

	// Use this version when division by zero is allowed and generates +/- infinity
	//SVec128 rayDelta = EVecSetW(EVecSub(_rayEnd, _rayStart), 1.f);
	//SVec128 invDelta = EVecRcp(rayDelta);

	_hitNode = 0xFFFFFFFF;
	float closestHit = FLT_MAX;

	int SP = 0;
	int stack[64];
	stack[SP++] = 0; // Start from root node

	while (SP > 0 && SP < 64) // While no underflow or overflow
	{
		int currentNodeIndex = stack[--SP];
		SRadixTreeNode &self = _nodes[currentNodeIndex];

		float enter = 0.f;
		bool hit = IntersectSlab(self.m_bounds.m_Min, self.m_bounds.m_Max, _rayStart, rayDelta, invDelta, enter);
		if (hit)
		{
			if (self.m_left == 0xFFFFFFFF /*&& self.m_right==0xFFFFFFFF*/) // Leaf node
			{
				float t = enter;
				// Hit only if we're going through the triangle and are closer than before
				bool triHit = _hitTestFunc(self, _rayStart, _rayEnd, t, closestHit, _hitInfo);
				if (triHit)
				{
					closestHit = t;
					// Remember this hit, and resume
					_hitNode = currentNodeIndex;
					_t = t;
				}
			}
			else
			{
				// Wasn't a leaf node, dive deeper
				if (self.m_left != 0xFFFFFFFF) stack[SP++] = self.m_left;
				if (self.m_right != 0xFFFFFFFF) stack[SP++] = self.m_right;
			}
		}
	}
}
