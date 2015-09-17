#ifndef Mural_MuGLProgram2DRadialGradient_h
#define Mural_MuGLProgram2DRadialGradient_h

#include "MuGLProgram2D.h"

namespace mural {

  struct MuGLProgram2DRadialGradient : public MuGLProgram2D {
    GLuint inner, diff;

    void getUniforms() {
      MuGLProgram2D::getUniforms();

      inner = glGetUniformLocation(program, "inner");
      diff = glGetUniformLocation(program, "diff");
    }
  };

}

#endif
