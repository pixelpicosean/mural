#include "MuCanvasContext2DScreen.h"
#include "AppController.h"

namespace mural {

  void MuCanvasContext2DScreen::setStyle(MuRect newStyle) {
    if ((style.size.x ? style.size.x : width) != newStyle.size.x ||
      (style.size.y ? style.size.y : height) != newStyle.size.y) {
      // Must resize
      style = newStyle;

      // Only resize if we already have a viewFrameBuffer. Otherwise the style
      // will be honored in the 'create' call.
      if (viewFrameBuffer) {
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

  void MuCanvasContext2DScreen::resizeTo(short newWidth, short newHeight) {
    flushBuffers();

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

    // Set up the renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderBuffer);

    // Flip the screen - OpenGL has the origin in the bottom left corner. We want the top left.
    upsideDown = true;

    resetFramebuffer();
  }

  MuTexture *MuCanvasContext2DScreen::getTexture() {
    // This context may not be the current one, but it has to be in order for
    // glReadPixels to succeed.
    MuCanvasContext *previousContext = app.currentRenderingContext;
    app.currentRenderingContext = this;

    float w = width * backingStoreRatio;
    float h = height * backingStoreRatio;

    MuTexture *texture = getImageDataScaled(1, upsideDown, 0, 0, w, h)->getTexture();
    texture->contentScale = backingStoreRatio;

    app.currentRenderingContext = previousContext;
    return texture;
  }

  void MuCanvasContext2DScreen::present() {
    flushBuffers();

    if (!needsPresenting) { return; }

    // TODO: render current render buffer to the screen
    // glContext->presentRenderbuffer(GL_RENDERBUFFER);

    needsPresenting = false;
  }

}
