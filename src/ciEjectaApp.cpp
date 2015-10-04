#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "MuCanvas.hpp"
#include "MuCanvasManager.hpp"
#include "MuCanvasContext2D.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciEjectaApp : public App {
  public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
  protected:
    std::shared_ptr<mural::MuCanvas> canvas;
    bool finished = false;
};

void ciEjectaApp::setup() {
  canvas = std::make_shared<mural::MuCanvas>();
}

void ciEjectaApp::mouseDown(MouseEvent event) {}

void ciEjectaApp::update() {}

void ciEjectaApp::draw() {
  // - TEST BEGIN -----------------------------------------------------
  if (!finished) {
    auto ctx = canvas->getContext(mural::kMuCanvasContextMode2D);

    // Tests
    // - LineCap
    auto testLineCap = [&]() {
      ctx->setStrokeColor(ColorAf(0.0f, 1.0f, 1.0f, 1.0f));
      ctx->state->lineWidth = 40;

      // - Butt
      ctx->state->lineCap = mural::kMuLineCapButt;

      ctx->moveTo(200, 100);
      ctx->lineTo(640 - 200, 100);
      ctx->stroke();

      // - Squire
      ctx->state->lineCap = mural::kMuLineCapSquare;

      ctx->moveTo(200, 200);
      ctx->lineTo(640 - 200, 200);
      ctx->stroke();

      // - Round
      ctx->state->lineCap = mural::kMuLineCapRound;

      ctx->moveTo(200, 300);
      ctx->lineTo(640 - 200, 300);
      ctx->stroke();
    };

    testLineCap();

    finished = true;
  }
  // - TEST END --------------------------------------------------------

  mural::theCanvasManager.drawScreenCanvas();
}

CINDER_APP(ciEjectaApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
