#ifndef Mural_AppController_h
#define Mural_AppController_h

#include "MuSharedOpenGLContext.h"
#include "MuCanvas/MuCanvasContext.h"

namespace mural {

  class MuCanvas;
  class MuCanvasContext2D;

  class MuTimerCollection;
  class AppController {
    public:
      short width = 0, height = 0;
      short bufferWidth = 0, bufferHeight = 0;
      int devicePixelRatio = 1;

      float averageFPS = 0.0f;

      GLint defaultFBO = -1;
      GLint defaultRBO = -1;

      bool hasScreenCanvas = false;
      bool isPaused = false;

      // JSGlobalContextRef jsGlobalContext;

      // MuSharedTextureCache *textureCache;
      // MuSharedOpenALManager *openALManager;

      MuCanvasContext *getCurrentRenderingContext() { return currentRenderingContext; }
      void setCurrentRenderingContext(MuCanvasContext *renderingContext);

      MuCanvasContext *getScreenRenderingContext() { return screenRenderingContext; }
      void setScreenRenderingContext(MuCanvasContext *renderingContext) { screenRenderingContext = renderingContext; }

    public:
      void init(int width = 640, int height = 400, int devicePixelRatio = 1);

      void update();
      void draw();

      static AppController& instance() {
        static AppController instance;
        return instance;
      }

    private:
      AppController():
        width(200),
        height(100),
        averageFPS(0.0f),
        hasScreenCanvas(false),
        isPaused(false)
      {}
      AppController(AppController const&) {}
      void operator=(AppController const&) {}

      ~AppController();

      MuCanvasContext *currentRenderingContext = nullptr;
      MuCanvasContext *screenRenderingContext = nullptr;

      // Tests only
      MuCanvas *canvas = nullptr;
      MuCanvasContext2D *ctx = nullptr;
  };

}

#define app mural::AppController::instance()

#endif
