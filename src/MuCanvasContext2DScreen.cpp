//
//  MuCanvasContext2DScreen.cpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#include "MuCanvasContext2DScreen.hpp"

namespace mural {

  MuCanvasContext2DScreen::MuCanvasContext2DScreen(int width, int height):
    MuCanvasContext2D(width, height)
  {}

  void MuCanvasContext2DScreen::create() {
    MuCanvasContext2D::create();

    console() <<
      "Creating ScreenCanvas (2D):\n" <<
      "  size:        " << size.x << 'x' << size.y << '\n' <<
      "  buffer size: " << bufferSize.x << 'x' << bufferSize.y << '\n' <<
      std::endl;
  }

  void MuCanvasContext2DScreen::present() {
    finish();

    // Reset composite operation for drawing
    MuCompositeOperation op = kMuCompositeOperationSourceOver;
    glBlendFunc(MuCompositeOperationFuncs[op].source, MuCompositeOperationFuncs[op].destination);

    // TODO: clear to `style.backgroundColor`
    gl::clear(ColorAf::zero());
    gl::viewport(0, 0, getWindowWidth(), getWindowHeight());
    gl::draw(getTexture(), getWindowBounds());
  }

}
