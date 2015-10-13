//
//  MuCanvas.hpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#pragma once

#include "MuCanvasContext2D.hpp"

namespace mural {

  using namespace ci;

  enum MuCanvasContextMode {
    kMuCanvasContextModeInvalid,
    kMuCanvasContextMode2D,
    kMuCanvasContextModeWebGL
  };

  class MuCanvas {
    public:
      gl::TextureRef getTexture();

      int getWidth() { return size.x; }
      void setWidth(int newWidth) { size.x = newWidth; }

      int getHeight() { return size.y; }
      void setHeight(int newHeight) { size.y = newHeight; }

      MuCanvas(int width = 200, int height = 150);
      ~MuCanvas();

      MuCanvasContext2D *getContext(MuCanvasContextMode mode);

    protected:
      MuCanvasContext2D *renderingContext = nullptr;
      MuCanvasContextMode contextMode = kMuCanvasContextModeInvalid;

      bool isScreenCanvas = false;

      ivec2 size = { 200, 150 };
  };

}
