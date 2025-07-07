#pragma once

#include "platform.h"

// --------------------------------------------------------------------------------
// Common defines
// --------------------------------------------------------------------------------

#define E_PI  3.1415927f
#define E_TAU 6.2831855f
#define E_PI_HALF (E_PI*0.5f)

// --------------------------------------------------------------------------------
// Windows and Linux
// --------------------------------------------------------------------------------

#include <emmintrin.h>
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <float.h>

// --------------------------------------------------------------------------------
// Helper macros
// --------------------------------------------------------------------------------

#define E_SINGLE_SHUFFLE(vec, mask) _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))

// --------------------------------------------------------------------------------
// Vector
// --------------------------------------------------------------------------------

typedef __m128 SVec128;
typedef __m128i SVec128i;

// --------------------------------------------------------------------------------
// Splines
// --------------------------------------------------------------------------------

// Hermite spline.
// The path is between P1 and P2.
// Tangents are supplied explicitly.
struct EAlign(16) SHermiteSpline
{
	SVec128 m_p0; // endpoint 0
	SVec128 m_t0; // out-tangent 0
	SVec128 m_t1; // in-tangent 1
	SVec128 m_p1; // endpoint 1
};

// Cardinal spline
// (Catmul-Rom spline is the same but with _a=0.5)
// The path is between P1 and P2.
// P0 and P3 are only for tangent generation.
struct EAlign(16) SCardinalSpline
{
	SVec128 m_p0; // endpoint  0
	SVec128 m_p1; // endpoint  1
	SVec128 m_p2; // endpoint -1
	SVec128 m_p3; // endpoint  2
};

// Cubic Bezier curve - unoptimized!
struct EAlign(16) SCubicBezierCurve
{
	float m_A[4];
	float m_B[4];
	float m_C[4];
	float m_D[4];
	//EVec128 m_p0;
	//EVec128 m_p1;
	//EVec128 m_p2;
	//EVec128 m_p3;
};

// Cubic Bezier curve first order derivative
struct EAlign(16) SCubicBezierCurveFirstDerivative
{
	float m_v1[4];
	float m_v2[4];
	float m_v3[4];
};

// Cubic Bezier curve second order derivative
struct EAlign(16) SCubicBezierCurveSecondDerivative
{
	float m_v1[4];
	float m_v2[4];
};

// --------------------------------------------------------------------------------
// Matrices
// --------------------------------------------------------------------------------

struct EAlign(16) SMatrix4x4
{
	union
	{
		EAlign(16) SVec128 r[4];
		EAlign(16) float m[4][4];
	};
};

struct EAlign(16) SMatrix3x4
{
	union
	{
		EAlign(16) SVec128 r[3];
		EAlign(16) float m[3][4];
	};
};

struct EAlign(16) SBoundingBox
{
	SVec128 m_Min{FLT_MAX,FLT_MAX,FLT_MAX,1.f};
	SVec128 m_Max{-FLT_MAX,-FLT_MAX,-FLT_MAX,1.f};
};

// --------------------------------------------------------------------------------
// Bit count
// --------------------------------------------------------------------------------

/*EInline int ECountTrailingZeros(const uint32_t value)
{
	uint32_t out_bit = 0;
	if (_BitScanForward(&out_bit, value))
		return int(out_bit);
	else
		return 32;
	//return int(_tzcnt_u32(value));
}*/

EInline int ECountLeadingZeros(const uint32_t value)
{
	#if defined(PLATFORM_WINDOWS)
		return __lzcnt(value);
	#else
		return value ? __builtin_clz(value) : 32;
	#endif
}

// --------------------------------------------------------------------------------
// Load / Store / Splat
// --------------------------------------------------------------------------------

EInline SVec128 EVecZero()
{
	return _mm_setzero_ps();
}

EInline SVec128 EVecConst(const float _x, const float _y, const float _z, const float _w)
{
	return _mm_set_ps(_w, _z, _y, _x);
}

EInline SVec128 EVecConst(const float _xyzw)
{
	return _mm_set_ps1(_xyzw);
}

EInline SVec128i EVecConsti(const uint32_t _x, const uint32_t _y, const uint32_t _z, const uint32_t _w)
{
	return _mm_set_epi32(_w, _z, _y, _x);
}

EInline SVec128i EVecConsti(const uint32_t _xyzw)
{
	return _mm_set1_epi32(_xyzw);
}

EInline SVec128 EVecIntAsFloat(const SVec128i _vec)
{
	return _mm_castsi128_ps(_vec);
}

EInline SVec128i EVecFloatAsInt(const SVec128 _vec)
{
	return _mm_castps_si128(_vec);
}

EInline SVec128i EVecConvertFloatToInt(const SVec128 _vec)
{
	return _mm_cvtps_epi32(_vec);
}

EInline SVec128 EVecConvertIntToFloat(const SVec128i _vec)
{
	return _mm_cvtepi32_ps(_vec);
}

EInline SVec128 EVecLoad(const float *_fp)
{
	return _mm_load_ps(_fp);
}

EInline SVec128 EVecLoad3(const float *_fp)
{
	return SVec128{_fp[0],_fp[1],_fp[2],0.f};
}

EInline SVec128 EVecLoad2(const float *_fp)
{
	return SVec128{_fp[0],_fp[1],0.f,0.f};
}

EInline void EVecStore(float *_fp, const SVec128 _Vec)
{
	_mm_store_ps(_fp, _Vec);
}

EInline void EVecStore3(float *_fp, const SVec128 _Vec)
{
	EAlign(16) float localfp[4];
	_mm_store_ps(localfp, _Vec);
	_fp[0] = localfp[0];
	_fp[1] = localfp[1];
	_fp[2] = localfp[2];
}

EInline void EVecStore2(float *_fp, const SVec128 _Vec)
{
	EAlign(16) float localfp[4];
	_mm_store_ps(localfp, _Vec);
	_fp[0] = localfp[0];
	_fp[1] = localfp[1];
}

EInline SVec128 EVecSetX(const SVec128 _Vec, const float _X)
{
	return _mm_move_ss(_Vec, _mm_set_ss(_X));
}

EInline SVec128 EVecSetY(const SVec128 _Vec, const float _Y)
{
	SVec128 R = _mm_move_ss(_mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(3, 2, 0, 1)), _mm_set_ss(_Y));
	return _mm_shuffle_ps(R, R, _MM_SHUFFLE(3, 2, 0, 1));
}

EInline SVec128 EVecSetZ(const SVec128 _Vec, const float _Z)
{
	SVec128 R = _mm_move_ss(_mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(3, 0, 1, 2)), _mm_set_ss(_Z));
	return _mm_shuffle_ps(R, R, _MM_SHUFFLE(3, 0, 1, 2));
}

EInline SVec128 EVecSetW(const SVec128 _Vec, const float _W)
{
	SVec128 R = _mm_move_ss(_mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 2, 1, 3)), _mm_set_ss(_W));
	return _mm_shuffle_ps(R, R, _MM_SHUFFLE(0, 2, 1, 3));
}

EInline SVec128 EVecSplatX(const SVec128 _Vec)
{
	return _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 0));
}

EInline SVec128 EVecSplatY(const SVec128 _Vec)
{
	return _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(1, 1, 1, 1));
}

EInline SVec128 EVecSplatZ(const SVec128 _Vec)
{
	return _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(2, 2, 2, 2));
}

EInline SVec128 EVecSplatW(const SVec128 _Vec)
{
	return _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(3, 3, 3, 3));
}

EInline int EVecGetIntX(const SVec128i _Vec)
{
	return _mm_extract_epi32(_Vec, 0);
}

EInline int EVecGetIntY(const SVec128i _Vec)
{
	return _mm_extract_epi32(_Vec, 1);
}

EInline int EVecGetIntZ(const SVec128i _Vec)
{
	return _mm_extract_epi32(_Vec, 2);
}

EInline int EVecGetIntW(const SVec128i _Vec)
{
	return _mm_extract_epi32(_Vec, 3);
}

EInline float EVecGetFloatX(const SVec128 _Vec)
{
	return _mm_cvtss_f32(_Vec);
}

EInline float EVecGetFloatY(const SVec128 _Vec)
{
	return _mm_cvtss_f32(_mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(1, 1, 1, 1)));
}

EInline float EVecGetFloatZ(const SVec128 _Vec)
{
	return _mm_cvtss_f32(_mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(2, 2, 2, 2)));
}

EInline float EVecGetFloatW(const SVec128 _Vec)
{
	return _mm_cvtss_f32(_mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(3, 3, 3, 3)));
}

// --------------------------------------------------------------------------------
// Shared constants
// --------------------------------------------------------------------------------

const SVec128 g_XMMaskY = EVecIntAsFloat(EVecConsti(0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000));
const SVec128 g_XMMask3 = EVecIntAsFloat(EVecConsti(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000));
const SVec128 g_XMMaskW = EVecIntAsFloat(EVecConsti(0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF));
const SVec128 g_XMOne = { 1.0f,1.0f,1.0f,1.0f };
const SVec128 g_XMZero = { 0.0f,0.0f,0.0f,0.0f };
const SVec128 g_XMMaxFloat = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
const SVec128 g_XMNegativeOne = {-1.0f,-1.0f,-1.0f,-1.0f};
const SVec128 g_XMIdentityR0 = {1.0f, 0.0f, 0.0f, 0.0f};
const SVec128 g_XMIdentityR1 = {0.0f, 1.0f, 0.0f, 0.0f};
const SVec128 g_XMIdentityR2 = {0.0f, 0.0f, 1.0f, 0.0f};
const SVec128 g_XMIdentityR3 = {0.0f, 0.0f, 0.0f, 1.0f};
const SVec128 g_XMNegIdentityR0 = {-1.0f,  0.0f,  0.0f,  0.0f};
const SVec128 g_XMNegIdentityR1 = { 0.0f, -1.0f,  0.0f,  0.0f};
const SVec128 g_XMNegIdentityR2 = { 0.0f,  0.0f, -1.0f,  0.0f};
const SVec128 g_XMNegIdentityR3 = { 0.0f,  0.0f,  0.0f, -1.0f};
const SVec128 g_XMNegateX = {-1.0f, 1.0f, 1.0f, 1.0f};
const SVec128 g_XMNegateY = { 1.0f,-1.0f, 1.0f, 1.0f};
const SVec128 g_XMNegateZ = { 1.0f, 1.0f,-1.0f, 1.0f};
const SVec128 g_XMNegateW = { 1.0f, 1.0f, 1.0f,-1.0f};
const SVec128 g_SignMask = _mm_set1_ps(-0.f);

