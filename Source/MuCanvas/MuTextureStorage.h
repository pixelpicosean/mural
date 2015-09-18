#ifndef Mural_MuTextureStorage_h
#define Mural_MuTextureStorage_h

#include <GLFW/glfw3.h>

namespace mural {

  typedef enum {
    kMuTextureParamMinFilter,
    kMuTextureParamMagFilter,
    kMuTextureParamWrapS,
    kMuTextureParamWrapT,
    kMuTextureParamLast
  } MuTextureParam;

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
