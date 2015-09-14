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

#include "MuGlobals.h"
#include "MuArea.h"
#include "MuVector.h"

#include "MuNanoVG.h"

namespace mural {

    struct NVGLUframebuffer {
        NVGcontext* ctx;
        GLuint fbo;
        GLuint rbo;
        GLuint texture;
        int image;
    };
    typedef struct NVGLUframebuffer NVGLUframebuffer;

    // Helper function to create GL frame buffer to render to.
    void nvgluBindFramebuffer(NVGLUframebuffer* fb);
    NVGLUframebuffer* nvgluCreateFramebuffer(NVGcontext* ctx, int w, int h, int imageFlags);
    void nvgluDeleteFramebuffer(NVGcontext* ctx, NVGLUframebuffer* fb);

    typedef std::shared_ptr<class MuFbo> MuFboRef;
    class MuFbo : public std::enable_shared_from_this<MuFbo>, public MuObject {
        public:

            static MuFboRef create(int width, int height, int imageFlags);
            ~MuFbo();

            int getWidth() const { return width; }
            int getHeight() const { return height; }
            ivec2 getSize() const { return ivec2(width, height); }
            Area getBounds() const { return Area(0, 0, width, height); }

            int getImage() const { return image; }
            GLuint getTexture() const { return texture; }

            void bindFrameBuffer();
            void unbindFrameBuffer();

        protected:
            MuFbo(int width, int height, int imageFlags);
            void init(int width, int height, int imageFlags);

            int width, height;

            GLuint fbo, rbo, texture;
            int image;
    };

}
