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

#include "MuFbo.h"
#include "MuCore.h"

#define NANOVG_FBO_VALID 1

namespace mural {

    static GLint defaultFBO = -1;

    NVGLUframebuffer* nvgluCreateFramebuffer(NVGcontext* ctx, int w, int h, int imageFlags)
    {
    #ifdef NANOVG_FBO_VALID
        GLint defaultFBO;
        GLint defaultRBO;
        NVGLUframebuffer* fb = NULL;

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

        fb = (NVGLUframebuffer*)malloc(sizeof(NVGLUframebuffer));
        if (fb == NULL) goto error;
        memset(fb, 0, sizeof(NVGLUframebuffer));

        fb->image = nvgCreateImageRGBA(ctx, w, h, imageFlags | NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED, NULL);
        fb->texture = nvg::textureFromImage(ctx, fb->image);

        // frame buffer object
        glGenFramebuffers(1, &fb->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

        // render buffer object
        glGenRenderbuffers(1, &fb->rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);

        // combine all
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) goto error;

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
        return fb;
    error:
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
        nvgluDeleteFramebuffer(ctx, fb);
        return NULL;
    #else
        NVG_NOTUSED(ctx);
        NVG_NOTUSED(w);
        NVG_NOTUSED(h);
        NVG_NOTUSED(imageFlags);
        return NULL;
    #endif
    }

    void nvgluBindFramebuffer(NVGLUframebuffer* fb)
    {
    #ifdef NANOVG_FBO_VALID
        if (defaultFBO == -1) glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, fb != NULL ? fb->fbo : defaultFBO);
    #else
        NVG_NOTUSED(fb);
    #endif
    }

    void nvgluDeleteFramebuffer(NVGcontext* ctx, NVGLUframebuffer* fb)
    {
    #ifdef NANOVG_FBO_VALID
        if (fb == NULL) return;
        if (fb->fbo != 0)
            glDeleteFramebuffers(1, &fb->fbo);
        if (fb->rbo != 0)
            glDeleteRenderbuffers(1, &fb->rbo);
        if (fb->image >= 0)
            nvgDeleteImage(ctx, fb->image);
        fb->fbo = 0;
        fb->rbo = 0;
        fb->texture = 0;
        fb->image = -1;
        free(fb);
    #else
        NVG_NOTUSED(ctx);
        NVG_NOTUSED(fb);
    #endif
    }

    MuFboRef MuFbo::create(int width, int height, int imageFlags) {
        return MuFboRef(new MuFbo(width, height, imageFlags));
    }

    MuFbo::MuFbo(int width, int height, int imageFlags) {
        init(width, height, imageFlags);
    }

    MuFbo::~MuFbo() {
        if (fbo != 0) glDeleteFramebuffers(1, &fbo);
        if (rbo != 0) glDeleteRenderbuffers(1, &rbo);
        if (image >= 0) nvgDeleteImage(app.getRenderContext(), image);
        fbo = rbo = texture = 0;
        image = -1;
    }

    void MuFbo::init(int width, int height, int imageFlags) {
        NVGcontext *ctx = app.getRenderContext();

        GLint defaultFBO;
        GLint defaultRBO;

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

        image = nvgCreateImageRGBA(ctx, width, height, imageFlags | NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED, NULL);
        texture = nvg::textureFromImage(ctx, image);
        nvgImageSize(ctx, image, &width, &height);

        // frame buffer object
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // render buffer object
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);

        // combine all
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw "Failed to create FBO";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    }

    void MuFbo::bindFrameBuffer() {
        Number pxRatio = app.getRatio();
        int winWidth = (int)(width / pxRatio),
            winHeight = (int)(height / pxRatio);

        NVGcontext *g = app.getRenderContext();

        // Bind FBO
        if (defaultFBO == -1) {
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Prepare for rendering
        glViewport(0, 0, width, height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        nvgBeginFrame(g, winWidth, winHeight, pxRatio);
    }

    void MuFbo::unbindFrameBuffer() {
        nvgEndFrame(app.getRenderContext());

        if (defaultFBO == -1) {
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    }

}
