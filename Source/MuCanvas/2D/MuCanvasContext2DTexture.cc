#include "MuCanvasContext2DTexture.h"
#include "AppController.h"

#include <cstdio>

namespace mural {

  void MuCanvasContext2DTexture::resizeTo(int newWidth, int newHeight) {
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
  }

}