// --------------------------------------------------------------------------------
// Simple arithmetic for float vectors
// --------------------------------------------------------------------------------

EInline SVec128 EVecAdd(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_add_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecAddi(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return _mm_add_epi32(_Vec1, _Vec2);
}

EInline SVec128 EVecSub(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_sub_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecSubi(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return _mm_sub_epi32(_Vec1, _Vec2);
}

EInline SVec128 EVecMul(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_mul_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecMuli(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return _mm_mullo_epi32(_Vec1, _Vec2);
}

EInline SVec128 EVecDiv(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_div_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecMini(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return _mm_min_epi32(_Vec1, _Vec2);
}

EInline SVec128 EVecMin(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_min_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecMaxi(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return _mm_max_epi32(_Vec1, _Vec2);
}

EInline SVec128 EVecMax(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_max_ps(_Vec1, _Vec2);
}

EInline SVec128 EVecAbs(const SVec128 _Vec)
{
	// Mask out sign bits
	return _mm_andnot_ps(g_SignMask, _Vec);
}

EInline SVec128 EVecSign(const SVec128 _Vec)
{
	// Mask out everything except sign bits
	return _mm_and_ps(g_SignMask, _Vec);
}

EInline float EVecMaxComponent3(const SVec128 _Vec)
{
	SVec128 v2 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 1));
	SVec128 v3 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 2));
	SVec128 maxEntry = _mm_max_ps(_Vec, _mm_max_ps(v2, v3));
	return _mm_cvtss_f32(maxEntry);
}

EInline float EVecMaxComponent4(const SVec128 _Vec)
{
	SVec128 v2 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 1));
	SVec128 v3 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 2));
	SVec128 v4 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 3));
	SVec128 maxEntry = _mm_max_ps(_mm_max_ps(_Vec,v2), _mm_max_ps(v3, v4));
	return _mm_cvtss_f32(maxEntry);
}

EInline float EVecMinComponent3(const SVec128 _Vec)
{
	SVec128 v2 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 1));
	SVec128 v3 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 2));
	SVec128 maxEntry = _mm_min_ps(_Vec, _mm_min_ps(v2, v3));
	return _mm_cvtss_f32(maxEntry);
}

EInline float EVecMinComponent4(const SVec128 _Vec)
{
	SVec128 v2 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 1));
	SVec128 v3 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 2));
	SVec128 v4 = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 3));
	SVec128 maxEntry = _mm_min_ps(_mm_min_ps(_Vec,v2), _mm_min_ps(v3, v4));
	return _mm_cvtss_f32(maxEntry);
}


// --------------------------------------------------------------------------------
// Mask generation for float vectors
// --------------------------------------------------------------------------------

EInline SVec128 EVecCmpEQ(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_cmpeq_ps(_Vec1, _Vec2);
}

EInline SVec128 EVecCmpNEQ(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_cmpneq_ps(_Vec1, _Vec2);
}

EInline SVec128 EVecCmpGT(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_cmpgt_ps(_Vec1, _Vec2);
}

EInline SVec128 EVecCmpGE(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_cmpge_ps(_Vec1, _Vec2);
}

EInline SVec128 EVecCmpLT(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_cmplt_ps(_Vec1, _Vec2);
}

EInline SVec128 EVecCmpLE(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_cmple_ps(_Vec1, _Vec2);
}

EInline int EVecMoveMask(const SVec128i _mask)
{
	return _mm_movemask_epi8(_mask);
}

// --------------------------------------------------------------------------------
// Boolean logic
// --------------------------------------------------------------------------------

EInline SVec128i EVecAnd(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return EVecFloatAsInt(_mm_and_ps(EVecIntAsFloat(_Vec1), EVecIntAsFloat(_Vec2)));
}

EInline SVec128 EVecAnd(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_and_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecOr(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return EVecFloatAsInt(_mm_or_ps(EVecIntAsFloat(_Vec1), EVecIntAsFloat(_Vec2)));
}

EInline SVec128 EVecOr(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_or_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecXor(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return EVecFloatAsInt(_mm_xor_ps(EVecIntAsFloat(_Vec1), EVecIntAsFloat(_Vec2)));
}

EInline SVec128 EVecXor(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_xor_ps(_Vec1, _Vec2);
}

EInline SVec128i EVecAndNot(const SVec128i _Vec1, const SVec128i _Vec2)
{
	return EVecFloatAsInt(_mm_andnot_ps(EVecIntAsFloat(_Vec1), EVecIntAsFloat(_Vec2)));
}

EInline SVec128 EVecAndNot(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_andnot_ps(_Vec1, _Vec2);
}

// --------------------------------------------------------------------------------
// Vector selection via mask
// --------------------------------------------------------------------------------

EInline SVec128 EVecSel(const SVec128 _Vec1, const SVec128 _Vec2, const SVec128 _Sel)
{
	return _mm_or_ps( _mm_and_ps(_Sel, _Vec1), _mm_andnot_ps(_Sel, _Vec2) );
}

EInline int EVecMakeMaskFromSignBits(const SVec128i _Vec)
{
	return _mm_movemask_ps(EVecIntAsFloat(_Vec));
}

EInline int EVecMakeMaskFromSignBits(const SVec128 _Vec)
{
	return _mm_movemask_ps(_Vec);
}

// --------------------------------------------------------------------------------
// Vector interpolation
// --------------------------------------------------------------------------------

EInline SVec128 EVecInterpolateLinear(const SVec128 _Vec1, const SVec128 _Vec2, const float _ipol)
{
	SVec128 t = _mm_set_ps1(_ipol);
	SVec128 ti = EVecSub(_mm_set_ps1(1.f), t);
	SVec128 A = _mm_mul_ps(_Vec1, t);
	SVec128 B = _mm_mul_ps(_Vec2, ti);
	return EVecAdd(A, B);
}

// --------------------------------------------------------------------------------
// Dot / Cross
// --------------------------------------------------------------------------------

EInline SVec128 EVecDot3(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_dp_ps( _Vec1, _Vec2, 0x7f );
}

EInline SVec128 EVecDot4(const SVec128 _Vec1, const SVec128 _Vec2)
{
	return _mm_dp_ps( _Vec1, _Vec2, 0xff );
}

EInline SVec128 EVecCross3(const SVec128 _Vec1, const SVec128 _Vec2)
{
	SVec128 vTemp1 = _mm_shuffle_ps(_Vec1, _Vec1, _MM_SHUFFLE(3, 0, 2, 1));
	SVec128 vTemp2 = _mm_shuffle_ps(_Vec2, _Vec2, _MM_SHUFFLE(3, 1, 0, 2));
	SVec128 vResult = _mm_mul_ps(vTemp1, vTemp2);
	vTemp1 = _mm_shuffle_ps(vTemp1, vTemp1,_MM_SHUFFLE(3, 0, 2, 1));
	vTemp2 = _mm_shuffle_ps(vTemp2, vTemp2,_MM_SHUFFLE(3, 1, 0, 2));
	vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
	vResult = _mm_sub_ps(vResult, vTemp1);
	SVec128 Mask = EVecIntAsFloat(EVecConsti(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000));
	return _mm_and_ps(vResult, Mask);
}

EInline SVec128 EVecTransform3(const SMatrix4x4 &_Mat, const SVec128 _Vec)
{
	SVec128 vResult = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 0));
	vResult = _mm_mul_ps(vResult,_Mat.r[0]);
	SVec128 vTemp = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp,_Mat.r[1]);
	vResult = _mm_add_ps(vResult, vTemp);
	vTemp = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp,_Mat.r[2]);
	vResult = _mm_add_ps(vResult, vTemp);
	vResult = _mm_add_ps(vResult, _Mat.r[3]);
	return vResult;
}

EInline SVec128 EVecTransform4(const SMatrix4x4 &_Mat, const SVec128 _Vec)
{
	SVec128 vTempX = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(0, 0, 0, 0));
	SVec128 vTempY = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(1, 1, 1, 1));
	SVec128 vTempZ = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(2, 2, 2, 2));
	SVec128 vTempW = _mm_shuffle_ps(_Vec, _Vec, _MM_SHUFFLE(3, 3, 3, 3));
	vTempX = _mm_mul_ps(vTempX,_Mat.r[0]);
	vTempY = _mm_mul_ps(vTempY,_Mat.r[1]);
	vTempZ = _mm_mul_ps(vTempZ,_Mat.r[2]);
	vTempW = _mm_mul_ps(vTempW,_Mat.r[3]);
	vTempX = _mm_add_ps(vTempX, vTempY);
	vTempZ = _mm_add_ps(vTempZ, vTempW);
	vTempX = _mm_add_ps(vTempX, vTempZ);
	return vTempX;
}

// --------------------------------------------------------------------------------
// Normalization and length
// --------------------------------------------------------------------------------

EInline SVec128 EVecRcp(const SVec128 _Vec)
{
	return _mm_rcp_ps(_Vec);
}

EInline SVec128 EVecRcpLen3(const SVec128 _Vec)
{
	return _mm_rsqrt_ps(EVecDot3(_Vec, _Vec));
}

