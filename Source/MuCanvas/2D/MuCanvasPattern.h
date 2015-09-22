#ifndef Mural_MuCanvasPattern_h
#define Mural_MuCanvasPattern_h

#include "MuCanvas/MuTexture.h"
#include "MuCanvasContext2D.h"

namespace mural {

  enum MuCanvasPatternRepeat {
    kMuCanvasPatternNoRepeat  = 0,
    kMuCanvasPatternRepeatX   = 1,
    kMuCanvasPatternRepeatY   = 2,
    kMuCanvasPatternRepeat    = 1 | 2
  };

  class MuCanvasPattern : public MuFillable {
    public:
      MuTexture *texture = nullptr;
      MuCanvasPatternRepeat repeat;

      MuCanvasPattern(MuTexture *texture, MuCanvasPatternRepeat repeat):
        texture(texture),
        repeat(repeat)
      {}

      ~MuCanvasPattern() {}
  };

}

#endif
