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
    }

    renderingContext->prepare();
    currentRenderingContext = renderingContext;
  }

  void MuCanvasManager::drawScreenCanvas() {
    if (hasScreenCanvas && screenRenderingContext) {
      setCurrentRenderingContext(screenRenderingContext);
      screenRenderingContext->present();
    }
  }

  MuCanvasManager::MuCanvasManager() {
    // Create shared mesh and batch for canvas contexts
    mesh = gl::VboMesh::create(MU_MAX_VERTICES, GL_TRIANGLES, {
      gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::POSITION, 2),
      gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::TEX_COORD_0, 2),
      gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::COLOR, 4)
    });
    batch = gl::Batch::create(mesh, gl::getStockShader(gl::ShaderDef()));

    // Vertex Shader - General
    auto vs = CI_GLSL(150,
      uniform mat4 ciModelViewProjection;

      in vec2 ciPosition;
      in highp vec2 ciTexCoord0;
      in lowp vec4 ciColor;

      out vec2 TexCoord0;
      out vec4 color;

      void main() {
        gl_Position = ciModelViewProjection * vec4(ciPosition, 0.0, 1.0);
        TexCoord0 = ciTexCoord0;
        color = ciColor;
      }
    );

    // Fragment Shader - Flat
    glsl2DFlat = gl::GlslProg::create(gl::GlslProg::Format()
      .vertex(vs)
      .fragment(CI_GLSL(150,
        in vec4 color;
        out vec4 oColor;

        void main() {
          oColor = color;
        }
      ))
    );

    // Fragment Shader - Texture
    glsl2DTexture = gl::GlslProg::create(gl::GlslProg::Format()
      .vertex(vs)
      .fragment(CI_GLSL(150,
        uniform sampler2D uTex0;

        in vec2 TexCoord0;
        out vec4 oColor;
        
        void main() {
          oColor = texture(uTex0, TexCoord0);
        }
      ))
    );

    // Fragment Shader - Pattern
    glsl2DPattern = gl::GlslProg::create(gl::GlslProg::Format()
      .vertex(vs)
      .fragment(CI_GLSL(150,
        uniform sampler2D uTex0;

        in vec4 color;
        in vec2 TexCoord0;

        out vec4 oColor;

        void main() {
          oColor = texture(uTex0, mod(TexCoord0, vec2(1.0, 1.0))) * color;
        }
      ))
    );
  }

}
