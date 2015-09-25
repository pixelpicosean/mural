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

    // Create a screen canvas
    canvas = new MuCanvas();
    ctx = dynamic_cast<MuCanvasContext2D *>(canvas->getContext(kMuCanvasContextMode2D));
    if (!ctx) {
      printf("Failed to get context of an screen canvas!\n");
    }
    // Draw something to screen canvas
    ctx->beginPath();
    ctx->setFillStyle(nvgRGB(0, 187, 211));
    ctx->rect(0, 0, 100, 100);
    ctx->fill();

    // Create a off-screen canvas
    auto canvas2 = new MuCanvas();
    auto ctx2 = dynamic_cast<MuCanvasContext2D *>(canvas2->getContext(kMuCanvasContextMode2D));
    if (!ctx2) {
      printf("Failed to get context of an off-screen canvas!\n");
    }

    // Draw part of the off-screen canvas to screen canvas after a period
    theScheduler.scheduleMessage([=] {
      printf("Draw something to the off-screen canvas\n");

      // ctx2->beginPath();
      // ctx2->setFillStyle(nvgRGB(252, 87, 46));
      // ctx2->rect(0, 0, 50, 50);
      // ctx2->fill();

      // ctx->drawImage(tex, 0, 0, imgWidth, imgHeight, 100, 100, imgWidth, imgHeight);

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
