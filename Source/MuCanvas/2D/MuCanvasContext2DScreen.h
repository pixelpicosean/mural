#ifndef Mural_MuCanvasContext2DScreen_h
#define Mural_MuCanvasContext2DScreen_h

#include "MuCanvasContext2D.h"

namespace mural {

  class MuCanvasContext2DScreen : public MuCanvasContext2D {
    public:
      MuTexture *getTexture();

      void present();
      void finish();
  };

}

#endif
