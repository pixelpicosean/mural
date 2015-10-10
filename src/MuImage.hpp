//
//  MuImage.hpp
//  ciEjecta
//
//  Created by Sean on 10/5/15.
//
//

#include <string>
#include <functional>

namespace mural {

  using ci::gl::Texture2dRef;

  class MuImage {
    public:
      std::string getSrc() const { return path; }
      void setSrc(const std::string &path);

      Texture2dRef getTexture() const { return texture; }

      void onLoad(const std::function<void(MuImage *)> callback);

      MuImage() {}

    protected:
      Texture2dRef texture = nullptr;
      std::string path = "";
      bool loading = false;
      std::function<void(MuImage *)> loadCallback;

      void beginLoad();
      void endLoad();
  };

}
