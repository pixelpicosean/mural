//
//  MuCanvasContext2DScreen.hpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#pragma once

#include "MuCanvasContext2D.hpp"

namespace mural {

  class MuCanvasContext2DScreen : public MuCanvasContext2D {
    public:
      MuCanvasContext2DScreen(int width, int height);

      void create() override;
      void present() override;
  };

}
