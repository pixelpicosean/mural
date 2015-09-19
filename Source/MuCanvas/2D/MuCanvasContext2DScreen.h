#ifndef Mural_MuCanvasContext2DScreen_h
#define Mural_MuCanvasContext2DScreen_h

#include "MuCanvasContext2D.h"

namespace mural {

  class MuCanvasContext2DScreen : public MuCanvasContext2D {
    public:
      MuTexture *getTexture();

      MuCanvasContext2DScreen(): MuCanvasContext2D(200, 150) {}

      void present();
      void finish() {}
  };

}

#endif
