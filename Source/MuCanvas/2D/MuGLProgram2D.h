#ifndef Mural_MuGLProgram_h
#define Mural_MuGLProgram_h

#include "MuOpenGL.h"

#include "MuCanvas2DTypes.h"

namespace mural {

  enum {
    kMuGLProgram2DAttributePos,
    kMuGLProgram2DAttributeUV,
    kMuGLProgram2DAttributeColor,
  };

  struct MuGLProgram2D {
    GLuint program = 0;
    GLuint screen = 0;

    MuGLProgram2D(const char *vertexShaderSource, const char *fragmentShaderSource);
    ~MuGLProgram2D();
    void bindAttributeLocations();
    void getUniforms();

    static GLint compileShaderSource(const char *source, GLenum type);
    static void linkProgram(GLuint program);
  };

}

#endif
