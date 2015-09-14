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

#include "MuVector.h"
#include "MuMatrixAffine2.h"
#include "MuRect.h"

// ONLY include GLEW for non-Apple platforms
#ifndef __APPLE__
#define NANOVG_GLEW
#endif

#ifdef NANOVG_GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#ifdef __APPLE__
#   define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>
#include "nanovg/nanovg.h"

namespace nvg {

    /**
     * Create rendering context for NavoVG
     */
    NVGcontext *createGLContext();
    /**
     * Clean up rendering context
     */
    void deleteGLContext(NVGcontext *ctx);

    /**
     * Create an image object from an OpenGL texture
     */
    int imageFromTexture(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
    /**
     * Get the texture id of an image
     */
    GLuint textureFromImage(NVGcontext* ctx, int image);

}
