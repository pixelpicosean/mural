#include "MuCanvasContext2D.h"
#include "MuSharedOpenGLContext.h"
#include "AppController.h"

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

  MuCanvasContext2D::MuCanvasContext2D(short widthp, short heightp) {
    vertexBuffer = (MuVertex *)(theSharedOpenGLContext.getVertexBuffer());
    vertexBufferSize = (int)(theSharedOpenGLContext.getVertexBufferLength() / sizeof(MuVertex));

    memset(stateStack, 0, sizeof(stateStack));
    stateIndex = 0;
    state = &stateStack[stateIndex];
    state->globalAlpha = 1;
    state->globalCompositeOperation = kMuCompositeOperationSourceOver;
    state->transform = glm::mat3();
    state->lineWidth = 1;
    state->lineCap = kMuLineCapButt;
    state->lineJoin = kMuLineJoinMiter;
    state->miterLimit = 10;
    state->textBaseline = kMuTextBaselineAlphabetic;
    state->textAlign = kMuTextAlignStart;
    // state->font = new MuFontDescriptor("Helvetica", 10);
    state->clipPath = nullptr;

    bufferWidth = width = widthp;
    bufferHeight = height = heightp;

    path = new MuPath();
    backingStoreRatio = 1;

    // fontCache = theFontCache;

    textureFilter = GL_LINEAR;
    msaaEnabled = false;
    msaaSamples = 2;
    stencilMask = 0x1;
  }

  MuCanvasContext2D::~MuCanvasContext2D() {
    if (viewFrameBuffer) { glDeleteFramebuffers( 1, &viewFrameBuffer); }
    if (viewRenderBuffer) { glDeleteRenderbuffers(1, &viewRenderBuffer); }
    if (msaaFrameBuffer) { glDeleteFramebuffers( 1, &msaaFrameBuffer); }
    if (msaaRenderBuffer) { glDeleteRenderbuffers(1, &msaaRenderBuffer); }
    if (stencilBuffer) { glDeleteRenderbuffers(1, &stencilBuffer); }
  }

  void MuCanvasContext2D::create() {
    if (msaaEnabled) {
      glGenFramebuffers(1, &msaaFrameBuffer);
      glGenRenderbuffers(1, &msaaRenderBuffer);
    }

    glGenFramebuffers(1, &viewFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);

    glGenRenderbuffers(1, &viewRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderBuffer);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DITHER);

    glEnable(GL_BLEND);
    glDepthFunc(GL_ALWAYS);

    resizeTo(width, height);
  }

  void MuCanvasContext2D::resizeTo(short newWidth, short newHeight) {
    // This function is a stub - Overwritten in both subclasses
    width = newWidth;
    height = newHeight;

    backingStoreRatio = (useRetinaResolution && app.devicePixelRatio == 2) ? 2 : 1;
    bufferWidth = width * backingStoreRatio;
    bufferHeight = height * backingStoreRatio;

    resetFramebuffer();
  }

  void MuCanvasContext2D::resetFramebuffer() {
    // Delete stencil buffer if present; it will be re-created when needed
    if (stencilBuffer) {
      glDeleteRenderbuffers(1, &stencilBuffer);
      stencilBuffer = 0;
    }

    // Resize the MSAA buffer
    if (msaaEnabled && msaaFrameBuffer && msaaRenderBuffer) {
      glBindFramebuffer(GL_FRAMEBUFFER, msaaFrameBuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, msaaRenderBuffer);

      glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA, bufferWidth, bufferHeight);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaRenderBuffer);
    }

    prepare();

    // Clear to transparent
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    needsPresenting = true;
  }

  void MuCanvasContext2D::createStencilBufferOnce() {
    if (stencilBuffer) { return; }

    glGenRenderbuffers(1, &stencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer);
    if (msaaEnabled) {
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    }
    else {
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, msaaEnabled ? msaaRenderBuffer : viewRenderBuffer );

    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
  }

  void MuCanvasContext2D::bindVertexBuffer() {
    glEnableVertexAttribArray(kMuGLProgram2DAttributePos);
    glVertexAttribPointer(kMuGLProgram2DAttributePos, 2, GL_FLOAT, GL_FALSE,
      sizeof(MuVertex), (char *)vertexBuffer + offsetof(MuVertex, pos));

    glEnableVertexAttribArray(kMuGLProgram2DAttributeUV);
    glVertexAttribPointer(kMuGLProgram2DAttributeUV, 2, GL_FLOAT, GL_FALSE,
      sizeof(MuVertex), (char *)vertexBuffer + offsetof(MuVertex, uv));

    glEnableVertexAttribArray(kMuGLProgram2DAttributeColor);
    glVertexAttribPointer(kMuGLProgram2DAttributeColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
      sizeof(MuVertex), (char *)vertexBuffer + offsetof(MuVertex, color));
  }

  void MuCanvasContext2D::prepare() {
    // Bind the frameBuffer and vertexBuffer array
    glBindFramebuffer(GL_FRAMEBUFFER, msaaEnabled ? msaaFrameBuffer : viewFrameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, msaaEnabled ? msaaRenderBuffer : viewRenderBuffer);

    glViewport(0, 0, bufferWidth, bufferHeight);

    MuCompositeOperation op = state->globalCompositeOperation;
    glBlendFunc(MuCompositeOperationFuncs[op].source, MuCompositeOperationFuncs[op].destination);
    currentTexture = nullptr;
    currentProgram = nullptr;

    bindVertexBuffer();

    if (stencilBuffer) {
      glEnable(GL_DEPTH_TEST);
    }
    else {
      glDisable(GL_DEPTH_TEST);
    }

    if (state->clipPath) {
      glDepthFunc(GL_EQUAL);
    }
    else {
      glDepthFunc(GL_ALWAYS);
    }

    needsPresenting = true;
  }

  void MuCanvasContext2D::setWidth(short newWidth) {
    if (newWidth == width) {
      // Same width as before? Just clear the canvas, as per the spec
      flushBuffers();
      glClear(GL_COLOR_BUFFER_BIT);
      return;
    }
    resizeTo(newWidth, height);
  }

  void MuCanvasContext2D::setHeight(short newHeight) {
    if (newHeight == height) {
      // Same height as before? Just clear the canvas, as per the spec
      flushBuffers();
      glClear(GL_COLOR_BUFFER_BIT);
      return;
    }
    resizeTo(width, newHeight);
  }

  void MuCanvasContext2D::setTexture(MuTexture *newTexture) {
    if (currentTexture == newTexture) { return; }

    flushBuffers();

    currentTexture = newTexture;
    currentTexture->bindWithFilter(textureFilter);
  }

  void MuCanvasContext2D::setProgram(MuGLProgram2D *newProgram) {
    if (currentProgram == newProgram) { return; }

    flushBuffers();
    currentProgram = newProgram;

    glUseProgram(currentProgram->program);
    glUniform2f(currentProgram->screen, width, height * (upsideDown ? -1 : 1));
  }

  void MuCanvasContext2D::pushTri(float x1, float y1, float x2, float y2, float x3, float y3, MuColorRGBA color, glm::mat3 transform) {
    if (vertexBufferIndex >= vertexBufferSize - 3) {
      flushBuffers();
    }

    glm::vec2 d1(x1, y1);
    glm::vec2 d2(x2, y2);
    glm::vec2 d3(x3, y3);

    glm::mat3 identity;
    if (transform != identity) {
      d1 = (glm::vec2)(transform * glm::vec3(d1, 1.0f));
      d2 = (glm::vec2)(transform * glm::vec3(d2, 1.0f));
      d3 = (glm::vec2)(transform * glm::vec3(d3, 1.0f));
    }

    MuVertex *vb = &vertexBuffer[vertexBufferIndex];
    vb[0] = (MuVertex) { d1, {0, 0}, color };
    vb[1] = (MuVertex) { d2, {0, 0}, color };
    vb[2] = (MuVertex) { d3, {0, 0}, color };

    vertexBufferIndex += 3;
  }

  void MuCanvasContext2D::pushQuad(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4, MuColorRGBA color, glm::mat3 transform) {
    if (vertexBufferIndex >= vertexBufferSize - 6) {
      flushBuffers();
    }

    glm::mat3 identity;
    if (transform != identity) {
      v1 = (glm::vec2)(transform * glm::vec3(v1, 1.0f));
      v2 = (glm::vec2)(transform * glm::vec3(v2, 1.0f));
      v3 = (glm::vec2)(transform * glm::vec3(v3, 1.0f));
      v4 = (glm::vec2)(transform * glm::vec3(v4, 1.0f));
    }

    MuVertex *vb = &vertexBuffer[vertexBufferIndex];
    vb[0] = (MuVertex) { v1, {0, 0}, color };
    vb[1] = (MuVertex) { v2, {0, 0}, color };
    vb[2] = (MuVertex) { v3, {0, 0}, color };
    vb[3] = (MuVertex) { v2, {0, 0}, color };
    vb[4] = (MuVertex) { v3, {0, 0}, color };
    vb[5] = (MuVertex) { v4, {0, 0}, color };

    vertexBufferIndex += 6;
  }

  void MuCanvasContext2D::pushRect(float x, float y, float w, float h, MuColorRGBA color, glm::mat3 transform) {
    if (vertexBufferIndex >= vertexBufferSize - 6) {
      flushBuffers();
    }

    glm::vec2 d11(x, y);
    glm::vec2 d21(x+w, y);
    glm::vec2 d12(x, y+h);
    glm::vec2 d22(x+w, y+h);

    glm::mat3 identity;
    if (transform != identity) {
      d11 = (glm::vec2)(transform * glm::vec3(d11, 1.0f));
      d21 = (glm::vec2)(transform * glm::vec3(d21, 1.0f));
      d12 = (glm::vec2)(transform * glm::vec3(d12, 1.0f));
      d22 = (glm::vec2)(transform * glm::vec3(d22, 1.0f));
    }

    MuVertex *vb = &vertexBuffer[vertexBufferIndex];
    vb[0] = (MuVertex) { d11, {0, 0}, color };  // top left
    vb[1] = (MuVertex) { d21, {0, 0}, color };  // top right
    vb[2] = (MuVertex) { d12, {0, 0}, color };  // bottom left

    vb[3] = (MuVertex) { d21, {0, 0}, color };  // top right
    vb[4] = (MuVertex) { d12, {0, 0}, color };  // bottom left
    vb[5] = (MuVertex) { d22, {0, 0}, color };  // bottom right

    vertexBufferIndex += 6;
  }

  void MuCanvasContext2D::pushFilledRect(float x, float y, float w, float h, MuFillable *fillable, MuColorRGBA color, glm::mat3 transform) {}
  void MuCanvasContext2D::pushGradientRect(float x, float y, float w, float h, MuCanvasGradient *gradient, MuColorRGBA color, glm::mat3 transform) {}
  void MuCanvasContext2D::pushPatternedRect(float x, float y, float w, float h, MuCanvasPattern *pattern, MuColorRGBA color, glm::mat3 transform) {}
  void MuCanvasContext2D::pushTexturedRect(
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    MuColorRGBA color,
    glm::mat3 transform
  ) {}

  void MuCanvasContext2D::flushBuffers() {
    if (vertexBufferIndex == 0) { return; }

    glDrawArrays(GL_TRIANGLES, 0, vertexBufferIndex);
    needsPresenting = true;
    vertexBufferIndex = 0;
  }

  void MuCanvasContext2D::save() {
    if (stateIndex == MU_CANVAS_STATE_STACK_SIZE - 1) {
      printf("Warning: MU_CANVAS_STATE_STACK_SIZE (%d) reached", MU_CANVAS_STATE_STACK_SIZE);
      return;
    }

    stateStack[stateIndex+1] = stateStack[stateIndex];
    stateIndex++;
    state = &stateStack[stateIndex];
  }

  void MuCanvasContext2D::restore() {
    if (stateIndex == 0) { return; }

    MuCompositeOperation oldCompositeOp = state->globalCompositeOperation;
    MuPath *oldClipPath = state->clipPath;

    // TODO: Clean up current state

    if (state->clipPath && state->clipPath != stateStack[stateIndex-1].clipPath) {
      resetClip();
    }

    // Load state from stack
    stateIndex--;
    state = &stateStack[stateIndex];

    path->transform = state->transform;

    // Set Composite op, if different
    if (state->globalCompositeOperation != oldCompositeOp) {
      globalCompositeOperation = state->globalCompositeOperation;
    }

    // Render clip path, if present and different
    if (state->clipPath && state->clipPath != oldClipPath) {
      setProgram(theSharedOpenGLContext.getGLProgram2DFlat());
      state->clipPath->drawPolygonsToContext(this,state->clipPath->fillRule, kMuPathPolygonTargetDepth);
    }
  }

  void MuCanvasContext2D::rotate(float angle) {
    state->transform = glm::rotate(state->transform, angle);
    path->transform = state->transform;
  }

  void MuCanvasContext2D::translate(float x, float y) {
    state->transform = glm::translate(state->transform, glm::vec2(x, y));
    path->transform = state->transform;
  }

  void MuCanvasContext2D::scale(float x, float y) {
    state->transform = glm::scale(state->transform, glm::vec2(x, y));
    path->transform = state->transform;
  }

  void MuCanvasContext2D::transform(float m11, float m12, float m21, float m22, float dx, float dy) {
    glm::mat3 t;
    t[0][0] = m11;
    t[1][0] = m12;
    t[0][1] = m21;
    t[1][1] = m22;
    t[0][2] = dx;
    t[1][2] = dy;

    state->transform = t * state->transform;
    path->transform = state->transform;
  }

  void MuCanvasContext2D::setTransform(float m11, float m12, float m21, float m22, float dx, float dy) {
    state->transform[0][0] = m11;
    state->transform[1][0] = m12;
    state->transform[0][1] = m21;
    state->transform[1][1] = m22;
    state->transform[0][2] = dx;
    state->transform[1][2] = dy;

    path->transform = state->transform;
  }

  void MuCanvasContext2D::drawImage(MuTexture *texture, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {
    float tw = texture->width;
    float th = texture->height;

    setProgram(theSharedOpenGLContext.getGLProgram2DTexture());
    setTexture(texture);
    pushTexturedRect(
      dx, dy, dw, dh,
      sx/tw, sy/th, sw/tw, sh/th,
      MuCanvasBlendWhiteColor(state),
      state->transform
    );
  }

  void MuCanvasContext2D::fillRect(float x, float y, float w, float h) {
    if (state->fillObject) {
      pushFilledRect(
        x, y, w, h,
        state->fillObject,
        MuCanvasBlendWhiteColor(state),
        state->transform
      );
    }
    else {
      setProgram(theSharedOpenGLContext.getGLProgram2DFlat());

      MuColorRGBA cc = MuCanvasBlendFillColor(state);
      pushRect(x, y, w, h,
        cc,
        state->transform
      );
    }
  }

  void MuCanvasContext2D::strokeRect(float x, float y, float w, float h) {
    MuPath *tempPath = new MuPath();

    tempPath->transform = state->transform;

    tempPath->moveTo(x, y);
    tempPath->lineTo(x+w, y);
    tempPath->lineTo(x+w, y+h);
    tempPath->lineTo(x, y+h);
    tempPath->close();

    setProgram(theSharedOpenGLContext.getGLProgram2DFlat());
    tempPath->drawLinesToContext(this);

    delete tempPath;
  }

  void MuCanvasContext2D::clearRect(float x, float y, float w, float h) {
    setProgram(theSharedOpenGLContext.getGLProgram2DFlat());

    MuCompositeOperation oldOp = state->globalCompositeOperation;
    globalCompositeOperation = kMuCompositeOperationDestinationOut;

    static MuColorRGBA white = { .hex = 0xffffffff };
    pushRect(x, y, w, h, white, state->transform);

    globalCompositeOperation = oldOp;
  }

  MuImageData *MuCanvasContext2D::getImageDataScaled(float scale, bool flipped, short sx, short sy, short sw, short sh) {
    return nullptr;
  }
  MuImageData *MuCanvasContext2D::getImageData(short sx, short sy, short sw, short sh) {
    return nullptr;
  }
  MuImageData *MuCanvasContext2D::getImageDataHD(short sx, short sy, short sw, short sh) {
    return nullptr;
  }
  void MuCanvasContext2D::putImageData(MuImageData* imageData, float dx, float dy) {}
  void MuCanvasContext2D::putImageDataHD(MuImageData* imageData, float dx, float dy) {}
  void MuCanvasContext2D::putImageData(MuImageData* imageData, float scale, float dx, float dy) {}

  void MuCanvasContext2D::beginPath() {
    path->reset();
  }

  void MuCanvasContext2D::closePath() {
    path->close();
  }

  void MuCanvasContext2D::fill(MuPathFillRule fillRule) {
    setProgram(theSharedOpenGLContext.getGLProgram2DFlat());
    path->drawPolygonsToContext(this, fillRule, kMuPathPolygonTargetColor);
  }

  void MuCanvasContext2D::stroke() {
    setProgram(theSharedOpenGLContext.getGLProgram2DFlat());
    path->drawLinesToContext(this);
  }

  void MuCanvasContext2D::moveTo(float x, float y) {
    path->moveTo(x, y);
  }

  void MuCanvasContext2D::lineTo(float x, float y) {
    path->lineTo(x, y);
  }

  void MuCanvasContext2D::rect(float x, float y, float w, float h) {
    path->moveTo(x, y);
    path->lineTo(x + w, y);
    path->lineTo(x + w, y + h);
    path->lineTo(x, y + h);
    path->close();
  }

  void MuCanvasContext2D::bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y) {}
  void MuCanvasContext2D::quadraticCurveTo(float cpx, float cpy, float x, float y) {}
  void MuCanvasContext2D::arcTo(float x1, float y1, float x2, float y2, float radius) {}
  void MuCanvasContext2D::arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {}

  // void MuCanvasContext2D::fillText(std::string text, float x, float y) {}
  // void MuCanvasContext2D::strokeText(std::string text, float x, float y) {}
  // MuTextMetrics MuCanvasContext2D::measureText(std::string text) {}

  void MuCanvasContext2D::clip(MuPathFillRule fillRule) {
    flushBuffers();
    state->clipPath = nullptr;

    state->clipPath = path; // TODO: copy
    setProgram(theSharedOpenGLContext.getGLProgram2DFlat());
    state->clipPath->drawPolygonsToContext(this, fillRule, kMuPathPolygonTargetDepth);
  }

  void MuCanvasContext2D::resetClip() {
    if (state->clipPath) {
      flushBuffers();
      // TODO: release clipPath
      state->clipPath = nullptr;

      glDepthMask(GL_TRUE);
      glClear(GL_DEPTH_BUFFER_BIT);
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_ALWAYS);
    }
  }

}
