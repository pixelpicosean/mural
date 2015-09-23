#ifndef Mural_AppController_h
#define Mural_AppController_h

#include "MuNanoVG.h"
#include "MuSharedOpenGLContext.h"
#include "MuCanvas/MuCanvasContext.h"

namespace mural {

  class MuCanvas;
  class MuCanvasContext2D;

  class MuTimerCollection;
  class AppController {
    public:
      short width = 0, height = 0;
      int devicePixelRatio = 1;
      float averageFPS = 0.0f;

      bool hasScreenCanvas = false;
      bool isPaused = false;

      // JSGlobalContextRef jsGlobalContext;
      // MuSharedOpenALManager *openALManager;

      NVGcontext *glContext2D;

      MuCanvasContext *currentRenderingContext = nullptr;
      MuCanvasContext *screenRenderingContext = nullptr;

    public:
      void init(int width, int height, int devicePixelRatio);
      void update();
      void terminate();

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

      // Tests only
      MuCanvas *canvas = nullptr;
      MuCanvasContext2D *ctx = nullptr;
  };

}

#define app mural::AppController::instance()

#endif
