#ifndef Mural_MuImageData_h
#define Mural_MuImageData_h

#include "MuCanvas/MuTexture.h"

namespace mural {

  struct MuImageData {
    int width, height;
    unsigned char *pixels;

    MuTexture *getTexture() {
      return new MuTexture(width, height, pixels);
    }

    MuImageData(int width, int height, unsigned char *pixels):
      width(width),
      height(height),
      pixels(pixels)
    {}
  };

}

#endif
