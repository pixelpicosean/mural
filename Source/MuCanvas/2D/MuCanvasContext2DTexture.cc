#include "MuCanvasContext2DTexture.h"
#include "AppController.h"

#include <cstdio>

namespace mural {

  MuTexture *MuCanvasContext2DTexture::getTexture() {
    // Special case where this canvas is drawn into itself - we have to use glReadPixels to get a texture
    MuCanvasContext2DTexture *ctx = dynamic_cast<MuCanvasContext2DTexture *>(app.getCurrentRenderingContext());
    if (ctx && ctx == this) {
      MuTexture *tempTexture = getImageDataScaled(1, upsideDown, 0, 0, width, height)->getTexture();
      tempTexture->contentScale = 1;
      return tempTexture;
    }

    // Just use the framebuffer texture directly
    else {
      return texture;
    }
  }

  void MuCanvasContext2DTexture::resizeTo(short newWidth, short newHeight) {
    flushBuffers();

    width = newWidth;
    height = newHeight;

    bufferWidth = width * app.devicePixelRatio;
    bufferHeight = height * app.devicePixelRatio;

    printf(
      "Creating Offscreen Canvas (2D):\n"
        "  size:        %dx%d\n"
        "  buffer size: %dx%d\n\n",
      width, height,
      bufferWidth, bufferHeight
    );

    // Release previous texture if any, create the new texture and set it as
    // the rendering target for this framebuffer
    // TODO: make texture an object with ref-count
    texture = nullptr;
    texture = new MuTexture(newWidth, newHeight, viewFrameBuffer, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTextureId(), 0);

    resetFramebuffer();
  }

}
