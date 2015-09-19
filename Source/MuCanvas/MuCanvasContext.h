#ifndef Mural_MuCanvasContext_h
#define Mural_MuCanvasContext_h

namespace mural {

  class MuCanvasContext {
    public:
      short width, height;

      bool msaaEnabled = false;
      bool needsPresenting;
      int msaaSamples;

      float backingStoreRatio;
      bool useRetinaResolution;

      virtual void create() {}
      virtual void flushBuffers() {}
      virtual void prepare() {}

      // Presentable
      virtual void present() {}
      virtual void finish() {}
  };

}

#endif