EInline SVec128 EVecSafeRcpLen3(const SVec128 _Vec)
{
	SVec128 len_sq = EVecDot3(_Vec, _Vec);
	SVec128 rcp = EVecGetFloatX(len_sq) != 0.f ? _mm_rsqrt_ps(len_sq) : EVecConst(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	return rcp;
}

EInline SVec128 EVecRcpLen4(const SVec128 _Vec)
{
	return _mm_rsqrt_ps(EVecDot4(_Vec, _Vec));
}

EInline SVec128 EVecSafeRcpLen4(const SVec128 _Vec)
{
	SVec128 len_sq = EVecDot4(_Vec, _Vec);
	SVec128 rcp = EVecGetFloatX(len_sq) != 0.f ? _mm_rsqrt_ps(len_sq) : EVecConst(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	return rcp;
}

EInline SVec128 EVecLen3(const SVec128 _Vec)
{
	return _mm_sqrt_ps(EVecDot3(_Vec, _Vec));
}

EInline SVec128 EVecLen4(const SVec128 _Vec)
{
	return _mm_sqrt_ps(EVecDot4(_Vec, _Vec));
}

EInline SVec128 EVecNorm3(const SVec128 _Vec)
{
	return _mm_mul_ps(_mm_rsqrt_ps(EVecDot3(_Vec, _Vec)), _Vec);
}

EInline SVec128 EVecSafeNorm3(const SVec128 _Vec)
{
	SVec128 len_sq = EVecDot3(_Vec, _Vec);
	SVec128 rcp = EVecGetFloatX(len_sq) != 0.f ? _mm_rsqrt_ps(len_sq) : g_XMZero;
	return _mm_mul_ps(rcp, _Vec);
}

EInline SVec128 EVecNorm4(const SVec128 _Vec)
{
	return _mm_mul_ps(_mm_rsqrt_ps(EVecDot4(_Vec, _Vec)), _Vec);
}

EInline SVec128 EVecSafeNorm4(const SVec128 _Vec)
{
	SVec128 len_sq = EVecDot4(_Vec, _Vec);
	SVec128 rcp = EVecGetFloatX(len_sq) != 0.f ? _mm_rsqrt_ps(len_sq) : g_XMZero;
	return _mm_mul_ps(rcp, _Vec);
}

EInline SVec128 EVecReflect(const SVec128 _incoming_direction, const SVec128 _normal)
{
	SVec128 dot = EVecDot3(_incoming_direction, _normal);
	SVec128 dot2 = EVecAdd(dot,dot);
	return EVecSub(_incoming_direction, EVecMul(dot2, _normal));
}

// --------------------------------------------------------------------------------
// Matrix routines
// --------------------------------------------------------------------------------

EInline void EMatIdentity(SMatrix4x4 &_Mat)
{
	_Mat.r[0] = g_XMIdentityR0;
	_Mat.r[1] = g_XMIdentityR1;
	_Mat.r[2] = g_XMIdentityR2;
	_Mat.r[3] = g_XMIdentityR3;
}

EInline void EMatIdentity(SMatrix3x4 &_Mat)
{
	_Mat.r[0] = g_XMIdentityR0;
	_Mat.r[1] = g_XMIdentityR1;
	_Mat.r[2] = g_XMIdentityR2;
}

EInline SMatrix4x4 EMatTranspose(const SMatrix4x4 &_Mat)
{
	SMatrix4x4 mResult;

	SVec128 vTemp1 = _mm_shuffle_ps(_Mat.r[0], _Mat.r[1], _MM_SHUFFLE(1, 0, 1, 0));
	SVec128 vTemp3 = _mm_shuffle_ps(_Mat.r[0], _Mat.r[1], _MM_SHUFFLE(3, 2, 3, 2));
	SVec128 vTemp2 = _mm_shuffle_ps(_Mat.r[2], _Mat.r[3], _MM_SHUFFLE(1, 0, 1, 0));
	SVec128 vTemp4 = _mm_shuffle_ps(_Mat.r[2], _Mat.r[3], _MM_SHUFFLE(3, 2, 3, 2));

	mResult.r[0] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));
	mResult.r[1] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));
	mResult.r[2] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));
	mResult.r[3] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(3, 1, 3, 1));

	return mResult;
}

EInline void EMatScale(SMatrix4x4 &_Mat, const float _sx, const float _sy, const float _sz)
{
	_Mat.r[0] = EVecConst(_sx, 0.f, 0.f, 0.f);
	_Mat.r[1] = EVecConst(0.f, _sy, 0.f, 0.f);
	_Mat.r[2] = EVecConst(0.f, 0.f, _sz, 0.f);
	_Mat.r[3] = g_XMIdentityR3;
}

EInline void EMatTranslate(SMatrix4x4 &_Mat, const float _x, const float _y, const float _z)
{
	_Mat.r[0] = g_XMIdentityR0;
	_Mat.r[1] = g_XMIdentityR1;
	_Mat.r[2] = g_XMIdentityR2;
	_Mat.r[3] = EVecConst(_x, _y, _z, 1.f);
}

EInline void EMatTranslate(SMatrix4x4 &_Mat, const SVec128 _posxyz)
{
	_Mat.r[0] = g_XMIdentityR0;
	_Mat.r[1] = g_XMIdentityR1;
	_Mat.r[2] = g_XMIdentityR2;
	_Mat.r[3] = _posxyz;
}

EInline float ERadianToDegree(const float _rads)
{
	return (_rads/E_PI)*180.f;
}

EInline float EDegreeToRadian(const float _degs)
{
	return (_degs/180.f)*E_PI;
}

EInline void EMatRotateX(SMatrix4x4 &_Mat, const float _angle)
{
	float sn = sinf(_angle);
	float cs = cosf(_angle);

	SVec128 vSin = _mm_set_ss(sn);
	SVec128 vCos = _mm_set_ss(cs);
	vCos = _mm_shuffle_ps(vCos, vSin, _MM_SHUFFLE(3, 0, 0, 3));
	_Mat.r[0] = g_XMIdentityR0;
	_Mat.r[1] = vCos;
	vCos = _mm_shuffle_ps(vCos, vCos, _MM_SHUFFLE(3, 1, 2, 0));
	vCos = _mm_mul_ps(vCos, g_XMNegateY);
	_Mat.r[2] = vCos;
	_Mat.r[3] = g_XMIdentityR3;
}

EInline void EMatRotateY(SMatrix4x4 &_Mat, const float _angle)
{
	float sn = sinf(_angle);
	float cs = cosf(_angle);

	SVec128 vSin = _mm_set_ss(sn);
	SVec128 vCos = _mm_set_ss(cs);
	vSin = _mm_shuffle_ps(vSin, vCos, _MM_SHUFFLE(3, 0, 3, 0));
	_Mat.r[2] = vSin;
	_Mat.r[1] = g_XMIdentityR1;
	vSin = _mm_shuffle_ps(vSin, vSin, _MM_SHUFFLE(3, 0, 1, 2));
	vSin = _mm_mul_ps(vSin, g_XMNegateZ);
	_Mat.r[0] = vSin;
	_Mat.r[3] = g_XMIdentityR3;
}

EInline void EMatRotateZ(SMatrix4x4 &_Mat, const float _angle)
{
	float sn = sinf(_angle);
	float cs = cosf(_angle);

	SVec128 vSin = _mm_set_ss(sn);
	SVec128 vCos = _mm_set_ss(cs);
	vCos = _mm_unpacklo_ps(vCos, vSin);
	_Mat.r[0] = vCos;
	vCos = _mm_shuffle_ps(vCos, vCos, _MM_SHUFFLE(3, 2, 0, 1));
	vCos = _mm_mul_ps(vCos, g_XMNegateX);
	_Mat.r[1] = vCos;
	_Mat.r[2] = g_XMIdentityR2;
	_Mat.r[3] = g_XMIdentityR3;
}

EInline void EMatToEuler(SMatrix4x4 _Mat, float &_x, float &_y, float &_z)
{
	const float epsilon = 0.999999f;
	if (_Mat.m[0][2] > epsilon)
	{
		_x = atan2f(-_Mat.m[1][0], -_Mat.m[2][0]);
		_y = -E_PI_HALF;
		_z = 0.0f;
	}
	else if (_Mat.m[0][2] < -epsilon)
	{
		_x = atan2f(_Mat.m[1][0], _Mat.m[2][0]);
		_y = E_PI_HALF;
		_z = 0.0f;
	}
	else
	{
		_x = atan2f(_Mat.m[1][2], _Mat.m[2][2]);
		_y = asinf(-_Mat.m[0][2]);
		_z = atan2f(_Mat.m[0][1], _Mat.m[0][0]);
	}
	_x = ERadianToDegree(_x);
	_y = ERadianToDegree(_y);
	_z = ERadianToDegree(_z);
}

EInline SVec128 EMatToQuaternion(SMatrix4x4 _Mat)
{
	float w = sqrtf(1.f + _Mat.m[0][0] + _Mat.m[1][1] + _Mat.m[2][2]) / 2.f;
	float w4 = (4.f * w);
	float x = (_Mat.m[1][2] - _Mat.m[2][1]) / w4;
	float y = (_Mat.m[2][0] - _Mat.m[0][2]) / w4;
	float z = (_Mat.m[0][1] - _Mat.m[1][0]) / w4;

	return EVecConst(x,y,z,w);
}

EInline void EMatFromQuaternion(SMatrix4x4 &_matrix, const SVec128 _quaternion)
{
	SVec128 Constant1110 = EVecConst(1.0f, 1.0f, 1.0f, 0.0f);

	SVec128 Q0 = _mm_add_ps(_quaternion, _quaternion);
	SVec128 Q1 = _mm_mul_ps(_quaternion, Q0);

	SVec128 V0 = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(3, 0, 0, 1));
	V0 = _mm_and_ps(V0, g_XMMask3);
	SVec128 V1 = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(3, 1, 2, 2));
	V1 = _mm_and_ps(V1, g_XMMask3);
	SVec128 R0 = _mm_sub_ps(Constant1110, V0);
	R0 = _mm_sub_ps(R0, V1);

	V0 = _mm_shuffle_ps(_quaternion, _quaternion, _MM_SHUFFLE(3, 1, 0, 0));
	V1 = _mm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(3, 2, 1, 2));
	V0 = _mm_mul_ps(V0, V1);

	V1 = _mm_shuffle_ps(_quaternion, _quaternion, _MM_SHUFFLE(3, 3, 3, 3));
	SVec128 V2 = _mm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(3, 0, 2, 1));
	V1 = _mm_mul_ps(V1, V2);

	SVec128 R1 = _mm_add_ps(V0, V1);
	SVec128 R2 = _mm_sub_ps(V0, V1);

	V0 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(1, 0, 2, 1));
	V0 = _mm_shuffle_ps(V0, V0, _MM_SHUFFLE(1, 3, 2, 0));
	V1 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(2, 2, 0, 0));
	V1 = _mm_shuffle_ps(V1, V1, _MM_SHUFFLE(2, 0, 2, 0));

	Q1 = _mm_shuffle_ps(R0, V0, _MM_SHUFFLE(1, 0, 3, 0));
	Q1 = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(1, 3, 2, 0));
	_matrix.r[0] = Q1;

	Q1 = _mm_shuffle_ps(R0, V0, _MM_SHUFFLE(3, 2, 3, 1));
	Q1 = _mm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(1, 3, 0, 2));
	_matrix.r[1] = Q1;

	Q1 = _mm_shuffle_ps(V1, R0, _MM_SHUFFLE(3, 2, 1, 0));
	_matrix.r[2] = Q1;

	_matrix.r[3] = g_XMIdentityR3;
}

