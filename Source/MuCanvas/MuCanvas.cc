#include "MuCanvas.h"

#include "AppController.h"
#include "MuCanvas/2D/MuCanvasContext2DScreen.h"
#include "MuCanvas/2D/MuCanvasContext2DTexture.h"

namespace mural {

  void MuCanvas::setWidth(short newWidth) {
    if (renderingContext) {
      app.currentRenderingContext = renderingContext;
      renderingContext->setWidth(newWidth);
      width = renderingContext->getWidth();
      return;
    }
    else {
      width = newWidth;
    }
  }

  void MuCanvas::setHeight(short newHeight) {
    if (renderingContext) {
      app.currentRenderingContext = renderingContext;
      renderingContext->setHeight(newHeight);
      height = renderingContext->getHeight();
      return;
    }
    else {
      height = newHeight;
    }
  }

  MuCanvas::MuCanvas():
    useRetinaResolution(false)
  {
    if (!app.hasScreenCanvas) {
      isScreenCanvas = true;
      app.hasScreenCanvas = true;
    }

    width = app.width;
    height = app.height;
  }

  MuCanvas::~MuCanvas() {
    if (isScreenCanvas) {
      app.hasScreenCanvas = false;
    }
    if (renderingContext) {
      delete renderingContext;
    }
  }

  MuCanvasContext *MuCanvas::getContext(MuCanvasContextMode mode) {
    if (mode != kMuCanvasContextMode2D) {
      printf("Warning: Only CanvasContext2D currently supported!\n");
      return nullptr;
    }

    contextMode = mode;
    app.currentRenderingContext = nullptr;

    // Configure and create the CanvasContext
    if (isScreenCanvas) {
      renderingContext = new MuCanvasContext2DScreen(app.width, app.height);
    }
    else {
      renderingContext = new MuCanvasContext2DTexture(app.width, app.height);
    }

    if (isScreenCanvas) {
      app.screenRenderingContext = renderingContext;
    }

    renderingContext->create();
    app.currentRenderingContext = renderingContext;

    return renderingContext;
  }

}
