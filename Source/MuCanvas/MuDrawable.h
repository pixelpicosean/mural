#ifndef Mural_MuDrawable_h
#define Mural_MuDrawable_h

#include "MuOpenGL.h"
#include "MuNanoVG.h"

namespace mural {

  class MuDrawable {
    public:
      virtual NVGpaint getTexture() { return NVGpaint(); }

      MuDrawable() {};
      virtual ~MuDrawable() {}
  };

}

#endif
