//
//  MuCanvasManager.hpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#pragma once

namespace mural {

  using ci::gl::GlslProgRef;

  class MuCanvasContext2D;
  class MuCanvasManager {
    public:
      bool hasScreenCanvas = false;

      MuCanvasContext2D *getCurrentRenderingContext() { return currentRenderingContext; }
      void setCurrentRenderingContext(MuCanvasContext2D *renderingContext);

      MuCanvasContext2D *getScreenRenderingContext() { return screenRenderingContext; }
      void setScreenRenderingContext(MuCanvasContext2D *renderingContext) { screenRenderingContext = renderingContext; }

      GlslProgRef getGlsl2DFlat() { return glsl2DFlat; }
      GlslProgRef getGlsl2DTexture() { return glsl2DTexture; }
      GlslProgRef getGlsl2DPattern() { return glsl2DPattern; }

      void drawScreenCanvas();

      static MuCanvasManager &instance() {
        static MuCanvasManager inst;
        return inst;
      }

    private:
      MuCanvasContext2D *currentRenderingContext = nullptr;
      MuCanvasContext2D *screenRenderingContext = nullptr;

      GlslProgRef glsl2DFlat = nullptr;
      GlslProgRef glsl2DTexture = nullptr;
      GlslProgRef glsl2DPattern = nullptr;

      MuCanvasManager();
      ~MuCanvasManager() {}

      MuCanvasManager(MuCanvasManager const&) {}
      void operator=(MuCanvasManager const&) {}
  };

  #define theCanvasManager MuCanvasManager::instance()

}