EInline void EMatMultiply(SMatrix4x4 &_Mat, const SMatrix4x4 &_Mat1, const SMatrix4x4 &_Mat2)
{
	SVec128 vW = _Mat1.r[0];
	SVec128 vX = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(0, 0, 0, 0));
	SVec128 vY = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(1, 1, 1, 1));
	SVec128 vZ = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(2, 2, 2, 2));
	vW = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(3, 3, 3, 3));
	vX = _mm_mul_ps(vX, _Mat2.r[0]);
	vY = _mm_mul_ps(vY, _Mat2.r[1]);
	vZ = _mm_mul_ps(vZ, _Mat2.r[2]);
	vW = _mm_mul_ps(vW, _Mat2.r[3]);
	vX = _mm_add_ps(vX, vZ);
	vY = _mm_add_ps(vY, vW);
	vX = _mm_add_ps(vX, vY);
	_Mat.r[0] = vX;
	vW = _Mat1.r[1];
	vX = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(0, 0, 0, 0));
	vY = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(1, 1, 1, 1));
	vZ = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(2, 2, 2, 2));
	vW = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(3, 3, 3, 3));
	vX = _mm_mul_ps(vX, _Mat2.r[0]);
	vY = _mm_mul_ps(vY, _Mat2.r[1]);
	vZ = _mm_mul_ps(vZ, _Mat2.r[2]);
	vW = _mm_mul_ps(vW, _Mat2.r[3]);
	vX = _mm_add_ps(vX, vZ);
	vY = _mm_add_ps(vY, vW);
	vX = _mm_add_ps(vX, vY);
	_Mat.r[1] = vX;
	vW = _Mat1.r[2];
	vX = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(0, 0, 0, 0));
	vY = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(1, 1, 1, 1));
	vZ = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(2, 2, 2, 2));
	vW = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(3, 3, 3, 3));
	vX = _mm_mul_ps(vX, _Mat2.r[0]);
	vY = _mm_mul_ps(vY, _Mat2.r[1]);
	vZ = _mm_mul_ps(vZ, _Mat2.r[2]);
	vW = _mm_mul_ps(vW, _Mat2.r[3]);
	vX = _mm_add_ps(vX, vZ);
	vY = _mm_add_ps(vY, vW);
	vX = _mm_add_ps(vX, vY);
	_Mat.r[2] = vX;
	vW = _Mat1.r[3];
	vX = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(0, 0, 0, 0));
	vY = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(1, 1, 1, 1));
	vZ = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(2, 2, 2, 2));
	vW = E_SINGLE_SHUFFLE(vW, _MM_SHUFFLE(3, 3, 3, 3));
	vX = _mm_mul_ps(vX, _Mat2.r[0]);
	vY = _mm_mul_ps(vY, _Mat2.r[1]);
	vZ = _mm_mul_ps(vZ, _Mat2.r[2]);
	vW = _mm_mul_ps(vW, _Mat2.r[3]);
	vX = _mm_add_ps(vX, vZ);
	vY = _mm_add_ps(vY, vW);
	vX = _mm_add_ps(vX, vY);
	_Mat.r[3] = vX;
}

EInline SMatrix4x4 EMatInverse(const SMatrix4x4 &_Mat)
{
	SMatrix4x4 MT = EMatTranspose(_Mat);
	SVec128 V00 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(1, 1, 0, 0));
	SVec128 V10 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(3, 2, 3, 2));
	SVec128 V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(1, 1, 0, 0));
	SVec128 V11 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(3, 2, 3, 2));
	SVec128 V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(2, 0, 2, 0));
	SVec128 V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(3, 1, 3, 1));
	SVec128 D0 = _mm_mul_ps(V00, V10);
	SVec128 D1 = _mm_mul_ps(V01, V11);
	SVec128 D2 = _mm_mul_ps(V02, V12);
	V00 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(3, 2, 3, 2));
	V10 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(1, 1, 0, 0));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(3, 2, 3, 2));
	V11 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(1, 1, 0, 0));
	V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(3, 1, 3, 1));
	V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(2, 0, 2, 0));
	V00 = _mm_mul_ps(V00, V10);
	V01 = _mm_mul_ps(V01, V11);
	V02 = _mm_mul_ps(V02, V12);
	D0 = _mm_sub_ps(D0, V00);
	D1 = _mm_sub_ps(D1, V01);
	D2 = _mm_sub_ps(D2, V02);
	V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
	V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(1, 0, 2, 1));
	V10 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(0, 3, 0, 2));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(0, 1, 0, 2));
	V11 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(2, 1, 2, 1));
	SVec128 V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
	V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(1, 0, 2, 1));
	V12 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(0, 3, 0, 2));
	SVec128 V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(0, 1, 0, 2));
	V13 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(2, 1, 2, 1));
	SVec128 C0 = _mm_mul_ps(V00, V10);
	SVec128 C2 = _mm_mul_ps(V01, V11);
	SVec128 C4 = _mm_mul_ps(V02, V12);
	SVec128 C6 = _mm_mul_ps(V03, V13);
	V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
	V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(2, 1, 3, 2));
	V10 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(2, 1, 0, 3));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(1, 3, 2, 3));
	V11 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(0, 2, 1, 2));
	V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
	V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(2, 1, 3, 2));
	V12 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(2, 1, 0, 3));
	V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(1, 3, 2, 3));
	V13 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(0, 2, 1, 2));
	V00 = _mm_mul_ps(V00, V10);
	V01 = _mm_mul_ps(V01, V11);
	V02 = _mm_mul_ps(V02, V12);
	V03 = _mm_mul_ps(V03, V13);
	C0 = _mm_sub_ps(C0, V00);
	C2 = _mm_sub_ps(C2, V01);
	C4 = _mm_sub_ps(C4, V02);
	C6 = _mm_sub_ps(C6, V03);
	V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(0, 3, 0, 3));
	V10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
	V10 = _mm_shuffle_ps(V10, V10, _MM_SHUFFLE(0, 2, 3, 0));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(2, 0, 3, 1));
	V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
	V11 = _mm_shuffle_ps(V11, V11, _MM_SHUFFLE(2, 1, 0, 3));
	V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(0, 3, 0, 3));
	V12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
	V12 = _mm_shuffle_ps(V12, V12, _MM_SHUFFLE(0, 2, 3, 0));
	V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(2, 0, 3, 1));
	V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
	V13 = _mm_shuffle_ps(V13, V13, _MM_SHUFFLE(2, 1, 0, 3));
	V00 = _mm_mul_ps(V00, V10);
	V01 = _mm_mul_ps(V01, V11);
	V02 = _mm_mul_ps(V02, V12);
	V03 = _mm_mul_ps(V03, V13);
	SVec128 C1 = _mm_sub_ps(C0, V00);
	C0 = _mm_add_ps(C0, V00);
	SVec128 C3 = _mm_add_ps(C2, V01);
	C2 = _mm_sub_ps(C2, V01);
	SVec128 C5 = _mm_sub_ps(C4, V02);
	C4 = _mm_add_ps(C4, V02);
	SVec128 C7 = _mm_add_ps(C6, V03);
	C6 = _mm_sub_ps(C6, V03);
	C0 = _mm_shuffle_ps(C0, C1, _MM_SHUFFLE(3, 1, 2, 0));
	C2 = _mm_shuffle_ps(C2, C3, _MM_SHUFFLE(3, 1, 2, 0));
	C4 = _mm_shuffle_ps(C4, C5, _MM_SHUFFLE(3, 1, 2, 0));
	C6 = _mm_shuffle_ps(C6, C7, _MM_SHUFFLE(3, 1, 2, 0));
	C0 = _mm_shuffle_ps(C0, C0, _MM_SHUFFLE(3, 1, 2, 0));
	C2 = _mm_shuffle_ps(C2, C2, _MM_SHUFFLE(3, 1, 2, 0));
	C4 = _mm_shuffle_ps(C4, C4, _MM_SHUFFLE(3, 1, 2, 0));
	C6 = _mm_shuffle_ps(C6, C6, _MM_SHUFFLE(3, 1, 2, 0));
	SVec128 vTemp = EVecDot4(C0, MT.r[0]);
	SVec128 One = EVecConst(1.f, 1.f, 1.f, 1.f);
	vTemp = _mm_div_ps(One, vTemp);
	SMatrix4x4 mResult;
	mResult.r[0] = _mm_mul_ps(C0, vTemp);
	mResult.r[1] = _mm_mul_ps(C2, vTemp);
	mResult.r[2] = _mm_mul_ps(C4, vTemp);
	mResult.r[3] = _mm_mul_ps(C6, vTemp);
	return mResult;
}

EInline SMatrix4x4 EMatLookAtRightHanded(const SVec128 _eye, const SVec128 _at, const SVec128 _up)
{
	SMatrix4x4 mat;

	SVec128 EyeDirection = EVecSub(_eye, _at);
	SVec128 R2 = EVecNorm3(EyeDirection);
	SVec128 R0 = EVecCross3(_up, R2);
	R0 = EVecNorm3(R0);
	SVec128 R1 = EVecCross3(R2, R0);
	SVec128 NegEyePosition = EVecMul(_eye, g_XMNegativeOne);
	SVec128 D0 = EVecDot3(R0, NegEyePosition);
	SVec128 D1 = EVecDot3(R1, NegEyePosition);
	SVec128 D2 = EVecDot3(R2, NegEyePosition);
	R0 = _mm_and_ps(R0, g_XMMask3);
	R1 = _mm_and_ps(R1, g_XMMask3);
	R2 = _mm_and_ps(R2, g_XMMask3);
	D0 = _mm_and_ps(D0, g_XMMaskW);
	D1 = _mm_and_ps(D1, g_XMMaskW);
	D2 = _mm_and_ps(D2, g_XMMaskW);
	D0 = _mm_or_ps(D0, R0);
	D1 = _mm_or_ps(D1, R1);
	D2 = _mm_or_ps(D2, R2);
	mat.r[0] = D0;
	mat.r[1] = D1;
	mat.r[2] = D2;
	mat.r[3] = g_XMIdentityR3;
	//mat = EMatTranspose(mat);
	return mat;
}

