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

    glViewport(0, 0, fbWidth, fbHeight);
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

  void AppController::terminate() {
    nvg::deleteGLContext(glContext2D);
  }

  void AppController::setCurrentRenderingContext(MuCanvasContext *renderingContext) {
    if (renderingContext != currentRenderingContext) {
      // currentRenderingContext->flushBuffers();

      // renderingContext->prepare();
      currentRenderingContext = renderingContext;
    }
  }

}
