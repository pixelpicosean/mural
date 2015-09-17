#ifndef Mural_AppController_h
#define Mural_AppController_h

#include "MuSharedOpenGLContext.h"

namespace mural {

  class MuTimerCollection;
  class AppController {
    public:
      float averageFPS;

      bool hasScreenCanvas;
      bool isPaused;

      // JSGlobalContextRef jsGlobalContext;

      MuSharedOpenGLContext *openGLContext;
      // MuSharedTextureCache *textureCache;
      // MuSharedOpenALManager *openALManager;

      // MuCanvasContext *currentRenderingContext;
      // MuCanvasContext<MuPresentable> *screenRenderingContext;

    public:
      void init(int width = 640, int height = 400);

      void update();
      void draw();

      static AppController& instance() {
        static AppController instance;
        return instance;
      }

    private:
      AppController():
        averageFPS(0.0f),
        hasScreenCanvas(false),
        isPaused(false)
      {}
      AppController(AppController const&) {}
      void operator=(AppController const&) {}

      ~AppController();
  };

}

#define app mural::AppController::instance()

#endif