EInline SMatrix4x4 EMatPersRightHanded(const float _FovAngleY,const float _AspectRatio,const float _NearZ,const float _FarZ)
{
	SMatrix4x4 mat;

	float SinFov = sinf(0.5f * _FovAngleY);
	float CosFov = cosf(0.5f * _FovAngleY);
	float fRange = _FarZ / (_NearZ - _FarZ);

	float Height = CosFov / SinFov;
	SVec128 rMem = {
		Height / _AspectRatio,
		Height,
		fRange,
		fRange * _NearZ
	};

	SVec128 vValues = rMem;
	SVec128 vTemp = _mm_setzero_ps();
	vTemp = _mm_move_ss(vTemp, vValues);
	mat.r[0] = vTemp;
	vTemp = vValues;
	vTemp = _mm_and_ps(vTemp, g_XMMaskY);
	mat.r[1] = vTemp;
	vTemp = _mm_setzero_ps();
	vValues = _mm_shuffle_ps(vValues, g_XMNegIdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
	vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
	mat.r[2] = vTemp;
	vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
	mat.r[3] = vTemp;

	return mat;
}

// --------------------------------------------------------------------------------
// Limits
// --------------------------------------------------------------------------------

EInline SVec128 EVecFloor(const SVec128 _vec)
{
	return _mm_floor_ps(_vec);
}

EInline SVec128 EVecCeiling(const SVec128 _vec)
{
	return _mm_ceil_ps(_vec);
}

// --------------------------------------------------------------------------------
// Scalar
// --------------------------------------------------------------------------------

EInline float EFloor(const float _val)
{
	return floorf(_val);
}

EInline float ECeiling(const float _val)
{
	return ceilf(_val);
}

// --------------------------------------------------------------------------------
//
// Common properties
//  Basis coefficients:
//  (_t is between 0..1)
//  const float t2 = _t*_t;
//  const float t3 = _t*_t*_t;
//  float h1 =  2.0f*t3 - 3.0f*t2 + 1.0f;
//  float h2 = -2.0f*t3 + 3.0f*t2 + 0.0f;
//  float h3 =  1.0f*t3 - 2.0f*t2 + _t;
//  float h4 =  1.0f*t3 - 1.0f*t2 + 0.0f;
//
// All spline forms take 4 vec4s as their control points and return one vec4.
//
// Hermite spline
// Control points:
//   endpoint1 endpoint2 tangent1 tangent2
// Point on the curve:
//   point_on_curve = h1*enpoint1 + h2*endpoint2 + h3*tangent1 + h4*tangent2;
//
// Catmul-Rom spline
// Control points:
//   endpoint0 endpoint1 endpoint2 endpoint3
// Tangents:
//   tangent1 = 0.5*(endpoint2 - endpoint0);
//   tangent2 = 0.5*(endpoint3 - endpoint1);
// Point on the curve:
//   same as Hermite
//
// Cardinal spline
// Control points:
//   endpoint0 endpoint1 endpoint2 endpoint3
// Tangents:
//   (_a is between 0..1)
//   tangent1 = _a*(endpoint2 - endpoint0);
//   tangent2 = _a*(endpoint3 - endpoint1);
// Point on the curve:
//   same as Hermite
//
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Hermite spline
// --------------------------------------------------------------------------------

EInline SVec128 EEvaluateCubicHermiteSpline(const SHermiteSpline *_h, const SVec128 &_tvec)
{
	//p(t) = (2t3 - 3t2 + 1)p0 + (t3 - 2t2 + t)m0 + (-2t3 + 3t2)p1 + (t3 - t2)m1

	const SVec128 c0 = EVecConst( 2.f,  1.f,                  -2.f,  1.f);
	const SVec128 c1 = EVecConst(-3.f, -2.f,                   3.f, -1.f);
	const SVec128 c2 = EVecConst( 1.f,  EVecGetFloatX(_tvec),  0.f,  0.f);

	SVec128 t2 = EVecMul(_tvec, _tvec);
	SVec128 t3 = EVecMul(t2, _tvec);

	SVec128 H1234 = EVecAdd( EVecAdd( EVecMul(c0, t3), EVecMul(c1, t2) ), c2);

	SVec128 H1 = EVecSplatX(H1234);
	SVec128 H2 = EVecSplatY(H1234);
	SVec128 H3 = EVecSplatZ(H1234);
	SVec128 H4 = EVecSplatW(H1234);

	return EVecAdd(
		EVecAdd(EVecMul(H1, _h->m_p0), EVecMul(H2, _h->m_t0)),
		EVecAdd(EVecMul(H3, _h->m_p1), EVecMul(H4, _h->m_t1)));
}

EInline SHermiteSpline EKeyframeAdjustHermiteSpline(const SHermiteSpline *_h, const float _num_keyframes_before, const float _num_keyframes)
{
	SHermiteSpline newhermite;
	newhermite.m_p0 = _h->m_p0;
	newhermite.m_p1 = _h->m_p1;

	float adj1 = (2.f*_num_keyframes_before) / (_num_keyframes_before + _num_keyframes);
	float adj2 = (2.f*_num_keyframes) / (_num_keyframes_before + _num_keyframes);

	SVec128 TA1 = EVecConst(adj1, adj1, adj1, adj1);
	SVec128 TA2 = EVecConst(adj2, adj2, adj2, adj2);

	newhermite.m_t0 = EVecMul(_h->m_t0, TA1);
	newhermite.m_t1 = EVecMul(_h->m_t1, TA2);

	return newhermite;
}

// --------------------------------------------------------------------------------
// Line
// --------------------------------------------------------------------------------

EInline int EIntersectLine2D(float p0[2], float p1[2], float p2[2], float p3[2], float *i)
{
	float s1[2], s2[2];
	s1[0] = p1[0] - p0[0];
	s1[1] = p1[1] - p0[1];
	s2[0] = p3[0] - p2[0];
	s2[1] = p3[1] - p2[1];

	float det = (-s2[0] * s1[1] + s1[0] * s2[1]);
	if (det == 0.f)
		return 0; // No intersection

	float s, t;
	s = (-s1[1] * (p0[0] - p2[0]) + s1[0] * (p0[1] - p2[1])) / det;
	t = (s2[0] * (p0[1] - p2[1]) - s2[1] * (p0[0] - p2[0])) / det;

	// We don't need to hit within the partial ray's bounds, anywhere on the infinite line is OK
	//if (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f)
	{
		// Collision detected
		i[0] = p0[0] + (t * s1[0]);
		i[1] = p0[1] + (t * s1[1]);
		return 1;
	}

	//return 0; // No collision
}

// --------------------------------------------------------------------------------
// Vector Lerp
// --------------------------------------------------------------------------------

EInline SVec128 EVecLerp(const SVec128 _Vec1, const SVec128 _Vec2, const SVec128 _t)
{
	return EVecAdd(EVecMul(_Vec2, _t), EVecMul(_Vec1, EVecSub(g_XMOne, _t)));
}

// --------------------------------------------------------------------------------
// Quaternion Slerp
// https://www.geometrictools.com/Documentation/FastAndAccurateSlerp.pdf
// --------------------------------------------------------------------------------

const float opmu = 1.85298109240830f;
const SVec128 u0123 = _mm_setr_ps(1.f / ( 1*3 ) , 1.f / ( 2*5 ) , 1.f / ( 3*7 ) , 1.f / ( 4 * 9 ) );
const SVec128 u4567 = _mm_setr_ps(1.f / ( 5*11 ) , 1.f / ( 6*13 ) , 1.f / ( 7*15 ) , opmu / ( 8 * 17 ) );
const SVec128 v0123 = _mm_setr_ps(1.f / 3 , 2.f / 5 , 3.f / 7 , 4.f / 9 );
const SVec128 v4567 = _mm_setr_ps(5.f / 11 , 6.f / 13 , 7.f / 15 , opmu*8 / 17 );

EInline SVec128 EVecSlerpCoef(const SVec128 _t, const SVec128 _xm1)
{
	SVec128 sqrT = _mm_mul_ps(_t, _t);
	SVec128 b0123, b4567, b, c;

	b4567 = _mm_mul_ps(u4567, sqrT);
	b4567 = _mm_sub_ps(b4567, v4567);
	b4567 = _mm_mul_ps(b4567, _xm1);

	b = _mm_shuffle_ps(b4567, b4567, _MM_SHUFFLE(3, 3, 3, 3));
	c = _mm_add_ps(b, g_XMOne);

	b = _mm_shuffle_ps(b4567, b4567, _MM_SHUFFLE(2, 2, 2, 2));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c );

	b = _mm_shuffle_ps(b4567, b4567, _MM_SHUFFLE(1, 1, 1, 1));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c);

	b = _mm_shuffle_ps(b4567, b4567, _MM_SHUFFLE(0, 0, 0, 0));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c);

	b0123 = _mm_mul_ps(u0123, sqrT);
	b0123 = _mm_sub_ps(b0123, v0123);
	b0123 = _mm_mul_ps(b0123, _xm1);

	b = _mm_shuffle_ps(b0123, b0123, _MM_SHUFFLE(3, 3, 3, 3));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c);

	b = _mm_shuffle_ps(b0123, b0123, _MM_SHUFFLE(2, 2, 2, 2));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c);

	b = _mm_shuffle_ps(b0123, b0123, _MM_SHUFFLE(1, 1, 1, 1));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c);

	b = _mm_shuffle_ps(b0123, b0123, _MM_SHUFFLE(0, 0, 0, 0));
	c = _mm_mul_ps(b, c);
	c = _mm_add_ps(g_XMOne, c);
	c = _mm_mul_ps(_t, c);

	return c;
}

EInline SVec128 EQuatSlerp(const SVec128 _q0, const SVec128 _q1, const SVec128 _t)
{
	SVec128 x = EVecDot4(_q0,_q1);
	SVec128 sign = _mm_and_ps(g_SignMask, x);
	x = _mm_xor_ps(sign, x);
	SVec128 localQ1 = _mm_xor_ps(sign, _q1);
	SVec128 xm1 = _mm_sub_ps(x, g_XMOne);
	SVec128 splatD = _mm_sub_ps(g_XMOne, _t);
	SVec128 cT = EVecSlerpCoef(_t, xm1);
	SVec128 cD = EVecSlerpCoef(splatD, xm1);
	cT = _mm_mul_ps(cT, localQ1);
	cD = _mm_mul_ps(cD, _q0);
	return _mm_add_ps(cT, cD);
}

// --------------------------------------------------------------------------------
// Cubic Bezier curve
// --------------------------------------------------------------------------------

