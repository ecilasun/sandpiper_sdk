#pragma once

// --------------------------------------------------------------------------------
// Wrapper macros.
// --------------------------------------------------------------------------------

#ifdef __cplusplus
    #define EXTERNC extern "C"
#else
    #define EXTERNC
#endif

#pragma once

#include <stdint.h>

#define EInline inline
//#define EInline __attribute__((always_inline))
#define EAlign(_x_) __attribute__ ((aligned (_x_)))
#define EExportLibraryFunction EXTERNC __attribute__ ((visibility("default")))

// --------------------------------------------------------------------------------
// OS headers.
// --------------------------------------------------------------------------------

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <signal.h>

#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <new>

#include <sys/types.h>
#include <sys/socket.h>
#include <asm/ioctl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>

#include <pthread.h>

#include <xmmintrin.h>

// --------------------------------------------------------------------------------
// Intel/OS specific cache, interlocked and timer functions.
// TODO: other processors
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

#ifdef __cplusplus
}
#endif

#ifndef LONG
    #define LONG long int
#endif

#ifndef LONG64
    #define LONG64 long long int
#endif

#ifndef LONGLONG
    #define LONGLONG unsigned long long
#endif

#define EReadWriteBarrier(_X_) asm volatile("": : :"memory")
#define ECPUIdle _mm_pause
#define EFlushCache _mm_clflush
#define ECompleteMemWrite _mm_sfence
#define ECompleteMemRead _mm_lfence
#define EMemoryFence _mm_mfence

/*typedef volatile LONG EInterlockedInt;

EInline long EInterlockedExchangeAdd(volatile long* Addend, long Increment)
{
    long ret;

    __asm__ __volatile__ (
        "lock; xadd %0, %1"
        :"=r" (ret), "=m" (*Addend)
        :"0" (Increment), "m" (*Addend)
        :"memory" );

    return ret;
}

EInline long long EInterlockedExchangeAdd64(volatile long long* Addend, long long Increment)
{
    long long ret;

    __asm__ __volatile__ (
        "lock; xaddl %0, %1"
        :"=r" (ret), "=m" (*Addend)
        :"0" (Increment), "m" (*Addend)
        :"memory" );

    return ret;
}

EInline long EInterlockedCompareExchange(volatile long *dest, long new_value, long old_value)
{
    long prev;

    __asm__ __volatile__ (
        "lock; cmpxchg %1, %2"
		: "=a" (prev)
		: "q" (new_value), "m" (*dest), "0" (old_value)
        : "memory");

    return prev;
}

EInline long long EInterlockedCompareExchange64(volatile long long *dest, long long new_value, long long old_value)
{
    long long prev;

    __asm__ __volatile__ (
        "lock; cmpxchgl %1, %2"
		: "=a" (prev)
		: "q" (new_value), "m" (*dest), "0" (old_value)
        : "memory");

    return prev;
}

EInline long EInterlockedExchange(volatile long* Target, long Value)
{
    long ReturnValue;

    __asm __volatile (
        "lock; xchg %1,%0"
        : "=r" (ReturnValue)
        : "m" (*Target), "0" (Value)
        : "memory" );

    return ReturnValue;
}

EInline long long EInterlockedExchange64(volatile long long* Target, long long Value)
{
    long long ReturnValue;

    __asm __volatile (
        "lock; xchg %1,%0"
        : "=r" (ReturnValue)
        : "m" (*Target), "0" (Value)
        : "memory" );

    return ReturnValue;
}

EInline long EInterlockedIncrement( volatile long* Addend )
{
    return EInterlockedExchangeAdd( Addend, 1 );
}

EInline long EInterlockedDecrement( volatile long* Addend )
{
    return EInterlockedExchangeAdd( Addend, -1 );
}

EInline long long EInterlockedIncrement64( long long* volatile Addend )
{
    return EInterlockedExchangeAdd64( Addend, 1 );
}

EInline long long EInterlockedDecrement64( long long* volatile Addend )
{
    return EInterlockedExchangeAdd64( Addend, -1 );
}

typedef volatile LONG64 EInterlockedPtr;

EInline unsigned long long __InterlockedAddPtr(EInterlockedPtr *Addend, unsigned long long Increment)
{
    unsigned long long ret;
    __asm__ __volatile__ (
        "lock; xadd %0,(%1)"
        :"=r" (ret)
        :"r" (Addend), "0" (Increment)
        :"memory" );
    return ret;
}

#define EInterlockedAddPtr(__ptr, __val) __InterlockedAddPtr(__ptr, __val)
#define EInterlockedIncrementPtr(__ptr) __InterlockedAddPtr(__ptr, 1)

typedef LONG EInt;

struct STimerContext
{
	unsigned long long m_old_count;
	float m_dt;
};

EInline unsigned long long EGetTickCount(STimerContext *_ctx)
{
    timeval ts;
    gettimeofday(&ts, 0);
    return (unsigned long long)(ts.tv_sec * 1000 + (ts.tv_usec / 1000));
}

EInline void ETimerUpdate(STimerContext *_ctx)
{
	const unsigned long long counter = EGetTickCount(_ctx);
	const unsigned long long delta_counts = counter - _ctx->m_old_count;
	_ctx->m_old_count = counter;
	_ctx->m_dt = (float)delta_counts;
}

EInline void ECreateTimerContext(STimerContext **_ctx)
{
	*_ctx = new STimerContext;
	(*_ctx)->m_old_count = EGetTickCount(*_ctx);
	(*_ctx)->m_dt = 0.f;
}

EInline void EDestroyTimerContext(STimerContext *_ctx)
{
	delete _ctx;
}

EInline void ESleep(const uint32 _milliseconds)
{
	usleep(_milliseconds * 1000);
}*/

// --------------------------------------------------------------------------------
// Platform context variant.
// --------------------------------------------------------------------------------

struct SLinuxContext
{
	Display *m_display;
	int m_screen;
	Window m_rootwindow;
	XEvent m_event;
	int *m_displayAttributes;
	Atom wm_delete;
	Cursor m_pointerCursor;
	Cursor m_fingerCursor;
};
