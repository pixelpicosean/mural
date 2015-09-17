#ifndef Mural_MuGLProgram_h
#define Mural_MuGLProgram_h

#include <GLFW/glfw3.h>

#include "MuCanvas2DTypes.h"

enum {
  kMuGLProgram2DAttributePos,
  kMuGLProgram2DAttributeUV,
  kMuGLProgram2DAttributeColor,
};

struct MuGLProgram2D {
  GLuint program;
  GLuint screen;

  MuGLProgram2D(const char *vertexShaderSource, const char *fragmentShaderSource);
  ~MuGLProgram2D();
  void bindAttributeLocations();
  void getUniforms();

  static GLint compileShaderSource(const char *source, GLenum type);
  static void linkProgram(GLuint program);
};

#endif
