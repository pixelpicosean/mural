//
//  MuCanvasContext2DTexture.hpp
//  ciEjecta
//
//  Created by Sean on 10/3/15.
//
//

#pragma once

#include "MuCanvasContext2D.hpp"

namespace mural {

  class MuCanvasContext2DTexture : public MuCanvasContext2D {
  public:
    MuCanvasContext2DTexture(int width, int height);

    void create() override;
  };

}
