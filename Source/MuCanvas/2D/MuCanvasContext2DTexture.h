#ifndef Mural_MuCanvasContext2DTexture_h
#define Mural_MuCanvasContext2DTexture_h

#include "MuCanvasContext2D.h"

namespace mural {

  class MuCanvasContext2DTexture : public MuCanvasContext2D {
    public:
      MuTexture *getTexture();

      MuCanvasContext2DTexture(): MuCanvasContext2D(200, 150) {}

      void resizeTo(short newWidth, short newHeight);
    private:
      MuTexture *texture;
  };

}

#endif
