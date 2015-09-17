#ifndef Mural_MuSharedOpenGLContext_h
#define Mural_MuSharedOpenGLContext_h

#include "MuCanvas/2D/MuGLProgram2D.h"
#include "MuCanvas/2D/MuGLProgram2DRadialGradient.h"

#include "MuCanvas/2D/MuCanvasShaders.h"

#define MU_OPENGL_VERTEX_BUFFER_SIZE (32 * 1024) // 32kb

namespace mural {

  class MuSharedOpenGLContext {
    public:
      MuGLProgram2D *getGLProgram2DFlat();
      MuGLProgram2D *getGLProgram2DTexture();
      MuGLProgram2D *getGLProgram2DAlphaTexture();
      MuGLProgram2D *getGLProgram2DPattern();
      MuGLProgram2D *getGLProgram2DRadialGradient();

      unsigned char *getVertexBuffer();

      static MuSharedOpenGLContext& instance() {
        static MuSharedOpenGLContext instance;
        return instance;
      }

    private:
      MuSharedOpenGLContext() {}
      MuSharedOpenGLContext(MuSharedOpenGLContext const&) {}
      void operator=(MuSharedOpenGLContext const&) {}

      MuGLProgram2D *glProgram2DFlat;
      MuGLProgram2D *glProgram2DTexture;
      MuGLProgram2D *glProgram2DAlphaTexture;
      MuGLProgram2D *glProgram2DPattern;
      MuGLProgram2DRadialGradient *glProgram2DRadialGradient;

      unsigned char *vertexBuffer;
  };

}

#endif
