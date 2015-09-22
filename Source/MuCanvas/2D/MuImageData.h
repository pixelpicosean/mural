#ifndef Mural_MuImageData_h
#define Mural_MuImageData_h

#include "MuCanvas/MuTexture.h"

namespace mural {

  struct MuImageData {
    int width = 0, height = 0;
    unsigned char *pixels = nullptr;

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
