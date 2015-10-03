//
//  MuCanvasContext2D.cpp
//  ciEjecta
//
//  Created by Sean on 10/1/15.
//
//

#include "MuCanvasContext2D.hpp"

namespace mural {

  MuCanvasContext2D::MuCanvasContext2D(int width, int height):
    size(width, height)
  {
    state = &stateStack[0];
  }

  const gl::TextureRef MuCanvasContext2D::getTexture() {
    return viewFramebuffer->getColorTexture();
  }

  void MuCanvasContext2D::setFillColor(const ColorAf &color) {
    state->fillColor = color;
  }

  void MuCanvasContext2D::setStrokeColor(const ColorAf &color) {
    state->strokeColor = color;
  }

  void MuCanvasContext2D::create() {
    // Create mesh and batch
    mesh = gl::VboMesh::create(MU_MAX_VERTICES, GL_TRIANGLES, {
      gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::POSITION, 2),
      gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::TEX_COORD_0, 2),
      gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::COLOR, 4)
    });
    batch = gl::Batch::create(mesh, gl::getStockShader(gl::ShaderDef()));

    // Save vertex buffers for each part for later use
    positionVbo = mesh->findAttrib(geom::POSITION)->second;
    texCoordVbo = mesh->findAttrib(geom::TEX_COORD_0)->second;
    colorVbo = mesh->findAttrib(geom::COLOR)->second;

    resizeTo(size.x, size.y);
  }

  void MuCanvasContext2D::resizeTo(int newWidth, int newHeight) {
    size.x = newWidth;
    size.y = newHeight;

    devicePixelRatio = getWindowContentScale();

    bufferSize.x = size.x * devicePixelRatio;
    bufferSize.y = size.y * devicePixelRatio;

    viewFramebuffer = nullptr;
    viewFramebuffer = gl::Fbo::create(bufferSize.x, bufferSize.y);

    resetFramebuffer();
  }

  void MuCanvasContext2D::resetFramebuffer() {
    prepare();

    gl::clearColor(ColorAf::zero());
    gl::clear(GL_COLOR_BUFFER_BIT);

    needsPresenting = true;

    flushBuffers();
  }

  void MuCanvasContext2D::bindVertexBuffer() {
    positions = (vec2 *)positionVbo->mapReplace();
    texCoords = (vec2 *)texCoordVbo->mapReplace();
    colors = (vec4 *)colorVbo->mapReplace();
  }

  void MuCanvasContext2D::beginPath() {
    state->path.clear();
  }

  void MuCanvasContext2D::closePath() {
    state->path.close();
  }

  void MuCanvasContext2D::fill() {
    gl::color(state->fillColor);
    flushBuffers();
  }

  void MuCanvasContext2D::stroke() {
    gl::color(state->strokeColor);
    flushBuffers();
  }

  void MuCanvasContext2D::moveTo(float x, float y) {
    state->path.moveTo(x, y);
  }

  void MuCanvasContext2D::lineTo(float x, float y) {
    state->path.lineTo(x, y);
  }

  void MuCanvasContext2D::rect(float x, float y, float w, float h) {
//    state->path.moveTo(x, y);
//    state->path.lineTo(x + w, y);
//    state->path.lineTo(x + w, y + h);
//    state->path.lineTo(x, y + h);
//    state->path.close();

    positions[0] = vec2(x    , y    );
    positions[1] = vec2(x + w, y    );
    positions[2] = vec2(x    , y + h);

    positions[3] = vec2(x + w, y    );
    positions[4] = vec2(x    , y + h);
    positions[5] = vec2(x + w, y + h);

    vertexBufferIndex += 6;
  }

  void MuCanvasContext2D::prepare() {
    if (needsPresenting) {
      return;
    }

    viewFramebuffer->bindFramebuffer();
    gl::viewport(bufferSize);

    bindVertexBuffer();

    needsPresenting = true;
  }

  void MuCanvasContext2D::flushBuffers() {
    if (!needsPresenting) {
      return;
    }

    positionVbo->unmap();
    texCoordVbo->unmap();
    colorVbo->unmap();

    if (vertexBufferIndex > 0) {
      batch->draw(0, vertexBufferIndex);
    }

    viewFramebuffer->unbindFramebuffer();

    needsPresenting = false;
    vertexBufferIndex = 0;
  }

  void MuCanvasContext2D::pushTri(float x1, float y1, float x2, float y2, float x3, float y3, const ColorAf &color, const MuAffineTransform &transform) {
    if (vertexBufferIndex >= MU_MAX_VERTICES - 3) {
      flushBuffers();
    }

    vec2 d1(x1, y1);
    vec2 d2(x2, y2);
    vec2 d3(x3, y3);

    if (transform.isIdentity()) {
      d1 = transform.applyTo(d1);
      d2 = transform.applyTo(d2);
      d3 = transform.applyTo(d3);
    }

    positions[vertexBufferIndex]      = d1;
    positions[vertexBufferIndex + 1]  = d2;
    positions[vertexBufferIndex + 2]  = d3;

    texCoords[vertexBufferIndex]      = { 0, 0 };
    texCoords[vertexBufferIndex + 1]  = { 0, 0 };
    texCoords[vertexBufferIndex + 2]  = { 0, 0 };

    colors[vertexBufferIndex]     = color;
    colors[vertexBufferIndex + 1] = color;
    colors[vertexBufferIndex + 2] = color;

    vertexBufferIndex += 3;
  }

  void MuCanvasContext2D::pushQuad(vec2 v1, vec2 v2, vec2 v3, vec2 v4, const ColorAf &color, const MuAffineTransform &transform) {
    if (vertexBufferIndex >= MU_MAX_VERTICES - 6) {
      flushBuffers();
    }

    if (!transform.isIdentity()) {
      transform.applyTo(v1);
      transform.applyTo(v2);
      transform.applyTo(v3);
      transform.applyTo(v4);
    }

    positions[vertexBufferIndex]      = v1;
    positions[vertexBufferIndex + 1]  = v2;
    positions[vertexBufferIndex + 2]  = v3;
    positions[vertexBufferIndex + 3]  = v2;
    positions[vertexBufferIndex + 4]  = v3;
    positions[vertexBufferIndex + 5]  = v4;

    texCoords[vertexBufferIndex]      = { 0, 0 };
    texCoords[vertexBufferIndex + 1]  = { 0, 0 };
    texCoords[vertexBufferIndex + 2]  = { 0, 0 };
    texCoords[vertexBufferIndex + 3]  = { 0, 0 };
    texCoords[vertexBufferIndex + 4]  = { 0, 0 };
    texCoords[vertexBufferIndex + 5]  = { 0, 0 };

    colors[vertexBufferIndex]     = color;
    colors[vertexBufferIndex + 1] = color;
    colors[vertexBufferIndex + 2] = color;
    colors[vertexBufferIndex + 3] = color;
    colors[vertexBufferIndex + 4] = color;
    colors[vertexBufferIndex + 5] = color;

    vertexBufferIndex += 6;
  }

  void MuCanvasContext2D::pushRect(float x, float y, float w, float h, const ColorAf &color, const MuAffineTransform &transform) {
    if (vertexBufferIndex >= MU_MAX_VERTICES - 6) {
      flushBuffers();
    }

    vec2 d11(x    , y    );
    vec2 d21(x + w, y    );
    vec2 d12(x    , y + h);
    vec2 d22(x + w, y + h);

    if (transform.isIdentity()) {
      transform.applyTo(d11);
      transform.applyTo(d12);
      transform.applyTo(d21);
      transform.applyTo(d22);
    }

    positions[vertexBufferIndex]      = d11;
    positions[vertexBufferIndex + 1]  = d21;
    positions[vertexBufferIndex + 2]  = d12;
    positions[vertexBufferIndex + 3]  = d21;
    positions[vertexBufferIndex + 4]  = d12;
    positions[vertexBufferIndex + 5]  = d22;

    texCoords[vertexBufferIndex]      = { 0, 0 };
    texCoords[vertexBufferIndex + 1]  = { 0, 0 };
    texCoords[vertexBufferIndex + 2]  = { 0, 0 };
    texCoords[vertexBufferIndex + 3]  = { 0, 0 };
    texCoords[vertexBufferIndex + 4]  = { 0, 0 };
    texCoords[vertexBufferIndex + 5]  = { 0, 0 };

    colors[vertexBufferIndex]     = color;
    colors[vertexBufferIndex + 1] = color;
    colors[vertexBufferIndex + 2] = color;
    colors[vertexBufferIndex + 3] = color;
    colors[vertexBufferIndex + 4] = color;
    colors[vertexBufferIndex + 5] = color;

    vertexBufferIndex += 6;
  }

  void MuCanvasContext2D::pushTexturedRect(float x, float y, float w, float h, float tx, float ty, float tw, float th, const ColorAf &color, const MuAffineTransform &transform) {
    if (vertexBufferIndex >= MU_MAX_VERTICES - 6) {
      flushBuffers();
    }

    vec2 d11(x    , y    );
    vec2 d21(x + w, y    );
    vec2 d12(x    , y + h);
    vec2 d22(x + w, y + h);

    if (transform.isIdentity()) {
      transform.applyTo(d11);
      transform.applyTo(d12);
      transform.applyTo(d21);
      transform.applyTo(d22);
    }

    positions[vertexBufferIndex]      = d11;
    positions[vertexBufferIndex + 1]  = d21;
    positions[vertexBufferIndex + 2]  = d12;
    positions[vertexBufferIndex + 3]  = d21;
    positions[vertexBufferIndex + 4]  = d12;
    positions[vertexBufferIndex + 5]  = d22;

    texCoords[vertexBufferIndex]      = { tx     , ty      };
    texCoords[vertexBufferIndex + 1]  = { tx + tw, ty      };
    texCoords[vertexBufferIndex + 2]  = { tx     , ty + th };
    texCoords[vertexBufferIndex + 3]  = { tx + tw, ty      };
    texCoords[vertexBufferIndex + 4]  = { tx     , ty + th };
    texCoords[vertexBufferIndex + 5]  = { tx + tw, ty + th };

    colors[vertexBufferIndex]     = color;
    colors[vertexBufferIndex + 1] = color;
    colors[vertexBufferIndex + 2] = color;
    colors[vertexBufferIndex + 3] = color;
    colors[vertexBufferIndex + 4] = color;
    colors[vertexBufferIndex + 5] = color;

    vertexBufferIndex += 6;
  }

}
