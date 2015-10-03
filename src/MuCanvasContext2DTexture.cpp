//
//  MuCanvasContext2DTexture.cpp
//  ciEjecta
//
//  Created by Sean on 10/3/15.
//
//

#include "MuCanvasContext2DTexture.hpp"

namespace mural {

  MuCanvasContext2DTexture::MuCanvasContext2DTexture(int width, int height):
    MuCanvasContext2D(width, height)
  {}

  void MuCanvasContext2DTexture::create() {
    MuCanvasContext2D::create();

    console() <<
      "Creating TextureCanvas (2D):\n" <<
      "  size:        " << size.x << 'x' << size.y << '\n' <<
      std::endl;
  }
  
}