EInline void EEvaluateCubicBezierCurve(const SCubicBezierCurve *_b, float _vec[4], const float _t)
{
	// f(t) = (1-t)�A + 3t(1-t)�B + 3t�(1-t)C + t�D

	float t1 = (1.f-_t);
	float t2 = t1*t1;
	float t3 = t2*t1;
	float _t2_t1 = 3.f*_t*_t*t1;
	float _t3 = _t*_t*_t;
	float _t_t2 = 3.f*_t*t2;

	_vec[0] = t3*_b->m_A[0] + _t_t2*_b->m_B[0] + _t2_t1*_b->m_C[0] + _t3*_b->m_D[0];
	_vec[1] = t3*_b->m_A[1] + _t_t2*_b->m_B[1] + _t2_t1*_b->m_C[1] + _t3*_b->m_D[1];
	_vec[2] = t3*_b->m_A[2] + _t_t2*_b->m_B[2] + _t2_t1*_b->m_C[2] + _t3*_b->m_D[2];
	_vec[3] = t3*_b->m_A[3] + _t_t2*_b->m_B[3] + _t2_t1*_b->m_C[3] + _t3*_b->m_D[3];
}

EInline void ECubicBezierCurveFindRoots(const SCubicBezierCurve *_b, float _rootA[3], float _rootB[3])
{
	// Go to quadratic representation
	float v1[3], v2[3], v3[3];
	v1[0] = 3.f*(_b->m_B[0]-_b->m_A[0]);
	v1[1] = 3.f*(_b->m_B[1] - _b->m_A[1]);
	v1[2] = 3.f*(_b->m_B[2] - _b->m_A[2]);
	v2[0] = 3.f*(_b->m_C[0] - _b->m_B[0]);
	v2[1] = 3.f*(_b->m_C[1] - _b->m_B[1]);
	v2[2] = 3.f*(_b->m_C[2] - _b->m_B[2]);
	v3[0] = 3.f*(_b->m_D[0] - _b->m_C[0]);
	v3[1] = 3.f*(_b->m_D[1] - _b->m_C[1]);
	v3[2] = 3.f*(_b->m_D[2] - _b->m_C[2]);

	float a[3], b[3], c[3];
	a[0] = v1[0] - 2.f*v2[0] + v3[0];
	a[1] = v1[1] - 2.f*v2[1] + v3[1];
	a[2] = v1[2] - 2.f*v2[2] + v3[2];
	b[0] = 2.f*(v2[0] - v1[0]);
	b[1] = 2.f*(v2[1] - v1[1]);
	b[2] = 2.f*(v2[2] - v1[2]);
	c[0] = v1[0];
	c[1] = v1[1];
	c[2] = v1[2];

	float delta[3];
	delta[0] = b[0] * b[0] - 4.f*a[0] * c[0];
	delta[1] = b[1] * b[1] - 4.f*a[1] * c[1];
	delta[2] = b[2] * b[2] - 4.f*a[2] * c[2];

	// Return roots
	_rootA[0] = delta[0] >= 0.f ? (-b[0] + sqrtf(delta[0])) / (2.f*a[0]) : 0.f;
	_rootA[1] = delta[1] >= 0.f ? (-b[1] + sqrtf(delta[1])) / (2.f*a[1]) : 0.f;
	_rootA[2] = delta[2] >= 0.f ? (-b[2] + sqrtf(delta[2])) / (2.f*a[2]) : 0.f;
	_rootB[0] = delta[0] >= 0.f ? (-b[0] - sqrtf(delta[0])) / (2.f*a[0]) : 0.f;
	_rootB[1] = delta[1] >= 0.f ? (-b[1] - sqrtf(delta[1])) / (2.f*a[1]) : 0.f;
	_rootB[2] = delta[2] >= 0.f ? (-b[2] - sqrtf(delta[2])) / (2.f*a[2]) : 0.f;
}

EInline void EDeriveCubicBezierCurve(const SCubicBezierCurve *_b, SCubicBezierCurveFirstDerivative *_d)
{
	// First derivative
	// d / dt = t�(-3A - 9C + 3D + 9B) + t(6A + 6C - 12B) + (-3A + 3B)
	//            A                       B                 C
	// Quadratic representation:
	// v1=3(p2-p1), v2=3(p3-p2), v3=3(p4-p3)
	// a = v1-2v2+v3 = 3(-p1+3p2-3p3+p4)
	// b = 2(v2-v1) = 6(p1-2p2+p3)
	// c = v1 = 3(p2-p1)
	// Analysis:
	// delta = b^2-4ac (from (-b+sqrt(b^2-4ac))/2a)
	// A==0 -> one inflection point
	// else if delta > 0 -> two inflection points
	// else if delta < 0 -> there is a loop
	// else if delta == 0 -> there is a cusp

	_d->m_v1[0] = 3.f*(3.f*(_b->m_B[0] - _b->m_C[0]) + _b->m_D[0] - _b->m_A[0]);
	_d->m_v1[1] = 3.f*(3.f*(_b->m_B[1] - _b->m_C[1]) + _b->m_D[1] - _b->m_A[1]);
	_d->m_v1[2] = 3.f*(3.f*(_b->m_B[2] - _b->m_C[2]) + _b->m_D[2] - _b->m_A[2]);
	_d->m_v1[3] = 3.f*(3.f*(_b->m_B[3] - _b->m_C[3]) + _b->m_D[3] - _b->m_A[3]);

	_d->m_v2[0] = 6.f*(_b->m_A[0] - 2.f*_b->m_B[0] + _b->m_C[0]);
	_d->m_v2[1] = 6.f*(_b->m_A[1] - 2.f*_b->m_B[1] + _b->m_C[1]);
	_d->m_v2[2] = 6.f*(_b->m_A[2] - 2.f*_b->m_B[2] + _b->m_C[2]);
	_d->m_v2[3] = 6.f*(_b->m_A[3] - 2.f*_b->m_B[3] + _b->m_C[3]);

	_d->m_v3[0] = 3.f*(_b->m_B[0] - _b->m_A[0]);
	_d->m_v3[1] = 3.f*(_b->m_B[1] - _b->m_A[1]);
	_d->m_v3[2] = 3.f*(_b->m_B[2] - _b->m_A[2]);
	_d->m_v3[3] = 3.f*(_b->m_B[3] - _b->m_A[3]);
}

EInline void EDeriveCubicBezierCurve(const SCubicBezierCurve *_b, SCubicBezierCurveSecondDerivative *_d)
{
	// Second derivative
	// d2/dt2 = t(6D-6A+18B-18C) + (6A-12B+6C)

	_d->m_v1[0] = 6.f*(_b->m_D[0] - _b->m_A[0] + 3.f*_b->m_B[0] - 3.f*_b->m_C[0]);
	_d->m_v1[1] = 6.f*(_b->m_D[1] - _b->m_A[1] + 3.f*_b->m_B[1] - 3.f*_b->m_C[1]);
	_d->m_v1[2] = 6.f*(_b->m_D[2] - _b->m_A[2] + 3.f*_b->m_B[2] - 3.f*_b->m_C[2]);
	_d->m_v1[3] = 6.f*(_b->m_D[3] - _b->m_A[3] + 3.f*_b->m_B[3] - 3.f*_b->m_C[3]);

	_d->m_v2[0] = 6.f*(_b->m_A[0] - 2.f*_b->m_B[0] + _b->m_C[0]);
	_d->m_v2[1] = 6.f*(_b->m_A[1] - 2.f*_b->m_B[1] + _b->m_C[1]);
	_d->m_v2[2] = 6.f*(_b->m_A[2] - 2.f*_b->m_B[2] + _b->m_C[2]);
	_d->m_v2[3] = 6.f*(_b->m_A[3] - 2.f*_b->m_B[3] + _b->m_C[3]);
}

EInline void EEvaluateCubicBezierCurveTangent(const SCubicBezierCurveFirstDerivative *_d, float _tangent[4], float _t)
{
	_tangent[0] = _t*_t*_d->m_v1[0] + _t*_d->m_v2[0] + _d->m_v3[0];
	_tangent[1] = _t*_t*_d->m_v1[1] + _t*_d->m_v2[1] + _d->m_v3[1];
	_tangent[2] = _t*_t*_d->m_v1[2] + _t*_d->m_v2[2] + _d->m_v3[2];
	_tangent[3] = _t*_t*_d->m_v1[3] + _t*_d->m_v2[3] + _d->m_v3[3];
}

// Step by _length units on derivative of a cubic bezier curve
EInline void EStepOnCubicBezierCurve(const SCubicBezierCurve *_bc, const SCubicBezierCurveFirstDerivative *_d, float &_t, float _stepsize)
{
	// NOTE: Avoid stepping too far on the parameter space
	/// D = length(t * t * v1 + t * v2 + v3);
	/// t = t + L / D;

	float tg[4];
	EEvaluateCubicBezierCurveTangent(_d, tg, _t);
	float D = 1.f / sqrtf(tg[0] * tg[0] + tg[1] * tg[1] + tg[2] * tg[2]);
	float V = _stepsize * D;

	// Fix for high deviation near t=0.0f and along high curvature areas, if a bezier curve was supplied
	if (_bc)
	{
		float p0[4], p1[4];
		EEvaluateCubicBezierCurve(_bc, p0, _t);
		EEvaluateCubicBezierCurve(_bc, p1, _t + V);
		// We actually stepped this far as a straight line segment.
		float L = sqrtf((p0[0] - p1[0])*(p0[0] - p1[0]) + (p0[1] - p1[1])*(p0[1] - p1[1]) + (p0[2] - p1[2])*(p0[2] - p1[2]));
		float Vnew = V*(_stepsize/L);
		_t += Vnew;
	}
	else
		_t += V;
}

EInline float EPickCubicBezierCurve(const SCubicBezierCurve *_b, SCubicBezierCurveFirstDerivative *_d, float _segment_length, float _pos[4])
{
	float t = 0.f;

	while (t < 1.f)
	{
		float vec[4];
		EEvaluateCubicBezierCurve(_b, vec, t);

		float D = sqrtf((vec[0] - _pos[0])*(vec[0] - _pos[0]) + (vec[1] - _pos[1])*(vec[1] - _pos[1]) + (vec[2] - _pos[2])*(vec[2] - _pos[2]));
		if (D < _segment_length)
			return t;

		EStepOnCubicBezierCurve(_b, _d, t, _segment_length);
	}

	return -1.f;
}

EInline void EEvaluateCubicBezierCurveNormal(const SCubicBezierCurveFirstDerivative *_d, SVec128 _upvector, float _normal[4], float _t)
{
	float tangent[4];
	EEvaluateCubicBezierCurveTangent(_d, tangent, _t);

	SVec128 normal = EVecCross3(_upvector, EVecNorm3(EVecConst(tangent[0], tangent[1], tangent[2], tangent[3])));

	_normal[0] = EVecGetFloatX(normal);
	_normal[1] = EVecGetFloatY(normal);
	_normal[2] = EVecGetFloatZ(normal);
	_normal[3] = EVecGetFloatW(normal);
}

