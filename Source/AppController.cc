#include "AppController.h"
#include "MuTimer.h"

#include "MuCanvas/MuCanvas.h"
#include "MuCanvas/2D/MuCanvasContext2D.h"

namespace mural {

  AppController::~AppController() {
    if (screenRenderingContext) {
      screenRenderingContext->finish();
    }

    if (canvas) {
      delete canvas;
    }
    if (ctx) {
      delete ctx;
    }
  }

  void AppController::init(int width, int height, int devicePixelRatio) {
    this->width = width;
    this->height = height;
    this->devicePixelRatio = devicePixelRatio;

    canvas = new MuCanvas();
    ctx = dynamic_cast<MuCanvasContext2D *>(canvas->getContext(kMuCanvasContextMode2D));

    if (ctx) {
      printf("canvas.context exists\n");
    }

    theScheduler.scheduleMessage([&] {
      printf("start to draw a rect\n");
      ctx->state->lineWidth = 5.0f;
      ctx->state->strokeColor = { .hex = 0xffffffff };

      ctx->moveTo(10, 10);
      ctx->lineTo(80, 10);
      ctx->lineTo(80, 80);
      ctx->lineTo(10, 80);
      ctx->stroke();
    }, 1000, false);
  }

  void AppController::update() {
    if (isPaused) return;

    // Update timers
    theScheduler.update();
  }

  void AppController::draw() {
    if (isPaused) return;

    // Redraw the canvas
    if (hasScreenCanvas) {
      setCurrentRenderingContext(screenRenderingContext);
      screenRenderingContext->present();
    }
  }

  void AppController::setCurrentRenderingContext(MuCanvasContext *renderingContext) {
    if (renderingContext != currentRenderingContext) {
      currentRenderingContext->flushBuffers();

      renderingContext->prepare();
      currentRenderingContext = renderingContext;
    }
  }

}
