#include "AppController.h"
#include "MuTimer.h"

#include "MuCanvas/MuCanvas.h"
#include "MuCanvas/2D/MuCanvasContext2D.h"

#include "MuOpenGL.h"

namespace mural {

  AppController::~AppController() {}

  void AppController::init(int width, int height, int devicePixelRatio) {
    this->width = width;
    this->height = height;
    this->devicePixelRatio = devicePixelRatio;
    this->fbWidth = width * devicePixelRatio;
    this->fbHeight = height * devicePixelRatio;

    this->glContext2D = nvg::createGLContext();

    canvas = new MuCanvas();
    ctx = dynamic_cast<MuCanvasContext2D *>(canvas->getContext(kMuCanvasContextMode2D));

    if (ctx) {
      printf("context exists, size = (%d, %d)\n", ctx->getWidth(), ctx->getHeight());
    }

    theScheduler.scheduleMessage([=] {
      printf("start to draw a rect\n");

      ctx->beginPath();
      ctx->setFillStyle(nvgRGB(0, 187, 211));
      ctx->rect(0, 0, 100, 100);
      ctx->fill();

    }, 1000, false);

    theScheduler.scheduleMessage([=] {
      printf("start to draw another rect\n");

      ctx->beginPath();
      ctx->setFillStyle(nvgRGB(204, 219, 56));
      ctx->rect(50, 50, 100, 100);
      ctx->fill();

    }, 2000, false);
  }

  void AppController::update() {
    if (isPaused) return;

    // Update timers
    theScheduler.update();

    // Redraw the canvas
    if (hasScreenCanvas) {
      setCurrentRenderingContext(screenRenderingContext);
      screenRenderingContext->present();
    }
  }

  void AppController::terminate() {
    nvg::deleteGLContext(glContext2D);
  }

  void AppController::setCurrentRenderingContext(MuCanvasContext *renderingContext) {
    if (renderingContext != currentRenderingContext) {
      currentRenderingContext->flushBuffers();
      currentRenderingContext = renderingContext;
    }
  }

}
