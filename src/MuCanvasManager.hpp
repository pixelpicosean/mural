//
//  MuCanvasManager.hpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#pragma once

namespace mural {

  class MuCanvasContext2D;
  class MuCanvasManager {
    public:
      bool hasScreenCanvas = false;

      MuCanvasContext2D *getCurrentRenderingContext() { return currentRenderingContext; }
      void setCurrentRenderingContext(MuCanvasContext2D *renderingContext);

      MuCanvasContext2D *getScreenRenderingContext() { return screenRenderingContext; }
      void setScreenRenderingContext(MuCanvasContext2D *renderingContext) { screenRenderingContext = renderingContext; }

      ci::gl::GlslProgRef getGlsl2DFlat() {
        return glsl2DFlat;
      }

      void drawScreenCanvas();

      static MuCanvasManager &instance() {
        static MuCanvasManager inst;
        return inst;
      }

    private:
      MuCanvasContext2D *currentRenderingContext = nullptr;
      MuCanvasContext2D *screenRenderingContext = nullptr;

      ci::gl::GlslProgRef glsl2DFlat = nullptr;

      MuCanvasManager();
      ~MuCanvasManager() {}

      MuCanvasManager(MuCanvasManager const&) {}
      void operator=(MuCanvasManager const&) {}
  };

  #define theCanvasManager MuCanvasManager::instance()

}
