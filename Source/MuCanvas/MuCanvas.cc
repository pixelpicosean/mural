#include "MuCanvas.h"

#include "AppController.h"
#include "MuCanvas/2D/MuCanvasContext2DScreen.h"
#include "MuCanvas/2D/MuCanvasContext2DTexture.h"

namespace mural {

  MuCanvas::MuCanvas():
    useRetinaResolution(true),
    msaaEnabled(false),
    msaaSamples(2)
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
      renderingContext = new MuCanvasContext2DScreen();
    }
    else {
      renderingContext = new MuCanvasContext2DTexture();
    }

    renderingContext->useRetinaResolution = useRetinaResolution;
    renderingContext->msaaEnabled = msaaEnabled;
    renderingContext->msaaSamples = msaaSamples;

    if (isScreenCanvas) {
      app.screenRenderingContext = renderingContext;
    }

    renderingContext->create();
    app.currentRenderingContext = renderingContext;

    return renderingContext;
  }

}
