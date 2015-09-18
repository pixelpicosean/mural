#ifndef Mural_MuTextureStorage_h
#define Mural_MuTextureStorage_h

#include "MuOpenGL.h"

namespace mural {

  enum MuTextureParam {
    kMuTextureParamMinFilter,
    kMuTextureParamMagFilter,
    kMuTextureParamWrapS,
    kMuTextureParamWrapT,
    kMuTextureParamLast
  };

  typedef MuTextureParam MuTextureParams[kMuTextureParamLast];

  class MuTextureStorage {
    public:
      GLuint textureId;
      bool immutable;
      double lastBound;

      MuTextureStorage(bool immutable = false);
      ~MuTextureStorage();

      void bindToTarget(GLenum target, MuTextureParam *newParams);

    private:
      MuTextureParams params;
  };

}

#endif
