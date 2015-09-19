#include "MuCanvasContext2DScreen.h"
#include "AppController.h"

namespace mural {

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

}
