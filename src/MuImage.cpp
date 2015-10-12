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

  void MuImage::on(const std::string &event, const std::function<void (MuImage *)> &callback) {
    if (event == "load") {
      loadSignal.connect(callback);
      if (texture) {
        loadSignal.emit(this);
      }
    }
    else if (event == "error") {
      errorSignal.connect(callback);
    }
  }

  void MuImage::addEventListener(const std::string &event, const std::function<void (MuImage *)> &callback) {
    on(event, callback);
  }

  void MuImage::removeEventListener(const std::string &event, const std::function<void (MuImage *)> &callback) {
    off(event, callback);
  }

  void MuImage::beginLoad() {
    loading = true;

    DataSourceRef img = nullptr;
    try {
      img = loadAsset(path);
    } catch (AssetLoadExc e) {}

    if (img) {
      texture = gl::Texture2d::create(loadImage(img));
      theScheduler.setTimeout(std::bind(&MuImage::endLoad, this, true), 0);
    }
    else {
      theScheduler.setTimeout(std::bind(&MuImage::endLoad, this, false), 0);
    }
  }

  void MuImage::endLoad(bool success) {
    loading = false;

    if (success) {
      loadSignal.emit(this);
    }
    else {
      errorSignal.emit(this);
    }
  }

}
