#include "AppController.h"
#include "MuTimer.h"

#include "MuCanvas/MuCanvas.h"
#include "MuCanvas/2D/MuCanvasContext2D.h"

namespace mural {

  AppController::~AppController() {
    if (canvas) {
      delete canvas;
    }
  }

  void AppController::init(int width, int height, int devicePixelRatio) {
    this->width = width;
    this->height = height;
    this->devicePixelRatio = devicePixelRatio;
    this->bufferWidth = width * devicePixelRatio;
    this->bufferHeight = height * devicePixelRatio;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

    canvas = new MuCanvas();
    ctx = dynamic_cast<MuCanvasContext2D *>(canvas->getContext(kMuCanvasContextMode2D));

    if (ctx) {
      printf("context exists, size = (%d, %d)\n", ctx->getWidth(), ctx->getHeight());
    }

    MuTexture *tex = new MuTexture("ship.png");
    printf("texture.size = (%d, %d)\n", tex->width, tex->height);

    theScheduler.scheduleMessage([=] {
      printf("start to draw a rect\n");

      ctx->state->lineWidth = 1;
      ctx->state->strokeColor = { .hex = 0xffffffff };
      ctx->state->fillColor = { .hex = 0xffffffff };

      ctx->fillRect(0, 0, 20, 20);
      // ctx->drawImage(tex, 0, 0, 99, 75, 0, 0, 99, 75);
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
