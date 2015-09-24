#include "MuCanvasContext2D.h"

#include "AppController.h"

namespace mural {

  void MuCanvasContext2D::setWidth(int newWidth) {}

  void MuCanvasContext2D::setHeight(int newHeight) {}

  MuCanvasContext2D::MuCanvasContext2D(int widthp, int heightp) {
    width = widthp;
    height = heightp;
    bufferWidth = width * app.devicePixelRatio;
    bufferHeight = height * app.devicePixelRatio;
  }

  MuCanvasContext2D::~MuCanvasContext2D() {
    if (framebuffer) delete framebuffer;
  }

  void MuCanvasContext2D::resizeTo(int newWidth, int newHeight) {}

  void MuCanvasContext2D::create() {
    framebuffer = new nvg::Framebuffer(app.glContext2D, bufferWidth, bufferHeight);
    glContext = nvg::createGLContext();
    image = nvgImagePattern(glContext, 0, 0, width, height, 0, framebuffer->image, 1.0f);
  }

  void MuCanvasContext2D::prepare() {
    // Already prepared
    if (needsPresenting) return;

    framebuffer->bind();

    glViewport(0, 0, bufferWidth, bufferHeight);

    nvgBeginFrame(glContext, width, height, app.devicePixelRatio);

    needsPresenting = true;
  }

  void MuCanvasContext2D::flushBuffers() {
    if (!needsPresenting) return;

    nvgEndFrame(glContext);

    framebuffer->unbind();

    needsPresenting = false;
  }

  void MuCanvasContext2D::save() {}

  void MuCanvasContext2D::restore() {}

  void MuCanvasContext2D::rotate(float angle) {}

  void MuCanvasContext2D::translate(float x, float y) {}

  void MuCanvasContext2D::scale(float x, float y) {}

  void MuCanvasContext2D::transform(float m11, float m12, float m21, float m22, float dx, float dy) {}

  void MuCanvasContext2D::setTransform(float m11, float m12, float m21, float m22, float dx, float dy) {}

  // void MuCanvasContext2D::drawImage(MuTexture *texture, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {}

  void MuCanvasContext2D::fillRect(float x, float y, float w, float h) {}

  void MuCanvasContext2D::strokeRect(float x, float y, float w, float h) {}

  void MuCanvasContext2D::clearRect(float x, float y, float w, float h) {}

  void MuCanvasContext2D::beginPath() {
    prepare();
    nvgBeginPath(glContext);
  }

  void MuCanvasContext2D::closePath() {
    prepare();
    nvgClosePath(glContext);
  }

  void MuCanvasContext2D::fill() {
    prepare();
    nvgFillColor(glContext, fillColor);
    nvgFill(glContext);
  }

  void MuCanvasContext2D::stroke() {}

  void MuCanvasContext2D::moveTo(float x, float y) {}

  void MuCanvasContext2D::lineTo(float x, float y) {}

  void MuCanvasContext2D::rect(float x, float y, float w, float h) {
    prepare();
    nvgRect(glContext, x, y, w, h);
  }

  void MuCanvasContext2D::bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y) {}
  void MuCanvasContext2D::quadraticCurveTo(float cpx, float cpy, float x, float y) {}
  void MuCanvasContext2D::arcTo(float x1, float y1, float x2, float y2, float radius) {}
  void MuCanvasContext2D::arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {}

  // void MuCanvasContext2D::fillText(std::string text, float x, float y) {}
  // void MuCanvasContext2D::strokeText(std::string text, float x, float y) {}
  // MuTextMetrics MuCanvasContext2D::measureText(std::string text) {}

}
