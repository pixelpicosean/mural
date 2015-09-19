#ifndef Mural_MuCanvas_h
#define Mural_MuCanvas_h

#include "MuTexture.h"
#include "MuCanvasContext.h"

namespace mural {

  typedef enum {
    kMuCanvasContextModeInvalid,
    kMuCanvasContextMode2D,
    kMuCanvasContextModeWebGL
  } MuCanvasContextMode;

  class MuCanvas {
    public:
      MuTexture *texture;

      MuCanvas();
      ~MuCanvas();

      MuCanvasContext *getContext(MuCanvasContextMode mode);

    private:
      short width, height;

      MuCanvasContext *renderingContext;
      MuCanvasContextMode contextMode;

      bool isScreenCanvas;
      bool useRetinaResolution;

      bool msaaEnabled;
      int msaaSamples;
  };

}

#endif
