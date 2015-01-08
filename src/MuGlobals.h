/*
Copyright (c) 2015 Sean Bohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#define MURAL_VERSION_STRING "0.2.0a"

/**
 * Shims
 */
#ifdef _WINDOWS
    // Prevent windows.h includes from generating min/max macros that
    // clash with the templates in <algorithm>
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
#endif

#include <stdint.h>

// Is this necessary while using C++11?
#ifndef NULL
    #define NULL 0
#endif

/**
 * Platform definition
 */
#define PLATFORM_MAC      1
#define PLATFORM_WINDOWS  2
#define PLATFORM_UNIX     3 // Linux, *BSD, RaspberryPi
#define PLATFORM_IOS      4
#define PLATFORM_ANDROID  5

// TODO: iOS and Android
#ifdef _WINDOWS
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM PLATFORM_MAC
#else
    #include <cstddef>
    #define PLATFORM PLATFORM_UNIX
#endif

/**
 * Number definition
 */
typedef double Number;

#define RANDOM_NUMBER ((Number)rand()/(Number)RAND_MAX)

inline Number clampf(Number x, Number a, Number b) {
    return x < a ? a : (x > b ? b : x);
}

#ifndef MIN
    #define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
    #define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

/**
 * String definition
 */
#include <string>
typedef std::string String;

/**
 * Base class for anything
 */
class MuObject {};
