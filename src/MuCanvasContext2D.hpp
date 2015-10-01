//
//  MuCanvasContext2D.hpp
//  ciEjecta
//
//  Created by Sean on 10/1/15.
//
//

#pragma once

#define MU_CANVAS_STATE_STACK_SIZE 16

namespace mural {

  using namespace ci;
  using namespace ci::app;

  enum MuLineCap {
    kMuLineCapButt,
    kMuLineCapRound,
    kMuLineCapSquare
  };

  enum MuLineJoin {
    kMuLineJoinMiter,
    kMuLineJoinBevel,
    kMuLineJoinRound
  };

  enum MuTextBaseline {
    kMuTextBaselineAlphabetic,
    kMuTextBaselineMiddle,
    kMuTextBaselineTop,
    kMuTextBaselineHanging,
    kMuTextBaselineBottom,
    kMuTextBaselineIdeographic
  };

  enum MuTextAlign {
    kMuTextAlignStart,
    kMuTextAlignEnd,
    kMuTextAlignLeft,
    kMuTextAlignCenter,
    kMuTextAlignRight
  };

  enum MuCompositeOperation {
    kMuCompositeOperationSourceOver,
    kMuCompositeOperationLighter,
    kMuCompositeOperationDarker,
    kMuCompositeOperationDestinationOut,
    kMuCompositeOperationDestinationOver,
    kMuCompositeOperationSourceAtop,
    kMuCompositeOperationXOR,
    kMuCompositeOperationCopy,
    kMuCompositeOperationSourceIn,
    kMuCompositeOperationDestinationIn,
    kMuCompositeOperationSourceOut,
    kMuCompositeOperationDestinationAtop
  };

  struct MuCanvasState {
    mat3 transform;

//    MuCompositeOperation globalCompositeOperation;
    ColorAf fillColor = ColorAf::white();
//    MuFillable *fillObject;
    ColorAf strokeColor = ColorAf::white();
//    MuFillable *strokeObject;
    float globalAlpha = 1.0f;

    float lineWidth = 1.0f;
    MuLineCap lineCap = kMuLineCapButt;
    MuLineJoin lineJoin = kMuLineJoinBevel;
    float miterLimit = 10.0f;

    MuTextAlign textAlign = kMuTextAlignStart;
    MuTextBaseline textBaseline = kMuTextBaselineAlphabetic;
    // MuFontDescriptor *font;

    Path2d path;
  };

  class MuCanvasContext2D {
    public:
      MuCanvasState *state;

      virtual const gl::TextureRef getTexture();

      void setFillColor(const ColorAf &color);
      void setStrokeColor(const ColorAf &color);

      MuCanvasContext2D(int width, int height);
      virtual ~MuCanvasContext2D() {}

      virtual void create();
      virtual void resizeTo(int width, int height);
      void resetFramebuffer();

      void save() {}
      void restore() {}
      void rotate(float angle) {}
      void translate(float x, float y) {}
      void scale(float x, float y) {}
      void transform(float m11, float m12, float m21, float m2, float dx, float dy) {}
      void setTransform(float m11, float m12, float m21, float m2, float dx, float dy) {}
      void drawImage(gl::TextureRef image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {}
      void fillRect(float x, float y, float w, float h) {}
      void strokeRect(float x, float y, float w, float h) {}
      void clearRect(float x, float y, float w, float h) {}
      void beginPath();
      void closePath();
      void fill();
      void stroke();
      void moveTo(float x, float y);
      void lineTo(float x, float y);
      void rect(float x, float y, float w, float h);
      void bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y) {}
      void quadraticCurveTo(float cpx, float cpy, float x, float y) {}
      void arcTo(float x1, float y1, float x2, float y2, float radius) {}
      void arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {}

      void prepare();
      void pushTri(float x1, float y1, float x2, float y2, float x3, float y3, const ColorAf &color, const mat3 &transform) {}
      void pushQuad(ci::vec2 v1, ci::vec2 v2, ci::vec2 v3, ci::vec2 v4, const ColorAf &color, const mat3 &transform) {}
      void pushRect(float x, float y, float w, float h, const ColorAf &color, const mat3 &transform) {}
      void pushTexturedRect(
        float x, float y, float w, float h,
        float tx, float ty, float tw, float th,
        const ColorAf &color,
        const mat3 &transform
        ) {}
      void flushBuffers();

    protected:
      bool needsPresenting = false;

      ivec2 size = { 200, 150 };
      float devicePixelRatio = 1.0f;

      ivec2 bufferSize = { 200, 150 };
      gl::FboRef viewFramebuffer;

      MuCanvasState stateStack[MU_CANVAS_STATE_STACK_SIZE];
  };

}
