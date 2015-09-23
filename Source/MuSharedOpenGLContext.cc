#include "MuSharedOpenGLContext.h"

namespace mural {

  MuGLProgram2D *MuSharedOpenGLContext::getGLProgramScreen() {
    if (!glProgramScreen) {
      glProgramScreen = new MuGLProgram2D(MuShaderScreenVertex, MuShaderScreenFragment);
    }
    return glProgramScreen;
  }

  MuGLProgram2D *MuSharedOpenGLContext::getGLProgram2DFlat() {
    if (!glProgram2DFlat) {
      glProgram2DFlat = new MuGLProgram2D(MuShaderVertex, MuShaderFlat);
    }
    return glProgram2DFlat;
  }

  MuGLProgram2D *MuSharedOpenGLContext::getGLProgram2DTexture() {
    if (!glProgram2DTexture) {
      glProgram2DTexture = new MuGLProgram2D(MuShaderVertex, MuShaderTexture);
    }
    return glProgram2DTexture;
  }

  MuGLProgram2D *MuSharedOpenGLContext::getGLProgram2DAlphaTexture() {
    if (!glProgram2DAlphaTexture) {
      glProgram2DAlphaTexture = new MuGLProgram2D(MuShaderVertex, MuShaderAlphaTexture);
    }
    return glProgram2DAlphaTexture;
  }

  MuGLProgram2D *MuSharedOpenGLContext::getGLProgram2DPattern() {
    if (!glProgram2DPattern) {
      glProgram2DPattern = new MuGLProgram2D(MuShaderVertex, MuShaderPattern);
    }
    return glProgram2DPattern;
  }

  MuGLProgram2DRadialGradient *MuSharedOpenGLContext::getGLProgram2DRadialGradient() {
    if (!glProgram2DRadialGradient) {
      glProgram2DRadialGradient = new MuGLProgram2DRadialGradient(MuShaderVertex, MuShaderRadialGradient);
    }
    return glProgram2DRadialGradient;
  }

  unsigned char *MuSharedOpenGLContext::getVertexBuffer() {
    if (!vertexBuffer) {
      vertexBuffer = new unsigned char[MU_OPENGL_VERTEX_BUFFER_SIZE];
      vertexBufferLength = (int)MU_OPENGL_VERTEX_BUFFER_SIZE;
    }
    return vertexBuffer;
  }

}
