#ifndef Mural_MuCanvasContext2DScreen_h
#define Mural_MuCanvasContext2DScreen_h

#include "MuCanvasContext2D.h"

namespace mural {

  class MuCanvasContext2DScreen : public MuCanvasContext2D {
    public:
      MuTexture *getTexture();
      void setStyle(MuRect newStyle);
      MuRect getFrame();

      MuCanvasContext2DScreen(short width, short height):
        MuCanvasContext2D(width, height),
        style(0.0f, 0.0f, width, height)
      {}

      void resizeTo(short newWidth, short newHeight);

      void present();
      void finish();

    private:
      MuRect style;
  };

}

#endif
