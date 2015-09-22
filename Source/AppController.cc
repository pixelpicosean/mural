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
  }

  void AppController::init(int width, int height, int devicePixelRatio) {
    this->width = width;
    this->height = height;
    this->devicePixelRatio = devicePixelRatio;

    canvas = new MuCanvas();
    ctx = dynamic_cast<MuCanvasContext2D *>(canvas->getContext(kMuCanvasContextMode2D));

    if (ctx) {
      printf("context exists, size = (%d, %d)\n", ctx->getWidth(), ctx->getHeight());
    }

    // ctx->clearRect(0.0f, 0.0f, ctx->width, ctx->height);

    // MuTexture *tex = new MuTexture("ship.png");
    // printf("texture.size = (%d, %d)\n", tex->width, tex->height);

    // ctx->drawImage(tex, 0, 0, 99, 75, 0, 0, 99, 75);

    theScheduler.scheduleMessage([=] {
      printf("start to draw a rect\n");

      ctx->state->lineWidth = 5.0f;
      ctx->state->strokeColor = { .hex = 0xffffffff };
      ctx->state->fillColor = { .hex = 0x2196f3ff };

      ctx->moveTo(10, 10);
      ctx->lineTo(width - 10, height - 10);
      ctx->lineTo(10, height - 10);
      ctx->stroke();
    }, 400, false);
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
