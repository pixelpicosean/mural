#include "AppController.h"
#include "MuTimer.h"

#include "MuCanvas/MuCanvas.h"
#include "MuCanvas/2D/MuCanvasContext2D.h"

#include "MuOpenGL.h"

namespace mural {

  AppController::~AppController() {
    if (canvas) {
      delete canvas;
    }
  }

  void AppController::init(int width, int height, int devicePixelRatio, NVGcontext *vg) {
    this->width = width;
    this->height = height;
    this->devicePixelRatio = devicePixelRatio;

    this->glContext2D = vg;

    // canvas = new MuCanvas();
    // ctx = dynamic_cast<MuCanvasContext2D *>(canvas->getContext(kMuCanvasContextMode2D));

    // if (ctx) {
    //   printf("context exists, size = (%d, %d)\n", ctx->getWidth(), ctx->getHeight());
    // }

    theScheduler.scheduleMessage([=] {
      printf("start to draw a rect\n");

      // glViewport(0, 0, width * devicePixelRatio, height * devicePixelRatio);
      // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      // nvgBeginFrame(glContext2D, width, height, devicePixelRatio);

      //   nvgBeginPath(glContext2D);
      //   nvgRect(glContext2D, 0, 0, 100, 100);
      //   nvgFillColor(glContext2D, nvgRGBA(255, 255, 255, 255));
      //   nvgFill(glContext2D);

      // nvgEndFrame(glContext2D);
    }, 400, false);
  }

  void AppController::update() {
    if (isPaused) return;

    glViewport(0, 0, width * devicePixelRatio, height * devicePixelRatio);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(glContext2D, width, height, devicePixelRatio);

      nvgBeginPath(glContext2D);
      nvgRect(glContext2D, 0, 0, 100, 100);
      nvgFillColor(glContext2D, nvgRGBA(255, 255, 255, 255));
      nvgFill(glContext2D);

    nvgEndFrame(glContext2D);

    // Update timers
    theScheduler.update();
  }

  void AppController::setCurrentRenderingContext(MuCanvasContext *renderingContext) {
    if (renderingContext != currentRenderingContext) {
      // currentRenderingContext->flushBuffers();

      // renderingContext->prepare();
      currentRenderingContext = renderingContext;
    }
  }

}
