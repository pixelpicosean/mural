#ifndef Mural_MuCanvasContext_h
#define Mural_MuCanvasContext_h

#include <cstdio>

namespace mural {

  class MuCanvasContext {
    public:
      bool needsPresenting = false;

      float backingStoreRatio = 1.0f;

      virtual int getWidth() { return width; }
      virtual void setWidth(int newWidth) { width = newWidth; }

      virtual int getHeight() { return height; }
      virtual void setHeight(int newHeight) { height = newHeight; }

      MuCanvasContext() {}
      virtual ~MuCanvasContext() {}

      virtual void create() {}
      virtual void prepare() {}
      virtual void flushBuffers() {}
      virtual void present() {}

    protected:
      int width = 200, height = 150;
  };

}

#endif
