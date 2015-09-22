#ifndef Mural_MuGLProgram2DRadialGradient_h
#define Mural_MuGLProgram2DRadialGradient_h

#include "MuGLProgram2D.h"

namespace mural {

  struct MuGLProgram2DRadialGradient : public MuGLProgram2D {
    GLuint inner = 0, diff = 0;

    MuGLProgram2DRadialGradient(const char *vertexShaderSource, const char *fragmentShaderSource):
      MuGLProgram2D(vertexShaderSource, fragmentShaderSource)
    {}

    void getUniforms() {
      MuGLProgram2D::getUniforms();

      inner = glGetUniformLocation(program, "inner");
      diff = glGetUniformLocation(program, "diff");
    }
  };

}

#endif
