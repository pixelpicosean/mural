#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "MuCanvas.hpp"

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
    ctx->rect(10, 10, 40, 40);
    ctx->setFillColor(ColorAf{ 0.0f, 1.0f, 1.0f, 1.0f });
    ctx->fill();

    finished = true;
  }
}

void ciEjectaApp::draw() {
	gl::clear(Color(0, 0, 0));

  gl::draw(canvas->getTexture(), getWindowBounds());
}

CINDER_APP(ciEjectaApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
