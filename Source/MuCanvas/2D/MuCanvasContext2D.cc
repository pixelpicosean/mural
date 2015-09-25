#include "MuCanvasContext2D.h"
#include "MuCanvasPattern.h"
#include "MuCanvasGradient.h"
#include "MuCanvasShaders.h"
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

  void MuCanvasContext2D::setWidth(short newWidth) {
    if (newWidth == width) {
      flushBuffers();
      glClear(GL_COLOR_BUFFER_BIT);
      return;
    }
    resizeTo(newWidth, height);
  }

  void MuCanvasContext2D::setHeight(short newHeight) {
    if (newHeight == height) {
      flushBuffers();
      glClear(GL_COLOR_BUFFER_BIT);
      return;
    }
    resizeTo(width, newHeight);
  }

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

    if (widthp <= 0 || heightp <= 0) {
      width = app.width;
      height = app.height;
      bufferWidth = app.bufferWidth;
      bufferHeight = app.bufferHeight;
    }
    else {
      width = widthp;
      height = heightp;
      bufferWidth = width * app.devicePixelRatio;
      bufferHeight = height * app.devicePixelRatio;
    }

    path = new MuPath();

    textureFilter = GL_LINEAR;
    stencilMask = 0x1;
  }

  MuCanvasContext2D::~MuCanvasContext2D() {
    if (viewFrameBuffer) { glDeleteFramebuffers( 1, &viewFrameBuffer); }
    if (viewRenderBuffer) { glDeleteRenderbuffers(1, &viewRenderBuffer); }
    if (stencilBuffer) { glDeleteRenderbuffers(1, &stencilBuffer); }
  }

  void MuCanvasContext2D::create() {
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

    bufferWidth = width * app.devicePixelRatio;
    bufferHeight = height * app.devicePixelRatio;

    resetFramebuffer();
  }

  void MuCanvasContext2D::resetFramebuffer() {
    // Delete stencil buffer if present; it will be re-created when needed
    if (stencilBuffer) {
      glDeleteRenderbuffers(1, &stencilBuffer);
      stencilBuffer = 0;
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);

    // FIXME: Ejecta uses these lines to bind render buffer, should I use them instead?
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, viewRenderBuffer);

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
    glBindFramebuffer(GL_FRAMEBUFFER, viewFrameBuffer);
    // FIXME: Ejecta also binds render buffer here, which may not required
    // glBindRenderbuffer(GL_RENDERBUFFER, viewRenderBuffer);

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

  void MuCanvasContext2D::pushFilledRect(float x, float y, float w, float h, MuFillable *fillable, MuColorRGBA color, glm::mat3 transform) {
    MuCanvasPattern *pattern;
    MuCanvasGradient *gradient;

    if ((pattern = dynamic_cast<MuCanvasPattern *>(fillable))) {
      pushPatternedRect(x, y, w, h, pattern, color, transform);
    }
    else if ((gradient = dynamic_cast<MuCanvasGradient *>(fillable))) {
      pushGradientRect(x, y, w, h, gradient, color, transform);
    }
  }

  void MuCanvasContext2D::pushGradientRect(float x, float y, float w, float h, MuCanvasGradient *gradient, MuColorRGBA color, glm::mat3 transform) {
    if (gradient->type == kMuCanvasGradientTypeLinear) {
      // Local positions inside the quad
      glm::vec2 p1((gradient->p1.x-x)/w, (gradient->p1.y-y)/h);
      glm::vec2 p2((gradient->p2.x-x)/w, (gradient->p2.y-y)/h);

      // Calculate the slope of (p1,p2) and the line orthogonal to it
      float aspect = w/h;
      glm::vec2 slope = p2 - p1;
      glm::vec2 ortho(slope.y/aspect, -slope.x*aspect);

      // Calculate the intersection points of the slope (starting at p1)
      // and the orthogonal starting at each corner of the quad - these
      // points are the final texture coordinates.
      float d = 1/(slope.y * ortho.x - slope.x * ortho.y);

      glm::vec2
        ot(ortho.x * d, ortho.y * d),
        st(slope.x * d, slope.y * d);

      glm::vec2
        a11(ot.x * -p1.y, st.x * -p1.y),
        a12(ot.y * p1.x, st.y * p1.x),
        a21(ot.x * (1 - p1.y), st.x * (1 - p1.y)),
        a22(ot.y * (p1.x - 1), st.y * (p1.x - 1));

      glm::vec2
        t11(a11.x + a12.x, a11.y + a12.y),
        t21(a11.x + a22.x, a11.y + a22.y),
        t12(a21.x + a12.x, a21.y + a12.y),
        t22(a21.x + a22.x, a21.y + a22.y);

      setProgram(theSharedOpenGLContext.getGLProgram2DTexture());
      setTexture(gradient->texture);
      if (vertexBufferIndex >= vertexBufferSize - 6) {
        flushBuffers();
      }

      // Vertex coordinates
      glm::vec2 d11(x, y);
      glm::vec2 d21(x+w, y);
      glm::vec2 d12(x, y+h);
      glm::vec2 d22(x+w, y+h);

      glm::mat3 identity;
      if (identity != transform) {
        d11 = (glm::vec2)(transform * glm::vec3(d11, 1.0f));
        d21 = (glm::vec2)(transform * glm::vec3(d21, 1.0f));
        d12 = (glm::vec2)(transform * glm::vec3(d12, 1.0f));
        d22 = (glm::vec2)(transform * glm::vec3(d22, 1.0f));
      }

      MuVertex *vb = &vertexBuffer[vertexBufferIndex];
      vb[0] = (MuVertex) { d11, t11, color }; // top left
      vb[1] = (MuVertex) { d21, t21, color }; // top right
      vb[2] = (MuVertex) { d12, t12, color }; // bottom left

      vb[3] = (MuVertex) { d21, t21, color }; // top right
      vb[4] = (MuVertex) { d12, t12, color }; // bottom left
      vb[5] = (MuVertex) { d22, t22, color }; // bottom right

      vertexBufferIndex += 6;
    }
    else if (gradient->type == kMuCanvasGradientTypeRadial) {
      flushBuffers();

      MuGLProgram2DRadialGradient *gradientProgram = theSharedOpenGLContext.getGLProgram2DRadialGradient();
      setProgram(gradientProgram);

      glUniform3f(gradientProgram->inner, gradient->p1.x, gradient->p1.y, gradient->r1);
      glm::vec2 dp = gradient->p2 - gradient->p1;
      float dr = gradient->r2 - gradient->r1;
      glUniform3f(gradientProgram->diff, dp.x, dp.y, dr);

      setTexture(gradient->texture);
      pushTexturedRect(x, y, w, h, x, y, w, h, color, transform);
    }
  }

  void MuCanvasContext2D::pushPatternedRect(float x, float y, float w, float h, MuCanvasPattern *pattern, MuColorRGBA color, glm::mat3 transform) {
    MuTexture *texture = pattern->texture;
    float scale = texture->contentScale;
    float
      tw = texture->width / scale,
      th = texture->height / scale,
      pw = w,
      ph = h;

    if (!(pattern->repeat & kMuCanvasPatternRepeatX)) {
      pw = std::min(tw - x, w);
    }
    if (!(pattern->repeat & kMuCanvasPatternRepeatY)) {
      ph = std::min(th - y, h);
    }

    if (pw > 0 && ph > 0) { // We may have to skip entirely
      setProgram(theSharedOpenGLContext.getGLProgram2DPattern());
      setTexture(texture);

      pushTexturedRect(
        x, y, pw, ph,
        x/tw, y/th, pw/tw, ph/th,
        color, transform
      );
    }

    if (pw < w || ph < h) {
      // Draw clearing rect for the stencil buffer if we didn't fill everything with
      // the pattern image - happens when not repeating in both directions
      setProgram(theSharedOpenGLContext.getGLProgram2DFlat());
      MuColorRGBA transparentBlack = { .hex = 0x00000000 };
      pushRect(x, y, w, h, transparentBlack, transform);
    }
  }

  void MuCanvasContext2D::pushTexturedRect(
    float x, float y, float w, float h,
    float tx, float ty, float tw, float th,
    MuColorRGBA color,
    glm::mat3 transform
  )
  {
    if (vertexBufferIndex >= vertexBufferSize - 6) {
      flushBuffers();
    }

    // Textures from offscreen WebGL contexts have to be draw upside down.
    // They're actually right-side up in memory, but everything else has
    // flipped y
    if (currentTexture->drawFlippedY) {
      ty = 1 - ty;
      th *= -1;
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
    vb[0] = (MuVertex) { d11, {tx, ty}, color };  // top left
    vb[1] = (MuVertex) { d21, {tx+tw, ty}, color }; // top right
    vb[2] = (MuVertex) { d12, {tx, ty+th}, color }; // bottom left

    vb[3] = (MuVertex) { d21, {tx+tw, ty}, color }; // top right
    vb[4] = (MuVertex) { d12, {tx, ty+th}, color }; // bottom left
    vb[5] = (MuVertex) { d22, {tx+tw, ty+th}, color };  // bottom right

    vertexBufferIndex += 6;
  }

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
    flushBuffers();

    unsigned char *pixels;

    // Fast case - no scaling, no flipping
    if (scale == 1 && !flipped) {
      pixels = new unsigned char[sw * sh * 4];
      glReadPixels(sx, sy, sw, sh, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    // More processing needed - take care of the flipped screen layout and the scaling
    else {
      int internalWidth = sw * scale;
      int internalHeight = sh * scale;
      int internalX = sx * scale;
      int internalY = ((bufferHeight/scale)-sy-sh) * scale;

      MuColorRGBA *internalPixels = (MuColorRGBA *)malloc(internalWidth * internalHeight * sizeof(MuColorRGBA));
      glReadPixels(internalX, internalY, internalWidth, internalHeight, GL_RGBA, GL_UNSIGNED_BYTE, internalPixels);

      int size = sw * sh * sizeof(MuColorRGBA);
      MuColorRGBA *scaledPixels = (MuColorRGBA *)malloc(size);
      int index = 0;
      for (int y = 0; y < sh; y++) {
        int rowIndex = (int)((flipped ? sh-y-1 : y) * scale) * internalWidth;
        for (int x = 0; x < sw; x++) {
          int internalIndex = rowIndex + (int)(x * scale);
          scaledPixels[index] = internalPixels[internalIndex];
          index++;
        }
      }
      free(internalPixels);

      pixels = (unsigned char *)scaledPixels;
    }

    return new MuImageData(sw, sh, pixels);
  }

  MuImageData *MuCanvasContext2D::getImageData(short sx, short sy, short sw, short sh) {
    return getImageDataScaled(1, upsideDown, sx, sy, sw, sh);
  }

  MuImageData *MuCanvasContext2D::getImageDataHD(short sx, short sy, short sw, short sh) {
    return getImageDataScaled(1.0f, upsideDown, sx, sy, sw, sh);
  }

  void MuCanvasContext2D::putImageData(MuImageData* imageData, float scale, float dx, float dy) {
    MuTexture *texture = imageData->getTexture();
    setProgram(theSharedOpenGLContext.getGLProgram2DTexture());
    setTexture(texture);

    short tw = texture->width / scale;
    short th = texture->height / scale;

    static MuColorRGBA white = { .hex = 0xffffffff };

    MuCompositeOperation oldOp = state->globalCompositeOperation;
    globalCompositeOperation = kMuCompositeOperationCopy;

    pushTexturedRect(dx, dy, tw, th, 0, 0, 1, 1, white, glm::mat3());
    flushBuffers();

    globalCompositeOperation = oldOp;
  }

  void MuCanvasContext2D::putImageDataHD(MuImageData* imageData, float dx, float dy) {
    putImageData(imageData, 1, dx, dy);
  }

  void MuCanvasContext2D::putImageData(MuImageData* imageData, float dx, float dy) {
    putImageData(imageData, 1, dx, dy);
  }

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
