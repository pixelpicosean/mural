#ifndef Mural_MuCanvas2DTypes_h
#define Mural_MuCanvas2DTypes_h

#include "MuOpenGL.h"

namespace mural {

  typedef union {
    struct {
      unsigned char r, g, b, a;
    } rgba;
    unsigned char components[4];
    unsigned int hex;
  } MuColorRGBA;

  struct MuVertex {
    glm::vec2 pos;
    glm::vec2 uv;
    MuColorRGBA color;
  };

  struct MuRect {
    glm::vec2 origin;
    glm::vec2 size;

    MuRect(float x, float y, float w, float h) {
      origin.x = x;
      origin.y = y;
      size.x = w;
      size.y = h;
    }
  };

}

#endif
