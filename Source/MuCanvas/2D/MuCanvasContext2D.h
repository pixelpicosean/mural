#ifndef Mural_MuCanvasContext2D_h
#define Mural_MuCanvasContext2D_h

#include "MuOpenGL.h"

#include "MuGLProgram2D.h"

#include "MuCanvas/MuTexture.h"
#include "MuImageData.h"
#include "MuPath.h"
#include "MuCanvas2DTypes.h"
#include "MuCanvas/MuCanvasContext.h"
// #include "MuFont.h"
// #include "MuFontCache.h"

#include <string>

namespace mural {

  #define MU_CANVAS_STATE_STACK_SIZE 16

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

  struct MuCompositeOperationFunc {
    GLenum source;
    GLenum destination;
    float alphaFactor;
  };
  extern const MuCompositeOperationFunc MuCompositeOperationFuncs[];

  class MuCanvasPattern;
  class MuCanvasGradient;

  struct MuFillable {
    virtual ~MuFillable() {}
  };

  struct MuCanvasState {
    MuAffineTransform transform;

    MuCompositeOperation globalCompositeOperation;
    MuColorRGBA fillColor;
    MuFillable *fillObject;
    MuColorRGBA strokeColor;
    MuFillable *strokeObject;
    float globalAlpha;

    float lineWidth;
    MuLineCap lineCap;
    MuLineJoin lineJoin;
    float miterLimit;

    MuTextAlign textAlign;
    MuTextBaseline textBaseline;
    // MuFontDescriptor *font;

    MuPath *clipPath;
  };

  static inline MuColorRGBA MuCanvasBlendColor(MuCanvasState *state, MuColorRGBA color) {
    float alpha = state->globalAlpha * (float)color.rgba.a / 255.0f;
    return (MuColorRGBA) { .rgba = {
      .r = (unsigned char)(color.rgba.r * alpha),
      .g = (unsigned char)(color.rgba.g * alpha),
      .b = (unsigned char)(color.rgba.b * alpha),
      .a = (unsigned char)(MuCompositeOperationFuncs[state->globalCompositeOperation].alphaFactor *
         (float)color.rgba.a * state->globalAlpha)
    }};
  }

  static inline MuColorRGBA MuCanvasBlendWhiteColor(MuCanvasState *state) {
    return MuCanvasBlendColor(state, (MuColorRGBA){.hex = 0xffffffff});
  }

  static inline MuColorRGBA MuCanvasBlendFillColor(MuCanvasState *state) {
    return MuCanvasBlendColor(state, state->fillColor);
  }

  static inline MuColorRGBA MuCanvasBlendStrokeColor(MuCanvasState *state) {
    return MuCanvasBlendColor(state, state->strokeColor);
  }


  class MuCanvasContext2D : public MuCanvasContext {
    public:
      MuCanvasState *state = nullptr;

      MuCompositeOperation globalCompositeOperation;
      bool imageSmoothingEnabled = false;;

      // MuFontDescriptor *font;
      MuFillable *fillObject = nullptr;
      MuFillable *strokeObject = nullptr;

      GLubyte stencilMask;

      virtual MuTexture *getTexture() { return nullptr; }
      void setTexture(MuTexture *newTexture);

      void setWidth(short newWidth);
      void setHeight(short newHeight);

      void setProgram(MuGLProgram2D *program);

      MuCanvasContext2D(short widthp, short heightp);
      virtual ~MuCanvasContext2D();

      void create();
      virtual void resizeTo(short newWidth, short newHeight);
      void resetFramebuffer();
      void createStencilBufferOnce();
      void bindVertexBuffer();
      void prepare();
      void pushTri(float x1, float y1, float x2, float y2, float x3, float y3, MuColorRGBA color, MuAffineTransform transform);
      void pushQuad(MuVector2 v1, MuVector2 v2, MuVector2 v3, MuVector2 v4, MuColorRGBA color, MuAffineTransform transform);
      void pushRect(float x, float y, float w, float h, MuColorRGBA color, MuAffineTransform transform);
      void pushFilledRect(float x, float y, float w, float h, MuFillable *fillable, MuColorRGBA color, MuAffineTransform transform);
      void pushGradientRect(float x, float y, float w, float h, MuCanvasGradient *gradient, MuColorRGBA color, MuAffineTransform transform);
      void pushPatternedRect(float x, float y, float w, float h, MuCanvasPattern *pattern, MuColorRGBA color, MuAffineTransform transform);
      void pushTexturedRect(
        float x, float y, float w, float h,
        float tx, float ty, float tw, float th,
        MuColorRGBA color,
        MuAffineTransform transform
      );
      void flushBuffers();

      void save();
      void restore();
      void rotate(float angle);
      void translate(float x, float y);
      void scale(float x, float y);
      void transform(float m11, float m12, float m21, float m2, float dx, float dy);
      void setTransform(float m11, float m12, float m21, float m2, float dx, float dy);
      void drawImage(MuTexture *image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh);
      void fillRect(float x, float y, float w, float h);
      void strokeRect(float x, float y, float w, float h);
      void clearRect(float x, float y, float w, float h);
      MuImageData *getImageDataScaled(float scale, bool flipped, short sx, short sy, short sw, short sh);
      MuImageData *getImageData(short sx, short sy, short sw, short sh);
      MuImageData *getImageDataHD(short sx, short sy, short sw, short sh);
      void putImageData(MuImageData* imageData, float dx, float dy);
      void putImageDataHD(MuImageData* imageData, float dx, float dy);
      void putImageData(MuImageData* imageData, float scale, float dx, float dy);
      void beginPath();
      void closePath();
      void fill(MuPathFillRule fillRule);
      void stroke();
      void moveTo(float x, float y);
      void lineTo(float x, float y);
      void rect(float x, float y, float w, float h);
      void bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y);
      void quadraticCurveTo(float cpx, float cpy, float x, float y);
      void arcTo(float x1, float y1, float x2, float y2, float radius);
      void arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise);

      // void fillText(std::string text, float x, float y);
      // void strokeText(std::string text, float x, float y);
      // MuTextMetrics measureText(std::string text);

      void clip(MuPathFillRule fillRule);
      void resetClip();

    protected:
      GLuint viewFrameBuffer = 0, viewRenderBuffer = 0;
      GLuint stencilBuffer = 0;

      short bufferWidth = 200, bufferHeight = 150;

      GLenum textureFilter;
      MuTexture *currentTexture = nullptr;
      MuPath *path = nullptr;

      MuVertex *vertexBuffer = nullptr;
      int vertexBufferSize = 0;
      int vertexBufferIndex = 0;

      int stateIndex = 0;
      MuCanvasState stateStack[MU_CANVAS_STATE_STACK_SIZE];

      bool upsideDown = false;

      // MuFontCache *fontCache;

      MuGLProgram2D *currentProgram = nullptr;
  };

  /*
    TODO: not yet implemented:
      shadowOffsetX
      shadowOffsetY
      shadowBlur
      shadowColor
      isPointInPath(x, y)
  */

}

#endif
