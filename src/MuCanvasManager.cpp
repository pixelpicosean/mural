//
//  MuCanvasManager.cpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#include "MuCanvasManager.hpp"
#include "MuCanvasContext2D.hpp"

namespace mural {

  void MuCanvasManager::setCurrentRenderingContext(mural::MuCanvasContext2D *renderingContext) {
    if (renderingContext != currentRenderingContext) {
      if (currentRenderingContext) {
        currentRenderingContext->flushBuffers();
      }

      renderingContext->prepare();
      currentRenderingContext = renderingContext;
    }
  }

  void MuCanvasManager::drawScreenCanvas() {
    if (hasScreenCanvas && screenRenderingContext) {
      setCurrentRenderingContext(screenRenderingContext);
      screenRenderingContext->present();
    }
  }

}
