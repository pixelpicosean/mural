//
//  MuImage.hpp
//  ciEjecta
//
//  Created by Sean on 10/5/15.
//
//

#include <string>
#include <functional>
#include <map>

namespace mural {

  using ci::gl::Texture2dRef;

  class MuImage {
    public:
      std::string getSrc() const { return path; }
      void setSrc(const std::string &path);

      Texture2dRef getTexture() const { return texture; }

      void on(const std::string &event, const std::function<void(MuImage *)> &callback);
      void off(const std::string &event, const std::function<void(MuImage *)> &callback) {}
      void addEventListener(const std::string &event, const std::function<void(MuImage *)> &callback);
      void removeEventListener(const std::string &event, const std::function<void(MuImage *)> &callback);

      MuImage() {}

    protected:
      Texture2dRef texture = nullptr;
      std::string path = "";
      bool loading = false;

      ci::signals::Signal<void(MuImage *)> loadSignal;
      ci::signals::Signal<void(MuImage *)> errorSignal;

      void beginLoad();
      void endLoad(bool success);
  };

}
