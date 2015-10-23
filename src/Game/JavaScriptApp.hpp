//
//  JavaScriptApp.hpp
//  Mural
//
//  Created by Sean on 10/22/15.
//
//

#pragma once

#include "MuGameController.hpp"

#include "MuCanvas.hpp"
#include "MuCanvasManager.hpp"
#include "MuCanvasContext2D.hpp"
#include "MuImage.hpp"
#include "MuTimer.hpp"
#include "MuCanvasPattern.hpp"

#include "duktape.h"

using namespace ci;
using namespace ci::app;

using namespace mural;

class JavaScriptApp : public MuGameController {
  public:
    JavaScriptApp();
    ~JavaScriptApp();

    void init() override;

  protected:
    duk_context* jsContext;
};
