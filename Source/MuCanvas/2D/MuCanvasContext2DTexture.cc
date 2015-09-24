#include "MuCanvasContext2DTexture.h"
#include "AppController.h"

#include <cstdio>

namespace mural {

  void MuCanvasContext2DTexture::create() {
    MuCanvasContext2D::create();

    printf(
      "Creating Offscreen Canvas (2D):\n"
        "  size:            %dx%d\n"
        "  buffer size:     %dx%d\n\n",
      width, height,
      bufferWidth, bufferHeight
    );
  }

}
