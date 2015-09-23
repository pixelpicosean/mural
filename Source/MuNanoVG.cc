#include "MuNanoVG.h"

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <nanovg_gl_utils.h>

namespace nvg {

  NVGcontext *createGLContext() {
      return nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
  }

  void deleteGLContext(NVGcontext *ctx) {
      nvgDeleteGL3(ctx);
  }

  NVGLUframebuffer *createFramebuffer(NVGcontext *ctx, int width, int height, int imageFlags) {
    return nvgluCreateFramebuffer(ctx, width, height, imageFlags);
  }

  void bindFramebuffer(NVGLUframebuffer *fb) {
    nvgluBindFramebuffer(fb);
  }

  int imageFromTexture(NVGcontext* ctx, GLuint textureId, int w, int h, int flags) {
      return nvglCreateImageFromHandle(ctx, textureId, w, h, flags);
  }

  GLuint textureFromImage(NVGcontext* ctx, int image) {
      return nvglImageHandle(ctx, image);
  }

}
