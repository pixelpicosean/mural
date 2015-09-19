#ifndef Mural_MuCanvas_h
#define Mural_MuCanvas_h

#include "MuTexture.h"
#include "MuCanvasContext.h"

namespace mural {

  enum MuCanvasContextMode {
    kMuCanvasContextModeInvalid,
    kMuCanvasContextMode2D,
    kMuCanvasContextModeWebGL
  };

  class MuCanvas {
    public:
      MuTexture *texture;
      short width, height;

      MuCanvas();
      ~MuCanvas();

      MuCanvasContext *getContext(MuCanvasContextMode mode);

    private:
      MuCanvasContext *renderingContext;
      MuCanvasContextMode contextMode;

      bool isScreenCanvas;
      bool useRetinaResolution;

      bool msaaEnabled;
      int msaaSamples;
  };

}

#endif
