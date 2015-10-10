//
//  MuImage.cpp
//  ciEjecta
//
//  Created by Sean on 10/5/15.
//
//

#include "MuImage.hpp"
#include "MuTimer.hpp"

namespace mural {

  using namespace ci;
  using namespace ci::app;

  void MuImage::setSrc(const std::string &newPath) {
    if (loading) return;
    if (path == newPath) return;

    if (texture) {
      texture = nullptr;
    }

    if (!newPath.empty()) {
      path = newPath;
      beginLoad();
    }
  }

  void MuImage::onLoad(const std::function<void (MuImage *)> callback) {
    loadCallback = callback;
    if (texture) {
      theScheduler.setTimeout(std::bind(callback, this), 0);
    }
  }

  void MuImage::beginLoad() {
    loading = true;

    texture = gl::Texture2d::create(loadImage(loadAsset(path)));

    theScheduler.setTimeout(std::bind(&MuImage::endLoad, this), 0);
  }

  void MuImage::endLoad() {
    loading = false;

    if (loadCallback) {
      loadCallback(this);
    }
  }

}
