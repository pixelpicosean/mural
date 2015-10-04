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

void ciEjectaApp::update() {
  if (!finished) {
    auto ctx = canvas->getContext(mural::kMuCanvasContextMode2D);

    // Draw something here
    ctx->setFillColor(ColorAf(0.0f, 1.0f, 1.0f, 1.0f));
    ctx->setStrokeColor(ColorAf(1.0f, 0.0f, 1.0f, 1.0f));
    ctx->state->lineWidth = 8;
    ctx->state->lineJoin = mural::kMuLineJoinRound;

    ctx->rect(100, 100, 100, 100);
    ctx->stroke();
    ctx->fill();

    ctx->rect(220, 100, 100, 100);
    ctx->fill();
    ctx->stroke();

    ctx->fillRect(220, 100, 100, 100);

    ctx->setStrokeColor(ColorAf(1.0f, 1.0f, 0.0f, 1.0f));
    ctx->state->lineWidth = 4;
    ctx->strokeRect(160, 220, 100, 100);

    ctx->translate(320, 100);
    ctx->save();
    ctx->setStrokeColor(ColorAf(0.8f, 0.3f, 0.0f, 1.0f));
    ctx->setFillColor(ColorAf(0.0f, 0.3f, 0.8f, 1.0f));
    ctx->beginPath();
    ctx->moveTo(20, 0);
    ctx->lineTo(120, 0);
    ctx->lineTo(120, 100);
    ctx->closePath();
    ctx->stroke();
    ctx->fill();
    ctx->restore();

    ctx->finish();

    finished = true;
  }
}

void ciEjectaApp::draw() {
	gl::clear(Color(0, 0, 0));
  mural::theCanvasManager.drawScreenCanvas();
}

CINDER_APP(ciEjectaApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
