#include "MuOpenGL.h"

#include "MuCanvasContext2DScreen.h"
#include "AppController.h"

namespace mural {

  MuCanvasContext2DScreen::MuCanvasContext2DScreen(int width, int height):
    MuCanvasContext2D(width, height),
    style(0.0f, 0.0f, width, height)
  {}

  MuCanvasContext2DScreen::~MuCanvasContext2DScreen() {}

  void MuCanvasContext2DScreen::setStyle(MuRect newStyle) {
    if ((style.size.x ? style.size.x : width) != newStyle.size.x ||
      (style.size.y ? style.size.y : height) != newStyle.size.y) {
      // Must resize
      style = newStyle;

      // Only resize if we already have a viewFrameBuffer. Otherwise the style
      // will be honored in the 'create' call.
      if (framebuffer) {
        resizeTo(width, height);
      }
    }
    else {
      // Just reposition
      style = newStyle;
    }
  }

  void MuCanvasContext2DScreen::create() {
    MuCanvasContext2D::create();

    printf(
      "Creating ScreenCanvas (2D):\n"
        "  size:    %dx%d\n"
        "  style:   %.0fx%.0f\n"
        "  retina:  %s (%dx%d)\n\n",
      width, height,
      style.size.x, style.size.y,
      (useRetinaResolution ? "true" : "false"),
      bufferWidth, bufferHeight
    );
  }

  MuRect MuCanvasContext2DScreen::getFrame() {
    return MuRect(
      style.origin.x, style.origin.y,
      (style.size.x ? style.size.x : width),
      (style.size.y ? style.size.y : height)
    );
  }

  void MuCanvasContext2DScreen::present() {
    flushBuffers();

    // Render to screen
    glViewport(0, 0, app.fbWidth, app.fbHeight);
    // TODO: clear with background color instead of black
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (framebuffer->valid) {
      auto ctx = app.glContext2D;
      auto width = app.width;
      auto height = app.height;
      nvgBeginFrame(ctx, width, height, app.devicePixelRatio);
        nvgBeginPath(ctx);
        nvgRect(ctx, 0, 0, width, height);
        nvgFillPaint(ctx, image);
        nvgFill(ctx);
      nvgEndFrame(ctx);
    }
  }

}