EInline float ECubicBezierCurveCurvature(const SCubicBezierCurveSecondDerivative *_d2, float _normal[4], float _t)
{
	float d0 = _t*_d2->m_v1[0] + _d2->m_v2[0];
	float d1 = _t*_d2->m_v1[1] + _d2->m_v2[1];
	float d2 = _t*_d2->m_v1[2] + _d2->m_v2[2];

	return d0*_normal[0] + d1*_normal[1] + d2*_normal[2];
}

EInline void ECubicBezierCurveMapCanonical2D(const SCubicBezierCurve *_bc, SCubicBezierCurve *_canonicalCurve)
{
	float y3 = _bc->m_C[1] - _bc->m_A[1];
	float y4 = _bc->m_D[1] - _bc->m_A[1];
	float y2 = _bc->m_B[1] - _bc->m_A[1];
	float x4 = _bc->m_D[0] - _bc->m_A[0];
	float x2 = _bc->m_B[0] - _bc->m_A[0];
	float x3 = _bc->m_C[0] - _bc->m_A[0];

	float f32 = y3 / y2;
	float f42 = y4 / y2;

	float det = (x3 - x2*f32);

	EAssert(det != 0.f, "Canonical form of cubic bezier curve: division by zero imminent.");

	float mappedX = (x4-x2*f42) / det;
	float mappedY = f42 + (1.f-f32)*mappedX;

	_canonicalCurve->m_A[0] = 0.f;
	_canonicalCurve->m_A[1] = 0.f;
	_canonicalCurve->m_A[2] = 0.f;
	_canonicalCurve->m_A[3] = 0.f;
	_canonicalCurve->m_B[0] = 0.f;
	_canonicalCurve->m_B[1] = 1000.f;
	_canonicalCurve->m_B[2] = 0.f;
	_canonicalCurve->m_B[3] = 0.f;
	_canonicalCurve->m_C[0] = 1000.f;
	_canonicalCurve->m_C[1] = 1000.f;
	_canonicalCurve->m_C[2] = 0.f;
	_canonicalCurve->m_C[3] = 0.f;
	_canonicalCurve->m_D[0] = mappedX*1000.f;
	_canonicalCurve->m_D[1] = mappedY*1000.f;
	_canonicalCurve->m_D[2] = 0.f;
	_canonicalCurve->m_D[3] = 0.f;
}

EInline float EFindMinimaCanonical2D(const SCubicBezierCurve *_canonical, const SCubicBezierCurveFirstDerivative *_d1)
{
	float retVal = 0.f;
	float t = 0.f;
	float pos[4];
	EEvaluateCubicBezierCurve(_canonical, pos, t);
	float oldy = pos[1];
	while (t < 1.f)
	{
		EStepOnCubicBezierCurve(_canonical, _d1, t, 1.f);
		EEvaluateCubicBezierCurve(_canonical, pos, t);

		if (pos[1] > oldy)
		{
			oldy = pos[1];
			retVal = t;
		}
	}

	return 1.f-retVal;
}

EInline float ECubicBezierCurveFindInflectionPoint(const SCubicBezierCurve *_bc, const SCubicBezierCurveFirstDerivative *_d1, const SCubicBezierCurveSecondDerivative *_d2, float _segment_length)
{
	SVec128 upvector = g_XMIdentityR2;
	float t = 0.f;

	float normal[4];
	EEvaluateCubicBezierCurveNormal(_d1, upvector, normal, t);
	float Io = ECubicBezierCurveCurvature(_d2, normal, t);
	while (t < 1.f)
	{
		EStepOnCubicBezierCurve(_bc, _d1, t, _segment_length);
		EEvaluateCubicBezierCurveNormal(_d1, upvector, normal, t);
		float I = ECubicBezierCurveCurvature(_d2, normal, t);

		if (Io*I < 0.f) // Sign flipped, inflection point available at _segment_length resolution.
			break;

		Io = I;
	}

	return t;
}

EInline void ESplitBezierCurve(const SCubicBezierCurve *_b, float _t, SCubicBezierCurve *_s0, SCubicBezierCurve *_s1)
{
	// Find the point of intersection
	float B[4];
	EEvaluateCubicBezierCurve(_b, B, _t);

	float nt = 1.f-_t;
	float nt3 = nt*nt*nt;
	float t3 = _t*_t*_t + nt3;

	// (1-t)^3 / (t^3+(1-t)^3)
	float ut = nt3/t3;
	float nut = 1.f-ut;
	float C[4] = { ut*_b->m_A[0] + nut*_b->m_D[0], ut*_b->m_A[1] + nut*_b->m_D[1], ut*_b->m_A[2] + nut*_b->m_D[2], ut*_b->m_A[3] + nut*_b->m_D[3] };

	// ratio(t) = (t^3+(1-t)^3-1.f)/(t^3+(1-t)^3)
	// A=B+((B-C)/ratio(t))
	float rt = 1.f / fabsf((t3-1.f) / t3);
	float A[4] = { B[0] + ((B[0] - C[0]) * rt), B[1] + ((B[1] - C[1]) * rt), B[2] + ((B[2] - C[2]) * rt), B[3] + ((B[3] - C[3]) * rt) };

	// Intermediate points for the A-B-C ratio trick.
	float CD[4] = { _b->m_C[0] + (_b->m_D[0] - _b->m_C[0])*_t, _b->m_C[1] + (_b->m_D[1] - _b->m_C[1])*_t, _b->m_C[2] + (_b->m_D[2] - _b->m_C[2])*_t, _b->m_C[3] + (_b->m_D[3] - _b->m_C[3])*_t };
	float AB[4] = { _b->m_A[0] + (_b->m_B[0] - _b->m_A[0])*_t, _b->m_A[1] + (_b->m_B[1] - _b->m_A[1])*_t, _b->m_A[2] + (_b->m_B[2] - _b->m_A[2])*_t, _b->m_A[3] + (_b->m_B[3] - _b->m_A[3])*_t };
	float AAB[4] = { AB[0] + (A[0] - AB[0])*_t, AB[1] + (A[1] - AB[1])*_t, AB[2] + (A[2] - AB[2])*_t, AB[3] + (A[3] - AB[3])*_t };
	float ACD[4] = { A[0] + (CD[0] - A[0])*_t, A[1] + (CD[1] - A[1])*_t, A[2] + (CD[2] - A[2])*_t, A[3] + (CD[3] - A[3])*_t };

	// Split hull 0: A AB AAB B
	if (_s0)
	{
		_s0->m_A[0] = _b->m_A[0];	_s0->m_A[1] = _b->m_A[1];	_s0->m_A[2] = _b->m_A[2];	_s0->m_A[3] = _b->m_A[3];
		_s0->m_B[0] = AB[0];		_s0->m_B[1] = AB[1];		_s0->m_B[2] = AB[2];		_s0->m_B[3] = AB[3];
		_s0->m_C[0] = AAB[0];		_s0->m_C[1] = AAB[1];		_s0->m_C[2] = AAB[2];		_s0->m_C[3] = AAB[3];
		_s0->m_D[0] = B[0];			_s0->m_D[1] = B[1];			_s0->m_D[2] = B[2];			_s0->m_D[3] = B[3];
	}

	// Split hull 1: B ACD CD D
	if (_s1)
	{
		_s1->m_A[0] = B[0];			_s1->m_A[1] = B[1];			_s1->m_A[2] = B[2];			_s1->m_A[3] = B[3];
		_s1->m_B[0] = ACD[0];		_s1->m_B[1] = ACD[1];		_s1->m_B[2] = ACD[2];		_s1->m_B[3] = ACD[3];
		_s1->m_C[0] = CD[0];		_s1->m_C[1] = CD[1];		_s1->m_C[2] = CD[2];		_s1->m_C[3] = CD[3];
		_s1->m_D[0] = _b->m_D[0];	_s1->m_D[1] = _b->m_D[1];	_s1->m_D[2] = _b->m_D[2];	_s1->m_D[3] = _b->m_D[3];
	}
}

// --------------------------------------------------------------------------------
// Catmul-Rom spline
// --------------------------------------------------------------------------------

EInline SVec128 EEvaluateCatmulRomSpline(const SCardinalSpline *_h, const float _t)
{
	const SVec128 tvec = EVecConst(_t, _t, _t, _t);

	const SVec128 A0 = EVecConst( 0.5f, 0.5f, 0.5f, 0.5f);
	const SVec128 c0 = EVecConst( 2.f, -2.f,  1.f,  1.f);
	const SVec128 c1 = EVecConst(-3.f,  3.f, -2.f, -1.f);
	const SVec128 c2 = EVecConst( 1.f,  0.f,   _t,  0.f);

	SVec128 t2 = EVecMul(tvec, tvec);
	SVec128 t3 = EVecMul(t2, tvec);

	SVec128 H1234 = EVecAdd( EVecAdd( EVecMul(c0, t3), EVecMul(c1, t2) ), c2);

	SVec128 H1 = EVecSplatX(H1234);
	SVec128 H2 = EVecSplatY(H1234);
	SVec128 H3 = EVecSplatZ(H1234);
	SVec128 H4 = EVecSplatW(H1234);

	SVec128 tangent1 = EVecMul(A0, EVecSub(_h->m_p2, _h->m_p0));
	SVec128 tangent2 = EVecMul(A0, EVecSub(_h->m_p3, _h->m_p1));

	SVec128 P = EVecAdd(
			EVecAdd(
				EVecMul(H1, _h->m_p1),
				EVecMul(H2, _h->m_p2)
			),
			EVecAdd(
				EVecMul(H3, tangent1),
				EVecMul(H4, tangent2)
			)
		);

	return P;
}

// --------------------------------------------------------------------------------
// Cardinal spline
// --------------------------------------------------------------------------------

