#include "MuGLProgram2D.h"

#include <cstdio>

MuGLProgram2D::MuGLProgram2D(const char *vertexShaderSource, const char *fragmentShaderSource) {
  program = glCreateProgram();
  GLuint vertexShader = MuGLProgram2D::compileShaderSource(vertexShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShader = MuGLProgram2D::compileShaderSource(fragmentShaderSource, GL_FRAGMENT_SHADER);

  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);

  bindAttributeLocations();

  MuGLProgram2D::linkProgram(program);

  getUniforms();

  glDetachShader(program, vertexShader);
  glDeleteShader(vertexShader);

  glDetachShader(program, fragmentShader);
  glDeleteShader(fragmentShader);
}

MuGLProgram2D::~MuGLProgram2D() {
  if (program) {
    glDeleteProgram(program);
  }
}

void MuGLProgram2D::bindAttributeLocations() {
  glBindAttribLocation(program, kMuGLProgram2DAttributePos, "pos");
  glBindAttribLocation(program, kMuGLProgram2DAttributeUV, "uv");
  glBindAttribLocation(program, kMuGLProgram2DAttributeColor, "color");
}

void MuGLProgram2D::getUniforms() {
  screen = glGetUniformLocation(program, "screen");
}

GLint MuGLProgram2D::compileShaderSource(const char *source, GLenum type) {
  GLint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      GLchar *log = (GLchar *)malloc(logLength);
      glGetShaderInfoLog(shader, logLength, &logLength, log);
      printf("Shader compile log:\n%s", log);
      free(log);
    }
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

void MuGLProgram2D::linkProgram(GLuint program) {
  GLint status;
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == 0) {
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      GLchar *log = (GLchar *)malloc(logLength);
      glGetProgramInfoLog(program, logLength, &logLength, log);
      printf("Program link log:\n%s", log);
      free(log);
    }
  }
}
