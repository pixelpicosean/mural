#ifndef Mural_MuCanvasContext2D_h
#define Mural_MuCanvasContext2D_h

#include "MuOpenGL.h"
#include "MuNanoVG.h"

#include "MuCanvas/MuCanvasContext.h"

#include <string>

namespace mural {

  class MuCanvasContext2D : public MuCanvasContext {
    public:
      bool imageSmoothingEnabled = false;

      NVGcolor fillColor;

      void setWidth(int newWidth);
      void setHeight(int newHeight);

      MuCanvasContext2D(int widthp, int heightp);
      virtual ~MuCanvasContext2D();

      virtual void resizeTo(int newWidth, int newHeight);
      virtual void create();
      virtual void prepare();
      virtual void flushBuffers();

      void save();
      void restore();
      void rotate(float angle);
      void translate(float x, float y);
      void scale(float x, float y);
      void transform(float m11, float m12, float m21, float m2, float dx, float dy);
      void setTransform(float m11, float m12, float m21, float m2, float dx, float dy);
      // void drawImage(MuTexture *image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh);
      void fillRect(float x, float y, float w, float h);
      void strokeRect(float x, float y, float w, float h);
      void clearRect(float x, float y, float w, float h);
      // MuImageData *getImageDataScaled(float scale, bool flipped, int sx, int sy, int sw, int sh);
      // MuImageData *getImageData(int sx, int sy, int sw, int sh);
      // MuImageData *getImageDataHD(int sx, int sy, int sw, int sh);
      // void putImageData(MuImageData* imageData, float dx, float dy);
      // void putImageDataHD(MuImageData* imageData, float dx, float dy);
      // void putImageData(MuImageData* imageData, float scale, float dx, float dy);
      void beginPath();
      void closePath();
      void fill();
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

    protected:
      int bufferWidth = 200, bufferHeight = 150;
      NVGcontext *glContext = nullptr;
      nvg::Framebuffer *framebuffer = nullptr;

      bool upsideDown = false;
  };

}

#endif
