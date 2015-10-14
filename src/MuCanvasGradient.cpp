//
//  MuCanvasGradient.cpp
//  Mural
//
//  Created by Sean on 10/14/15.
//
//

#include "MuCanvasGradient.hpp"

namespace mural {

  ci::gl::TextureRef& MuCanvasGradient::getTexture() {
    if (!texture) {
      rebuild();
    }

    return texture;
  }

  MuCanvasGradient::MuCanvasGradient(const ci::vec2& p1p, const ci::vec2& p2p):
    type(kMuCanvasGradientTypeLinear),
    p1(p1p), p2(p2p)
  {
    colorStops.resize(4);
  }

  MuCanvasGradient::MuCanvasGradient(const ci::vec2& p1p, float r1p, const ci::vec2& p2p, float r2p):
    type(kMuCanvasGradientTypeRadial),
    p1(p1p), p2(p2p), r1(r1p), r2(r2p)
  {
    colorStops.resize(4);
  }

  void MuCanvasGradient::addStop(const ColorAf &color, float pos) {
    float alpha = color.a / 255.0f;
    ColorAf premultiplied = {
      color.r * alpha,
      color.g * alpha,
      color.b * alpha,
      color.a
    };

    colorStops.emplace_back(pos, (unsigned int)colorStops.size(), premultiplied);

    // Release current texture; it's invalid now
    texture = nullptr;
  }

  void MuCanvasGradient::rebuild() {}

}
