#include "MuNanoVG.h"

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

namespace nvg {

  static GLint defaultFBO = -1;

  Framebuffer::Framebuffer(NVGcontext *ctxp, int width, int height, int imageFlags) {
    GLint defaultFBO;
    GLint defaultRBO;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

    image = nvgCreateImageRGBA(ctx, width, height, imageFlags | NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED, nullptr);
    texture = nvglImageHandle(ctx, image);
    ctx = ctxp;

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
      glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
      glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);

    valid = true;
  }

  Framebuffer::~Framebuffer() {
    if (fbo != 0) {
      glDeleteFramebuffers(1, &fbo);
    }
    if (rbo != 0) {
      glDeleteRenderbuffers(1, &rbo);
    }
    if (image >= 0) {
      nvgDeleteImage(ctx, image);
    }
  }

  void Framebuffer::bind() {
    if (!valid) return;

    if (defaultFBO == -1) glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  }

  void Framebuffer::unbind() {
    if (!valid) return;

    if (defaultFBO == -1) glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
  }

  NVGcontext *createGLContext() {
      return nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
  }

  void deleteGLContext(NVGcontext *ctx) {
      nvgDeleteGL3(ctx);
  }

  int imageFromTexture(NVGcontext* ctx, GLuint textureId, int w, int h, int flags) {
      return nvglCreateImageFromHandle(ctx, textureId, w, h, flags);
  }

  GLuint textureFromImage(NVGcontext* ctx, int image) {
      return nvglImageHandle(ctx, image);
  }

}
