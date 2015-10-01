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
  }

  void MuCanvasContext2D::beginPath() {
    state->path.clear();
  }

  void MuCanvasContext2D::closePath() {
    state->path.close();
  }

  void MuCanvasContext2D::fill() {
    gl::color(state->fillColor);
    gl::drawSolid(state->path);
    flushBuffers();
  }

  void MuCanvasContext2D::stroke() {
    gl::color(state->strokeColor);
    gl::draw(state->path);
    flushBuffers();
  }

  void MuCanvasContext2D::moveTo(float x, float y) {
    state->path.moveTo(x, y);
  }

  void MuCanvasContext2D::lineTo(float x, float y) {
    state->path.lineTo(x, y);
  }

  void MuCanvasContext2D::rect(float x, float y, float w, float h) {
    state->path.moveTo(x, y);
    state->path.lineTo(x + w, y);
    state->path.lineTo(x + w, y + h);
    state->path.lineTo(x, y + h);
    state->path.close();
  }

  void MuCanvasContext2D::prepare() {
    viewFramebuffer->bindFramebuffer();
    gl::viewport(bufferSize);
    needsPresenting = true;
  }

  void MuCanvasContext2D::flushBuffers() {
    viewFramebuffer->unbindFramebuffer();
    needsPresenting = false;
  }

}
