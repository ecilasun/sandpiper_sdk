#pragma once

#if !defined(PLATFORM_LINUX) && !defined(PLATFORM_WINDOWS)
#define PLATFORM_LINUX
#endif

#if defined(PLATFORM_LINUX)
#include "platformlinux.h"
#include "SDL2/SDL.h"
#else // PLATFORM_WINDOWS
#include "platformwin.h"
#include "SDL.h"
#endif

#include <stdio.h>

template <class T> constexpr const T& EMinimum(const T& a, const T& b) { return a < b ? a : b; }
template <class T> constexpr const T& EMaximum(const T& a, const T& b) { return a > b ? a : b; }
template <class T> constexpr const T& EClamp(const T& value, const T& lowerlimit, const T& upperlimit) { return (value < lowerlimit) ? lowerlimit : (value > upperlimit) ? upperlimit : value; }

EInline void EAssert(bool _condition, const char *_format, ...)
{
	if (!_condition)
	{
		static const uint32_t up = 0x1;
		static char dest[4096];

		va_list args;
		va_start (args, _format);
		vsprintf(dest, _format, args);
		va_end (args);

#ifdef PLATFORM_WINDOWS
		MessageBoxA(nullptr, dest, "Assertion Failed", MB_OK);
#else
		fprintf(stderr, "Assertion Failed: %s\n", dest);
#endif

		throw(up);
	}
}
