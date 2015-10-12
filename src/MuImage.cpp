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

  ci::signals::Connection MuImage::on(const std::string &event, const std::function<void (MuImage *)> &callback) {
    ci::signals::Connection c;
    if (event == "load") {
      c = loadSignal.connect(callback);
      if (texture) {
        loadSignal.emit(this);
      }
    }
    else if (event == "error") {
      c = errorSignal.connect(callback);
    }

    return c;
  }

  ci::signals::Connection MuImage::addEventListener(const std::string &event, const std::function<void (MuImage *)> &callback) {
    return on(event, callback);
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
