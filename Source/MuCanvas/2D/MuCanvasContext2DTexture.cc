#include "MuCanvasContext2DTexture.h"
#include "AppController.h"

#include <cstdio>

namespace mural {

  MuTexture *MuCanvasContext2DTexture::getTexture() {
    // Special case where this canvas is drawn into itself - we have to use glReadPixels to get a texture
    MuCanvasContext2DTexture *ctx = dynamic_cast<MuCanvasContext2DTexture *>(app.currentRenderingContext);
    if (ctx && ctx == this) {
      float w = width * backingStoreRatio;
      float h = height * backingStoreRatio;

      MuTexture *tempTexture = getImageDataScaled(1, upsideDown, 0, 0, w, h)->getTexture();
      tempTexture->contentScale = backingStoreRatio;
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

    backingStoreRatio = (useRetinaResolution && app.devicePixelRatio == 2) ? 2 : 1;
    bufferWidth = width * backingStoreRatio;
    bufferHeight = height * backingStoreRatio;

    printf(
      "Creating Offscreen Canvas (2D):\n"
        "  size:    %dx%d\n"
        "  retina:  %s (%.0fx%.0f)\n\n",
      width, height,
      (useRetinaResolution ? "true" : "false"),
      width * backingStoreRatio, height * backingStoreRatio
    );

    // Release previous texture if any, create the new texture and set it as
    // the rendering target for this framebuffer
    // TODO: make texture an object with ref-count
    texture = nullptr;
    texture = new MuTexture(newWidth, newHeight, viewFrameBuffer, backingStoreRatio);

    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTextureId(), 0);

    resetFramebuffer();
  }

}
