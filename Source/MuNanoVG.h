#ifndef Mural_MuNanoVG_h
#define Mural_MuNanoVG_h

#include "MuOpenGL.h"

#define NANOVG_GLEW
#include <nanovg.h>

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

#endif
