#ifndef Mural_MuCanvasContext2DTexture_h
#define Mural_MuCanvasContext2DTexture_h

#include "MuCanvasContext2D.h"

namespace mural {

  class MuCanvasContext2DTexture : public MuCanvasContext2D {
    public:
      MuCanvasContext2DTexture(int width, int height):
        MuCanvasContext2D(width, height)
      {}

      void create();
  };

}

#endif
