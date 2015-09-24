#include "MuCanvasContext2D.h"

#include "AppController.h"

namespace mural {

  void MuCanvasContext2D::setWidth(int newWidth) {}

  void MuCanvasContext2D::setHeight(int newHeight) {}

  void MuCanvasContext2D::setFillStyle(NVGcolor color) {
    prepare();
    nvgFillColor(glContext, color);
  }

  void MuCanvasContext2D::setFillStyle(NVGpaint paint) {
    prepare();
    nvgFillPaint(glContext, paint);
  }

  void MuCanvasContext2D::setStrokeStyle(NVGcolor color) {
    prepare();
    nvgStrokeColor(glContext, color);
  }

  void MuCanvasContext2D::setStrokeStyle(NVGpaint paint) {
    prepare();
    nvgStrokePaint(glContext, paint);
  }

  MuCanvasContext2D::MuCanvasContext2D(int widthp, int heightp) {
    width = widthp;
    height = heightp;
    bufferWidth = width * app.devicePixelRatio;
    bufferHeight = height * app.devicePixelRatio;
  }

  MuCanvasContext2D::~MuCanvasContext2D() {
    if (framebuffer) delete framebuffer;
    if (glContext) delete glContext;
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

    app.setCurrentRenderingContext(this);

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

  void MuCanvasContext2D::save() {
    prepare();
    nvgSave(glContext);
  }

  void MuCanvasContext2D::restore() {
    prepare();
    nvgRestore(glContext);
  }

  void MuCanvasContext2D::rotate(float angle) {
    prepare();
    nvgRotate(glContext, angle);
  }

  void MuCanvasContext2D::translate(float x, float y) {
    prepare();
    nvgTranslate(glContext, x, y);
  }

  void MuCanvasContext2D::scale(float x, float y) {
    prepare();
    nvgScale(glContext, x, y);
  }

  void MuCanvasContext2D::transform(float m11, float m12, float m21, float m22, float dx, float dy) {
    prepare();
    nvgTransform(glContext, m11, m12, m21, m22, dx, dy);
  }

  void MuCanvasContext2D::setTransform(float m11, float m12, float m21, float m22, float dx, float dy) {
    prepare();
    nvgResetTransform(glContext);
    nvgTransform(glContext, m11, m12, m21, m22, dx, dy);
  }

  void MuCanvasContext2D::drawImage(NVGpaint image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {
    prepare();
    nvgBeginPath(glContext);
    nvgRect(glContext, dx, dy, dw, dh);
    nvgFillPaint(glContext, image);
    nvgFill(glContext);
  }

  void MuCanvasContext2D::fillRect(float x, float y, float w, float h) {
    prepare();
    nvgRect(glContext, x, y, w, h);
    nvgFill(glContext);
  }

  void MuCanvasContext2D::strokeRect(float x, float y, float w, float h) {
    prepare();
    nvgRect(glContext, x, y, w, h);
    nvgStroke(glContext);
  }

  void MuCanvasContext2D::clearRect(float x, float y, float w, float h) {
    prepare();
    nvgSave(glContext);
    nvgFillColor(glContext, nvgRGB(255, 255, 255));
    nvgRect(glContext, x, y, w, h);
    nvgFill(glContext);
    nvgRestore(glContext);
  }

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
    nvgFill(glContext);
  }

  void MuCanvasContext2D::stroke() {
    prepare();
    nvgStroke(glContext);
  }

  void MuCanvasContext2D::moveTo(float x, float y) {
    prepare();
    nvgMoveTo(glContext, x, y);
  }

  void MuCanvasContext2D::lineTo(float x, float y) {
    prepare();
    nvgLineTo(glContext, x, y);
  }

  void MuCanvasContext2D::rect(float x, float y, float w, float h) {
    prepare();
    nvgRect(glContext, x, y, w, h);
  }

  void MuCanvasContext2D::bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y) {
    prepare();
    nvgBezierTo(glContext, cpx1, cpy1, cpx2, cpy2, x, y);
  }

  void MuCanvasContext2D::quadraticCurveTo(float cpx, float cpy, float x, float y) {
    prepare();
    nvgQuadTo(glContext, cpx, cpy, x, y);
  }

  void MuCanvasContext2D::arcTo(float x1, float y1, float x2, float y2, float radius) {
    prepare();
    nvgArcTo(glContext, x1, y1, x2, y2, radius);
  }

  void MuCanvasContext2D::arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {
    prepare();
    nvgArc(glContext, x, y, radius, startAngle, endAngle, antiClockwise ? NVG_CCW : NVG_CW);
  }

  // void MuCanvasContext2D::fillText(std::string text, float x, float y) {}
  // void MuCanvasContext2D::strokeText(std::string text, float x, float y) {}
  // MuTextMetrics MuCanvasContext2D::measureText(std::string text) {}

}
