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

#define MURAL_VERSION        020
#define MURAL_VERSION_STRING "0.2.0a"

/**
 * Dependencies
 */
#include <boost/version.hpp>
#if BOOST_VERSION < 105500
    #error "Cinder requires Boost version 1.55 or later"
#endif

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
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #if defined(WINAPI_PARTITION_DESKTOP)
        #define PLATFORM PLATFORM_WINDOWS
    #endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define PLATFORM PLATFORM_MAC
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #include <cstddef>
    #define PLATFORM PLATFORM_UNIX
#else
    #error "Mural compile error: Unknown platform"
#endif

/**
 * Number definition
 */
#include <cstdint>
typedef double Number;
namespace mural {
    using std::int8_t;
    using std::uint8_t;
    using std::int16_t;
    using std::uint16_t;
    using std::int32_t;
    using std::uint32_t;
    using std::int64_t;
    using std::uint64_t;
}

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

#include <string>

/**
 * Base class for anything
 */
class MuObject {};

// Create a namepace alias as shorthand for mural::
namespace mu = mural;
