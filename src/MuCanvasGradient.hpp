//
//  MuCanvasGradient.hpp
//  Mural
//
//  Created by Sean on 10/14/15.
//
//

#pragma once

#include "MuCanvasContext2D.hpp"

#include <vector>

namespace mural {

  #define MU_CANVAS_GRADIENT_WIDTH 1024

  enum MuCanvasGradientType {
    kMuCanvasGradientTypeLinear,
    kMuCanvasGradientTypeRadial,
  };

  struct MuCanvasGradientColorStop {
    float pos;
    unsigned int order;
    ci::ColorAf color;

    MuCanvasGradientColorStop(float posp = 0.0f, unsigned int orderp = 0, const ci::ColorAf& colorp = ci::ColorAf::zero()):
      pos(posp), order(orderp), color(colorp)
    {}
  };

  class MuCanvasGradient : public MuFillable {
    public:
      MuCanvasGradientType type;
      ci::vec2 p1, p2;
      float r1, r2;

      ci::gl::TextureRef& getTexture();

      MuCanvasGradient(const ci::vec2& p1, const ci::vec2& p2);
      MuCanvasGradient(const ci::vec2& p1, float r1, const ci::vec2& p2, float r2);

      void addStop(const ColorAf& color, float pos);
      void rebuild();

    protected:
      std::vector<MuCanvasGradientColorStop> colorStops;
      ci::gl::TextureRef texture;
  };

}
