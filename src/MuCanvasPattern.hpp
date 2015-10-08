//
//  MuCanvasPattern.hpp
//  Mural
//
//  Created by Sean on 10/8/15.
//
//

#pragma once

#include "MuCanvasContext2D.hpp"

namespace mural {

  enum MuCanvasPatternRepeat {
    kMuCanvasPatternNoRepeat = 0,
    kMuCanvasPatternRepeatX = 1,
    kMuCanvasPatternRepeatY = 2,
    kMuCanvasPatternRepeat = 1 | 2
  };

  struct MuCanvasPattern : public MuFillable {
    ci::gl::Texture2dRef texture;
    MuCanvasPatternRepeat repeat;

    MuCanvasPattern(const ci::gl::Texture2dRef &texturep, MuCanvasPatternRepeat repeatp):
      texture(texturep), repeat(repeatp)
    {}
  };

}
