#ifndef Mural_MuDrawable_h
#define Mural_MuDrawable_h

#include "MuOpenGL.h"
#include "MuNanoVG.h"

namespace mural {

  class MuDrawable {
    public:
      NVGpaint getTexture() { return texture; }

      MuDrawable() {};
      virtual ~MuDrawable() {}

    protected:
      NVGpaint texture;
  };

}

#endif
