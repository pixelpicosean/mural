//
//  MuCanvas.cpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#include "MuCanvas.hpp"
#include "MuCanvasManager.hpp"

namespace mural {

  gl::TextureRef MuCanvas::getTexture() {
    if (renderingContext) {
      return renderingContext->getTexture();
    }

    return nullptr;
  }

  MuCanvas::MuCanvas() {
    if (!theCanvasManager.hasScreenCanvas) {
      isScreenCanvas = true;
      theCanvasManager.hasScreenCanvas = true;

      size = getWindowSize();
    }
  }

  MuCanvas::~MuCanvas() {
    if (renderingContext) {
      delete renderingContext;
    }
  }

  MuCanvasContext2D *MuCanvas::getContext(mural::MuCanvasContextMode mode) {
    if (mode != kMuCanvasContextMode2D) {
      printf("Warning: Only CanvasContext2D currently supported!\n");
      return nullptr;
    }

    if (renderingContext) {
      return renderingContext;
    }

    contextMode = mode;

    if (isScreenCanvas) {
      // TODO: screen context
      renderingContext = new MuCanvasContext2D(size.x, size.y);
    }
    else {
      // TODO: texture context
      renderingContext = new MuCanvasContext2D(size.x, size.y);
    }

    renderingContext->create();
    theCanvasManager.setCurrentRenderingContext(renderingContext);

    return renderingContext;
  }

}
