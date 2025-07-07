#pragma once

#if defined(PLATFORM_LINUX)

 // Irrelevant

#else // PLATFORM_WINDOWS

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

// --------------------------------------------------------------------------------
// Wrapper macros.
// --------------------------------------------------------------------------------

#define EInline __forceinline
#define EAlign(_x_) __declspec(align(_x_))
// is __attribute__ ((aligned (_x_))) for gcc
#define EExportLibraryFunction __declspec(dllexport)

// --------------------------------------------------------------------------------
// Shut up some compiler warnings.
// --------------------------------------------------------------------------------

#pragma warning(disable:4200) // zero-sized arrays (C4200) in EPropertyContext & EClassContext

// --------------------------------------------------------------------------------
// Disable some runtime library stuff.
// --------------------------------------------------------------------------------

#define NOMINMAX

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS

#ifndef DECLSPEC_DEPRECATED
#define DECLSPEC_DEPRECATED
#endif // DECLSPEC_DEPRECATED

// --------------------------------------------------------------------------------
// Define missing socket library constants.
// --------------------------------------------------------------------------------

#if !defined(SHUT_RDWR)
	#define SHUT_RDWR 2
#endif

// --------------------------------------------------------------------------------
// OS headers.
// --------------------------------------------------------------------------------

#include <windows.h>
#include <winsock.h>

#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <new>

#include <xmmintrin.h>
#include <intrin.h>

#include <atomic>

#include <time.h>
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif


// --------------------------------------------------------------------------------
// Intel/OS specific cache, interlocked and timer functions.
// --------------------------------------------------------------------------------

// TODO: Actual cache line size here
#define E_CACHELINE_SIZE 64

#define E_CACHE_ALIGN __declspec(align(E_CACHELINE_SIZE))
#define E_ALIGN(_x_) __declspec(align(_x_))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SSE2__ // Already defined on some compilers
	void _mm_clflush(void const *_p);
	void _mm_lfence(void);
	void _mm_sfence(void);
	void _mm_mfence(void);
#endif // __SSE2__

#ifdef _MSC_VER // MSVC compiler specific intrinsics
	LONG _InterlockedIncrement(LONG volatile *_p);
	LONG _InterlockedDecrement(LONG volatile *_p);
	LONG _InterlockedExchangeAdd(LONG volatile *_p, LONG _v);
	LONG _InterlockedCompareExchange(LONG volatile *_p, LONG _exch, LONG _comp);
	LONG _InterlockedExchange(LONG volatile *_p, LONG _exch);
	LONGLONG _InterlockedExchangeAdd64(LONGLONG volatile *_p, LONGLONG _v);
#else // Non-MSVC compiler
	#define _InterlockedIncrement InterlockedIncrement
	#define _InterlockedDecrement InterlockedDecrement
	#define _InterlockedExchangeAdd InterlockedExchangeAdd
	#define _InterlockedCompareExchange InterlockedCompareExchange
	#define _InterlockedExchange InterlockedExchange
#endif

#ifdef __cplusplus
}
#endif

#if !defined(__clang__)
#pragma intrinsic(_mm_pause)
#pragma intrinsic(_mm_clflush)
#pragma intrinsic(_mm_lfence)
#pragma intrinsic(_mm_mfence)
#pragma intrinsic(_mm_sfence)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_ReadWriteBarrier)
#endif

// NOTE: EReadWriteBarrier is __lwsync for PPC
//#define EReadWriteBarrier(_X_) atomic_thread_fence(std::memory_order_seq_cst) //_ReadWriteBarrier
#define EReadWriteBarrier(_X_) _STL_DISABLE_DEPRECATED_WARNING _ReadWriteBarrier() _STL_RESTORE_DEPRECATED_WARNING
#define ECPUIdle _mm_pause
#define EFlushCache _mm_clflush
#define ECompleteMemWrite _mm_sfence
#define ECompleteMemRead _mm_lfence
#define EMemoryFence _mm_mfence
#define EInterlockedCompareExchange _InterlockedCompareExchange
#define EInterlockedIncrement _InterlockedIncrement
#define EInterlockedDecrement _InterlockedDecrement
#define EInterlockedExchangeAdd _InterlockedExchangeAdd
#define EInterlockedExchange _InterlockedExchange
#define EInterlockedExchange64 _InterlockedExchange64
#define EInterlockedAddPtr _InterlockedExchangeAdd64
#define EInterlockedIncrementPtr _InterlockedIncrement64
typedef volatile LONG64 EInterlockedPtr;

typedef volatile LONG EInterlockedInt;

typedef LONG EInt;

// --------------------------------------------------------------------------------
// Timer related functions.
// --------------------------------------------------------------------------------

struct STimerContext
{
	double m_freq;
	unsigned long long m_old_count;
	float m_dt;
};

EInline unsigned long long EGetTickCount(STimerContext * /*_ctx*/)
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return (unsigned long long)t.QuadPart;
}

EInline void ETimerUpdate(STimerContext *_ctx)
{
	const unsigned long long counter = EGetTickCount(_ctx);
	const unsigned long long delta_counts = counter - _ctx->m_old_count;
	_ctx->m_old_count = counter;
	_ctx->m_dt = (float)((double)delta_counts / _ctx->m_freq);
}

EInline void ECreateTimerContext(STimerContext **_ctx)
{
	*_ctx = new STimerContext;
	LARGE_INTEGER t;
	QueryPerformanceFrequency(&t);
	(*_ctx)->m_freq = (double)t.QuadPart/1000.0;
	(*_ctx)->m_old_count = EGetTickCount(*_ctx);
	(*_ctx)->m_dt = 0.f;
}

EInline void EDestroyTimerContext(STimerContext *_ctx)
{
	delete _ctx;
}

EInline void ESleep(const uint32_t _milliseconds)
{
	SleepEx(_milliseconds, FALSE);
}

// --------------------------------------------------------------------------------
// Platform context variant.
// --------------------------------------------------------------------------------

struct SWin32Context
{
	MSG m_msg;
	HINSTANCE m_hInst;
	HANDLE m_ConsoleHandle;
	void RegisterWindowClasses(HINSTANCE _hInstance);
	HCURSOR m_arrowCursor;
	HCURSOR m_handCursor;
};

#endif // PLATFORM_WINDOWS
