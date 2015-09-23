#ifndef Mural_MuNanoVG_h
#define Mural_MuNanoVG_h

#include "MuOpenGL.h"

#define NANOVG_GLEW
#include <nanovg.h>

namespace nvg {

  struct Framebuffer {
    bool valid = false;

    NVGcontext* ctx = nullptr;
    GLuint fbo = 0;
    GLuint rbo = 0;
    GLuint texture = 0;
    int image = 0;

    Framebuffer(NVGcontext *ctx, int width, int height, int imageFlags = 0);
    ~Framebuffer();

    void bind();
    void unbind();
  };

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
