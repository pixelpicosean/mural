#ifndef Mural_MuCanvasContext_h
#define Mural_MuCanvasContext_h

#include <cstdio>

namespace mural {

  class MuCanvasContext {
    public:
      bool needsPresenting = false;

      float backingStoreRatio = 1.0f;
      bool useRetinaResolution = false;

      virtual short getWidth() { return width; }
      virtual void setWidth(short newWidth) { width = newWidth; }

      virtual short getHeight() { return height; }
      virtual void setHeight(short newHeight) { height = newHeight; }

      MuCanvasContext() {}
      virtual ~MuCanvasContext() {}

      virtual void create() {}
      virtual void flushBuffers() {}
      virtual void prepare() {}

      // Presentable
      virtual void present() {}

    protected:
      short width = 200, height = 150;
  };

}

#endif
