#include "MuTextureStorage.h"

namespace mural {

  MuTextureStorage::MuTextureStorage(bool immutable): immutable(immutable) {
    glGenTextures(1, &textureId);
  }

  MuTextureStorage::~MuTextureStorage() {
    if (textureId) {
      glDeleteTextures(1, &textureId);
    }
  }

  void MuTextureStorage::bindToTarget(GLenum target, MuTextureParam *newParams) {
    glBindTexture(target, textureId);

    // Check if we have to set a param
    if (params[kMuTextureParamMinFilter] != newParams[kMuTextureParamMinFilter]) {
      params[kMuTextureParamMinFilter] = newParams[kMuTextureParamMinFilter];
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, params[kMuTextureParamMinFilter]);
    }
    if (params[kMuTextureParamMagFilter] != newParams[kMuTextureParamMagFilter]) {
      params[kMuTextureParamMagFilter] = newParams[kMuTextureParamMagFilter];
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, params[kMuTextureParamMagFilter]);
    }
    if (params[kMuTextureParamWrapS] != newParams[kMuTextureParamWrapS]) {
      params[kMuTextureParamWrapS] = newParams[kMuTextureParamWrapS];
      glTexParameteri(target, GL_TEXTURE_WRAP_S, params[kMuTextureParamWrapS]);
    }
    if (params[kMuTextureParamWrapT] != newParams[kMuTextureParamWrapT]) {
      params[kMuTextureParamWrapT] = newParams[kMuTextureParamWrapT];
      glTexParameteri(target, GL_TEXTURE_WRAP_T, params[kMuTextureParamWrapT]);
    }

    lastBound = glfwGetTime();
  }

}
