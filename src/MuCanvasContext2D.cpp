//
//  MuCanvasContext2D.cpp
//  ciEjecta
//
//  Created by Sean on 10/1/15.
//
//

#include "MuCanvasContext2D.hpp"
#include "MuCanvasManager.hpp"

namespace mural {

  const MuCompositeOperationFunc MuCompositeOperationFuncs[] = {
    [kMuCompositeOperationSourceOver] = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA, 1},
    [kMuCompositeOperationLighter] = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA, 0},
    [kMuCompositeOperationDarker] = {GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, 1},
    [kMuCompositeOperationDestinationOut] = {GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, 1},
    [kMuCompositeOperationDestinationOver] = {GL_ONE_MINUS_DST_ALPHA, GL_ONE, 1},
    [kMuCompositeOperationSourceAtop] = {GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 1},
    [kMuCompositeOperationXOR] = {GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 1},
    [kMuCompositeOperationCopy] = {GL_ONE, GL_ZERO, 1},
    [kMuCompositeOperationSourceIn] = {GL_DST_ALPHA, GL_ZERO, 1},
    [kMuCompositeOperationDestinationIn] = {GL_ZERO, GL_SRC_ALPHA, 1},
    [kMuCompositeOperationSourceOut] = {GL_ONE_MINUS_DST_ALPHA, GL_ZERO, 1},
    [kMuCompositeOperationDestinationAtop] = {GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA, 1}
  };

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

    flushBuffers();
  }

  void MuCanvasContext2D::bindVertexBuffer() {
    if (vboMapped) {
      return;
    }

    positions = (vec2 *)positionVbo->mapReplace();
    texCoords = (vec2 *)texCoordVbo->mapReplace();
    colors = (vec4 *)colorVbo->mapReplace();

    vboMapped = true;
  }

  void MuCanvasContext2D::unbindVertexBuffer() {
    if (!vboMapped) {
      return;
    }

    positionVbo->unmap();
    texCoordVbo->unmap();
    colorVbo->unmap();

    vboMapped = false;
  }

  void MuCanvasContext2D::save() {
    if (stateIndex == MU_CANVAS_STATE_STACK_SIZE - 1) {
      printf("Warning: MU_CANVAS_STATE_STACK_SIZE (%d) reached", MU_CANVAS_STATE_STACK_SIZE);
      return;
    }

    stateStack[stateIndex + 1] = stateStack[stateIndex];
    stateIndex++;
    state = &stateStack[stateIndex];
  }

  void MuCanvasContext2D::restore() {
    if (stateIndex == 0) { return; }

    MuCompositeOperation oldCompositeOp = state->globalCompositeOperation;

    // Load state from stack
    stateIndex--;
    state = &stateStack[stateIndex];

    path->transform = state->transform;

    // Set Composite op, if different
    if (state->globalCompositeOperation != oldCompositeOp) {
      globalCompositeOperation = state->globalCompositeOperation;
    }
  }

  void MuCanvasContext2D::rotate(float angle) {
    state->transform.rotate(angle);
    path->transform = state->transform;
  }

  void MuCanvasContext2D::translate(float x, float y) {
    state->transform.translate(x, y);
    path->transform = state->transform;
  }

  void MuCanvasContext2D::scale(float x, float y) {
    state->transform.scale(x, y);
    path->transform = state->transform;
  }

  void MuCanvasContext2D::transform(float m11, float m12, float m21, float m22, float dx, float dy) {
    MuAffineTransform t(m11, m12, m21, m22, dx, dy);
    t.concat(state->transform);
    path->transform = state->transform = t;
  }

  void MuCanvasContext2D::setTransform(float m11, float m12, float m21, float m22, float dx, float dy) {
    state->transform = MuAffineTransform(m11, m12, m21, m22, dx, dy);
    path->transform = state->transform;
  }

  void MuCanvasContext2D::drawImage(const gl::TextureRef &image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {
    auto tw = image->getWidth();
    auto th = image->getHeight();

    setProgram(theCanvasManager.getGlsl2DTexture());
    setTexture(image);
    pushTexturedRect(dx, dy, dw, dh, sx/tw, sy/th, sw/tw, sh/th, blendWhiteColor(state), state->transform);
  }

  void MuCanvasContext2D::drawImage(const gl::TextureRef &image, float dx, float dy) {
    auto tw = image->getWidth();
    auto th = image->getHeight();

    drawImage(image, 0.0f, 0.0f, tw, th, dx, dy, tw, th);
  }

  void MuCanvasContext2D::drawImage(const gl::TextureRef &image, float dx, float dy, float dw, float dh) {
    auto tw = image->getWidth();
    auto th = image->getHeight();

    drawImage(image, 0.0f, 0.0f, tw, th, dx, dy, dw, dh);
  }

  void MuCanvasContext2D::fillRect(float x, float y, float w, float h) {
    setProgram(theCanvasManager.getGlsl2DFlat());

    pushRect(x, y, w, h,
      blendFillColor(state),
      state->transform
    );
  }

  void MuCanvasContext2D::strokeRect(float x, float y, float w, float h) {
    MuPath tempPath;

    tempPath.transform = state->transform;

    tempPath.moveTo(x    , y    );
    tempPath.lineTo(x + w, y    );
    tempPath.lineTo(x + w, y + h);
    tempPath.lineTo(x    , y + h);
    tempPath.close();

    setProgram(theCanvasManager.getGlsl2DFlat());
    tempPath.drawLinesToContext(this);
  }

  void MuCanvasContext2D::clearRect(float x, float y, float w, float h) {
    setProgram(theCanvasManager.getGlsl2DFlat());

    MuCompositeOperation oldOp = state->globalCompositeOperation;
    globalCompositeOperation = kMuCompositeOperationDestinationOut;

    pushRect(x, y, w, h, ColorAf::white(), state->transform);

    globalCompositeOperation = oldOp;
  }

  void MuCanvasContext2D::beginPath() {
    path->reset();
  }

  void MuCanvasContext2D::closePath() {
    path->close();
  }

  void MuCanvasContext2D::fill() {
    setProgram(theCanvasManager.getGlsl2DFlat());
    path->drawPolygonsToContext(this, kMuPathPolygonTargetColor);
  }

  void MuCanvasContext2D::stroke() {
    setProgram(theCanvasManager.getGlsl2DFlat());
    path->drawLinesToContext(this);
  }

  void MuCanvasContext2D::moveTo(float x, float y) {
    path->moveTo(x, y);
  }

  void MuCanvasContext2D::lineTo(float x, float y) {
    path->lineTo(x, y);
  }

  void MuCanvasContext2D::rect(float x, float y, float w, float h) {
    path->moveTo(x    , y    );
    path->lineTo(x + w, y    );
    path->lineTo(x + w, y + h);
    path->lineTo(x    , y + h);
    path->close();
  }

  void MuCanvasContext2D::bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y) {
    float scale = state->transform.getScale();
    path->bezierCurveTo(cpx1, cpy1, cpx2, cpy2, x, y, scale);
  }

  void MuCanvasContext2D::quadraticCurveTo(float cpx, float cpy, float x, float y) {
    float scale = state->transform.getScale();
    path->quadraticCurveTo(cpx, cpy, x, y, scale);
  }

  void MuCanvasContext2D::arcTo(float x1, float y1, float x2, float y2, float radius) {
    path->arcTo(x1, y1, x2, y2, radius);
  }

  void MuCanvasContext2D::arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {
    path->arc(x, y, radius, startAngle, endAngle, antiClockwise);
  }

  void MuCanvasContext2D::prepare() {
    if (!frameBufferBinded) {
      viewFramebuffer->bindFramebuffer();
      gl::viewport(bufferSize);
      frameBufferBinded = true;
    }

    bindVertexBuffer();
  }

  void MuCanvasContext2D::flushBuffers() {
    if (vertexBufferIndex == 0) {
      return;
    }

    unbindVertexBuffer();

    batch->draw(0, vertexBufferIndex);
    vertexBufferIndex = 0;
  }

  void MuCanvasContext2D::finish() {
    flushBuffers();

    if (frameBufferBinded) {
      viewFramebuffer->unbindFramebuffer();
      frameBufferBinded = false;
    }
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

    // Flip y of the texture for Canvas2D drawing
    ty = 1 - ty;
    th *= -1;

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

  void MuCanvasContext2D::setProgram(const gl::GlslProgRef &glsl) {
    if (glsl != batch->getGlslProg()) {
      batch->replaceGlslProg(glsl);
    }
  }

  void MuCanvasContext2D::setTexture(const gl::Texture2dRef &texture) {
    if (texture == currentTexture) {
      return;
    }

    flushBuffers();

    currentTexture = texture;
    currentTexture->bind();

    // TODO: set to linear when imageSmoothEnabled is true
    currentTexture->setMagFilter(GL_NEAREST);
    currentTexture->setMinFilter(GL_NEAREST);
    currentTexture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  }

}
