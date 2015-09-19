#ifndef Mural_MuCanvasGradient_h
#define Mural_MuCanvasGradient_h

#include "MuOpenGL.h"

#include "MuCanvas2DTypes.h"
#include "MuCanvasContext2D.h"
#include "MuCanvas/MuTexture.h"

#define MU_CANVAS_GRADIENT_WIDTH 1024

namespace mural {

  enum MuCanvasGradientType {
    kMuCanvasGradientTypeLinear,
    kMuCanvasGradientTypeRadial
  };

  struct MuCanvasGradientColorStop {
    float pos;
    unsigned int order;
    MuColorRGBA color;
  };

  class MuCanvasGradient : public MuFillable {
    public:
      MuCanvasGradientType type;
      MuTexture *texture;
      glm::vec2 p1, p2;
      float r1, r2;

      /* Linear */
      MuCanvasGradient(glm::vec2 p1, glm::vec2 p2) {}
      /* Radial */
      MuCanvasGradient(glm::vec2 p1, float r1, glm::vec2 p2, float r2) {}

      void addStop(MuColorRGBA color, float pos) {}
      void rebuild() {}
      unsigned char *getPixelsWithWidth(int width, float *stops) { return nullptr; }

    private:
      std::vector<MuCanvasGradientColorStop *> colorStops;
  };

}

#endif
