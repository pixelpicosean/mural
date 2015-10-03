//
//  MuCanvasManager.cpp
//  ciEjecta
//
//  Created by Sean on 10/2/15.
//
//

#include "MuCanvasManager.hpp"
#include "MuCanvasContext2D.hpp"

namespace mural {

  using namespace ci;

  void MuCanvasManager::setCurrentRenderingContext(mural::MuCanvasContext2D *renderingContext) {
    if (renderingContext != currentRenderingContext) {
      if (currentRenderingContext) {
        currentRenderingContext->finish();
      }

      renderingContext->prepare();
      currentRenderingContext = renderingContext;
    }
  }

  void MuCanvasManager::drawScreenCanvas() {
    if (hasScreenCanvas && screenRenderingContext) {
      setCurrentRenderingContext(screenRenderingContext);
      screenRenderingContext->present();
    }
  }

  MuCanvasManager::MuCanvasManager() {
    glsl2DFlat = gl::GlslProg::create(gl::GlslProg::Format()
      .vertex(CI_GLSL(150,
        uniform mat4 ciModelViewProjection;

        in vec2 ciPosition;
        in highp vec2 ciTexCoord0;
        in lowp vec4 ciColor;

        out vec4 color;

        void main() {
          gl_Position = ciModelViewProjection * vec4(ciPosition, 0.0, 1.0);
          color = ciColor;
        }
      ))
      .fragment(CI_GLSL(150,
        in vec4 color;
        out vec4 oColor;

        void main() {
          oColor = color;
        }
      ))
    );
  }

}