EInline SVec128 EEvaluateCardinalSpline(const SCardinalSpline *_h, const float _a, const float _t)
{
	const SVec128 tvec = EVecConst(_t, _t, _t, _t);

	const SVec128 A0 = EVecConst(  _a,   _a,   _a,   _a);
	const SVec128 c0 = EVecConst( 2.f, -2.f,  1.f,  1.f);
	const SVec128 c1 = EVecConst(-3.f,  3.f, -2.f, -1.f);
	const SVec128 c2 = EVecConst( 1.f,  0.f,   _t,  0.f);

	SVec128 t2 = EVecMul(tvec, tvec);
	SVec128 t3 = EVecMul(t2, tvec);

	SVec128 H1234 = EVecAdd( EVecAdd( EVecMul(c0, t3), EVecMul(c1, t2) ), c2);

	SVec128 H1 = EVecSplatX(H1234);
	SVec128 H2 = EVecSplatY(H1234);
	SVec128 H3 = EVecSplatZ(H1234);
	SVec128 H4 = EVecSplatW(H1234);

	SVec128 tangent1 = EVecMul(A0, EVecSub(_h->m_p2, _h->m_p0));
	SVec128 tangent2 = EVecMul(A0, EVecSub(_h->m_p3, _h->m_p1));

	SVec128 P = EVecAdd(
			EVecAdd(
				EVecMul(H1, _h->m_p1),
				EVecMul(H2, _h->m_p2)
			),
			EVecAdd(
				EVecMul(H3, tangent1),
				EVecMul(H4, tangent2)
			)
		);

	return P;
}

// --------------------------------------------------------------------------------
// Axis aligned bounding box
// --------------------------------------------------------------------------------

EInline bool IsBoundingBoxHit(SBoundingBox &aabb, SVec128 rayOrigin, SVec128 rayDir, SVec128 &isect, float &enter, float &exit)
{
	SVec128 invRayDir = EVecRcp(rayDir);
	SVec128 t0 = EVecMul(EVecSub(aabb.m_Min,rayOrigin),invRayDir);
	SVec128 t1 = EVecMul(EVecSub(aabb.m_Max,rayOrigin),invRayDir);
	SVec128 tmin = EVecMin(t0, t1);
	SVec128 tmax = EVecMax(t0, t1);
	enter = EMaximum(EVecGetFloatX(tmin), EMaximum(EVecGetFloatY(tmin), EVecGetFloatZ(tmin))); // Do not clamp entry point, we wish to know if we were inside during the test
	exit = EMinimum(EVecGetFloatX(tmax), EMinimum(EVecGetFloatY(tmax), EVecGetFloatZ(tmax))); // Exit point is free (not clamped)
	isect = EVecAdd(rayOrigin, EVecMul(rayDir, EVecConst(enter,enter,enter,0.f)));
	return enter <= exit; // Also true on corners
}

EInline void EResetBounds(SBoundingBox &_bounds)
{
	_bounds.m_Min = EVecConst(FLT_MAX,FLT_MAX,FLT_MAX,1.f);
	_bounds.m_Max = EVecConst(-FLT_MAX,-FLT_MAX,-FLT_MAX,1.f);
}

EInline void EExpandBounds(SBoundingBox &_bounds, const SVec128 &_newPoint)
{
	_bounds.m_Min = EVecMin(_bounds.m_Min, _newPoint);
	_bounds.m_Max = EVecMax(_bounds.m_Max, _newPoint);
}

EInline void EJoinBounds(SBoundingBox &_bounds, const SBoundingBox &_A, const SBoundingBox &_B)
{
	_bounds.m_Min = EVecMin(_A.m_Min, _B.m_Min);
	_bounds.m_Max = EVecMax(_A.m_Max, _B.m_Max);
}

// --------------------------------------------------------------------------------
// Geometry
// --------------------------------------------------------------------------------

EInline void CalculateBarycentrics(SVec128 &P, SVec128 &v0, SVec128 &v1, SVec128 &v2, float *uvw)
{
	SVec128 e1 = EVecSub(v2, v0);
	SVec128 e2 = EVecSub(v1, v0);
	SVec128 e = EVecSub(P, v0);
	float d00 = EVecGetFloatX(EVecDot3(e1,e1));
	float d01 = EVecGetFloatX(EVecDot3(e1,e2));
	float d11 = EVecGetFloatX(EVecDot3(e2,e2));
	float d20 = EVecGetFloatX(EVecDot3(e,e1));
	float d21 = EVecGetFloatX(EVecDot3(e,e2));

	float invdenom = 1.f/(d00*d11-d01*d01);
	uvw[0] = (d11*d20-d01*d21)*invdenom;
	uvw[1] = (d00*d21-d01*d20)*invdenom;
	uvw[2] = 1.f - uvw[0] - uvw[1];
}

EInline bool HitTriangle(SVec128 &v0, SVec128 &v1, SVec128 &v2, SVec128 rayStart, SVec128 rayDir, float &t)
{
	t = FLT_MAX;

	SVec128 e1 = EVecSub(v2, v0);
	SVec128 e2 = EVecSub(v1, v0);
	SVec128 s1 = EVecCross3(rayDir, e2);
	float K = EVecGetFloatX(EVecDot3(s1, e1));
	if (K >= 0.f)
		return false; // Ignore backfacing

	float invd = 1.f/K;
	SVec128 d = EVecSub(rayStart, v0);
	float b1 = EVecGetFloatX(EVecDot3(d, s1)) * invd;
	SVec128 s2 = EVecCross3(d, e1);
	float b2 = EVecGetFloatX(EVecDot3(rayDir, s2)) * invd;
	float temp = EVecGetFloatX(EVecDot3(e2, s2)) * invd;

	if (b1<0.f || b1>1.f || b2<0.f || b1+b2>1.f || temp<0.f)
		return false;

	t = temp;
	return true;
}

EInline bool IntersectSlab(SVec128 p0, SVec128 p1, SVec128 rayOrigin, SVec128 rayDir, SVec128 invRayDir, float &enter)
{
	SVec128 t0 = EVecMul(EVecSub(p0,rayOrigin), invRayDir);
	SVec128 t1 = EVecMul(EVecSub(p1,rayOrigin), invRayDir);
	SVec128 tmin = EVecMin(t0, t1);
	SVec128 tmax = EVecMax(t0, t1);
	enter = EMaximum(0.f, EVecMaxComponent3(tmin));
	float exit = EMinimum(1.f, EVecMinComponent3(tmax));
	return enter <= exit;
}

// Intersect ray with AABB, return true and entry point if hit
EInline bool IntersectSlab(SVec128 p0, SVec128 p1, SVec128 rayOrigin, SVec128 rayDir, SVec128 invRayDir, SVec128& entryPos, SVec128& exitPos)
{
	SVec128 t0 = EVecMul(EVecSub(p0,rayOrigin), invRayDir);
	SVec128 t1 = EVecMul(EVecSub(p1,rayOrigin), invRayDir);
	SVec128 tmin = EVecMin(t0, t1);
	SVec128 tmax = EVecMax(t0, t1);
	float maxcompt = EVecMaxComponent3(tmin);
	float mincompt = EVecMinComponent3(tmax);
	float enter = EMaximum(0.f, maxcompt);
	float exit = EMinimum(1.f, mincompt);
	entryPos = EVecAdd(rayOrigin, EVecMul(rayDir,EVecConst(enter,enter,enter,1.f)));
	exitPos = EVecAdd(rayOrigin, EVecMul(rayDir,EVecConst(exit,exit,exit,1.f)));
	return enter <= exit && maxcompt>0.f && mincompt<1.f;
}

// --------------------------------------------------------------------------------------------------------------------------
// Spatial encoding helpers
// --------------------------------------------------------------------------------------------------------------------------

EInline uint32_t EMortonEncode(uint32_t _x, uint32_t _y, uint32_t _z)
{
	// Pack 3 10-bit indices into a 30-bit Morton code
	// Logic below is HLSL compatible
	_x &= 0x000003ff;	_y &= 0x000003ff;	_z &= 0x000003ff;
	_x |= (_x << 16);	_y |= (_y << 16);	_z |= (_z << 16);
	_x &= 0xff0000ff;	_y &= 0xff0000ff;	_z &= 0xff0000ff;
	_x |= (_x << 8);	_y |= (_y << 8);	_z |= (_z << 8);
	_x &= 0x0300f00f;	_y &= 0x0300f00f;	_z &= 0x0300f00f;
	_x |= (_x << 4);	_y |= (_y << 4);	_z |= (_z << 4);
	_x &= 0x030c30c3;	_y &= 0x030c30c3;	_z &= 0x030c30c3;
	_x |= (_x << 2);	_y |= (_y << 2);	_z |= (_z << 2);
	_x &= 0x09249249;	_y &= 0x09249249;	_z &= 0x09249249;
	return (_x) | (_y << 1) | (_z << 2);
}

EInline void EMortonDecode(const uint32_t _morton, uint32_t& _x, uint32_t& _y, uint32_t& _z)
{
	// Unpack 3 10-bit indices from a 30-bit Morton code
	// Logic below is HLSL compatible
	uint32_t x = (_morton);
	uint32_t y = (_morton >> 1);
	uint32_t z = (_morton >> 2);
	x &= 0x09249249;	y &= 0x09249249;	z &= 0x09249249;
	x |= (x >> 2);		y |= (y >> 2);		z |= (z >> 2);
	x &= 0x030c30c3;	y &= 0x030c30c3;	z &= 0x030c30c3;
	x |= (x >> 4);		y |= (y >> 4);		z |= (z >> 4);
	x &= 0x0300f00f;	y &= 0x0300f00f;	z &= 0x0300f00f;
	x |= (x >> 8);		y |= (y >> 8);		z |= (z >> 8);
	x &= 0xff0000ff;	y &= 0xff0000ff;	z &= 0xff0000ff;
	x |= (x >> 16);		y |= (y >> 16);		z |= (z >> 16);
	x &= 0x000003ff;	y &= 0x000003ff;	z &= 0x000003ff;
	_x = x;
	_y = y;
	_z = z;
}

EInline void EQuantizePosition(const SVec128 &_worldpos, uint32_t _qXYZ[3], SVec128 _gridAABBMin, SVec128 _gridCellSize)
{
	SVec128 gridLocalPosition = EVecDiv(EVecSub(_worldpos, _gridAABBMin), _gridCellSize);

	// Clamp within 0-SSDGridEntryCountPerAxis-1 range (inclusive)
	static const SVec128 cellClampMax{ 1023.f, 1023.f, 1023.f, 0.f};
	static const SVec128 cellClampMin{ 0.f, 0.f, 0.f, 0.f };
	gridLocalPosition = EVecMin(EVecMax(gridLocalPosition, cellClampMin), cellClampMax);

	_qXYZ[0] = uint32_t(EFloor(EVecGetFloatX(gridLocalPosition)));
	_qXYZ[1] = uint32_t(EFloor(EVecGetFloatY(gridLocalPosition)));
	_qXYZ[2] = uint32_t(EFloor(EVecGetFloatZ(gridLocalPosition)));
}