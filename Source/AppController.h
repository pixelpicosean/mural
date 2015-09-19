#ifndef Mural_AppController_h
#define Mural_AppController_h

#include "MuSharedOpenGLContext.h"
#include "MuCanvas/MuCanvasContext.h"

namespace mural {

  class MuTimerCollection;
  class AppController {
    public:
      short width, height;
      int devicePixelRatio;
      float averageFPS;

      bool hasScreenCanvas;
      bool isPaused;

      // JSGlobalContextRef jsGlobalContext;

      MuSharedOpenGLContext *openGLContext;
      // MuSharedTextureCache *textureCache;
      // MuSharedOpenALManager *openALManager;

      MuCanvasContext *currentRenderingContext;
      MuCanvasContext *screenRenderingContext;

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

      void setCurrentRenderingContext(MuCanvasContext *renderingContext);
  };

}

#define app mural::AppController::instance()

#endif
