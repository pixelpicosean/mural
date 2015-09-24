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

  MuRect MuCanvasContext2DScreen::getFrame() {
    return MuRect(
      style.origin.x, style.origin.y,
      (style.size.x ? style.size.x : width),
      (style.size.y ? style.size.y : height)
    );
  }

  void MuCanvasContext2DScreen::resizeTo(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;

    MuRect frame = getFrame();

    float contentScale = useRetinaResolution ? app.devicePixelRatio : 1;
    backingStoreRatio = (frame.size.x / (float)width) * contentScale;

    bufferWidth = frame.size.x * contentScale;
    bufferHeight = frame.size.y * contentScale;

    printf(
      "Creating ScreenCanvas (2D):\n"
        "  size:    %dx%d\n"
        "  style:   %.0fx%.0f\n"
        "  retina:  %s (%.0fx%.0f)\n\n",
      width, height,
      frame.size.x, frame.size.y,
      (useRetinaResolution ? "true" : "false"),
      frame.size.x * contentScale, frame.size.y * contentScale
    );

    // Flip the screen - OpenGL has the origin in the bottom left corner. We want the top left.
    upsideDown = true;
  }

  void MuCanvasContext2DScreen::create() {
    MuCanvasContext2D::create();
  }

  void MuCanvasContext2DScreen::present() {
    // flushBuffers();

    framebuffer->bind();
    glViewport(0, 0, bufferWidth, bufferHeight);
    glClearColor(0.37f, 0.49f, 0.54f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(glContext, width, height, app.devicePixelRatio);
      nvgBeginPath(glContext);
      nvgFillColor(glContext, nvgRGB(0, 187, 211));
      nvgRect(glContext, 0, 0, 100, 100);
      nvgFill(glContext);
    nvgEndFrame(glContext);
    framebuffer->unbind();

    NVGpaint img = nvgImagePattern(glContext, 0, 0, width, height, 0, framebuffer->image, 1.0f);

    // Render to screen
    glViewport(0, 0, app.fbWidth, app.fbHeight);
    // TODO: clear with background color instead of black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (framebuffer->valid) {
      auto ctx = app.glContext2D;
      auto width = app.width;
      auto height = app.height;
      nvgBeginFrame(ctx, width, height, app.devicePixelRatio);
        // Draw a circle on the left
        nvgBeginPath(ctx);
        nvgCircle(ctx, 100, 100, 40);
        nvgFillColor(ctx, nvgRGB(155, 38, 175));
        nvgFill(ctx);

        // Draw frame buffer
        nvgBeginPath(ctx);
        nvgRect(ctx, 0, 0, width, height);
        nvgFillPaint(ctx, img);
        nvgFill(ctx);

        nvgBeginPath(ctx);
        nvgFillColor(ctx, nvgRGB(0, 187, 211));
        nvgRect(ctx, 110, 0, 100, 100);
        nvgFill(ctx);

        // Draw a circle on the right
        nvgBeginPath(ctx);
        nvgCircle(ctx, 540, 100, 40);
        nvgFillColor(ctx, nvgRGB(254, 151, 0));
        nvgFill(ctx);
      nvgEndFrame(ctx);
    }
  }

}
