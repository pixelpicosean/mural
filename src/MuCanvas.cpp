//
//  MuCanvas.cpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#include "MuCanvas.hpp"
#include "MuCanvasManager.hpp"

#include "MuCanvasContext2DScreen.hpp"
#include "MuCanvasContext2DTexture.hpp"

namespace mural {

  gl::TextureRef MuCanvas::getTexture() {
    if (renderingContext) {
      return renderingContext->getTexture();
    }

    return nullptr;
  }

  MuCanvas::MuCanvas(int width, int height): size(width, height) {
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
      renderingContext = new MuCanvasContext2DScreen(size.x, size.y);
    }
    else {
      renderingContext = new MuCanvasContext2DTexture(size.x, size.y);
    }

    renderingContext->msaaEnabled = msaaEnabled;
    renderingContext->msaaSamples = msaaSamples;

    renderingContext->create();

    if (isScreenCanvas) {
      theCanvasManager.setScreenRenderingContext(renderingContext);
    }

    return renderingContext;
  }

}
